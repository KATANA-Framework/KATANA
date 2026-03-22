# KATANA Benchmarking System

Comprehensive benchmarking framework for the KATANA server framework. This document describes the automated benchmark system and reporting workflow.

## Overview

The KATANA benchmarking system provides:

- **Unified Automation**: Single command to run all benchmarks
- **Comprehensive Coverage**: Tests core runtime, codegen quality, routing, serialization, and integration
- **Profile-Based Workloads**: `best case` / `typical mixed` / `hard edge` scenarios per operation category, plus canonical and peak `wrk` HTTP profiles
- **Seeded Dataset Generation**: Fixed-seed workload synthesis with configurable scenario proportions
- **Detailed Reporting**: JSON and Markdown reports with metrics, trends, and regression detection
- **CI Integration**: Can be used in continuous integration pipelines
- **Pre-commit Hooks**: Automatically update benchmark results before commits

## Directory Roles

- `benchmark/`: benchmark source code (`*_benchmark.cpp`) and benchmark utilities.
- `benchmarks/`: committed baseline snapshots used for regression checks in CI.
- `benchmark_results/`: generated local/CI benchmark artifacts (JSON outputs), not source of truth.
- `BENCHMARK_RESULTS.md`: human-readable current report in repository root.
- `docker/`: optional reproducible benchmark/profiling environments used by `Makefile` docker targets.

## Quick Start

### Running All Benchmarks

```bash
# Build benchmarks
cmake --preset bench
cmake --build --preset bench -j$(nproc)

# Run comprehensive benchmark suite
./scripts/run_benchmarks.py \
  --aggregation median \
  --stage-repeat 1=20 \
  --output benchmark_results \
  --update-docs \
  --include-e2e
```

The maintained source of truth is `scripts/run_benchmarks.py`. Legacy helper scripts (`run_benchmarks.sh`, `benchmark.sh`, `generate_benchmark_report.py`) may still exist for ad hoc experiments, but they are not the benchmark contract.

### Perf Smoke (Quick Local Regression Check)

For a quick (< 3 min) local check against committed baselines:

```bash
make perf-smoke
```

This runs Stage 1 (Core Runtime) and Stage 2 (Codegen Quality) microbenchmarks and compares results with `benchmarks/baseline_stage1.json` / `benchmarks/baseline_stage2.json`. Exit code 1 means regression detected (threshold: 5% locally).

The same check runs automatically in CI via `perf-regression.yml` (threshold: 3%) on PRs that touch `katana/core/` or `tools/katana_gen/`.

**CI benchmark runner**: `tools/katana_bench.py` — lightweight stage 1+2 runner with JSON output and baseline comparison. This is the CI contract runner, distinct from the full `scripts/run_benchmarks.py` which covers stages 1–12.

## Benchmark Categories

### 1. Core Runtime Benchmarks

Tests fundamental framework components:

- **Reactor Performance**: Event loop dispatch, task scheduling
- **Ring Buffer Queue**: Single-thread, core-saturation, and oversubscribed contention scenarios
- **Contention Diagnostics**: Producer/consumer retries and `retries/op` under load
- **HTTP Parser**: Request parsing throughput and latency
- **Arena Allocator**: Per-request memory management
- **IO Buffer**: Zero-copy buffer operations
- **MPSC Queue**: Multi-producer single-consumer lock-free queue
- **Timer**: Wheel timer for timeouts and delays

**Executables:**
- `build/bench/benchmark/simple_benchmark`
- `build/bench/benchmark/performance_benchmark`
- `build/bench/benchmark/mpsc_benchmark`
- `build/bench/benchmark/timer_benchmark`
- `build/bench/benchmark/io_buffer_benchmark`

### 2. Code Generation Quality Benchmarks

Tests the `katana_gen` tool output quality:

- **Parse Performance**: Generated JSON parsers with profile datasets (`best` / `typical` / `hard`)
- **Serialization Helpers**: String escaping and object construction hot paths
- **Validation**: Generated validators with realistic and adversarial inputs
- **Type Safety**: Compile-time type checking

**Executables:**
- `build/bench/benchmark/codegen_quality_benchmark`
- `build/bench/benchmark/generated_json_benchmark`
- `build/bench/benchmark/generated_api_benchmark`
- `build/bench/benchmark/benchmark_api_codegen_benchmark`
- `build/bench/benchmark/benchmark_api_framework_benchmark`

