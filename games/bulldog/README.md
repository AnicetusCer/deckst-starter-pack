# Bulldog First Playable

`Bulldog` is an original single-player Atari ST starter game for DeckST.

This first playable uses colored terminal-style graphics on the host build and
plain ASCII-compatible drawing for the Atari path. That lets the gameplay loop
stay testable before bitmap sprite art and disk packaging are added.

## Controls

- Arrow keys: move the fox
- Space: dash while safe
- Q or Escape: quit

The fox may move while the bulldog's back is turned. Stop when the bulldog is
watching. Touch the bulldog before the timer runs out and avoid the moving toys.
Getting caught, hit, or timed out costs one life. Lose all lives and the level
shows a game-over screen. Press Space to restart.

The right-side HUD shows the level, lives, timer, bulldog state, and target
column.

## Build On Fedora For Development

```bash
make host
./build/bulldog
```

## Build For Atari ST

Install an Atari target compiler such as `m68k-atari-mint-gcc`, then run:

```bash
make atari
```

The intended output is:

```text
build/BULLDOG.PRG
```

## Package A Floppy Image

After `BULLDOG.PRG` exists, package it into a 720 KiB Atari-compatible floppy
image with:

```bash
make floppy
```

The intended output is:

```text
build/Bulldog.st
```

This step uses `mformat` and `mcopy` from `mtools`.

## Test With EmuTOS And Hatari

Once Hatari and EmuTOS are available:

```bash
hatari --tos /path/to/EmuTOS.rom --disk-a build/Bulldog.st
```

With the Hatari Flatpak:

```bash
flatpak run --command=hatari org.tuxfamily.hatari \
  --tos /path/to/EmuTOS.rom \
  --disk-a build/Bulldog.st
```

DeckST will eventually use the same model: EmuTOS as the TOS ROM and
`Bulldog.st` as the game floppy.

## License

The Bulldog source and documentation in this folder are licensed under the MIT
License. Future original Bulldog art/audio assets should be CC0 or CC BY 4.0
and documented before release packaging.
