#!/usr/bin/env bash
#
# Pre-commit hook for benchmark report generation
# This ensures benchmark results are always up-to-date before commits
#
# To enable, add to .git/hooks/pre-commit or use with pre-commit framework
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Pre-commit Benchmark Check${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if benchmarks are built
BENCH_BUILD_DIR="${PROJECT_ROOT}/build/bench"
if [ ! -d "${BENCH_BUILD_DIR}" ]; then
    echo -e "${YELLOW}Benchmarks not built. Run: cmake --preset bench && cmake --build --preset bench${NC}"
    echo -e "${YELLOW}Skipping benchmark report generation.${NC}"
    exit 0
fi

# Check if benchmark executables exist
REQUIRED_BENCHMARKS=(
    "simple_benchmark"
    "performance_benchmark"
    "router_benchmark"
)

MISSING_BENCHMARKS=()
for bench in "${REQUIRED_BENCHMARKS[@]}"; do
    if [ ! -f "${BENCH_BUILD_DIR}/benchmark/${bench}" ]; then
        MISSING_BENCHMARKS+=("${bench}")
    fi
done

if [ ${#MISSING_BENCHMARKS[@]} -gt 0 ]; then
    echo -e "${YELLOW}Some benchmarks not found: ${MISSING_BENCHMARKS[*]}${NC}"
    echo -e "${YELLOW}Skipping benchmark report generation.${NC}"
    exit 0
fi

# Check if benchmark results need updating
BENCHMARK_RESULTS="${PROJECT_ROOT}/BENCHMARK_RESULTS.md"
STAGED_FILES=$(git diff --cached --name-only)

# Determine if we should regenerate benchmarks
SHOULD_REGENERATE=false

# Check if any source files that affect performance are staged
while IFS= read -r file; do
    case "$file" in
        katana/core/src/*.cpp|\
        katana/core/include/*.hpp|\
        benchmark/*.cpp|\
        tools/katana_gen/*.cpp|\
        examples/codegen/*/main.cpp)
            SHOULD_REGENERATE=true
            break
            ;;
    esac
done <<< "$STAGED_FILES"

if [ "$SHOULD_REGENERATE" = false ]; then
    echo -e "${GREEN}No performance-critical files changed. Skipping benchmark regeneration.${NC}"
    exit 0
fi

echo -e "${YELLOW}Performance-critical files detected in commit.${NC}"
echo -e "${BLUE}Regenerating benchmark report...${NC}"
echo ""

# Run benchmark report generation
cd "${PROJECT_ROOT}"

if command -v python3 &> /dev/null; then
    # Use comprehensive benchmark script if available
    if [ -f "${SCRIPT_DIR}/run_all_benchmarks.py" ]; then
        echo -e "${BLUE}Running comprehensive benchmark suite...${NC}"
        python3 "${SCRIPT_DIR}/run_all_benchmarks.py" \
            --build-dir "${BENCH_BUILD_DIR}" \
            --output benchmark_results/pre-commit-report.json \
            --verbose

        echo ""
        echo -e "${GREEN}Benchmark report generated successfully.${NC}"
    else
        # Fallback to simple benchmark run
        echo -e "${BLUE}Running basic benchmarks...${NC}"
        "${BENCH_BUILD_DIR}/benchmark/simple_benchmark" > /tmp/bench_simple.txt 2>&1 || true
        "${BENCH_BUILD_DIR}/benchmark/performance_benchmark" > /tmp/bench_perf.txt 2>&1 || true
        "${BENCH_BUILD_DIR}/benchmark/router_benchmark" > /tmp/bench_router.txt 2>&1 || true

        echo -e "${GREEN}Basic benchmarks completed.${NC}"
    fi
else
    echo -e "${YELLOW}Python3 not found. Skipping benchmark regeneration.${NC}"
    exit 0
fi

# Check if BENCHMARK_RESULTS.md was modified
if [ -f "${BENCHMARK_RESULTS}" ]; then
    if git diff --quiet "${BENCHMARK_RESULTS}"; then
        echo -e "${GREEN}BENCHMARK_RESULTS.md is up to date.${NC}"
    else
        echo ""
        echo -e "${YELLOW}========================================${NC}"
        echo -e "${YELLOW}BENCHMARK_RESULTS.md has been updated!${NC}"
        echo -e "${YELLOW}========================================${NC}"
        echo ""
        echo -e "The benchmark report has been regenerated."
        echo -e "Review the changes and add them to your commit if appropriate:"
        echo ""
        echo -e "  ${GREEN}git add BENCHMARK_RESULTS.md${NC}"
        echo ""
        echo -e "Or skip if the changes are not relevant:"
        echo ""
        echo -e "  ${YELLOW}git commit --no-verify${NC}"
        echo ""

        # Ask user what to do
        read -p "Add BENCHMARK_RESULTS.md to this commit? [y/N] " -n 1 -r
        echo ""
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            git add "${BENCHMARK_RESULTS}"
            echo -e "${GREEN}BENCHMARK_RESULTS.md added to commit.${NC}"
        else
            echo -e "${YELLOW}BENCHMARK_RESULTS.md not added. Remember to commit it separately if needed.${NC}"
        fi
    fi
fi

echo ""
echo -e "${GREEN}Pre-commit benchmark check completed.${NC}"
exit 0