### 3. Router Benchmarks

Tests HTTP routing performance:

- **Route Matching**: Static and dynamic routes
- **Parameter Extraction**: Path, query, header parameters
- **404/405 Handling**: Not found and method not allowed
- **Middleware Chain**: Multiple middleware execution
- **Validation Errors Field**: `Errors` counts expectation mismatches (e.g. non-404 in a not-found scenario)

**Executables:**
- `build/bench/benchmark/router_benchmark`

### 4. Serialization Benchmarks

Tests JSON handling:

- **Escaping**: String escaping fast path, mixed path, and adversarial heavy-escape path
- **Serialization**: DTO-like object and array serialization
- **Profiled Workloads**: `best case`, `typical mixed`, `hard edge` datasets with fixed seed
- **Data Rate Metrics**: `bytes/sec` for data-dependent workloads

**Executables:**
- `build/bench/benchmark/serialize_benchmark`
- `build/bench/benchmark/json_benchmark`
- `build/bench/benchmark/json_parsing_benchmark`

### 5. Integration Benchmarks

End-to-end tests with complete services:

- **Hello World Server**: Raw framework HTTP path
- **Compute API**: Generated parse/dispatch/business path
- **Benchmark API Framework**: Generated framework path without network
- **Benchmark API Codegen**: Mixed generated API workload

**Services:**
- `build/bench/examples/hello_world_server`
- `build/bench/examples/examples/codegen/compute_api/compute_api`
- `build/bench/benchmark/benchmark_api_framework_benchmark`
- `build/bench/benchmark/benchmark_api_codegen_benchmark`

## Edge Case Testing

The benchmark system tests comprehensive edge cases with deterministic datasets
(`benchmark/bench_utils.hpp`, `build_profile_dataset`, fixed seed + configurable mix):

### Boundary Values

- **Minimum Values**: Shortest strings (minLength=1), smallest integers (minimum=1)
- **Maximum Values**: Longest strings (maxLength=200/2000), max integers (maximum=5)
- **Array Limits**: minItems=1, maxItems=100, uniqueItems validation
- **Numeric Ranges**: priority 1-5, limit 1-100

### Special Cases

- **Empty Input**: Empty strings, empty arrays, zero values
- **Null Handling**: Optional fields, nullable types
- **Unicode**: International characters, emojis, special symbols
- **Large Payloads**: Maximum allowed request/response sizes
- **Concurrent Access**: Multiple simultaneous requests

### Invalid Input

- **Out of Range**: Values exceeding min/max constraints
- **Format Violations**: Invalid email, date-time, UUID formats
- **Type Mismatches**: String where number expected
- **Missing Required**: Required fields omitted
- **Malformed JSON**: Syntax errors, incomplete objects

## Task API Edge Cases

The Task Management API (`examples/codegen/task_api`) remains a useful spec for edge-case generation and integration tests, even though the maintained E2E benchmark contract now focuses on `hello_world_server` and `compute_api`.

### Tested Scenarios

1. **Minimum Title**: `{"title":"A","priority":1}` (minLength=1)
2. **Maximum Title**: 200 characters (maxLength=200)
3. **Maximum Description**: 2000 characters (maxLength=2000)
4. **Maximum Tags**: 20 unique tags (maxItems=20, uniqueItems=true)
5. **Null Optional Fields**: `assignee_id: null`, `due_date: null`
6. **Batch Minimum**: 1 task (minItems=1)
7. **Batch Maximum**: 100 tasks (maxItems=100)
8. **Query Max Limit**: `?limit=100` (maximum=100)
9. **Complex Search**: All filter combinations
10. **Invalid Title Length**: 201 characters (expect 400)
11. **Invalid Priority**: 6 (expect 400, max=5)
12. **Invalid Email**: Malformed email addresses
13. **Invalid Date**: Malformed ISO 8601 dates

## Automated Benchmark Report

The unified runner `scripts/run_benchmarks.py` can automatically generate
`BENCHMARK_RESULTS.md` and JSON reports with:

