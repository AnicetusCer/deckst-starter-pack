# DeckST Starter Pack

This repository contains optional, public-domain / redistributable starter assets used by DeckST
when the user opts in via the DeckST Starter Settings flow.

## Scope

- **Safe redistributable content only**
- Community-redistributable binaries for first-run convenience
- No source code for DeckST
- No proprietary / user-licensed ROMs or commercial game media

## Layout

- `LICENSE.md`
- `ATTRIBUTION.md`
- `LICENSES/`
- `games/bulldog/DESIGN.md`
- `assets/tos/`
  - `EmuTOS.rom`
- `assets/floppies/`
  - `Bulldog.st`
- `assets/profiles/`
  - `bulldog.game.json`
- `assets/config/`
  - `firmware.json`
- `checksums/sha256.json`
- `metadata/sources.json`
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
- `assets/floppies/Bulldog.st`
- `assets/profiles/bulldog.game.json`
- `assets/config/firmware.json`

The planned original starter game is `Bulldog`, documented in
`games/bulldog/DESIGN.md`. It is intended to become a cleanly redistributable
single-player Atari ST game made specifically for DeckST.

Keep hashes in sync using:

```bash
python3 tools/generate-checksum-index.py assets checksums/sha256.json
```

Validate a release package before tagging:

```bash
python3 tools/verify-starter-pack.py
```

For stricter verification (fail if there are extra files under `assets/`):

```bash
python3 tools/verify-starter-pack.py --strict
```

If you use custom paths:

```bash
python3 tools/verify-starter-pack.py \
  --assets-root /path/to/assets \
  --checksums /path/to/checksums/sha256.json
```

## Attribution policy

This repository carries its own license and attribution records:

- `LICENSE.md` explains the aggregate repository license policy.
- `ATTRIBUTION.md` credits each upstream project and source location.
- `metadata/sources.json` provides machine-readable source and license metadata.
- `LICENSES/` contains bundled license texts required by included assets.

The main `deckst` repository also tracks matching launcher-facing metadata in
`public/starter-bundle/manifest.json`.
