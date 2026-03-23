#!/usr/bin/env bash

set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
  scripts/run_canonical_pipeline.sh --preset <name> [options]

Options:
  --build-target <target>   Build a specific target instead of the preset default.
  --cmake-arg <arg>         Extra argument passed to `cmake --preset`.
  --ctest-arg <arg>         Extra argument passed to `ctest --preset`.
  --no-tests                Configure and build only.
  --help                    Show this help.

Environment:
  CC / CXX                  Optional compiler override for configure.
  KATANA_POLL               Optional reactor backend override.
  KATANA_JOBS               Parallelism for build/test. Defaults to `nproc`.
  CMAKE_C_COMPILER_LAUNCHER / CMAKE_CXX_COMPILER_LAUNCHER
                            Optional launcher override (for example `ccache`).
EOF
}

preset=""
build_target=""
run_tests=1
cmake_extra=()
ctest_extra=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        --preset)
            preset="${2:-}"
            shift 2
            ;;
        --build-target)
            build_target="${2:-}"
            shift 2
            ;;
        --cmake-arg)
            cmake_extra+=("${2:-}")
            shift 2
            ;;
        --ctest-arg)
            ctest_extra+=("${2:-}")
            shift 2
            ;;
        --no-tests)
            run_tests=0
            shift
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage >&2
            exit 1
            ;;
    esac
done

if [[ -z "$preset" ]]; then
    echo "--preset is required" >&2
    usage >&2
    exit 1
fi

jobs="${KATANA_JOBS:-$(nproc)}"
build_dir="build/${preset}"
current_source_dir="$(pwd -P)"

cache_file="${build_dir}/CMakeCache.txt"
if [[ -f "$cache_file" ]]; then
    cached_source_dir="$(
        sed -n 's/^CMAKE_HOME_DIRECTORY:INTERNAL=//p' "$cache_file" | head -n 1
    )"
    if [[ -n "$cached_source_dir" ]]; then
        cached_source_dir="$(realpath -m "$cached_source_dir")"
        if [[ "$cached_source_dir" != "$current_source_dir" ]]; then
            echo "==> Removing stale preset build dir: ${build_dir}"
            echo "    cached source: ${cached_source_dir}"
            echo "    current source: ${current_source_dir}"
            rm -rf "$build_dir"
        fi
    fi
fi

cmake_args=(--preset "$preset")

if [[ -n "${CC:-}" ]]; then
    cmake_args+=("-DCMAKE_C_COMPILER=${CC}")
fi

if [[ -n "${CXX:-}" ]]; then
    cmake_args+=("-DCMAKE_CXX_COMPILER=${CXX}")
fi

if [[ -n "${KATANA_POLL:-}" ]]; then
    cmake_args+=("-DKATANA_POLL=${KATANA_POLL}")
fi

if [[ -n "${CMAKE_C_COMPILER_LAUNCHER:-}" ]]; then
    cmake_args+=("-DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}")
fi

if [[ -n "${CMAKE_CXX_COMPILER_LAUNCHER:-}" ]]; then
    cmake_args+=("-DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}")
fi

clang_compiler="${CXX:-${CC:-}}"
if [[ "$clang_compiler" == *clang* ]]; then
    for candidate in clang-scan-deps-18 clang-scan-deps; do
        if command -v "$candidate" >/dev/null 2>&1; then
            scan_deps="$(command -v "$candidate")"
            cmake_args+=("-DCMAKE_C_COMPILER_CLANG_SCAN_DEPS=${scan_deps}")
            cmake_args+=("-DCMAKE_CXX_COMPILER_CLANG_SCAN_DEPS=${scan_deps}")
            break
        fi
    done
fi

cmake_args+=("${cmake_extra[@]}")

echo "==> Configure preset: ${preset}"
cmake "${cmake_args[@]}"

build_args=(--build --preset "$preset" "-j${jobs}")
if [[ -n "$build_target" ]]; then
    build_args+=(--target "$build_target")
fi

echo "==> Build preset: ${preset}"
cmake "${build_args[@]}"

if [[ "$run_tests" -eq 1 ]]; then
    echo "==> Test preset: ${preset}"
    ctest --preset "$preset" --output-on-failure "${ctest_extra[@]}"
fi
