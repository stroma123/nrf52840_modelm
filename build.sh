#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BOARD="${1:-}"
SHIELD="${2:-}"

if [ -z "$BOARD" ]; then
    echo "usage: build.sh <board> [shield] [extra cmake args...]" >&2
    echo "example : build.sh nice_nano_v2 model_x" >&2
    echo "example : build.sh modelm '' -DSNIPPET=studio-rpc-usb-uart -DCONFIG_ZMK_STUDIO=y" >&2
    exit 1
fi

FILENAME="${SHIELD:-$BOARD}"

CMAKE_ARGS=(-DZMK_CONFIG="/zmk-config" -DZMK_EXTRA_MODULES="/boards")
if [ -n "$SHIELD" ]; then
    CMAKE_ARGS+=(-DSHIELD="$SHIELD")
fi
shift
[ $# -gt 0 ] && shift
CMAKE_ARGS+=("$@")

echo "BOARD=$BOARD"
echo "SHIELD=$SHIELD"
echo "FILENAME=$FILENAME"
echo "BUILD command: west build /zmk/app -p -b $BOARD -- ${CMAKE_ARGS[*]}"

# --tty breaks non-interactive callers (CI, pipes)
TTY_ARGS=()
if [ -t 0 ]; then
    TTY_ARGS+=(--interactive --tty)
fi

# clean up a container left over from an interrupted run
docker rm -f zmk-modelm >/dev/null 2>&1 || true

docker run --rm --name zmk-modelm "${TTY_ARGS[@]}" --workdir /zmk \
    --volume "$SCRIPT_DIR/config:/zmk-config" \
    --volume "$SCRIPT_DIR/zmk:/zmk" \
    --volume "$SCRIPT_DIR:/boards" \
    --user "$(id -u):$(id -g)" \
    zmkfirmware/zmk-dev-arm:3.5 \
    west build /zmk/app -p -b "$BOARD" -- "${CMAKE_ARGS[@]}"

# /zmk is a bind mount, so the artifact is already on the host
cp "$SCRIPT_DIR/zmk/build/zephyr/zmk.uf2" "$SCRIPT_DIR/${FILENAME}.uf2"
echo "OK: ${FILENAME}.uf2"
