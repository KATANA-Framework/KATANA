# KATANA Benchmark Results

> Last updated: 2026-03-12 12:41:46
> Commit: 1208b43

## Summary

- Stability verdict: stable enough for trend tracking
- Noisy metrics (CV > 20.0%): 0
- Severely noisy metrics (CV > 40.0%): 0
- E2E network stages included: yes

> **Note**: Results shown use median-of-N aggregation across 1 run(s) per stage. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Environment

| Key | Value |
|-----|-------|
| affinity_cpus | 12 |
| benchmark_dir | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark |
| build_dir | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl |
| cmake | CMAKE_BUILD_TYPE=Release; CMAKE_CXX_COMPILER=/usr/bin/c++; CMAKE_CXX_FLAGS=; CMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG |
| cpu_model | AMD Ryzen 5 5600 6-Core Processor |
| hostname | DESKTOP-2LALARK |
| logical_cores | 12 |
| lscpu_brief | Architecture:                         x86_64; CPU(s):                               12; Model name:                           AMD Ryzen 5 5600 6-Core Processor; Thread(s) per core:                   2; Core(s) per socket:                   6; Socket(s):                            1; NUMA node0 CPU(s):                    0-11 |
| machine | x86_64 |
| physical_cores | 6 |
| platform | Linux |
| platform_release | 6.6.87.2-microsoft-standard-WSL2 |
| platform_version | #1 SMP PREEMPT_DYNAMIC Thu Jun  5 18:30:46 UTC 2025 |
| python | 3.12.3 |
| wrk_binary | /usr/bin/wrk |

---

## Quality Gates

- CV warning threshold: 20.00%
- Noisy metrics: 0
- Severely noisy metrics: 0

---

## E2E Summary

> **Note**: For pipeline stages, `wrk` latency metrics reflect one scripted batch/request() call,
> not isolated single-request service time. Throughput and error-free completion are the primary
> comparable signals across different pipeline depths.

| Stage | Profile | Benchmark | Workers | wrk t/c | Depth | Duration | Throughput | Avg Latency | p50 | p95 | p99 | Data Rate | Errors |
|-------|---------|-----------|---------|---------|-------|----------|------------|-------------|-----|-----|-----|-----------|--------|
| 9 | canonical | wrk hello_world GET / depth10 | 4 | 4/512 | 10 | 10s | 1.3M req/sec | 4380.000 us | 2331.000 us | 14259.000 us | 25031.000 us | 132.1M bytes/sec | 0 |
| 10 | canonical | wrk compute_api POST /compute/sum depth10 | 4 | 4/512 | 10 | 10s | 1.2M req/sec | 2710.000 us | 2469.000 us | 5430.000 us | 8017.000 us | 113.2M bytes/sec | 0 |
| 11 | peak | wrk hello_world GET / depth20 | 4 | 4/512 | 20 | 5s | 2.8M req/sec | 2210.000 us | 1930.000 us | 4534.000 us | 8114.000 us | 283.0M bytes/sec | 0 |
| 12 | peak | wrk compute_api POST /compute/sum depth40 | 4 | 4/512 | 40 | 5s | 1.4M req/sec | 8060.000 us | 8074.000 us | 17419.000 us | 0.000 us | 130.9M bytes/sec | 0 |

---

## Core Runtime Benchmarks

_Runs: 1 | Aggregation: median_

Ring buffer, circular buffer, SIMD operations, arena allocation

