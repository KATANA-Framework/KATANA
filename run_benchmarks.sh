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
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_BENCHMARKS=ON -DENABLE_EXAMPLES=ON ..

echo -e "${GREEN}[2/3] Building benchmarks...${NC}"
cmake --build . --target \
  simple_benchmark \
  performance_benchmark \
  mpsc_benchmark \
  timer_benchmark \
  headers_benchmark \
  io_buffer_benchmark \
  router_benchmark \
  openapi_benchmark \
  generated_api_benchmark \
  hello_world_server \
  compute_api \
  validation_api \
  -j$(nproc)

mkdir -p "${RESULTS_DIR}"

echo -e "${GREEN}[3/3] Running all benchmarks and generating report...${NC}"
echo ""

cd "${SCRIPT_DIR}"

if [ -f "generate_benchmark_report.py" ]; then
    HELLO_PORT=${HELLO_PORT:-18080} python3 generate_benchmark_report.py

    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}All benchmarks completed!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo -e "Comprehensive report: ${YELLOW}BENCHMARK_RESULTS.md${NC}"
else
    echo -e "${RED}Error: generate_benchmark_report.py not found${NC}"
    exit 1
fi
