# KATANA Benchmarking Methodology

**Version**: 1.0
**Date**: 2025-12-08
**Purpose**: Guidelines for reproducible, meaningful performance measurements

---

## Overview

This document describes KATANA's benchmarking approach, covering microbenchmarks, end-to-end tests, and long-running load tests. The goal is to provide repeatable measurements that accurately reflect real-world performance.

---

## 1. Benchmark Types

### 1.1 Microbenchmarks

**Purpose**: Measure individual component performance in isolation.

**Location**: `benchmark/*_benchmark.cpp`

**Examples**:
- `performance_benchmark.cpp`: Ring buffer, arena, SIMD, HTTP parser
- `router_benchmark.cpp`: Route dispatch, 404/405 handling
- `headers_benchmark.cpp`: HTTP header operations (get, set, compare)
- `io_buffer_benchmark.cpp`: Buffer operations (append, read, scatter/gather)
- `json_benchmark.cpp`: JSON encoding/decoding
- `products_api_benchmark.cpp`: Router + validation combined

**Methodology**:
1. **Warmup phase** (10k-50k operations): Prime caches, stabilize CPU frequency
2. **Measurement phase** (200k+ operations): Collect latency samples
3. **Statistical analysis**: Calculate p50, p99, p999 percentiles
4. **Throughput reporting**: Operations per second

### 1.2 End-to-End Benchmarks

**Purpose**: Measure full HTTP request/response cycle under load.

**Location**: `test/load/`

**Examples**:
- `simple_benchmark`: Basic HTTP server load test
- `products_api_load_test.sh`: Long-running CRUD workload

**Methodology**:
1. **Server startup**: Launch application on dedicated port
2. **Client load generation**: Use `wrk` or custom client
3. **Mixed workload**: Realistic operation distribution (70% GET, 20% POST, etc.)
4. **Duration**: 5-10 minutes minimum for sustained load tests
5. **Metrics**: RPS, latency distribution, error rate, connection churn

### 1.3 Long-Running Stability Tests

**Purpose**: Verify memory stability, arena reuse, no leaks.

**Duration**: 30 minutes to 24 hours

**Key Metrics**:
- Memory growth over time (should be flat after warmup)
- Latency degradation (should remain stable)
- Connection churn handling (frequent open/close)
- Error rate (should remain near zero)

---

## 2. System Configuration

### 2.1 Hardware Recommendations

**CPU**:
- Dedicated physical cores (not hyperthreads)
- Consistent frequency (disable turbo boost for reproducibility)
- Governor set to `performance`: `sudo cpupower frequency-set -g performance`

**Memory**:
- Sufficient RAM to avoid swapping (>= 8 GB for typical tests)
- Disable transparent huge pages: `echo never | sudo tee /sys/kernel/mm/transparent_hugepage/enabled`

**Network**:
- Loopback for microbenchmarks (eliminates network variability)
- Dedicated NICs for distributed tests (separate load generator machine)

### 2.2 Operating System Tuning

**File Descriptor Limits**:
```bash
ulimit -n 1048576
```

**TCP Buffer Sizes**:
```bash
sudo sysctl -w net.core.rmem_max=16777216
sudo sysctl -w net.core.wmem_max=16777216
sudo sysctl -w net.ipv4.tcp_rmem="4096 87380 16777216"
sudo sysctl -w net.ipv4.tcp_wmem="4096 65536 16777216"
```

**Local Port Range**:
```bash
sudo sysctl -w net.ipv4.ip_local_port_range="1024 65535"
```

**Connection Queue**:
```bash
sudo sysctl -w net.core.somaxconn=65535
```

**Disable Transparent Huge Pages**:
```bash
echo never | sudo tee /sys/kernel/mm/transparent_hugepage/enabled
```

### 2.3 Build Configuration

**Release Build** (required for accurate measurements):
```bash
cmake --preset bench
cmake --build --preset bench
```

**Flags automatically applied**:
- `-O3`: Maximum optimizations
- `-DNDEBUG`: Disable assertions
- `-march=native`: CPU-specific optimizations
- `-mtune=native`: CPU-specific tuning
- LTO enabled: Cross-module inlining

**DO NOT benchmark**:
- Debug builds (`-O0 -g`)
- Sanitizer builds (ASan, TSan, UBSan)
- Builds with logging enabled

---

## 3. Running Benchmarks

### 3.1 Microbenchmarks

```bash
# Build all benchmarks
cmake --preset bench
cmake --build --preset bench

# Run individual benchmark
./build/bench/benchmark/performance_benchmark
./build/bench/benchmark/router_benchmark
./build/bench/benchmark/products_api_benchmark
./build/bench/benchmark/json_benchmark

# Run all benchmarks sequentially
cd build/bench/benchmark
for bench in *_benchmark; do
    echo "Running $bench..."
    ./"$bench"
done
```

