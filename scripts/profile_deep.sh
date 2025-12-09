#!/bin/bash
#
# Deep Performance Profiling Script for KATANA Framework
# ======================================================
# Comprehensive profiling using perf with microarchitectural counters
#
# Usage:
#   ./scripts/profile_deep.sh --target <benchmark_name> [options]
#
# Options:
#   --target <name>        Benchmark to profile (required)
#   --mode <mode>          Profiling mode: full|cache|branch|cycles|stalls (default: full)
#   --duration <seconds>   Duration in seconds (default: 30)
#   --output <dir>         Output directory (default: profiling_results/<target>_<timestamp>)
#   --flamegraph           Generate flamegraphs (user, kernel, mixed)
#   --no-sudo              Run without sudo (limited counters)
#   --help                 Show this help message
#
# Available targets:
#   - performance_benchmark    Core performance tests
#   - simple_benchmark         Ring buffer, arena, IO buffer tests
#   - router_benchmark         HTTP router dispatch tests
#   - openapi_benchmark        OpenAPI validation tests
#   - io_buffer_benchmark      IO buffer operations
#   - timer_benchmark          Wheel timer tests
#   - products_api_benchmark   Generated API benchmark
#
# Examples:
#   ./scripts/profile_deep.sh --target simple_benchmark --mode cache --duration 60
#   ./scripts/profile_deep.sh --target performance_benchmark --flamegraph
#   ./scripts/profile_deep.sh --target router_benchmark --mode branch

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
TARGET=""
MODE="full"
DURATION=30
OUTPUT_DIR=""
GENERATE_FLAMEGRAPH=false
USE_SUDO=true
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Print usage
usage() {
    head -35 "$0" | tail -33
    exit 0
}

# Print colored message
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[OK]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --target)
            TARGET="$2"
            shift 2
            ;;
        --mode)
            MODE="$2"
            shift 2
            ;;
        --duration)
            DURATION="$2"
            shift 2
            ;;
        --output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --flamegraph)
            GENERATE_FLAMEGRAPH=true
            shift
            ;;
        --no-sudo)
            USE_SUDO=false
            shift
            ;;
        --help|-h)
            usage
            ;;
        *)
            log_error "Unknown option: $1"
            usage
            ;;
    esac
done

# Validate target
if [ -z "$TARGET" ]; then
    log_error "Target is required. Use --target <benchmark_name>"
    usage
fi

# Create output directory
TIMESTAMP=$(date +%Y%m%dT%H%M%SZ)
if [ -z "$OUTPUT_DIR" ]; then
    OUTPUT_DIR="${PROJECT_ROOT}/profiling_results/${TARGET}_${TIMESTAMP}"
fi
mkdir -p "$OUTPUT_DIR"

log_info "Output directory: $OUTPUT_DIR"

# Check for perf
if ! command -v perf &> /dev/null; then
    log_error "perf is not installed. Install with:"
    echo "  Ubuntu: sudo apt install linux-tools-generic linux-tools-\$(uname -r)"
    exit 1
fi

# Define perf events for different modes
get_perf_events() {
    case "$1" in
        cache)
            # Cache-focused events
            echo "cycles,instructions,cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses,L1-icache-load-misses,LLC-loads,LLC-load-misses,dTLB-loads,dTLB-load-misses,iTLB-loads,iTLB-load-misses"
            ;;
        branch)
            # Branch prediction events
            echo "cycles,instructions,branches,branch-misses,branch-loads,branch-load-misses"
            ;;
        cycles)
            # CPU cycles and instructions
            echo "cycles,instructions,cpu-clock,task-clock,context-switches,cpu-migrations,page-faults"
            ;;
        stalls)
            # Pipeline stalls (may not be available on all CPUs)
            echo "cycles,instructions,stalled-cycles-frontend,stalled-cycles-backend,cache-misses,branch-misses"
            ;;
        full|*)
            # Comprehensive analysis
            echo "cycles,instructions,cache-references,cache-misses,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branches,branch-misses,dTLB-loads,dTLB-load-misses,page-faults"
            ;;
    esac
}

# Build benchmark with profiling flags
build_benchmark() {
    log_info "Building benchmark with profiling-friendly flags..."

    cd "$PROJECT_ROOT"

    # Use RelWithDebInfo for good performance with debug symbols
    cmake -B build/profile \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DENABLE_BENCHMARKS=ON \
        -DCMAKE_CXX_FLAGS="-fno-omit-frame-pointer -g" \
        -G Ninja 2>&1 | tee "$OUTPUT_DIR/cmake_output.log"

    ninja -C build/profile "$TARGET" 2>&1 | tee -a "$OUTPUT_DIR/build_output.log"

    BENCHMARK_PATH="build/profile/benchmark/$TARGET"

    if [ ! -f "$BENCHMARK_PATH" ]; then
        log_error "Benchmark $TARGET not found at $BENCHMARK_PATH"
        exit 1
    fi

    log_success "Benchmark built: $BENCHMARK_PATH"
}

