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
clang_compiler="${CXX:-${CC:-}}"
clang_scan_deps=""
clang_ar=""
clang_ranlib=""
clang_nm=""

if [[ "$clang_compiler" == *clang* ]]; then
    for candidate in clang-scan-deps-18 clang-scan-deps; do
        if command -v "$candidate" >/dev/null 2>&1; then
            clang_scan_deps="$(command -v "$candidate")"
            break
        fi
    done

    for candidate in llvm-ar-18 llvm-ar; do
        if command -v "$candidate" >/dev/null 2>&1; then
            clang_ar="$(command -v "$candidate")"
            break
        fi
    done

    for candidate in llvm-ranlib-18 llvm-ranlib; do
        if command -v "$candidate" >/dev/null 2>&1; then
            clang_ranlib="$(command -v "$candidate")"
            break
        fi
    done

    for candidate in llvm-nm-18 llvm-nm; do
        if command -v "$candidate" >/dev/null 2>&1; then
            clang_nm="$(command -v "$candidate")"
            break
        fi
    done
fi

cache_file="${build_dir}/CMakeCache.txt"
if [[ -f "$cache_file" ]]; then
    remove_stale_build=0
    cached_source_dir="$(
        sed -n 's/^CMAKE_HOME_DIRECTORY:INTERNAL=//p' "$cache_file" | head -n 1
    )"
    if [[ -n "$cached_source_dir" ]]; then
        cached_source_dir="$(realpath -m "$cached_source_dir")"
        if [[ "$cached_source_dir" != "$current_source_dir" ]]; then
            echo "==> Removing stale preset build dir: ${build_dir}"
            echo "    reason: source path mismatch"
            echo "    cached source: ${cached_source_dir}"
            echo "    current source: ${current_source_dir}"
            remove_stale_build=1
        fi
    fi

    if [[ "$remove_stale_build" -eq 0 && -n "${CXX:-}" ]]; then
        cached_cxx="$(
            sed -n \
                -e 's/^CMAKE_CXX_COMPILER:FILEPATH=//p' \
                -e 's/^CMAKE_CXX_COMPILER:STRING=//p' \
                "$cache_file" | head -n 1
        )"
        if [[ -n "$cached_cxx" ]]; then
            requested_cxx="$(command -v "${CXX}" 2>/dev/null || true)"
            if [[ -z "$requested_cxx" ]]; then
                requested_cxx="${CXX}"
            fi
            cached_cxx="$(realpath -m "$cached_cxx")"
            requested_cxx="$(realpath -m "$requested_cxx")"
            if [[ "$cached_cxx" != "$requested_cxx" ]]; then
                echo "==> Removing stale preset build dir: ${build_dir}"
                echo "    reason: CXX compiler mismatch"
                echo "    cached CXX: ${cached_cxx}"
                echo "    requested CXX: ${requested_cxx}"
                remove_stale_build=1
            fi
        fi
    fi

    if [[ "$remove_stale_build" -eq 0 && -n "$clang_ar" ]]; then
        cached_ar="$(
            sed -n -e 's/^CMAKE_AR:FILEPATH=//p' "$cache_file" | head -n 1
        )"
        if [[ -n "$cached_ar" ]]; then
            cached_ar="$(realpath -m "$cached_ar")"
            expected_ar="$(realpath -m "$clang_ar")"
            if [[ "$cached_ar" != "$expected_ar" ]]; then
                echo "==> Removing stale preset build dir: ${build_dir}"
                echo "    reason: archive tool mismatch"
                echo "    cached AR: ${cached_ar}"
                echo "    expected AR: ${expected_ar}"
                remove_stale_build=1
            fi
        fi
    fi

    if [[ "$remove_stale_build" -eq 0 && -n "$clang_ranlib" ]]; then
        cached_ranlib="$(
            sed -n -e 's/^CMAKE_RANLIB:FILEPATH=//p' "$cache_file" | head -n 1
        )"
        if [[ -n "$cached_ranlib" ]]; then
            cached_ranlib="$(realpath -m "$cached_ranlib")"
            expected_ranlib="$(realpath -m "$clang_ranlib")"
            if [[ "$cached_ranlib" != "$expected_ranlib" ]]; then
                echo "==> Removing stale preset build dir: ${build_dir}"
                echo "    reason: ranlib tool mismatch"
                echo "    cached RANLIB: ${cached_ranlib}"
                echo "    expected RANLIB: ${expected_ranlib}"
                remove_stale_build=1
            fi
        fi
    fi

    if [[ "$remove_stale_build" -eq 0 && -n "$clang_nm" ]]; then
        cached_nm="$(
            sed -n -e 's/^CMAKE_NM:FILEPATH=//p' "$cache_file" | head -n 1
        )"
        if [[ -n "$cached_nm" ]]; then
            cached_nm="$(realpath -m "$cached_nm")"
            expected_nm="$(realpath -m "$clang_nm")"
            if [[ "$cached_nm" != "$expected_nm" ]]; then
                echo "==> Removing stale preset build dir: ${build_dir}"
                echo "    reason: nm tool mismatch"
                echo "    cached NM: ${cached_nm}"
                echo "    expected NM: ${expected_nm}"
                remove_stale_build=1
            fi
        fi
    fi

    if [[ "$remove_stale_build" -eq 1 ]]; then
        rm -rf "$build_dir"
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

if [[ "$clang_compiler" == *clang* ]]; then
    if [[ -n "$clang_scan_deps" ]]; then
        cmake_args+=("-DCMAKE_C_COMPILER_CLANG_SCAN_DEPS=${clang_scan_deps}")
        cmake_args+=("-DCMAKE_CXX_COMPILER_CLANG_SCAN_DEPS=${clang_scan_deps}")
    fi
    if [[ -n "$clang_ar" ]]; then
        cmake_args+=("-DCMAKE_AR=${clang_ar}")
    fi
    if [[ -n "$clang_ranlib" ]]; then
        cmake_args+=("-DCMAKE_RANLIB=${clang_ranlib}")
    fi
    if [[ -n "$clang_nm" ]]; then
        cmake_args+=("-DCMAKE_NM=${clang_nm}")
    fi
fi

# The project does not currently use C++20 modules. GCC's dependency scanner
# is fragile here and can break canonical CI builds on ordinary header-only
# includes, so keep scanning disabled unless a caller explicitly overrides it.
cmake_args+=("-DCMAKE_CXX_SCAN_FOR_MODULES=OFF")

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