### 3.2 Docker-Based Benchmarks

**Using Dockerfile.bench** (optimized, reproducible):
```bash
# Build image
docker build -f docker/benchmarks/Dockerfile.bench -t katana-bench:latest .

# Run with system tuning (requires --privileged)
docker run --privileged --rm katana-bench:latest

# Run without tuning (limited but safe)
docker run --rm katana-bench:latest

# Run with CPU pinning (recommended)
docker run --privileged --cpuset-cpus="0-3" --rm katana-bench:latest
```

### 3.3 Long-Running Load Tests

**products_api load test**:
```bash
# Build products_api example
cmake --preset examples
cmake --build --preset examples --target products_api

# Run load test (default: 5 minutes, 64 concurrency)
./test/load/products_api_load_test.sh

# Custom duration and concurrency
TEST_DURATION=600 CONCURRENCY=128 ./test/load/products_api_load_test.sh

# Results saved to profiling_results/load_test_TIMESTAMP/
```

**Phases**:
1. **Phase 1**: Fixed concurrency sustained load (configurable duration)
2. **Phase 2**: Variable concurrency ramp (64 → 128 → 256 connections)
3. **Phase 3**: Connection churn test (10k requests with Connection: close)

---

## 4. Interpreting Results

### 4.1 Microbenchmark Metrics

**Throughput** (operations/second):
- Higher is better
- Compare against baseline or competitors
- Expected: 1M+ ops/s for core primitives (arena, ring buffer)

**Latency Percentiles**:
- **p50** (median): Typical case performance
- **p99**: Captures tail latency (critical for SLAs)
- **p999**: Worst 0.1% of requests (early warning for issues)

**Example Interpretation**:
```
Router Dispatch:
  Throughput: 1.05M ops/s
  p50: 0.511 μs
  p99: 1.152 μs
  p999: 5.578 μs
```

**Analysis**:
- ✅ Sub-microsecond median: Excellent hot-path performance
- ✅ p99 < 2 μs: Predictable, low tail latency
- ⚠️ p999 spike to 5.5 μs: Likely scheduler/cache effects (acceptable if rare)

### 4.2 End-to-End Metrics

**Requests per Second (RPS)**:
- Aggregate throughput under load
- Expected: 100k-350k RPS per reactor thread (single GET)
- Expected: 800k-1.2M RPS aggregate (8 reactors)

**Latency Under Load**:
- Compare p99 at 50%, 80%, 100% load
- Should remain < 5ms at 80% capacity
- Graceful degradation at saturation (not cliff)

**Error Rate**:
- Should be 0% for valid requests
- 4xx/5xx only for intentionally invalid requests

### 4.3 Memory Stability

**Indicators of Healthy Behavior**:
- ✅ Memory usage plateaus after warmup (first 1-2 minutes)
- ✅ No growth over time (arena reuse working)
- ✅ No "leak" or "oom" in server logs

**Red Flags**:
- ❌ Linear memory growth over time (potential leak)
- ❌ Sudden spikes (fragmentation or large requests not handled)
- ❌ OOM crashes (arena blocks not being reused)

---

## 5. Comparing Results

### 5.1 Baseline Establishment

**First run**: Establish baseline on known-good hardware
```bash
# Run benchmark 3 times, take median
./build/bench/benchmark/performance_benchmark > run1.txt
./build/bench/benchmark/performance_benchmark > run2.txt
./build/bench/benchmark/performance_benchmark > run3.txt

# Baseline is median of 3 runs
```

### 5.2 Regression Detection

**Significant regression**: > 5% slowdown in p99 latency or throughput
**Acceptable variance**: ±2-3% run-to-run variation

**Example**:
```
Baseline: Router p99 = 1.15 μs
New:      Router p99 = 1.25 μs
Change:   +8.7% → REGRESSION (investigate)
```

### 5.3 Cross-Platform Comparison

**Normalize by CPU frequency**:
- 3.5 GHz baseline
- 4.0 GHz machine: Expect ~14% faster (+0.5 / 3.5 = 14%)

**Account for architecture**:
- AVX2 vs SSE2: 20-30% speedup for SIMD paths
- Cache sizes: Larger L3 improves throughput at high concurrency

---

## 6. Common Pitfalls

### 6.1 Cold Cache Effects

**Problem**: First few iterations are slower due to cold caches.

**Solution**: Always include warmup phase (10k-50k operations).

### 6.2 Thermal Throttling

**Problem**: CPU frequency drops under sustained load, skewing results.

**Solution**:
- Monitor CPU temperature during long tests
- Disable turbo boost for consistent frequency
- Ensure adequate cooling