# Run perf stat for hardware counters
run_perf_stat() {
    log_info "Collecting hardware performance counters (mode: $MODE)..."

    local EVENTS
    EVENTS=$(get_perf_events "$MODE")

    local PERF_CMD="perf stat -e $EVENTS --detailed --detailed --detailed -o $OUTPUT_DIR/perf_stat.txt"

    if [ "$USE_SUDO" = true ]; then
        PERF_CMD="sudo $PERF_CMD"
    fi

    log_info "Running: $PERF_CMD -- $BENCHMARK_PATH"

    # Run benchmark with timeout
    timeout "${DURATION}s" $PERF_CMD -- "$BENCHMARK_PATH" 2>&1 || true

    log_success "Hardware counters saved to $OUTPUT_DIR/perf_stat.txt"
}

# Run perf record for call graph sampling
run_perf_record() {
    log_info "Recording call graph samples for ${DURATION}s..."

    local PERF_CMD="perf record -F 999 -g --call-graph dwarf -o $OUTPUT_DIR/perf.data"

    if [ "$USE_SUDO" = true ]; then
        PERF_CMD="sudo $PERF_CMD"
    fi

    # Start benchmark in background
    $PERF_CMD -- "$BENCHMARK_PATH" &
    local PID=$!

    sleep "$DURATION"

    # Stop recording
    kill -SIGINT $PID 2>/dev/null || true
    wait $PID 2>/dev/null || true

    log_success "Call graph data saved to $OUTPUT_DIR/perf.data"

    # Generate text report
    log_info "Generating perf report..."
    if [ "$USE_SUDO" = true ]; then
        sudo perf report -i "$OUTPUT_DIR/perf.data" --stdio > "$OUTPUT_DIR/perf_report.txt"
    else
        perf report -i "$OUTPUT_DIR/perf.data" --stdio > "$OUTPUT_DIR/perf_report.txt"
    fi
    log_success "Report saved to $OUTPUT_DIR/perf_report.txt"
}

# Generate flamegraphs
generate_flamegraphs() {
    if [ "$GENERATE_FLAMEGRAPH" != true ]; then
        return
    fi

    log_info "Generating flamegraphs..."

    # Find FlameGraph scripts
    FLAMEGRAPH_DIR=""
    if [ -d "/opt/FlameGraph" ]; then
        FLAMEGRAPH_DIR="/opt/FlameGraph"
    elif [ -d "$HOME/FlameGraph" ]; then
        FLAMEGRAPH_DIR="$HOME/FlameGraph"
    elif command -v stackcollapse-perf.pl &> /dev/null; then
        FLAMEGRAPH_DIR=""
    else
        log_warn "FlameGraph not found. Install with: git clone https://github.com/brendangregg/FlameGraph.git ~/FlameGraph"
        return
    fi

    # Create flamegraphs directory
    mkdir -p "$OUTPUT_DIR/flamegraphs"

    local PERF_SCRIPT_CMD="perf script -i $OUTPUT_DIR/perf.data"
    if [ "$USE_SUDO" = true ]; then
        PERF_SCRIPT_CMD="sudo $PERF_SCRIPT_CMD"
    fi

    if [ -n "$FLAMEGRAPH_DIR" ]; then
        STACKCOLLAPSE="$FLAMEGRAPH_DIR/stackcollapse-perf.pl"
        FLAMEGRAPH="$FLAMEGRAPH_DIR/flamegraph.pl"
    else
        STACKCOLLAPSE="stackcollapse-perf.pl"
        FLAMEGRAPH="flamegraph.pl"
    fi

    # Generate folded stacks
    $PERF_SCRIPT_CMD | $STACKCOLLAPSE > "$OUTPUT_DIR/perf.folded"

    # User-space flamegraph
    log_info "Generating user-space flamegraph..."
    grep -v '^$' "$OUTPUT_DIR/perf.folded" | \
        $FLAMEGRAPH --title "KATANA $TARGET - User Space" \
        --subtitle "Mode: $MODE, Duration: ${DURATION}s" \
        --colors=java > "$OUTPUT_DIR/flamegraphs/user_flamegraph.svg"

    # Kernel-space flamegraph
    log_info "Generating kernel-space flamegraph..."
    grep -E '^\[kernel\]|_\[k\]' "$OUTPUT_DIR/perf.folded" | \
        $FLAMEGRAPH --title "KATANA $TARGET - Kernel Space" \
        --subtitle "Mode: $MODE, Duration: ${DURATION}s" \
        --colors=green > "$OUTPUT_DIR/flamegraphs/kernel_flamegraph.svg" 2>/dev/null || true

    # Mixed flamegraph (default)
    log_info "Generating mixed flamegraph..."
    $FLAMEGRAPH --title "KATANA $TARGET - Mixed" \
        --subtitle "Mode: $MODE, Duration: ${DURATION}s" \
        < "$OUTPUT_DIR/perf.folded" > "$OUTPUT_DIR/flamegraphs/mixed_flamegraph.svg"

    log_success "Flamegraphs saved to $OUTPUT_DIR/flamegraphs/"
}

