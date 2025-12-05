#!/usr/bin/env bash
set -euo pipefail

TARGET="${TARGET:-performance_benchmark}"
ITERATIONS="${ITERATIONS:-2000000}"
KATANA_POLL="${KATANA_POLL:-epoll}"
PERCENT_LIMIT="${PERCENT_LIMIT:-0}"
PERF_FREQ="${PERF_FREQ:-999}"
PERF_MAX_STACK="${PERF_MAX_STACK:-1024}"
# Report arguments (defaults to detailed view similar to manual full report)
PERF_REPORT_ARGS="${PERF_REPORT_ARGS:---children --percent-limit ${PERCENT_LIMIT} --max-stack ${PERF_MAX_STACK} -g graph,0.5,caller}"
# Script arguments (control stack depth)
PERF_SCRIPT_ARGS="${PERF_SCRIPT_ARGS:---max-stack ${PERF_MAX_STACK}}"
HOST_PERF_DEFAULT="/workspace/WSL2-Linux-Kernel/tools/perf/perf"
# Allow overriding the host perf path (e.g. HOST_PERF=~/src/WSL2-Linux-Kernel/tools/perf/perf).
HOST_PERF="${HOST_PERF:-${HOST_PERF_DEFAULT}}"
# Expand leading tilde so HOST_PERF can use ~.
HOST_PERF="${HOST_PERF/#\~/${HOME}}"

if [ -z "${PERF_BIN:-}" ] && [ -x "${HOST_PERF}" ]; then
    PERF_BIN="${HOST_PERF}"
elif [ -z "${PERF_BIN:-}" ]; then
    PERF_BIN="perf"
fi

ROOT="/workspace"
BUILD_DIR="${ROOT}/.docker_build/profile"
BIN_PATH="${BUILD_DIR}/benchmark/${TARGET}"

if [ ! -d "${ROOT}" ]; then
    echo "Expected repo mounted at /workspace"
    exit 1
fi

mkdir -p "${BUILD_DIR}"

# Ensure perf exists; prefer host-provided perf if available.
if ! command -v "${PERF_BIN}" >/dev/null 2>&1; then
    echo "perf not found, attempting to install linux-tools for $(uname -r)..."
    apt-get update -y >/dev/null 2>&1 || true
    apt-get install -y "linux-tools-$(uname -r)" "linux-cloud-tools-$(uname -r)" >/dev/null 2>&1 || true
fi

if ! command -v "${PERF_BIN}" >/dev/null 2>&1 && [ -x "${HOST_PERF}" ]; then
    PERF_BIN="${HOST_PERF}"
fi

if ! command -v "${PERF_BIN}" >/dev/null 2>&1; then
    echo "perf still not available; aborting."
    exit 1
fi

echo "==> Configuring (${KATANA_POLL})..."
cmake -S "${ROOT}" -B "${BUILD_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DENABLE_BENCHMARKS=ON \
    -DENABLE_EXAMPLES=ON \
    -DENABLE_TESTING=OFF \
    -DKATANA_POLL="${KATANA_POLL}"

echo "==> Building ${TARGET}..."
cmake --build "${BUILD_DIR}" --target "${TARGET}" -j"$(nproc)"

if [ ! -x "${BIN_PATH}" ]; then
    echo "Binary not found at ${BIN_PATH}"
    exit 1
fi

TS="$(date -u +%Y%m%dT%H%M%SZ)"
OUT_DIR="${ROOT}/profiling_results/${TARGET}_${TS}"
mkdir -p "${OUT_DIR}"

PERF_DATA="${OUT_DIR}/perf.data"
PERF_REPORT="${OUT_DIR}/perf_report.txt"
PERF_SCRIPT="${OUT_DIR}/perf.script"
PERF_FOLDED="${OUT_DIR}/perf.folded"
FLAME_SVG="${OUT_DIR}/flamegraph.svg"

echo "==> Recording perf to ${PERF_DATA}..."
if ! "${PERF_BIN}" record -F "${PERF_FREQ}" --call-graph dwarf -g -o "${PERF_DATA}" \
    "${BIN_PATH}" --iterations "${ITERATIONS}"; then
    echo "perf record failed. If using host perf, ensure dependent libs (e.g., libunwind) are installed in the container."
    exit 1
fi

echo "==> Generating perf report..."
# shellcheck disable=SC2086
"${PERF_BIN}" report --stdio --force ${PERF_REPORT_ARGS} -i "${PERF_DATA}" > "${PERF_REPORT}"

echo "==> Dumping perf script..."
"${PERF_BIN}" script -i "${PERF_DATA}" --force ${PERF_SCRIPT_ARGS} > "${PERF_SCRIPT}"

if command -v stackcollapse-perf.pl >/dev/null 2>&1 && command -v flamegraph.pl >/dev/null 2>&1; then
    echo "==> Building flamegraph..."
    "${PERF_BIN}" script -i "${PERF_DATA}" --force ${PERF_SCRIPT_ARGS} | stackcollapse-perf.pl > "${PERF_FOLDED}"
    flamegraph.pl "${PERF_FOLDED}" > "${FLAME_SVG}"
else
    echo "Flamegraph tools not found; skipping flamegraph."
fi

echo "==> Artifacts in ${OUT_DIR}"
