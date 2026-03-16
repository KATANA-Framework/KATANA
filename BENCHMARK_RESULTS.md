# KATANA Benchmark Results

> Last updated: 2026-03-16 15:11:27
> Commit: 1164aeb

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
| benchmark_dir | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e/benchmark |
| build_dir | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e |
| cmake | CMAKE_BUILD_TYPE=Release; CMAKE_CXX_COMPILER=/usr/bin/c++; CMAKE_CXX_FLAGS=; CMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG |
| cpu_governor | performance |
| cpu_model | AMD Ryzen 5 5600 6-Core Processor |
| hostname | visage |
| logical_cores | 12 |
| lscpu_brief | Architecture:                            x86_64; CPU(s):                                  12; Model name:                              AMD Ryzen 5 5600 6-Core Processor; Thread(s) per core:                      2; Core(s) per socket:                      6; Socket(s):                               1; NUMA node0 CPU(s):                       0-11 |
| machine | x86_64 |
| physical_cores | 6 |
| platform | Linux |
| platform_release | 6.16.8+kali-amd64 |
| platform_version | #1 SMP PREEMPT_DYNAMIC Kali 6.16.8-1kali1 (2025-09-24) |
| python | 3.13.12 |
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
| 9 | canonical | wrk hello_world GET / depth10 | 4 | 4/512 | 10 | 10s | 3.6M req/sec | 772.080 us | 651.000 us | 1642.000 us | 2482.000 us | 363.7M bytes/sec | 0 |
| 10 | canonical | wrk compute_api POST /compute/sum depth10 | 4 | 4/512 | 10 | 10s | 2.4M req/sec | 1390.000 us | 1220.000 us | 2888.000 us | 4235.000 us | 226.0M bytes/sec | 0 |
| 11 | peak | wrk hello_world GET / depth20 | 4 | 4/512 | 20 | 5s | 5.5M req/sec | 1070.000 us | 902.000 us | 2495.000 us | 3715.000 us | 556.3M bytes/sec | 0 |
| 12 | peak | wrk compute_api POST /compute/sum depth40 | 4 | 4/512 | 40 | 5s | 3.4M req/sec | 3130.000 us | 3408.000 us | 0.000 us | 0.000 us | 330.6M bytes/sec | 0 |
| 5 | e2e_keepalive | HTTP E2E Keep-Alive (16 conn x 200 req) | - | -/- | - | - | 62.4K req/sec | - | 215.141 us | 516.138 us | 674.666 us | - | 0 |

---

## Core Runtime Benchmarks

_Runs: 1 | Aggregation: median_

Ring buffer, circular buffer, SIMD operations, arena allocation

| Config | Value |
|--------|-------|
| binary | performance_benchmark |
| binary_path | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e/benchmark/performance_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Retries/op | Errors |
|-----------|------------|-------------|-------------|--------------|------------|--------|
| Ring Buffer Queue (Single Thread) | 219.4M ops/sec | 0.005 us | 0.005 us | 0.007 us | - | 0 |
| Ring Buffer Queue (Concurrent 2x2) | 31.9M ops/sec | 0.052 us | 0.174 us | 0.238 us | 0.040 | 0 |
| Ring Buffer Queue (Concurrent 4x4) | 16.5M ops/sec | 0.240 us | 0.382 us | 0.481 us | 0.009 | 0 |
| Ring Buffer Queue (Core Saturation 6x6) | 12.8M ops/sec | 0.446 us | 0.717 us | 3.298 us | 0.328 | 0 |
| Ring Buffer Queue (High Contention 8x8) | 8.7M ops/sec | 0.386 us | 0.646 us | 125.590 us | 15.587 | 0 |
| Ring Buffer Queue (Extreme Contention 12x12) | 6.5M ops/sec | 0.374 us | 1.242 us | 308.469 us | 38.022 | 0 |
| Ring Buffer Queue (Max Contention 16x16) | 3.2M ops/sec | 0.252 us | 0.854 us | 1000.586 us | 108.034 | 0 |
| Ring Buffer Queue (Oversubscribed 14x14) | 4.7M ops/sec | 0.356 us | 1.072 us | 500.366 us | 66.772 | 0 |
| Circular Buffer | 77.8M ops/sec | 0.013 us | 0.016 us | 0.018 us | - | 0 |
| SIMD CRLF Search (1.5KB buffer) | 63.2M ops/sec | 0.015 us | 0.018 us | 0.029 us | - | 0 |
| SIMD CRLF Search (16KB buffer) | 5.3M ops/sec | 0.187 us | 0.273 us | 0.433 us | - | 0 |
| SIMD CRLF Search (32KB buffer) | 2.5M ops/sec | 0.392 us | 0.399 us | 0.580 us | - | 0 |
| SIMD CRLF Search (64KB buffer) | 1.3M ops/sec | 0.776 us | 0.990 us | 1.353 us | - | 0 |
| SIMD CRLF Search (128KB buffer) | 639.2K ops/sec | 1.540 us | 2.159 us | 2.854 us | - | 0 |
| HTTP Parser (Complete Request) | 1.9M ops/sec | 0.516 us | 0.701 us | 1.040 us | - | 0 |
| HTTP Parser (Fragmented Request) | 2.2M ops/sec | 0.453 us | 0.502 us | 0.961 us | - | 0 |
| Arena Allocations (64B objects) | 22.4M ops/sec | - | - | - | - | 0 |
| Memory Allocations (String Queue) | 34.5M ops/sec | - | - | - | - | 0 |

