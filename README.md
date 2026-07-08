# DeckST Starter Pack

This repository contains optional, public-domain / redistributable starter assets used by DeckST
when the user opts in via the DeckST Starter Settings flow.

## Scope

- **Safe redistributable content only**
- Community-redistributable binaries for first-run convenience
- No source code for DeckST
- No proprietary / user-licensed ROMs or commercial game media

## Layout

- `assets/tos/`
  - `EmuTOS.rom`
- `assets/floppies/`
  - `Ballerburg.st`
- `checksums/sha256.json`
- `tools/generate-checksum-index.py`
- `metadata/release-notes.md`

## Quick setup (user side)

DeckST expects this repository to be discoverable via `DECKST_PUBLIC_BUNDLE_ASSETS_PATH`.

```bash
# after cloning this repo as ~/DeckST/starter-pack
export DECKST_PUBLIC_BUNDLE_ASSETS_PATH="$HOME/DeckST/starter-pack"
export DECKST_PUBLIC_TOS_PATH="$DECKST_PUBLIC_BUNDLE_ASSETS_PATH/assets/tos"
export DECKST_PUBLIC_FLOPPY_PATH="$DECKST_PUBLIC_BUNDLE_ASSETS_PATH/assets/floppies"
export DECKST_TOS_PATH="$DECKST_PUBLIC_TOS_PATH:$HOME/DeckST/tos"
export DECKST_FLOPPY_PATH="$DECKST_PUBLIC_FLOPPY_PATH:$HOME/DeckST/floppies"
```

DeckST will install from `public/starter-bundle/manifest.json` in the main
`deckst` repo and copy these files into the user library on demand.

## Expected files

Populate the files listed below under `assets/` before tagging a release:

- `assets/tos/EmuTOS.rom`
- `assets/floppies/Ballerburg.st`

Keep hashes in sync using:

```bash
python3 tools/generate-checksum-index.py assets checksums/sha256.json
```

## Attribution policy

This repository tracks public-safe sources and licenses used for all bundled files.
The authoritative metadata is currently maintained in
`deckst/public/starter-bundle/manifest.json`.