# Parse and analyze perf stat output
analyze_results() {
    log_info "Analyzing profiling results..."

    # Create analysis report
    cat > "$OUTPUT_DIR/analysis.md" << 'EOF'
# Deep Performance Analysis Report

EOF

    echo "## Benchmark: $TARGET" >> "$OUTPUT_DIR/analysis.md"
    echo "## Mode: $MODE" >> "$OUTPUT_DIR/analysis.md"
    echo "## Duration: ${DURATION}s" >> "$OUTPUT_DIR/analysis.md"
    echo "## Date: $(date)" >> "$OUTPUT_DIR/analysis.md"
    echo "" >> "$OUTPUT_DIR/analysis.md"

    if [ -f "$OUTPUT_DIR/perf_stat.txt" ]; then
        echo "### Hardware Performance Counters" >> "$OUTPUT_DIR/analysis.md"
        echo "" >> "$OUTPUT_DIR/analysis.md"
        echo '```' >> "$OUTPUT_DIR/analysis.md"
        cat "$OUTPUT_DIR/perf_stat.txt" >> "$OUTPUT_DIR/analysis.md"
        echo '```' >> "$OUTPUT_DIR/analysis.md"
        echo "" >> "$OUTPUT_DIR/analysis.md"

        # Calculate key metrics
        echo "### Key Metrics" >> "$OUTPUT_DIR/analysis.md"
        echo "" >> "$OUTPUT_DIR/analysis.md"

        # Extract metrics using awk
        local INSTRUCTIONS CYCLES CACHE_REFS CACHE_MISSES L1_MISSES BRANCH_MISSES

        INSTRUCTIONS=$(grep -E "^\s+[0-9,]+\s+instructions" "$OUTPUT_DIR/perf_stat.txt" | awk '{print $1}' | tr -d ',')
        CYCLES=$(grep -E "^\s+[0-9,]+\s+cycles" "$OUTPUT_DIR/perf_stat.txt" | awk '{print $1}' | tr -d ',')

        if [ -n "$INSTRUCTIONS" ] && [ -n "$CYCLES" ] && [ "$INSTRUCTIONS" != "0" ]; then
            local IPC
            IPC=$(echo "scale=3; $INSTRUCTIONS / $CYCLES" | bc 2>/dev/null || echo "N/A")
            echo "- **Instructions Per Cycle (IPC):** $IPC" >> "$OUTPUT_DIR/analysis.md"
            echo "  - Theoretical max ~4.0 on modern CPUs" >> "$OUTPUT_DIR/analysis.md"
            echo "  - IPC < 1.0 suggests memory/cache bottlenecks" >> "$OUTPUT_DIR/analysis.md"
            echo "" >> "$OUTPUT_DIR/analysis.md"
        fi

        CACHE_REFS=$(grep -E "^\s+[0-9,]+\s+cache-references" "$OUTPUT_DIR/perf_stat.txt" | awk '{print $1}' | tr -d ',')
        CACHE_MISSES=$(grep -E "^\s+[0-9,]+\s+cache-misses" "$OUTPUT_DIR/perf_stat.txt" | awk '{print $1}' | tr -d ',')

        if [ -n "$CACHE_REFS" ] && [ -n "$CACHE_MISSES" ] && [ "$CACHE_REFS" != "0" ]; then
            local CACHE_MISS_RATE
            CACHE_MISS_RATE=$(echo "scale=2; $CACHE_MISSES * 100 / $CACHE_REFS" | bc 2>/dev/null || echo "N/A")
            echo "- **Cache Miss Rate:** ${CACHE_MISS_RATE}%" >> "$OUTPUT_DIR/analysis.md"
            echo "  - <5% is excellent, 5-10% acceptable, >10% needs attention" >> "$OUTPUT_DIR/analysis.md"
            echo "" >> "$OUTPUT_DIR/analysis.md"
        fi
    fi

    log_success "Analysis saved to $OUTPUT_DIR/analysis.md"
}

# Main execution
main() {
    log_info "=== KATANA Deep Performance Profiling ==="
    log_info "Target: $TARGET"
    log_info "Mode: $MODE"
    log_info "Duration: ${DURATION}s"

    cd "$PROJECT_ROOT"

    build_benchmark
    run_perf_stat
    run_perf_record
    generate_flamegraphs
    analyze_results

    echo ""
    log_success "=== Profiling Complete ==="
    log_info "Results saved to: $OUTPUT_DIR"
    echo ""
    echo "Files generated:"
    ls -la "$OUTPUT_DIR/"

    if [ -d "$OUTPUT_DIR/flamegraphs" ]; then
        echo ""
        echo "Flamegraphs:"
        ls -la "$OUTPUT_DIR/flamegraphs/"
    fi
}

main
