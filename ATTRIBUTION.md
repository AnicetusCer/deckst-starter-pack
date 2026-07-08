# Attribution

DeckST is grateful to the public and open-source Atari ST community projects
that make a first-run starter experience possible.

## EmuTOS

- File: `assets/tos/EmuTOS.rom`
- Project: EmuTOS
- Source: https://emutos.sourceforge.io/
- License: GPL-2.0-only
- Local license copy: `LICENSES/GPL-2.0-only.txt`
- Credit: EmuTOS Team, community-maintained open-source replacement TOS

If this repository ships an EmuTOS binary, release notes should identify the
exact upstream release file and checksum used. Keep corresponding source access
available through the upstream project link above.

## Release Rule

Before tagging a starter-pack release:

- update `checksums/sha256.json`;
- verify the package with `python3 tools/verify-starter-pack.py --strict`;
- update `metadata/release-notes.md` with exact source versions, file names, and
  review date; and
- keep this attribution file in the release.
