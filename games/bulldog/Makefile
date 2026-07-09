HOST_CC ?= cc
ATARI_CC ?= m68k-atari-mint-gcc

BUILD_DIR := build
SRC := src/bulldog.c
FLOPPY_IMAGE := $(BUILD_DIR)/Bulldog.st

.PHONY: all host atari floppy clean

all: host

host: $(BUILD_DIR)/bulldog

atari: $(BUILD_DIR)/BULLDOG.PRG

floppy: $(FLOPPY_IMAGE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/bulldog: $(SRC) | $(BUILD_DIR)
	$(HOST_CC) -std=c99 -Wall -Wextra -pedantic -O2 -o $@ $<

$(BUILD_DIR)/BULLDOG.PRG: $(SRC) | $(BUILD_DIR)
	$(ATARI_CC) -std=c99 -Wall -Wextra -Os -o $@ $<

$(FLOPPY_IMAGE): $(BUILD_DIR)/BULLDOG.PRG | $(BUILD_DIR)
	dd if=/dev/zero of=$@ bs=1024 count=720
	mformat -i $@ -f 720 ::
	mmd -i $@ ::AUTO
	mcopy -i $@ $(BUILD_DIR)/BULLDOG.PRG ::AUTO/BULLDOG.PRG

clean:
	rm -rf $(BUILD_DIR)
