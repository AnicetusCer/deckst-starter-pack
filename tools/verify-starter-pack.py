#!/usr/bin/env python3
"""Validate DeckST starter-pack files against checksums/sha256.json."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
from typing import Any

PLACEHOLDER = "REPLACE_WITH_HASH"
HASH_RE = re.compile(r"^[0-9a-fA-F]{64}$")


def sha256_for_file(path: pathlib.Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def load_manifest(checksums_path: pathlib.Path) -> dict[str, Any]:
    payload = json.loads(checksums_path.read_text(encoding="utf-8"))
    items = payload.get("items")
    if not isinstance(items, list):
        raise ValueError("checksum manifest must contain an 'items' array")
    return payload


def manifest_relative_path(raw_path: str) -> pathlib.Path:
    normalized = raw_path.replace("\\", "/").lstrip("/")
    if normalized.startswith("assets/"):
        normalized = normalized[len("assets/") :]
    return pathlib.Path(normalized)


def all_asset_files(assets_root: pathlib.Path) -> set[pathlib.Path]:
    files = set()
    if not assets_root.is_dir():
        return files
    for entry in sorted(assets_root.rglob("*")):
        if not entry.is_file():
            continue
        if entry.name == ".keep":
            continue
        files.add(entry.relative_to(assets_root))
    return files


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Validate starter-pack assets against checksums/sha256.json."
    )
    parser.add_argument(
        "--assets-root",
        type=pathlib.Path,
        default=pathlib.Path("assets"),
        help="Root directory containing assets (default: assets)",
    )
    parser.add_argument(
        "--checksums",
        type=pathlib.Path,
        default=pathlib.Path("checksums/sha256.json"),
        help="Checksum manifest path (default: checksums/sha256.json)",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Fail if assets contain files that are not listed in checksums.",
    )
    args = parser.parse_args()

    try:
        manifest = load_manifest(args.checksums)
    except FileNotFoundError:
        print(f"ERROR: checksum manifest not found: {args.checksums}")
        return 2
    except (json.JSONDecodeError, ValueError) as error:
        print(f"ERROR: invalid checksum manifest: {error}")
        return 2

    errors: list[str] = []
    warnings: list[str] = []
    expected_files: set[pathlib.Path] = set()

    if not manifest.get("items"):
        warnings.append("manifest contains no items")

    for item in manifest.get("items", []):
        if not isinstance(item, dict):
            errors.append("manifest item is not an object")
            continue

        raw_path = str(item.get("path", "")).strip()
        expected_hash = item.get("sha256")
        expected_bytes = item.get("bytes")

        if not raw_path:
            errors.append("manifest item missing path")
            continue
        if expected_hash is None:
            errors.append(f"{raw_path}: missing sha256")
            continue
        if not isinstance(expected_hash, str):
            errors.append(f"{raw_path}: sha256 is not a string")
            continue
        if expected_hash == PLACEHOLDER:
            errors.append(f"{raw_path}: placeholder checksum still set")
            continue
        if not HASH_RE.match(expected_hash):
            errors.append(f"{raw_path}: invalid sha256 value '{expected_hash}'")
            continue
        if expected_bytes is not None and not isinstance(expected_bytes, int):
            errors.append(f"{raw_path}: bytes is not an integer")
            continue

        rel_path = manifest_relative_path(raw_path)
        if rel_path.parts and rel_path.parts[0] == "assets":
            rel_path = pathlib.Path(*rel_path.parts[1:])
        path = args.assets_root / rel_path
        expected_files.add(rel_path)

        if not path.exists():
            errors.append(f"{raw_path}: missing file at {path}")
            continue
        actual_hash = sha256_for_file(path)
        if actual_hash.lower() != expected_hash.lower():
            errors.append(
                f"{raw_path}: hash mismatch (expected {expected_hash}, got {actual_hash})"
            )
        if isinstance(expected_bytes, int) and path.stat().st_size != expected_bytes:
            errors.append(
                f"{raw_path}: size mismatch (expected {expected_bytes}, got {path.stat().st_size})"
            )

    if args.strict:
        existing = all_asset_files(args.assets_root)
        extra = sorted(existing - expected_files)
        if extra:
            errors.append(
                "strict check failed: unlisted files under assets: "
                + ", ".join(str(path) for path in extra)
            )

    for warning in warnings:
        print(f"WARNING: {warning}")

    if errors:
        print("Starter-pack verification FAILED")
        for message in errors:
            print(f"  - {message}")
        return 1

    print("Starter-pack verification passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
