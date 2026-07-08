HOST_CC ?= cc
ATARI_CC ?= m68k-atari-mint-gcc

BUILD_DIR := build
SRC := src/bulldog.c

.PHONY: all host atari clean

all: host

host: $(BUILD_DIR)/bulldog

atari: $(BUILD_DIR)/BULLDOG.PRG

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/bulldog: $(SRC) | $(BUILD_DIR)
	$(HOST_CC) -std=c99 -Wall -Wextra -pedantic -O2 -o $@ $<

$(BUILD_DIR)/BULLDOG.PRG: $(SRC) | $(BUILD_DIR)
	$(ATARI_CC) -std=c99 -Wall -Wextra -Os -o $@ $<

clean:
	rm -rf $(BUILD_DIR)
