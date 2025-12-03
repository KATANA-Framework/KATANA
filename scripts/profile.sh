#!/bin/bash

set -e

# Profiling script for KATANA benchmarks using perf
# Usage: ./scripts/profile.sh <benchmark_name> [duration_seconds]

BENCHMARK=$1
DURATION=${2:-30}

if [ -z "$BENCHMARK" ]; then
    echo "Usage: $0 <benchmark_name> [duration_seconds]"
    echo ""
    echo "Available benchmarks:"
    echo "  - openapi_benchmark"
    echo "  - router_benchmark"
    echo "  - performance_benchmark"
    echo "  - simple_benchmark"
    echo ""
    exit 1
fi

# Build with profiling-friendly optimizations
echo "[1/4] Building benchmark with profiling flags..."
cmake -B build/profile -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_BENCHMARKS=ON
ninja -C build/profile "$BENCHMARK"

BENCHMARK_PATH="build/profile/benchmark/$BENCHMARK"

if [ ! -f "$BENCHMARK_PATH" ]; then
    echo "Error: Benchmark $BENCHMARK not found at $BENCHMARK_PATH"
    exit 1
fi

# Check if perf is available
if ! command -v perf &> /dev/null; then
    echo "Error: perf is not installed. Install with:"
    echo "  sudo apt install linux-tools-generic linux-tools-$(uname -r)"
    exit 1
fi

# Record performance data
echo "[2/4] Recording performance data for ${DURATION}s..."
sudo perf record -F 999 -g --call-graph dwarf -o perf.data -- "$BENCHMARK_PATH" &
PID=$!
sleep "$DURATION"
kill -SIGINT $PID 2>/dev/null || true
wait $PID 2>/dev/null || true

# Generate report
echo "[3/4] Generating perf report..."
sudo perf report -i perf.data --stdio > perf_report.txt
echo "Report saved to perf_report.txt"

# Generate flamegraph if available
if command -v flamegraph &> /dev/null; then
    echo "[4/4] Generating flamegraph..."
    sudo perf script -i perf.data | flamegraph > flamegraph.svg
    echo "Flamegraph saved to flamegraph.svg"
elif [ -d "/opt/FlameGraph" ] || [ -d "$HOME/FlameGraph" ]; then
    echo "[4/4] Generating flamegraph..."
    FLAMEGRAPH_DIR="/opt/FlameGraph"
    if [ -d "$HOME/FlameGraph" ]; then
        FLAMEGRAPH_DIR="$HOME/FlameGraph"
    fi
    sudo perf script -i perf.data | "$FLAMEGRAPH_DIR/stackcollapse-perf.pl" | "$FLAMEGRAPH_DIR/flamegraph.pl" > flamegraph.svg
    echo "Flamegraph saved to flamegraph.svg"
else
    echo "[4/4] Skipping flamegraph (FlameGraph not found)"
    echo "To install: git clone https://github.com/brendangregg/FlameGraph.git ~/FlameGraph"
fi

echo ""
echo "Profiling complete!"
echo "  - Raw data: perf.data"
echo "  - Report: perf_report.txt"
echo "  - Flamegraph: flamegraph.svg (if available)"
