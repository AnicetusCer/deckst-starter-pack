#if !defined(__MINT__) && !defined(__TOS__) && !defined(__atarist__)
#define _POSIX_C_SOURCE 199309L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__MINT__) || defined(__TOS__) || defined(__atarist__)
#define BULLDOG_ATARI 1
#include <osbind.h>
#else
#define BULLDOG_ATARI 0
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#endif

enum {
    FIELD_W = 32,
    FIELD_H = 18,
    MAX_OBSTACLES = 6,
    FPS = 20,
    START_LIVES = 3
};

typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_FIRE,
    KEY_QUIT
} Key;

typedef enum {
    PHASE_BACK = 0,
    PHASE_WARN,
    PHASE_WATCH
} Phase;

typedef enum {
    MODE_PLAY = 0,
    MODE_GAME_OVER,
    MODE_WIN
} Mode;

typedef enum {
    STYLE_RESET = 0,
    STYLE_HEADER,
    STYLE_BORDER,
    STYLE_FIELD,
    STYLE_FINISH,
    STYLE_FOX,
    STYLE_BULLDOG_SAFE,
    STYLE_BULLDOG_WARN,
    STYLE_BULLDOG_WATCH,
    STYLE_OBSTACLE,
    STYLE_MESSAGE
} Style;

typedef struct {
    int y;
    int x;
    int dx;
    int min_x;
    int max_x;
} Obstacle;

typedef struct {
    Mode mode;
    int level;
    int fox_x;
    int fox_y;
    int bulldog_x;
    int lives;
    int timer_ticks;
    int phase_ticks;
    int phase_limit;
    Phase phase;
    int message_ticks;
    char message[64];
    Obstacle obstacles[MAX_OBSTACLES];
    int obstacle_count;
} Game;

#if !BULLDOG_ATARI
static struct termios original_termios;
static int terminal_changed = 0;

static void host_restore_terminal(void) {
    if (terminal_changed) {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
        terminal_changed = 0;
    }
    printf("\033[?25h\033[0m\n");
}

static void host_raw_terminal(void) {
    struct termios raw;
    if (tcgetattr(STDIN_FILENO, &original_termios) != 0) return;
    raw = original_termios;
    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
        terminal_changed = 1;
        atexit(host_restore_terminal);
    }
}
#endif

static void backend_init(void) {
#if BULLDOG_ATARI
    printf("\033E\033f");
#else
    host_raw_terminal();
    printf("\033[2J\033[?25l");
#endif
    fflush(stdout);
}

static void backend_shutdown(void) {
#if BULLDOG_ATARI
    printf("\033E\033e");
#else
    host_restore_terminal();
#endif
    fflush(stdout);
}

static void backend_goto(int row, int col) {
#if BULLDOG_ATARI
    putchar(27);
    putchar('Y');
    putchar((char)(row + 32));
    putchar((char)(col + 32));
#else
    printf("\033[%d;%dH", row + 1, col + 1);
#endif
}

static void backend_style(Style style) {
#if BULLDOG_ATARI
    (void)style;
#else
    static const char *codes[] = {
        "\033[0m",
        "\033[1;37;44m",
        "\033[1;32m",
        "\033[0;32m",
        "\033[1;36m",
        "\033[1;33m",
        "\033[1;34m",
        "\033[1;33;41m",
        "\033[1;37;41m",
        "\033[1;35m",
        "\033[1;37m"
    };
    printf("%s", codes[(int)style]);
#endif
}

static void backend_sleep_frame(void) {
#if BULLDOG_ATARI
    long i;
    for (i = 0; i < 5000; ++i) {
        ;
    }
#else
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 1000000000L / FPS;
    nanosleep(&delay, NULL);
#endif
}

