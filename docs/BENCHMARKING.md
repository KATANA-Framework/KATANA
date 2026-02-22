# KATANA Benchmarking System

Comprehensive benchmarking framework for the KATANA server framework. This document describes the automated benchmark system created for issue #5.

## Overview

The KATANA benchmarking system provides:

- **Unified Automation**: Single command to run all benchmarks
- **Comprehensive Coverage**: Tests core runtime, codegen quality, routing, serialization, and integration
- **Edge Case Testing**: Boundary conditions, max/min values, null handling, concurrent access
- **Detailed Reporting**: JSON and Markdown reports with metrics, trends, and regression detection
- **CI Integration**: Can be used in continuous integration pipelines
- **Pre-commit Hooks**: Automatically update benchmark results before commits

## Quick Start

### Running All Benchmarks

```bash
# Build benchmarks
cmake --preset bench
cmake --build --preset bench -j$(nproc)

# Build examples (for integration tests)
cmake --preset examples
cmake --build --preset examples --target task_api

# Run comprehensive benchmark suite
python3 scripts/run_all_benchmarks.py --verbose
```

### Using the Original Script

```bash
# Run using the original unified script
./run_benchmarks.sh
```

## Benchmark Categories

### 1. Core Runtime Benchmarks

Tests fundamental framework components:

- **Reactor Performance**: Event loop dispatch, task scheduling
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

- **Parse Performance**: Generated JSON parsers
- **Serialization**: Generated serializers
- **Validation**: Generated validators with all OpenAPI constraints
- **Type Safety**: Compile-time type checking

**Executables:**
- `build/bench/benchmark/codegen_quality_benchmark`
- `build/bench/benchmark/generated_json_benchmark`
- `build/bench/benchmark/generated_api_benchmark`

### 3. Router Benchmarks

Tests HTTP routing performance:

- **Route Matching**: Static and dynamic routes
- **Parameter Extraction**: Path, query, header parameters
- **404/405 Handling**: Not found and method not allowed
- **Middleware Chain**: Multiple middleware execution

**Executables:**
- `build/bench/benchmark/router_benchmark`

### 4. Serialization Benchmarks

Tests JSON handling:

- **Parsing**: JSON to DTO conversion
- **Serialization**: DTO to JSON conversion
- **Escaping**: String escaping and unescaping
- **Validation**: Schema validation during parsing

**Executables:**
- `build/bench/benchmark/serialize_benchmark`
- `build/bench/benchmark/json_benchmark`
- `build/bench/benchmark/json_parsing_benchmark`

### 5. Integration Benchmarks

End-to-end tests with complete services:

- **Task API**: Full CRUD service with complex validation
- **Compute API**: CPU-intensive operations
- **Validation API**: Complex validation rules

**Services:**
- `build/examples/examples/codegen/task_api/task_api`
- `build/examples/examples/codegen/compute_api/compute_api`
- `build/examples/examples/codegen/validation_api/validation_api`

## Edge Case Testing

The benchmark system tests comprehensive edge cases:

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

The Task Management API (`examples/codegen/task_api`) demonstrates comprehensive testing:

```bash
# Start the service
./build/examples/examples/codegen/task_api/task_api 18081

# Run edge case tests
python3 scripts/run_all_benchmarks.py --build-dir build/bench --verbose
```

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
- **Performance Metrics**: Throughput, latency (p50/p99), and benchmark errors
- **Stability Metrics**: Mean, stddev, min/max, and percentiles across repeated runs
- **Optional Raw Runs**: `--include-runs` adds per-run data to JSON for deep analysis

Recommended command:

```bash
./scripts/run_benchmarks.py --repeats 5 --aggregation median --update-docs --output benchmark_results
```

### Report Structure

```markdown
# KATANA Benchmark Results

> Last updated: 2026-02-09 12:00:00
> Commit: abc1234

## Summary

> **Note**: Results shown use median-of-N aggregation across 5 run(s) per stage.

## Core Runtime Benchmarks

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| HTTP Parser (Complete Request) | 1.2M ops/sec | 0.754 us | 1.450 us |
| Arena Allocations (64B objects) | 6.1M ops/sec | - | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| Ring Buffer Queue (Concurrent 4x4) | 19.2M | 0.4M | 2.1% | 18.8M | 19.3M | 19.6M | 19.8M |

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
          python3 scripts/run_all_benchmarks.py \
            --build-dir build/bench \
            --output benchmark_results/ci-report.json \
            --fail-on-error

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

### Integration (Task API)

- **Simple Create**: <1ms end-to-end
- **Batch Create (100)**: <10ms end-to-end
- **Search (complex filters)**: <2ms end-to-end
- **Edge Case Tests**: All pass with correct status codes

## Regression Detection

The system can detect performance regressions:

```python
# Compare with baseline
python3 scripts/run_all_benchmarks.py \
  --build-dir build/bench \
  --baseline benchmark_results/baseline-report.json \
  --threshold 10  # Fail if >10% slower
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
./build/examples/examples/codegen/task_api/task_api 18081 &
PID=$!

# Test with curl
curl -X POST http://localhost:18081/tasks \
  -H "Content-Type: application/json" \
  -d '{"title":"Test","priority":5}'

# Load test with wrk (if installed)
wrk -t4 -c100 -d30s --latency http://localhost:18081/health

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
  "repeats": 5,
  "aggregation_mode": "median",
  "stages": [
    {
      "stage_id": 1,
      "stage_name": "Core Runtime Benchmarks",
      "duration_ms": 3542,
      "success": true,
      "error_message": null,
      "run_count": 5,
      "aggregation_mode": "median",
      "benchmarks": [
        {
          "name": "HTTP Parser (Complete Request)",
          "throughput": 1180000.0,
          "latency_p50_us": 0.754,
          "latency_p99_us": 1.45
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
      "benchmark_runs": {}
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
nice -n -20 python3 scripts/run_benchmarks.py --repeats 7 --aggregation median --update-docs
```

### CI Timeouts

```bash
# Reduce benchmark duration
export BENCH_QUICK=1
python3 scripts/run_all_benchmarks.py --build-dir build/bench
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
   - Update `scripts/run_all_benchmarks.py` if integration coverage changes
   - Add to `CMakeLists.txt`

4. **Document edge cases**:
   - List what boundary conditions are tested
   - Explain expected results

5. **Add to BENCHMARK_RESULTS.md**:
   - Regenerate with `scripts/run_benchmarks.py --update-docs`
   - Verify the new benchmark appears in the generated section

## References

- [Issue #5](https://github.com/VisageDvachevsky/codegen/issues/5) - Original requirements
- `generate_benchmark_report.py` - Original report generator
- `scripts/run_benchmarks.py` - Unified microbenchmark runner and report generator
- `scripts/run_all_benchmarks.py` - Comprehensive automation
- `scripts/pre-commit-benchmarks.sh` - Pre-commit integration

---

**Created for KATANA Framework - Issue #5: Comprehensive Benchmark System**
