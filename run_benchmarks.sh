#!/usr/bin/env bash

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
RESULTS_DIR="${SCRIPT_DIR}/benchmark_results"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}   KATANA Benchmark Runner${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

if [ ! -d "${BUILD_DIR}" ]; then
    echo -e "${YELLOW}Build directory not found. Creating...${NC}"
    mkdir -p "${BUILD_DIR}"
fi

cd "${BUILD_DIR}"

echo -e "${GREEN}[1/3] Configuring build system...${NC}"
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_BENCHMARKS=ON ..

echo -e "${GREEN}[2/3] Building benchmarks...${NC}"
cmake --build . --target simple_benchmark performance_benchmark mpsc_benchmark timer_benchmark headers_benchmark io_buffer_benchmark -j$(nproc)

mkdir -p "${RESULTS_DIR}"

echo -e "${GREEN}[3/3] Running benchmarks...${NC}"
echo ""

BENCHMARKS=(
    "performance_benchmark"
    "mpsc_benchmark"
    "timer_benchmark"
    "headers_benchmark"
    "io_buffer_benchmark"
)

TIMESTAMP=$(date +%Y%m%d_%H%M%S)
SUMMARY_FILE="${RESULTS_DIR}/summary_${TIMESTAMP}.txt"

echo "KATANA Framework - Benchmark Results" > "${SUMMARY_FILE}"
echo "Generated: $(date '+%Y-%m-%d %H:%M:%S')" >> "${SUMMARY_FILE}"
echo "" >> "${SUMMARY_FILE}"

for bench in "${BENCHMARKS[@]}"; do
    if [ -f "./benchmark/${bench}" ]; then
        echo -e "${BLUE}Running ${bench}...${NC}"
        OUTPUT_FILE="${RESULTS_DIR}/${bench}_${TIMESTAMP}.txt"

        ./benchmark/${bench} | tee "${OUTPUT_FILE}"

        echo "" >> "${SUMMARY_FILE}"
        echo "=== ${bench} ===" >> "${SUMMARY_FILE}"
        grep -E "(Operations:|Throughput:|Latency)" "${OUTPUT_FILE}" >> "${SUMMARY_FILE}" || true
        echo ""
    else
        echo -e "${RED}Warning: ${bench} not found, skipping...${NC}"
    fi
done

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}All benchmarks completed!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "Results saved to: ${YELLOW}${RESULTS_DIR}${NC}"
echo -e "Summary: ${YELLOW}${SUMMARY_FILE}${NC}"
echo ""

if [ -f "./benchmark/simple_benchmark" ]; then
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}HTTP Server Benchmark Available${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
    echo -e "To run the HTTP server benchmark:"
    echo -e "  1. Start the server: ${YELLOW}./build/examples/hello_world_server${NC}"
    echo -e "  2. In another terminal: ${YELLOW}./build/benchmark/simple_benchmark${NC}"
    echo ""
fi
