#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-../../build}"
RESULTS_DIR="${SCRIPT_DIR}/results"

mkdir -p "${RESULTS_DIR}"

TIMESTAMP=$(date +%Y%m%d_%H%M%S)

echo "==================================="
echo "KATANA Load Testing Suite"
echo "==================================="
echo "Timestamp: ${TIMESTAMP}"
echo "Results will be saved to: ${RESULTS_DIR}"
echo ""

check_tool() {
    if ! command -v "$1" &> /dev/null; then
        echo "Warning: $1 not found. Skipping $1 tests."
        return 1
    fi
    return 0
}

run_wrk() {
    if ! check_tool wrk; then
        return
    fi

    echo "Running wrk benchmarks..."

    wrk -t4 -c100 -d10s --latency http://localhost:8080/ > "${RESULTS_DIR}/wrk_baseline_${TIMESTAMP}.txt"
    echo "  ✓ Baseline test completed"

    wrk -t4 -c100 -d10s -s "${SCRIPT_DIR}/scripts/post.lua" http://localhost:8080/ > "${RESULTS_DIR}/wrk_post_${TIMESTAMP}.txt"
    echo "  ✓ POST test completed"

    echo ""
}

run_bombardier() {
    if ! check_tool bombardier; then
        return
    fi

    echo "Running bombardier benchmarks..."

    bombardier -c 125 -d 10s --print r --format json http://localhost:8080/ > "${RESULTS_DIR}/bombardier_baseline_${TIMESTAMP}.json"
    echo "  ✓ Baseline test completed"

    bombardier -c 125 -d 10s -m POST -H "Content-Type: application/json" -b '{"test":"data"}' http://localhost:8080/ > "${RESULTS_DIR}/bombardier_post_${TIMESTAMP}.json"
    echo "  ✓ POST test completed"

    echo ""
}

run_hey() {
    if ! check_tool hey; then
        return
    fi

    echo "Running hey benchmarks..."

    hey -n 50000 -c 200 http://localhost:8080/ > "${RESULTS_DIR}/hey_baseline_${TIMESTAMP}.txt"
    echo "  ✓ Baseline test completed"

    hey -n 10000 -c 100 -m POST -d '{"test":"data"}' http://localhost:8080/ > "${RESULTS_DIR}/hey_post_${TIMESTAMP}.txt"
    echo "  ✓ POST test completed"

    echo ""
}

echo "Make sure the server is running before proceeding!"
echo "Start it with: ${BUILD_DIR}/hello_world_server"
echo ""
read -p "Press Enter to continue..."

run_wrk
run_bombardier
run_hey

echo "==================================="
echo "All benchmarks completed!"
echo "Results saved to: ${RESULTS_DIR}"
echo "==================================="
echo ""
echo "Summary files:"
ls -lh "${RESULTS_DIR}"/*${TIMESTAMP}*