---

## Codegen Quality Benchmarks

_Runs: 1 | Aggregation: median_

JSON parsing, string escaping, key dispatch

| Config | Value |
|--------|-------|
| binary | codegen_quality_benchmark |
| binary_path | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e/benchmark/codegen_quality_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Tail p95 | Tail p99 | Data Rate |
|-----------|---------|------------|----------|----------|-----------|
| parse_int64 (simple) | 7.4 ns | 134.3M ops/sec | - | - | - |
| parse_int64 (negative) | 7.7 ns | 129.7M ops/sec | - | - | - |
| parse_double | 14.2 ns | 70.3M ops/sec | - | - | - |
| parse_bool (strict validation) | 1.6 ns | 633.8M ops/sec | - | - | - |
| parse_bool (mixed true/false/quoted) | 4.3 ns | 231.6M ops/sec | - | - | - |
| parse_int64 profile (best case) | 10.7 ns | 93.1M ops/sec | 40.0 ns | 40.0 ns | 256.9M bytes/sec |
| parse_int64 profile (typical mixed) | 11.6 ns | 85.8M ops/sec | 40.0 ns | 41.0 ns | 430.7M bytes/sec |
| parse_int64 profile (hard edge) | 15.1 ns | 66.4M ops/sec | 40.0 ns | 41.0 ns | 677.2M bytes/sec |
| parse_bool profile (best case) | 4.1 ns | 246.2M ops/sec | 30.0 ns | 31.0 ns | 1.23B bytes/sec |
| parse_bool profile (typical mixed) | 8.5 ns | 118.1M ops/sec | 30.0 ns | 40.0 ns | 954.6M bytes/sec |
| parse_bool profile (hard edge) | 12.5 ns | 79.9M ops/sec | 30.0 ns | 31.0 ns | 1.72B bytes/sec |
| validate_user profile (best case) | 7.6 ns | 130.9M ops/sec | 30.0 ns | 31.0 ns | 4.01B bytes/sec |
| validate_user profile (typical mixed) | 9.6 ns | 104.6M ops/sec | 30.0 ns | 31.0 ns | 3.27B bytes/sec |
| validate_user profile (hard edge) | 7.8 ns | 127.4M ops/sec | 30.0 ns | 31.0 ns | 4.13B bytes/sec |
| needs_json_escaping (clean, 58 chars) | 10.0 ns | 99.7M ops/sec | - | - | - |
| needs_json_escaping (dirty, 25 chars) | 2.3 ns | 443.1M ops/sec | - | - | - |
| escape_json_string (no-alloc path) | 12.6 ns | 79.2M ops/sec | - | - | - |
| escape_json_string_into (append) | 7.7 ns | 130.4M ops/sec | - | - | - |
| skip_value (nested obj with strings) | 33.7 ns | 29.7M ops/sec | - | - | - |
| 3-field object parse (linear) | 36.8 ns | 27.2M ops/sec | - | - | - |
| 8-field object parse (length-switch) | 116.0 ns | 8.6M ops/sec | - | - | - |
| arena alloc+reset cycle (4KB) | 58.4 ns | 17.1M ops/sec | - | - | - |

