#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="/opt/katana/build"

echo "==> Running unit tests (ctest)..."
ctest --test-dir "${BUILD_DIR}" --output-on-failure -j"$(nproc)"

echo "==> Running benchmarks and generating BENCHMARK_RESULTS.md..."
python3 /opt/katana/generate_benchmark_report.py

# Copy report to a mounted workspace if present
if [ -d /workspace ] && [ -w /workspace ]; then
  cp /opt/katana/BENCHMARK_RESULTS.md /workspace/BENCHMARK_RESULTS.md 2>/dev/null || true
fi

echo "==> Done. Report at /opt/katana/BENCHMARK_RESULTS.md"
