#!/bin/sh
set -eu

repo=${1:-"$HOME/github/bulldogs-food"}
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
package="$repo/packaging/deckst"

for file in Bulldog.st BulldogsFood.bmp bulldog.game.json; do
    if [ ! -f "$package/$file" ]; then
        echo "missing Bulldog's Food package file: $package/$file" >&2
        exit 1
    fi
done

install -Dm644 "$package/Bulldog.st" "$root/assets/floppies/Bulldog.st"
install -Dm644 "$package/BulldogsFood.bmp" "$root/assets/artwork/BulldogsFood.bmp"
install -Dm644 "$package/bulldog.game.json" "$root/assets/profiles/bulldog.game.json"
python3 "$root/tools/generate-checksum-index.py" \
    "$root/assets" "$root/checksums/sha256.json"
python3 "$root/tools/verify-starter-pack.py" --strict