---

## Serialization Benchmarks

_Runs: 1 | Aggregation: median_

JSON serialization, string escaping, array construction

| Config | Value |
|--------|-------|
| binary | serialize_benchmark |
| binary_path | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e/benchmark/serialize_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Data Rate |
|-----------|---------|------------|-----------|
| escape_json_string (clean, return) | 12.9 ns | 77.5M ops/sec | - |
| escape_json_string_into (clean, append) | 7.5 ns | 134.2M ops/sec | - |
| escape_json_string (dirty, return) | 72.2 ns | 13.9M ops/sec | - |
| escape_json_string_into (dirty, append) | 54.3 ns | 18.4M ops/sec | - |
| escape profile (best case) | 13.9 ns | 71.8M ops/sec | 1.13B bytes/sec |
| escape profile (typical mixed) | 346.9 ns | 2.9M ops/sec | 236.7M bytes/sec |
| escape profile (hard edge) | 4310.0 ns | 232.2K ops/sec | 207.2M bytes/sec |
| serialize object profile (best case) | 70.4 ns | 14.2M ops/sec | 402.3M bytes/sec |
| serialize object profile (typical mixed) | 425.0 ns | 2.4M ops/sec | 282.4M bytes/sec |
| serialize object profile (hard edge) | 4620.0 ns | 216.2K ops/sec | 254.1M bytes/sec |
| needs_json_escaping (16 byte clean) | 2.7 ns | 368.7M ops/sec | - |
| needs_json_escaping (64 byte clean) | 3.2 ns | 316.0M ops/sec | - |
| needs_json_escaping (256 byte clean) | 6.8 ns | 147.2M ops/sec | - |
| needs_json_escaping (64 byte, escape at end) | 2.3 ns | 442.3M ops/sec | - |
| needs_json_escaping (64 byte, escape at start) | 0.7 ns | 1.48B ops/sec | - |
| serialize 5-field obj (embedded commas) | 66.5 ns | 15.0M ops/sec | - |
| serialize array 100 ints (to_chars flat buffer) | 214.6 ns | 4.7M ops/sec | - |
| serialize array 100 ints (to_chars into string) | 785.7 ns | 1.3M ops/sec | - |
| serialize array 100 ints (lookup table into string) | 193.0 ns | 5.2M ops/sec | - |
| serialize array 1000 ints (optimized) | 1920.0 ns | 521.8K ops/sec | - |
| serialize array 10000 ints (optimized) | 44310.0 ns | 22.6K ops/sec | - |
| serialize nested obj (5 levels deep) | 24.3 ns | 41.1M ops/sec | - |
| serialize 20-field obj | 350.1 ns | 2.9M ops/sec | - |
| serialize 100 nested objects | 3430.0 ns | 291.5K ops/sec | - |
| escape_json_string (10KB clean) | 241.9 ns | 4.1M ops/sec | - |
| escape_json_string (10KB heavy escaping) | 14690.0 ns | 68.1K ops/sec | - |
| escape_json_string (escape at SIMD boundary) | 292.5 ns | 3.4M ops/sec | - |
| needs_json_escaping (1KB clean) | 18.9 ns | 52.9M ops/sec | - |
| needs_json_escaping (1KB, escape at end) | 15.9 ns | 63.0M ops/sec | - |
| escape hot (same string repeated) | 12.5 ns | 80.2M ops/sec | - |
| escape cold (different strings) | 20.7 ns | 48.2M ops/sec | - |

---

## Router Benchmarks

_Runs: 1 | Aggregation: median_

HTTP routing dispatch performance

