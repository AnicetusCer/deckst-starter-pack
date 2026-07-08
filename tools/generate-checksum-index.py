#!/usr/bin/env python3
"""Generate SHA-256 checksums for DeckST starter assets."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
from datetime import datetime, timezone


def sha256_for_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def build(root: pathlib.Path, output: pathlib.Path) -> None:
    entries = []
    assets_root = root / "assets"
    for relative in sorted(assets_root.rglob("*")):
        if not relative.is_file():
            continue
        if relative.name == ".keep":
            continue
        path = relative.as_posix().replace("\\", "/")
        entries.append(
            {
                "id": relative.stem,
                "path": path,
                "sha256": sha256_for_file(relative),
                "bytes": relative.stat().st_size,
            }
        )

    payload = {
        "generated_utc": datetime.now(timezone.utc).strftime("%Y-%m-%d"),
        "items": entries,
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate DeckST starter pack checksums.")
    parser.add_argument("assets_root", type=pathlib.Path, help="Directory containing assets/")
    parser.add_argument("output", type=pathlib.Path, help="Path to checksums json")
    args = parser.parse_args()

    build(args.assets_root, args.output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