| Config | Value |
|--------|-------|
| binary | performance_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/performance_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Retries/op | Errors |
|-----------|------------|-------------|-------------|--------------|------------|--------|
| Ring Buffer Queue (Single Thread) | 238.8M ops/sec | 0.004 us | 0.008 us | 0.025 us | - | 0 |
| Ring Buffer Queue (Concurrent 2x2) | 32.6M ops/sec | 0.056 us | 0.212 us | 0.431 us | 0.048 | 0 |
| Ring Buffer Queue (Concurrent 4x4) | 18.2M ops/sec | 0.191 us | 0.544 us | 0.974 us | 0.098 | 0 |
| Ring Buffer Queue (Core Saturation 6x6) | 15.2M ops/sec | 0.266 us | 1.025 us | 17.705 us | 0.967 | 0 |
| Ring Buffer Queue (High Contention 8x8) | 9.1M ops/sec | 0.256 us | 1.532 us | 127.792 us | 9.735 | 0 |
| Ring Buffer Queue (Extreme Contention 12x12) | 7.1M ops/sec | 0.244 us | 1.442 us | 329.224 us | 18.691 | 0 |
| Ring Buffer Queue (Max Contention 16x16) | 6.6M ops/sec | 0.277 us | 8.560 us | 366.594 us | 27.771 | 0 |
| Ring Buffer Queue (Oversubscribed 14x14) | 6.7M ops/sec | 0.240 us | 2.604 us | 374.822 us | 20.548 | 0 |
| Circular Buffer | 208.0M ops/sec | 0.004 us | 0.010 us | 0.034 us | - | 0 |
| SIMD CRLF Search (1.5KB buffer) | 59.1M ops/sec | 0.016 us | 0.037 us | 0.123 us | - | 0 |
| SIMD CRLF Search (16KB buffer) | 4.3M ops/sec | 0.220 us | 0.405 us | 1.008 us | - | 0 |
| SIMD CRLF Search (32KB buffer) | 2.1M ops/sec | 0.433 us | 0.802 us | 1.477 us | - | 0 |
| SIMD CRLF Search (64KB buffer) | 1.0M ops/sec | 0.918 us | 1.839 us | 2.729 us | - | 0 |
| SIMD CRLF Search (128KB buffer) | 522.6K ops/sec | 1.805 us | 3.807 us | 9.905 us | - | 0 |
| HTTP Parser (Complete Request) | 1.6M ops/sec | 0.566 us | 1.375 us | 2.383 us | - | 0 |
| HTTP Parser (Fragmented Request) | 2.0M ops/sec | 0.471 us | 0.912 us | 1.756 us | - | 0 |
| Arena Allocations (64B objects) | 18.0M ops/sec | - | - | - | - | 0 |
| Memory Allocations (String Queue) | 24.3M ops/sec | - | - | - | - | 0 |

---

## Codegen Quality Benchmarks

_Runs: 1 | Aggregation: median_

JSON parsing, string escaping, key dispatch

| Config | Value |
|--------|-------|
| binary | codegen_quality_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/codegen_quality_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Tail p95 | Tail p99 | Data Rate |
|-----------|---------|------------|----------|----------|-----------|
| parse_int64 (simple) | 7.6 ns | 131.3M ops/sec | - | - | - |
| parse_int64 (negative) | 8.5 ns | 118.3M ops/sec | - | - | - |
| parse_double | 17.3 ns | 57.6M ops/sec | - | - | - |
| parse_bool (strict validation) | 2.0 ns | 491.8M ops/sec | - | - | - |
| parse_bool (mixed true/false/quoted) | 5.0 ns | 201.3M ops/sec | - | - | - |
| parse_int64 profile (best case) | 9.8 ns | 101.6M ops/sec | 39.0 ns | 108.0 ns | 280.5M bytes/sec |
| parse_int64 profile (typical mixed) | 11.9 ns | 83.7M ops/sec | 39.0 ns | 97.0 ns | 419.9M bytes/sec |
| parse_int64 profile (hard edge) | 15.2 ns | 65.7M ops/sec | 49.0 ns | 88.0 ns | 670.2M bytes/sec |
| parse_bool profile (best case) | 4.2 ns | 238.0M ops/sec | 30.0 ns | 39.0 ns | 1.19B bytes/sec |
| parse_bool profile (typical mixed) | 9.8 ns | 101.9M ops/sec | 30.0 ns | 88.0 ns | 824.0M bytes/sec |
| parse_bool profile (hard edge) | 13.1 ns | 76.6M ops/sec | 49.0 ns | 127.0 ns | 1.65B bytes/sec |
| validate_user profile (best case) | 8.0 ns | 125.7M ops/sec | 30.0 ns | 58.0 ns | 3.85B bytes/sec |
| validate_user profile (typical mixed) | 9.4 ns | 106.8M ops/sec | 30.0 ns | 58.0 ns | 3.33B bytes/sec |
| validate_user profile (hard edge) | 10.3 ns | 96.7M ops/sec | 39.0 ns | 68.0 ns | 3.13B bytes/sec |
| needs_json_escaping (clean, 58 chars) | 9.2 ns | 108.4M ops/sec | - | - | - |
| needs_json_escaping (dirty, 25 chars) | 1.1 ns | 892.2M ops/sec | - | - | - |
| escape_json_string (no-alloc path) | 16.3 ns | 61.5M ops/sec | - | - | - |
| escape_json_string_into (append) | 6.5 ns | 153.1M ops/sec | - | - | - |
| skip_value (nested obj with strings) | 36.6 ns | 27.3M ops/sec | - | - | - |
| 3-field object parse (linear) | 33.6 ns | 29.7M ops/sec | - | - | - |
| 8-field object parse (length-switch) | 162.0 ns | 6.2M ops/sec | - | - | - |
| arena alloc+reset cycle (4KB) | 56.3 ns | 17.8M ops/sec | - | - | - |