| Config | Value |
|--------|-------|
| binary | router_benchmark |
| binary_path | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e/benchmark/router_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Router dispatch (hits) | 3.7M ops/sec | 0.267 us | 0.338 us | 0.533 us | 0 |
| Router dispatch (not found) | 3.7M ops/sec | 0.266 us | 0.270 us | 0.495 us | 0 |
| Router dispatch (405) | 2.3M ops/sec | 0.436 us | 0.444 us | 0.722 us | 0 |
| Large router (hits) | 2.9M ops/sec | 0.349 us | 0.360 us | 0.562 us | 0 |
| Large router (not found) | 2.9M ops/sec | 0.340 us | 0.354 us | 0.590 us | 0 |
| Large router (405) | 1.7M ops/sec | 0.582 us | 0.593 us | 0.855 us | 0 |
| Large router (edge cases) | 2.0M ops/sec | 0.488 us | 0.494 us | 0.718 us | 0 |

---

## Generated API Dispatch Benchmarks

_Runs: 1 | Aggregation: median_

Generated router bindings, param extraction, parse and serialize on ready request objects

| Config | Value |
|--------|-------|
| binary | generated_api_benchmark |
| binary_path | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e/benchmark/generated_api_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Generated API dispatch+parse | 3.8M ops/sec | 0.140 us | 0.301 us | 0.371 us | 0 |

---

## Generated API Mixed Workload Benchmarks

_Runs: 1 | Aggregation: median_

Generated stack mixed valid/error workload on benchmark_api spec

| Config | Value |
|--------|-------|
| binary | benchmark_api_codegen_benchmark |
| binary_path | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e/benchmark/benchmark_api_codegen_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_codegen_valid_mix | 794.3K ops/sec | 0.752 us | 4.910 us | 5.001 us | 8.137 us | 0 |
| benchmark_api_codegen_error_mix | 1.7M ops/sec | 0.501 us | 0.561 us | 0.571 us | 0.621 us | 120000 |

---

## Generated API Framework Path Benchmarks

_Runs: 1 | Aggregation: median_

Generated stack valid/error workload on benchmark_api spec with minimal handlers

| Config | Value |
|--------|-------|
| binary | benchmark_api_framework_benchmark |
| binary_path | /home/visage/Рабочий стол/katana/KATANA/build/bench-e2e/benchmark/benchmark_api_framework_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_framework_valid_mix | 1.5M ops/sec | 0.501 us | 0.962 us | 0.973 us | 1.003 us | 0 |
| benchmark_api_framework_error_mix | 1.7M ops/sec | 0.481 us | 0.551 us | 0.561 us | 0.992 us | 120000 |

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
| wrk hello_world GET / depth10 | 3.6M req/sec | 363.7M bytes/sec | 772.080 us | 651.000 us | 1642.000 us | 2482.000 us | 7208.000 us | 20561.000 us | 0 |

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
| wrk compute_api POST /compute/sum depth10 | 2.4M req/sec | 226.0M bytes/sec | 1390.000 us | 1220.000 us | 2888.000 us | 4235.000 us | 19673.000 us | 36537.000 us | 0 |

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
| wrk hello_world GET / depth20 | 5.5M req/sec | 556.3M bytes/sec | 1070.000 us | 902.000 us | 2495.000 us | 3715.000 us | 5092.000 us | 19870.000 us | 0 |

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
| wrk compute_api POST /compute/sum depth40 | 3.4M req/sec | 330.6M bytes/sec | 3130.000 us | 3408.000 us | 0.000 us | 0.000 us | 0.000 us | 43811.000 us | 0 |

---

## Legacy E2E HTTP Keep-Alive Benchmark

_Runs: 1 | Aggregation: median_

Legacy Python keep-alive scenario for compute_api; kept for compatibility only

| Config | Value |
|--------|-------|
| connections | 16 |
| kind | e2e_keepalive |
| port | 18082 |
| requests_per_connection | 200 |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Latency max | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|-------------|--------|
| HTTP E2E Keep-Alive (16 conn x 200 req) | 62.4K req/sec | 215.141 us | 516.138 us | 674.666 us | 891.159 us | 1013.371 us | 0 |

---

## Baseline Comparison

Baseline not loaded or incompatible format.

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
