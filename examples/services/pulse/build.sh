#!/usr/bin/env bash
# Build the Pulse service: regenerate both contracts (namespaced) + their SQL, then compile
# main.cpp against the prebuilt libkatana_core.a. Mirrors the standalone build used by the
# other examples (the `examples` CMake preset is unrelated).
#
#   KATANA_ROOT=/path/to/KATANA ./build.sh
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="${KATANA_ROOT:-$(cd "$HERE/../../.." && pwd)}"
GEN="${KATANA_GEN:-$ROOT/build/debug/katana_gen}"
LIB="${KATANA_LIB:-$ROOT/build/debug/libkatana_core.a}"
OUT="${1:-/tmp/pulse}"

[ -x "$GEN" ] || { echo "katana_gen not found at $GEN (build it first)"; exit 1; }
[ -f "$LIB" ] || { echo "libkatana_core.a not found at $LIB (build it first)"; exit 1; }

echo "==> regenerating contracts"
rm -rf "$HERE/generated"
mkdir -p "$HERE/generated/catalog" "$HERE/generated/analytics"
"$GEN" openapi -i "$HERE/catalog.yaml"   -o "$HERE/generated/catalog"   --emit all --inline-naming operation --namespace catalog   >/dev/null
"$GEN" openapi -i "$HERE/analytics.yaml" -o "$HERE/generated/analytics" --emit all --inline-naming operation --namespace analytics >/dev/null
"$GEN" sql -i "$HERE/sql/catalog"   -o "$HERE/generated/catalog"   --namespace catalog   >/dev/null
"$GEN" sql -i "$HERE/sql/analytics" -o "$HERE/generated/analytics" --namespace analytics >/dev/null

echo "==> compiling pulse"
g++ -O2 -std=c++23 -DKATANA_USE_EPOLL -DKATANA_HAS_LIBPQ \
    -I "$ROOT/katana/core/include" -I "$HERE" -I/usr/include/postgresql \
    "$HERE/main.cpp" "$LIB" -lpq -pthread -o "$OUT"
echo "==> built $OUT"