- **Timestamp and Commit Info**: Generation time and git SHA
- **Configurable Aggregation**: `--aggregation best|median|mean` across repeated runs
- **Performance Metrics**: Throughput (`ops/sec`), data rate (`bytes/sec`), and latency
- **Contention Metrics**: `producer_retries`, `consumer_retries`, and `retries_per_op_total` for queue benchmarks
- **Stability Metrics**: Mean, stddev, min/max, and percentiles across repeated runs
- **Optional Raw Runs**: `--include-runs` adds per-run data to JSON for deep analysis
- **CV-Aware Regression Checks**: `--compare` + `--cv-threshold-pct` distinguish hard vs noisy regressions
- **Optional perf Counters**: `--perf-stat` adds cycles/instructions/IPC/cache/branch/context-switch metrics
- **Optional E2E Stages**: `--include-e2e` runs maintained `wrk` stages for `hello_world_server` and `compute_api`

Recommended command:

```bash
./scripts/run_benchmarks.py --aggregation median --stage-repeat 1=20 --update-docs --output benchmark_results
```

### Report Structure

```markdown
# KATANA Benchmark Results

> Last updated: 2026-02-09 12:00:00
> Commit: abc1234

## Summary

> **Note**: Results shown use median-of-N aggregation with stage-specific repeat policy (e.g. stage 1 uses more runs).

## Core Runtime Benchmarks

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency max | Retries/op | Errors |
|-----------|------------|-------------|-------------|-------------|-------------|------------|--------|
| HTTP Parser (Complete Request) | 1.2M ops/sec | 0.754 us | 1.100 us | 1.450 us | 3.000 us | - | 0 |
| Ring Buffer Queue (Core Saturation 6x6) | 13.0M ops/sec | 0.300 us | - | 4.000 us | - | 2.4 | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| Ring Buffer Queue (Concurrent 4x4) | 19.2M | 0.4M | 2.1% | 18.8M | 19.3M | 19.6M | 19.8M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | p50 | p95 | Max |
|-----------|--------|------|--------|----|-----|-----|-----|
| HTTP Parser (Complete Request) | p99 us | 1.52 | 0.11 | 7.2% | 1.45 | 1.67 | 1.80 |

...
```

## CI Integration

### GitHub Actions

Add to `.github/workflows/benchmark.yml`:

```yaml
name: Benchmarks

on: [push, pull_request]

jobs:
  benchmark:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Install Dependencies
        run: sudo apt-get update && sudo apt-get install -y cmake ninja-build

      - name: Build Benchmarks
        run: |
          cmake --preset bench
          cmake --build --preset bench -j$(nproc)

      - name: Build Examples
        run: |
          cmake --preset examples
          cmake --build --preset examples -j$(nproc)

      - name: Run Benchmarks
        run: |
          ./scripts/run_benchmarks.py \
            --aggregation median \
            --stage-repeat 1=20 \
            --output benchmark_results \
            --update-docs

      - name: Upload Results
        uses: actions/upload-artifact@v3
        with:
          name: benchmark-results
          path: benchmark_results/
```

### Pre-commit Hook

Enable automatic benchmark updates:

```bash
# Install pre-commit hook
ln -s ../../scripts/pre-commit-benchmarks.sh .git/hooks/pre-commit

# Or use pre-commit framework
pip install pre-commit
pre-commit install
```

The hook will:
1. Detect if performance-critical files changed
2. Regenerate benchmark report if needed
3. Prompt to add `BENCHMARK_RESULTS.md` to commit
4. Allow skipping with `--no-verify` if needed

## Performance Baselines

### Core Runtime

- **HTTP Parser**: >1M req/sec, p99 <2us
- **Arena Allocator**: >5M allocs/sec
- **Router Dispatch**: >2M req/sec, p99 <1us
- **MPSC Queue**: >4M ops/sec (concurrent 4x4)

### Code Generation

- **Parse int64**: <10ns per operation
- **Parse double**: <15ns per operation
- **JSON Escaping**: <15ns for clean strings
- **Object Parse (3 fields)**: <50ns
- **Object Parse (8 fields)**: <80ns

### Integration (Current Runner)

- **Hello canonical**: use committed `BENCHMARK_RESULTS.md` or benchmark artifacts as source of truth
- **Compute canonical**: use committed `BENCHMARK_RESULTS.md` or benchmark artifacts as source of truth
- **Peak profiles**: tracked separately from canonical low-latency operating points
- **Edge Case Tests**: integration tests stay green alongside benchmark runs

