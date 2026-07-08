# Bulldog First Playable

`Bulldog` is an original single-player Atari ST starter game for DeckST.

This first playable uses a simple terminal-style display so the gameplay loop
can be tested before sprite art and disk packaging are added.

## Controls

- Arrow keys: move the fox
- Space: dash while safe
- Q or Escape: quit

The fox may move while the bulldog's back is turned. Stop when the bulldog is
watching. Touch the bulldog before the timer runs out and avoid the moving toys.
Getting caught, hit, or timed out costs one life. Lose all lives and the level
restarts.

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

## License

The Bulldog source and documentation in this folder are licensed under the MIT
License. Future original Bulldog art/audio assets should be CC0 or CC BY 4.0
and documented before release packaging.