static Key backend_read_key(void) {
#if BULLDOG_ATARI
    long raw;
    int ascii;
    int scan;
    if (!Bconstat(2)) return KEY_NONE;
    raw = Bconin(2);
    ascii = (int)(raw & 0xff);
    scan = (int)((raw >> 16) & 0xff);
    if (ascii == 27 || ascii == 'q' || ascii == 'Q') return KEY_QUIT;
    if (ascii == ' ') return KEY_FIRE;
    if (scan == 0x48) return KEY_UP;
    if (scan == 0x50) return KEY_DOWN;
    if (scan == 0x4b) return KEY_LEFT;
    if (scan == 0x4d) return KEY_RIGHT;
    return KEY_NONE;
#else
    unsigned char ch = 0;
    fd_set set;
    struct timeval tv;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    if (select(STDIN_FILENO + 1, &set, NULL, NULL, &tv) <= 0) return KEY_NONE;
    if (read(STDIN_FILENO, &ch, 1) != 1) return KEY_NONE;
    if (ch == 27) {
        unsigned char seq[2] = {0, 0};
        if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
            read(STDIN_FILENO, &seq[1], 1) == 1 && seq[0] == '[') {
            if (seq[1] == 'A') return KEY_UP;
            if (seq[1] == 'B') return KEY_DOWN;
            if (seq[1] == 'C') return KEY_RIGHT;
            if (seq[1] == 'D') return KEY_LEFT;
        }
        return KEY_QUIT;
    }
    if (ch == 'q' || ch == 'Q') return KEY_QUIT;
    if (ch == ' ') return KEY_FIRE;
    if (ch == 'w' || ch == 'W') return KEY_UP;
    if (ch == 's' || ch == 'S') return KEY_DOWN;
    if (ch == 'a' || ch == 'A') return KEY_LEFT;
    if (ch == 'd' || ch == 'D') return KEY_RIGHT;
    return KEY_NONE;
#endif
}