### 6.3 Background Processes

**Problem**: Other processes compete for CPU/memory.

**Solution**:
- Close browsers, IDEs, etc.
- Use `top` or `htop` to verify low CPU usage before testing
- Consider dedicated benchmark machine

### 6.4 Insufficient Test Duration

**Problem**: Short tests don't capture tail latency or memory behavior.

**Solution**:
- Microbenchmarks: >= 200k operations
- Load tests: >= 5 minutes
- Stability tests: >= 30 minutes

### 6.5 Load Generator Saturation

**Problem**: Load generator (client) becomes bottleneck, not server.

**Solution**:
- Monitor client CPU usage (should be < 80%)
- Use multiple client threads
- Run client on separate machine for high-throughput tests

---

## 7. Benchmark Suite Coverage

### 7.1 Core Components

| Component | Benchmark | Metrics |
|-----------|-----------|---------|
| Ring Buffer | performance_benchmark | Throughput (single/concurrent) |
| Arena Allocator | performance_benchmark | Allocation rate, p99 latency |
| HTTP Parser | performance_benchmark | Parse rate, fragment handling |
| Router Dispatch | router_benchmark | Dispatch rate, 404/405 handling |
| Headers | headers_benchmark | Get/set/compare performance |
| IO Buffer | io_buffer_benchmark | Append/read/scatter-gather |
| Timers | timer_benchmark | Add/cancel/tick/fire rates |
| JSON | json_benchmark | Encode/decode various sizes |

### 7.2 Integration Tests

| Scenario | Benchmark | Purpose |
|----------|-----------|---------|
| Generated API | generated_api_benchmark | Full codegen path (parse + validate + route) |
| Products API | products_api_benchmark | Mixed CRUD workload |
| Routing-only | products_api_benchmark | Pure dispatch overhead |
| Validation-heavy | products_api_benchmark | Constraint checking cost |

### 7.3 Stability Tests

| Test | Script | Duration | Focus |
|------|--------|----------|-------|
| Sustained Load | products_api_load_test.sh | 5-60 min | Memory stability, throughput |
| Connection Churn | products_api_load_test.sh | 10k requests | FD cleanup, arena reuse |
| Variable Concurrency | products_api_load_test.sh | 3 × 30s | Scalability under different loads |

---

## 8. Reporting Results

### 8.1 Benchmark Report Format

```markdown
## Performance Results

**Environment**:
- CPU: Intel Xeon E5-2680 v4 @ 2.4 GHz (14 cores)
- RAM: 64 GB DDR4 2400 MHz
- OS: Ubuntu 24.04 LTS (kernel 6.8)
- Compiler: GCC 14.2.0
- Build: Release (-O3 -march=native + LTO)

**Microbenchmarks**:
| Component | Throughput | p50 | p99 | p999 |
|-----------|------------|-----|-----|------|
| Arena Allocations | 7.2M ops/s | 0.000 μs | 0.000 μs | 0.000 μs |
| Router Dispatch | 1.05M ops/s | 0.511 μs | 1.152 μs | 5.578 μs |
| HTTP Parser | 1.35M ops/s | 0.669 μs | 1.188 μs | 1.490 μs |

**End-to-End** (products_api, single reactor thread):
- Simple GET: 320k RPS, p99 = 0.18 ms
- Mixed CRUD: 155k RPS, p99 = 0.28 ms
- Validation stress: 185k RPS, p99 = 0.22 ms

**Stability** (30 min sustained, 128 concurrency):
- Memory: Stable at 45 MB (no growth after warmup)
- Throughput: 148k ± 2k RPS (consistent)
- Error rate: 0.00%
```

### 8.2 Regression Reports

When detecting regressions, include:
1. **What changed**: Commit hash or description
2. **Impact**: Percent change in key metrics
3. **Affected components**: Specific benchmarks showing regression
4. **Potential cause**: Hypothesis based on code changes

---

## 9. Future Enhancements

**Potential additions**:
- Automated regression detection in CI
- Historical trend tracking
- Flamegraph generation for profiling
- Comparison against other frameworks (Drogon, uWebSockets, etc.)
- Cloud-based reproducible benchmarks (fixed hardware)

---

## 10. References

- **Stage 2 Analysis**: `STAGE2_ANALYSIS.md` - Baseline benchmark results
- **Framework Internals**: `FRAMEWORK_INTERNALS.md` - Performance characteristics
- **Docker Benchmarking**: `docker/benchmarks/Dockerfile.bench`
- **Load Testing**: `test/load/products_api_load_test.sh`

---

**Document Version**: 1.0
**Maintained By**: KATANA Core Team
**Last Updated**: 2025-12-08