---

## Serialization Benchmarks

_Runs: 1 | Aggregation: median_

JSON serialization, string escaping, array construction

| Config | Value |
|--------|-------|
| binary | serialize_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/serialize_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Data Rate |
|-----------|---------|------------|-----------|
| escape_json_string (clean, return) | 36.8 ns | 27.1M ops/sec | - |
| escape_json_string_into (clean, append) | 21.5 ns | 46.5M ops/sec | - |
| escape_json_string (dirty, return) | 195.6 ns | 5.1M ops/sec | - |
| escape_json_string_into (dirty, append) | 141.3 ns | 7.1M ops/sec | - |
| escape profile (best case) | 28.6 ns | 34.9M ops/sec | 548.9M bytes/sec |
| escape profile (typical mixed) | 590.9 ns | 1.7M ops/sec | 139.0M bytes/sec |
| escape profile (hard edge) | 4500.0 ns | 222.3K ops/sec | 198.4M bytes/sec |
| serialize object profile (best case) | 78.9 ns | 12.7M ops/sec | 358.8M bytes/sec |
| serialize object profile (typical mixed) | 477.3 ns | 2.1M ops/sec | 251.4M bytes/sec |
| serialize object profile (hard edge) | 5290.0 ns | 189.0K ops/sec | 222.1M bytes/sec |
| needs_json_escaping (16 byte clean) | 2.6 ns | 383.0M ops/sec | - |
| needs_json_escaping (64 byte clean) | 2.8 ns | 361.4M ops/sec | - |
| needs_json_escaping (256 byte clean) | 5.5 ns | 182.3M ops/sec | - |
| needs_json_escaping (64 byte, escape at end) | 2.3 ns | 435.5M ops/sec | - |
| needs_json_escaping (64 byte, escape at start) | 1.7 ns | 591.4M ops/sec | - |
| serialize 5-field obj (embedded commas) | 69.5 ns | 14.4M ops/sec | - |
| serialize array 100 ints (to_chars flat buffer) | 252.6 ns | 4.0M ops/sec | - |
| serialize array 100 ints (to_chars into string) | 1460.0 ns | 683.9K ops/sec | - |
| serialize array 100 ints (lookup table into string) | 132.0 ns | 7.6M ops/sec | - |
| serialize array 1000 ints (optimized) | 1800.0 ns | 554.8K ops/sec | - |
| serialize array 10000 ints (optimized) | 38720.0 ns | 25.8K ops/sec | - |
| serialize nested obj (5 levels deep) | 20.9 ns | 47.9M ops/sec | - |
| serialize 20-field obj | 324.2 ns | 3.1M ops/sec | - |
| serialize 100 nested objects | 4170.0 ns | 239.6K ops/sec | - |
| escape_json_string (10KB clean) | 335.0 ns | 3.0M ops/sec | - |
| escape_json_string (10KB heavy escaping) | 15650.0 ns | 63.9K ops/sec | - |
| escape_json_string (escape at SIMD boundary) | 605.1 ns | 1.7M ops/sec | - |
| needs_json_escaping (1KB clean) | 25.6 ns | 39.1M ops/sec | - |
| needs_json_escaping (1KB, escape at end) | 25.3 ns | 39.6M ops/sec | - |
| escape hot (same string repeated) | 12.4 ns | 80.9M ops/sec | - |
| escape cold (different strings) | 23.5 ns | 42.6M ops/sec | - |