static int clamp_int(int value, int low, int high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

static int safe_ticks_for_level(int level) {
    return 95 - level * 5;
}

static int warn_ticks_for_level(int level) {
    return 24 - level;
}

static int watch_ticks_for_level(int level) {
    return 42 + level * 2;
}

static int bulldog_x_for_level(int level) {
    static const int positions[10] = {
        16, 11, 21, 7, 25, 14, 19, 5, 27, 16
    };
    return positions[(level - 1) % 10];
}

static void add_obstacle(Game *game, int y, int x, int dx, int min_x, int max_x) {
    Obstacle *obstacle;
    if (game->obstacle_count >= MAX_OBSTACLES) return;
    obstacle = &game->obstacles[game->obstacle_count++];
    obstacle->y = y;
    obstacle->x = x;
    obstacle->dx = dx;
    obstacle->min_x = min_x;
    obstacle->max_x = max_x;
}

static void set_message(Game *game, const char *message, int ticks) {
    strncpy(game->message, message, sizeof(game->message) - 1);
    game->message[sizeof(game->message) - 1] = '\0';
    game->message_ticks = ticks;
}

static void load_level(Game *game, int level) {
    int lives = game->lives > 0 ? game->lives : START_LIVES;
    memset(game, 0, sizeof(*game));
    game->mode = MODE_PLAY;
    game->level = level;
    game->fox_x = FIELD_W / 2;
    game->fox_y = FIELD_H - 2;
    game->bulldog_x = bulldog_x_for_level(level);
    game->lives = lives;
    game->timer_ticks = (45 - level * 2) * FPS;
    game->phase = PHASE_BACK;
    game->phase_limit = safe_ticks_for_level(level);

    if (level >= 2) add_obstacle(game, 12, 4, 1, 2, FIELD_W - 3);
    if (level >= 3) add_obstacle(game, 8, FIELD_W - 5, -1, 2, FIELD_W - 3);
    if (level >= 4) add_obstacle(game, 5, 8, 1, 4, FIELD_W - 8);
    if (level >= 5) add_obstacle(game, 14, 20, -1, 3, FIELD_W - 4);
    if (level >= 7) add_obstacle(game, 10, 14, 1, 8, FIELD_W - 6);
    if (level >= 9) add_obstacle(game, 6, 22, -1, 5, FIELD_W - 5);

    set_message(game, "Touch Bulldog. Move only while his back is turned.", FPS * 3);
}

static const char *phase_text(Phase phase) {
    if (phase == PHASE_BACK) return "BACK TURNED";
    if (phase == PHASE_WARN) return "WARNING";
    return "WATCHING";
}

static char bulldog_char(Phase phase) {
    if (phase == PHASE_WATCH) return 'D';
    if (phase == PHASE_WARN) return '!';
    return 'B';
}

static Style bulldog_style(Phase phase) {
    if (phase == PHASE_WATCH) return STYLE_BULLDOG_WATCH;
    if (phase == PHASE_WARN) return STYLE_BULLDOG_WARN;
    return STYLE_BULLDOG_SAFE;
}

static int obstacle_index_at(const Game *game, int x, int y) {
    int i;
    for (i = 0; i < game->obstacle_count; ++i) {
        if (game->obstacles[i].x == x && game->obstacles[i].y == y) return i;
    }
    return -1;
}

static int obstacle_at(const Game *game, int x, int y) {
    return obstacle_index_at(game, x, y) >= 0;
}

static void draw_cell(char cell, Style style) {
    backend_style(style);
    putchar(cell);
    backend_style(STYLE_RESET);
}

static char field_tile(int x, int y) {
    if (y == 1) return '=';
    if ((x + y) % 7 == 0) return ',';
    if ((x * 3 + y) % 11 == 0) return '.';
    return ' ';
}

static void draw_lives(int lives) {
    int i;
    for (i = 0; i < START_LIVES; ++i) {
        putchar(i < lives ? '*' : '.');
    }
}

static void draw_hud_line(const Game *game, int row, int seconds) {
    backend_style(STYLE_MESSAGE);
    if (row == 0) {
        printf("  LEVEL %02d", game->level);
    } else if (row == 2) {
        printf("  LIVES ");
        draw_lives(game->lives);
    } else if (row == 4) {
        printf("  TIME  %02d", seconds);
    } else if (row == 6) {
        printf("  DOG");
    } else if (row == 7) {
        printf("  %-11s", phase_text(game->phase));
    } else if (row == 10) {
        printf("  TARGET");
    } else if (row == 11) {
        printf("  column %02d", game->bulldog_x + 1);
    } else if (row == 14 && game->mode == MODE_GAME_OVER) {
        backend_style(STYLE_BULLDOG_WATCH);
        printf("  GAME OVER");
    } else if (row == 14 && game->mode == MODE_WIN) {
        backend_style(STYLE_FOX);
        printf("  YOU WIN");
    } else if (row == 15 && game->mode != MODE_PLAY) {
        printf("  Space restart");
    } else if (row == 16 && game->mode != MODE_PLAY) {
        printf("  Q quit");
    }
    backend_style(STYLE_RESET);
}

static void draw(const Game *game) {
    int x;
    int y;
    int seconds = (game->timer_ticks + FPS - 1) / FPS;

    backend_goto(0, 0);
    backend_style(STYLE_HEADER);
    printf(" BULLDOG  FOX VS THE WATCHFUL DOG                         ");
    backend_style(STYLE_RESET);
    printf("        \n");
    backend_style(STYLE_BORDER);
    printf("+");
    for (x = 0; x < FIELD_W; ++x) putchar('-');
    printf("+");
    backend_style(STYLE_RESET);
    printf("\n");

    for (y = 0; y < FIELD_H; ++y) {
        draw_cell('|', STYLE_BORDER);
        for (x = 0; x < FIELD_W; ++x) {
            int obstacle_index = obstacle_index_at(game, x, y);
            if (y == 0 && x == game->bulldog_x) {
                draw_cell(bulldog_char(game->phase), bulldog_style(game->phase));
            } else if (game->fox_x == x && game->fox_y == y) {
                draw_cell('F', STYLE_FOX);
            } else if (obstacle_index >= 0) {
                static const char obstacle_chars[] = {'o', '@', '*', 'O'};
                draw_cell(obstacle_chars[obstacle_index % 4], STYLE_OBSTACLE);
            } else if (y == 0) {
                draw_cell('^', STYLE_FINISH);
            } else if (y == 1) {
                draw_cell('=', STYLE_FINISH);
            } else {
                draw_cell(field_tile(x, y), STYLE_FIELD);
            }
        }
        draw_cell('|', STYLE_BORDER);
        draw_hud_line(game, y, seconds);
        printf("\n");
    }

    backend_style(STYLE_BORDER);
    printf("+");
    for (x = 0; x < FIELD_W; ++x) putchar('-');
    printf("+");
    backend_style(STYLE_RESET);
    printf("\n");
    backend_style(STYLE_MESSAGE);
    if (game->mode == MODE_PLAY) {
        printf("Arrows/WASD move  Space dash  Q quit                         \n");
        printf("%-68s", game->message_ticks > 0 ? game->message : "");
    } else {
        printf("Space restart  Q quit                                         \n");
        printf("%-68s", game->message_ticks > 0 ? game->message : "");
    }
    backend_style(STYLE_RESET);
    printf("\n");
    fflush(stdout);
}

static void advance_phase(Game *game) {
    game->phase_ticks++;
    if (game->phase_ticks < game->phase_limit) return;

    game->phase_ticks = 0;
    if (game->phase == PHASE_BACK) {
        game->phase = PHASE_WARN;
        game->phase_limit = warn_ticks_for_level(game->level);
        set_message(game, "Bulldog is turning. Get ready to freeze.", FPS);
    } else if (game->phase == PHASE_WARN) {
        game->phase = PHASE_WATCH;
        game->phase_limit = watch_ticks_for_level(game->level);
        set_message(game, "Freeze.", FPS);
    } else {
        game->phase = PHASE_BACK;
        game->phase_limit = safe_ticks_for_level(game->level);
        set_message(game, "Go.", FPS);
    }
}

static void advance_obstacles(Game *game) {
    int i;
    if ((game->timer_ticks % 3) != 0) return;
    for (i = 0; i < game->obstacle_count; ++i) {
        Obstacle *obstacle = &game->obstacles[i];
        obstacle->x += obstacle->dx;
        if (obstacle->x <= obstacle->min_x || obstacle->x >= obstacle->max_x) {
            obstacle->dx = -obstacle->dx;
            obstacle->x += obstacle->dx;
        }
    }
}

static void restart_after_miss(Game *game, const char *reason) {
    int lives = game->lives - 1;
    if (lives <= 0) {
        game->mode = MODE_GAME_OVER;
        game->lives = 0;
        game->timer_ticks = 0;
        set_message(game, "Game over. Press Space to restart.", FPS * 999);
        return;
    }
    game->fox_x = FIELD_W / 2;
    game->fox_y = FIELD_H - 2;
    game->lives = lives;
    game->timer_ticks -= FPS * 4;
    if (game->timer_ticks < 1) game->timer_ticks = 1;
    set_message(game, reason, FPS * 2);
}

static void apply_input(Game *game, Key key) {
    int next_x = game->fox_x;
    int next_y = game->fox_y;
    int moved = 0;
    int step = key == KEY_FIRE ? 2 : 1;

    if (key == KEY_UP || key == KEY_FIRE) {
        next_y -= step;
        moved = 1;
    } else if (key == KEY_DOWN) {
        next_y += 1;
        moved = 1;
    } else if (key == KEY_LEFT) {
        next_x -= 1;
        moved = 1;
    } else if (key == KEY_RIGHT) {
        next_x += 1;
        moved = 1;
    }

    if (!moved) return;

    if (game->phase == PHASE_WATCH) {
        restart_after_miss(game, "Caught moving. Lost a life.");
        return;
    }

    next_x = clamp_int(next_x, 0, FIELD_W - 1);
    if (next_y < 1 && next_x != game->bulldog_x) next_y = 1;
    game->fox_x = next_x;
    game->fox_y = clamp_int(next_y, 0, FIELD_H - 1);
}

static int fox_hit_obstacle(const Game *game) {
    return obstacle_at(game, game->fox_x, game->fox_y);
}

static int tick_game(Game *game) {
    Key key = backend_read_key();
    if (key == KEY_QUIT) return 0;

    if (game->mode != MODE_PLAY) {
        if (key == KEY_FIRE) {
            game->lives = START_LIVES;
            load_level(game, 1);
        }
        return 1;
    }

    apply_input(game, key);
    advance_obstacles(game);
    if (fox_hit_obstacle(game)) {
        restart_after_miss(game, "Bumped by a toy. Lost a life.");
    }

    if (game->fox_y == 0 && game->fox_x == game->bulldog_x) {
        if (game->level >= 10) {
            game->mode = MODE_WIN;
            set_message(game, "You won all 10 Bulldog levels. Press Space.", FPS * 999);
            return 1;
        }
        load_level(game, game->level + 1);
        set_message(game, "Level clear.", FPS * 2);
    }

    game->timer_ticks--;
    if (game->timer_ticks <= 0) {
        restart_after_miss(game, "Time up. Lost a life.");
    }

    if (game->message_ticks > 0) game->message_ticks--;
    advance_phase(game);
    return 1;
}

int main(void) {
    Game game;
    int running = 1;

    backend_init();
    load_level(&game, 1);

    while (running) {
        draw(&game);
        running = tick_game(&game);
        backend_sleep_frame();
    }

    backend_shutdown();
    return 0;
}