## Regression Detection

The system can detect performance regressions:

```bash
python3 scripts/run_benchmarks.py \
  --compare benchmark_results/baseline_micro.json \
  --regression-threshold-pct 5 \
  --cv-threshold-pct 10 \
  --fail-on-regression
```

## Manual Benchmark Runs

### Individual Benchmarks

```bash
# Run specific benchmark
./build/bench/benchmark/performance_benchmark

# With custom parameters
./build/bench/benchmark/router_benchmark --iterations 1000000
```

### Custom Integration Tests

```bash
# Start service
./build/bench/examples/examples/codegen/compute_api/compute_api &
PID=$!

# Test with curl
curl -X POST http://localhost:8080/compute/sum \
  -H "Content-Type: application/json" \
  -d '[1,2,3,4,5,6,7,8]'

# Load test with wrk (if installed)
KATANA_PIPELINE_DEPTH=10 wrk -t4 -c512 -d10s --latency -s test/load/scripts/compute_sum_pipeline.lua http://localhost:8080/

# Cleanup
kill $PID
```

## Benchmark Output Formats

### JSON Report

```json
{
  "generated_at": "2026-02-22 09:30:00",
  "commit_sha": "abc1234",
  "total_duration_ms": 12345,
  "repeats": 10,
  "aggregation_mode": "median",
  "stage_repeats": {
    "1": 20,
    "2": 10
  },
  "comparison_summary": null,
  "stages": [
    {
      "stage_id": 1,
      "stage_name": "Core Runtime Benchmarks",
      "duration_ms": 3542,
      "success": true,
      "error_message": null,
      "run_count": 20,
      "aggregation_mode": "median",
      "benchmarks": [
        {
          "name": "HTTP Parser (Complete Request)",
          "throughput": 1180000.0,
          "latency_p50_us": 0.754,
          "latency_p95_us": 1.1,
          "latency_p99_us": 1.45,
          "latency_max_us": 3.0,
          "errors": 0
        }
      ],
      "benchmark_stats": {
        "HTTP Parser (Complete Request)": {
          "throughput": {
            "count": 5,
            "min": 1160000.0,
            "mean": 1182000.0,
            "stddev": 9200.0,
            "p50": 1180000.0,
            "p95": 1195000.0,
            "max": 1200000.0
          }
        }
      },
      "benchmark_runs": {},
      "perf_stat": {},
      "perf_derived": {},
      "perf_error": null
    }
  ]
}
```

### Markdown Report

See `BENCHMARK_RESULTS.md` for the human-readable format.

## Troubleshooting

### Benchmarks Not Found

```bash
# Rebuild benchmarks
cmake --preset bench
cmake --build --preset bench -j$(nproc)
```

### Server Won't Start

```bash
# Check port availability
lsof -i :18081

# Kill existing process
pkill -f task_api
```

### Inconsistent Results

```bash
# Reduce system noise
sudo sh -c 'echo 1 > /proc/sys/kernel/perf_event_paranoid'

# Run with higher priority
nice -n -20 python3 scripts/run_benchmarks.py --aggregation median --stage-repeat 1=20 --update-docs
```

### CI Timeouts

```bash
# Reduce benchmark duration and repeats
python3 scripts/run_benchmarks.py --stage 1 2 --repeats 3
```

## Contributing

When adding new benchmarks:

1. **Place in appropriate directory**:
   - Core benchmarks: `benchmark/`
   - Integration: `examples/codegen/*/`

2. **Follow naming convention**:
   - `*_benchmark.cpp` for binaries
   - `*_api` for services

3. **Add to automation**:
   - Update `scripts/run_benchmarks.py` stage definitions and parser
   - Add to `CMakeLists.txt`

4. **Document edge cases**:
   - List what boundary conditions are tested
   - Explain expected results

5. **Add to BENCHMARK_RESULTS.md**:
   - Regenerate with `scripts/run_benchmarks.py --update-docs`
   - Verify the new benchmark appears in the generated section

## References

- `generate_benchmark_report.py` - Original report generator
- `scripts/run_benchmarks.py` - Unified microbenchmark runner and report generator
- `scripts/pre-commit-benchmarks.sh` - Pre-commit integration

---