---

## Router Benchmarks

_Runs: 1 | Aggregation: median_

HTTP routing dispatch performance

| Config | Value |
|--------|-------|
| binary | router_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/router_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Router dispatch (hits) | 1.9M ops/sec | 0.378 us | 1.880 us | 4.377 us | 0 |
| Router dispatch (not found) | 1.8M ops/sec | 0.468 us | 1.262 us | 2.284 us | 0 |
| Router dispatch (405) | 1.3M ops/sec | 0.609 us | 1.642 us | 2.554 us | 0 |
| Large router (hits) | 1.9M ops/sec | 0.471 us | 0.995 us | 2.095 us | 0 |
| Large router (not found) | 1.6M ops/sec | 0.558 us | 1.329 us | 2.361 us | 0 |
| Large router (405) | 1.1M ops/sec | 0.747 us | 1.999 us | 4.312 us | 0 |
| Large router (edge cases) | 1.5M ops/sec | 0.578 us | 1.393 us | 2.360 us | 0 |

---

## Generated API Dispatch Benchmarks

_Runs: 1 | Aggregation: median_

Generated router bindings, param extraction, parse and serialize on ready request objects

| Config | Value |
|--------|-------|
| binary | generated_api_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/generated_api_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Generated API dispatch+parse | 2.6M ops/sec | 0.215 us | 0.598 us | 1.216 us | 0 |

---

## Generated API Mixed Workload Benchmarks

_Runs: 1 | Aggregation: median_

Generated stack mixed valid/error workload on benchmark_api spec

| Config | Value |
|--------|-------|
| binary | benchmark_api_codegen_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/benchmark_api_codegen_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_codegen_valid_mix | 409.5K ops/sec | 1.094 us | 10.517 us | 12.080 us | 58.580 us | 0 |
| benchmark_api_codegen_error_mix | 978.9K ops/sec | 0.791 us | 1.143 us | 1.386 us | 20.341 us | 120000 |

---

## Generated API Framework Path Benchmarks

_Runs: 1 | Aggregation: median_

Generated stack valid/error workload on benchmark_api spec with minimal handlers

| Config | Value |
|--------|-------|
| binary | benchmark_api_framework_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/benchmark_api_framework_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_framework_valid_mix | 1.2M ops/sec | 0.605 us | 1.201 us | 1.875 us | 10.097 us | 0 |
| benchmark_api_framework_error_mix | 1.4M ops/sec | 0.537 us | 1.006 us | 1.171 us | 2.725 us | 120000 |

---

## HTTP Load Benchmark (hello_world_server canonical pipeline via wrk)

_Runs: 1 | Aggregation: median_

Canonical low-latency pipelined GET profile for hello_world_server

| Config | Value |
|--------|-------|
| bench_workers | 4 |
| kind | wrk_http |
| pipeline_depth | 10 |
| port | 18080 |
| profile | canonical |
| server_target | hello_world_server |
| wrk_connections | 512 |
| wrk_duration_sec | 10 |
| wrk_script | test/load/scripts/hello_pipeline.lua |
| wrk_threads | 4 |
| wrk_url | http://127.0.0.1:{port}/ |

| Benchmark | Throughput | Data Rate | Latency avg | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Latency max | Errors |
|-----------|------------|-----------|-------------|-------------|-------------|-------------|--------------|-------------|--------|
| wrk hello_world GET / depth10 | 1.3M req/sec | 132.1M bytes/sec | 4380.000 us | 2331.000 us | 14259.000 us | 25031.000 us | 36245.000 us | 58585.000 us | 0 |

---

## HTTP Load Benchmark (compute_api canonical pipeline via wrk)

_Runs: 1 | Aggregation: median_

Canonical low-latency pipelined POST profile for compute_api

