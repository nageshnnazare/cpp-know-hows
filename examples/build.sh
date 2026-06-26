#!/usr/bin/env bash
# Build (and optionally run) the cpp_mastery examples.
#
#   ./build.sh             build every *.cpp into ./bin/
#   ./build.sh 06_move     build & run a single example (prefix match)
#   ./build.sh --asan 03   build with sanitizers, then run
#
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-clang++}"
STD="-std=c++20"
WARN="-Wall -Wextra -Wpedantic -Wshadow"
SAN=""
if [[ "${1:-}" == "--asan" ]]; then
    SAN="-fsanitize=address,undefined -g"
    shift
fi

mkdir -p bin

build_one() {
    local src="$1"
    local out="bin/$(basename "${src%.cpp}")"
    echo ">> building $src"
    # Threading examples need -pthread on some platforms.
    $CXX $STD $WARN $SAN -pthread "$src" -o "$out"
    echo "$out"
}

if [[ $# -eq 0 ]]; then
    for f in *.cpp; do build_one "$f" >/dev/null; done
    echo "All examples built into ./bin/"
else
    # find the example whose name starts with the given prefix
    match=$(ls *.cpp | grep -E "^${1}" | head -n1 || true)
    if [[ -z "$match" ]]; then
        echo "No example matching '$1'. Available:"; ls *.cpp; exit 1
    fi
    out=$(build_one "$match")
    echo "----- running $out -----"
    "./$out"
fi