| Config | Value |
|--------|-------|
| bench_workers | 4 |
| kind | wrk_http |
| pipeline_depth | 10 |
| port | 18081 |
| profile | canonical |
| server_target | compute_api |
| wrk_connections | 512 |
| wrk_duration_sec | 10 |
| wrk_script | test/load/scripts/compute_sum_pipeline.lua |
| wrk_threads | 4 |
| wrk_url | http://127.0.0.1:{port}/ |

| Benchmark | Throughput | Data Rate | Latency avg | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Latency max | Errors |
|-----------|------------|-----------|-------------|-------------|-------------|-------------|--------------|-------------|--------|
| wrk compute_api POST /compute/sum depth10 | 1.2M req/sec | 113.2M bytes/sec | 2710.000 us | 2469.000 us | 5430.000 us | 8017.000 us | 20459.000 us | 70747.000 us | 0 |

---

## HTTP Load Benchmark (hello_world_server peak pipeline via wrk)

_Runs: 1 | Aggregation: median_

Peak-throughput pipelined GET profile for hello_world_server

| Config | Value |
|--------|-------|
| bench_workers | 4 |
| kind | wrk_http |
| pipeline_depth | 20 |
| port | 18080 |
| profile | peak |
| server_target | hello_world_server |
| wrk_connections | 512 |
| wrk_duration_sec | 5 |
| wrk_script | test/load/scripts/hello_pipeline.lua |
| wrk_threads | 4 |
| wrk_url | http://127.0.0.1:{port}/ |

| Benchmark | Throughput | Data Rate | Latency avg | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Latency max | Errors |
|-----------|------------|-----------|-------------|-------------|-------------|-------------|--------------|-------------|--------|
| wrk hello_world GET / depth20 | 2.8M req/sec | 283.0M bytes/sec | 2210.000 us | 1930.000 us | 4534.000 us | 8114.000 us | 33656.000 us | 57099.000 us | 0 |

---

## HTTP Load Benchmark (compute_api peak pipeline via wrk)

_Runs: 1 | Aggregation: median_

Peak-throughput pipelined POST profile for compute_api

| Config | Value |
|--------|-------|
| bench_workers | 4 |
| kind | wrk_http |
| pipeline_depth | 40 |
| port | 18081 |
| profile | peak |
| server_target | compute_api |
| wrk_connections | 512 |
| wrk_duration_sec | 5 |
| wrk_script | test/load/scripts/compute_sum_pipeline.lua |
| wrk_threads | 4 |
| wrk_url | http://127.0.0.1:{port}/ |

| Benchmark | Throughput | Data Rate | Latency avg | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Latency max | Errors |
|-----------|------------|-----------|-------------|-------------|-------------|-------------|--------------|-------------|--------|
| wrk compute_api POST /compute/sum depth40 | 1.4M req/sec | 130.9M bytes/sec | 8060.000 us | 8074.000 us | 17419.000 us | 0.000 us | 0.000 us | 62005.000 us | 0 |

---

## Running Benchmarks

```bash
# Build benchmark targets
cmake --preset bench
cmake --build build/bench -j$(nproc)

# Run all benchmarks
./scripts/run_benchmarks.py

# Run full micro + wrk-based E2E pipeline
./scripts/run_benchmarks.py --include-e2e

# Recommended repeat policy with explicit quality gate
./scripts/run_benchmarks.py --aggregation median --stage-repeat 1=20 --cv-threshold-pct 15

# Point the runner at a custom build tree or wrk binary
KATANA_BENCH_BUILD_DIR=build/bench-wsl KATANA_WRK_BIN=wrk ./scripts/run_benchmarks.py --include-e2e

# Collect perf counters (requires perf permissions)
./scripts/run_benchmarks.py --perf-stat

# Compare against a baseline
./scripts/run_benchmarks.py --compare benchmarks/baseline.json --fail-on-regression

# Run specific stages
./scripts/run_benchmarks.py --stage 1 2 9 10

# Run individual benchmarks
./build/bench/benchmark/codegen_quality_benchmark
./build/bench/benchmark/serialize_benchmark
./build/bench/benchmark/router_benchmark
./build/bench/benchmark/performance_benchmark
```
