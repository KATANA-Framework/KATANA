# KATANA Benchmark Results

> Last updated: 2026-03-14 06:46:36
> Commit: e6335a2

## Summary

- Stability verdict: contains noisy metrics, inspect before trusting small deltas
- Noisy metrics (CV > 20.0%): 64
- Severely noisy metrics (CV > 40.0%): 34
- E2E network stages included: yes

> **Note**: Results shown use median-of-N aggregation across 5 run(s) per stage. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Environment

| Key | Value |
|-----|-------|
| affinity_cpus | 6 |
| benchmark_dir | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs/benchmark |
| build_dir | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs |
| cmake | CMAKE_BUILD_TYPE=Release; CMAKE_CXX_COMPILER=/usr/bin/c++; CMAKE_CXX_FLAGS=; CMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG |
| cpu_model | AMD Ryzen 5 5600 6-Core Processor |
| hostname | visage |
| logical_cores | 6 |
| lscpu_brief | Architecture:                            x86_64; CPU(s):                                  6; Model name:                              AMD Ryzen 5 5600 6-Core Processor; Thread(s) per core:                      1; Core(s) per socket:                      6; Socket(s):                               1; NUMA node0 CPU(s):                       0-5 |
| machine | x86_64 |
| physical_cores | 6 |
| platform | Linux |
| platform_release | 6.8.0-101-generic |
| platform_version | #101-Ubuntu SMP PREEMPT_DYNAMIC Mon Feb  9 10:15:05 UTC 2026 |
| python | 3.12.3 |
| wrk_binary | /usr/bin/wrk |

---

## Quality Gates

- CV warning threshold: 20.00%
- Noisy metrics: 64
- Severely noisy metrics: 34

| Top Noisy Metric | CV |
|------------------|----|
| stage12.wrk compute_api POST /compute/sum depth40.latency_p999_us | 200.00% |
| stage1.Ring Buffer Queue (Max Contention 16x16).latency_p999_us | 196.17% |
| stage1.Ring Buffer Queue (Concurrent 4x4).retries_per_op_total | 181.02% |
| stage1.Ring Buffer Queue (High Contention 8x8).latency_p999_us | 166.54% |
| stage1.Ring Buffer Queue (Max Contention 16x16).throughput | 159.52% |
| stage1.Ring Buffer Queue (Extreme Contention 12x12).retries_per_op_total | 154.58% |
| stage1.Ring Buffer Queue (Extreme Contention 12x12).latency_p999_us | 137.69% |
| stage12.wrk compute_api POST /compute/sum depth40.latency_p99_us | 122.80% |
| stage1.Ring Buffer Queue (High Contention 8x8).throughput | 112.93% |
| stage1.Ring Buffer Queue (Extreme Contention 12x12).throughput | 87.48% |

---

## E2E Summary

> **Note**: For pipeline stages, `wrk` latency metrics reflect one scripted batch/request() call,
> not isolated single-request service time. Throughput and error-free completion are the primary
> comparable signals across different pipeline depths.

| Stage | Profile | Benchmark | Workers | wrk t/c | Depth | Duration | Throughput | Avg Latency | p50 | p95 | p99 | Data Rate | Errors |
|-------|---------|-----------|---------|---------|-------|----------|------------|-------------|-----|-----|-----|-----------|--------|
| 9 | canonical | wrk hello_world GET / depth10 | 4 | 4/512 | 10 | 10s | 1.7M req/sec | 2800.000 us | 1839.000 us | 8388.000 us | 14888.000 us | 170.0M bytes/sec | 0 |
| 10 | canonical | wrk compute_api POST /compute/sum depth10 | 4 | 4/512 | 10 | 10s | 1.1M req/sec | 3430.000 us | 2661.000 us | 8877.000 us | 13201.000 us | 109.3M bytes/sec | 0 |
| 11 | peak | wrk hello_world GET / depth20 | 4 | 4/512 | 20 | 5s | 2.5M req/sec | 2850.000 us | 2153.000 us | 7468.000 us | 11558.000 us | 256.1M bytes/sec | 0 |
| 12 | peak | wrk compute_api POST /compute/sum depth40 | 4 | 4/512 | 40 | 5s | 1.5M req/sec | 7350.000 us | 6953.000 us | 15916.000 us | 0.000 us | 145.8M bytes/sec | 0 |
| 5 | e2e_keepalive | HTTP E2E Keep-Alive (16 conn x 200 req) | - | -/- | - | - | 2.0K req/sec | - | 4871.079 us | 23527.530 us | 36622.630 us | - | 0 |

---

## Core Runtime Benchmarks

_Runs: 5 | Aggregation: median_

Ring buffer, circular buffer, SIMD operations, arena allocation

| Config | Value |
|--------|-------|
| binary | performance_benchmark |
| binary_path | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs/benchmark/performance_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Retries/op | Errors |
|-----------|------------|-------------|-------------|--------------|------------|--------|
| Ring Buffer Queue (Single Thread) | 178.9M ops/sec | 0.004 us | 0.009 us | 0.303 us | - | 0 |
| Ring Buffer Queue (Concurrent 2x2) | 30.2M ops/sec | 0.052 us | 0.240 us | 2.789 us | 0.195 | 0 |
| Ring Buffer Queue (Concurrent 4x4) | 14.8M ops/sec | 0.131 us | 0.383 us | 1.059 us | 2.610 | 0 |
| Ring Buffer Queue (Core Saturation 3x3) | 19.4M ops/sec | 0.135 us | 0.393 us | 1.676 us | 0.150 | 0 |
| Ring Buffer Queue (High Contention 8x8) | 1.1M ops/sec | 0.154 us | 0.556 us | 2.891 us | 198.051 | 0 |
| Ring Buffer Queue (Extreme Contention 12x12) | 5.3M ops/sec | 0.132 us | 0.478 us | 0.875 us | 28.311 | 0 |
| Ring Buffer Queue (Max Contention 16x16) | 957.7K ops/sec | 0.137 us | 0.487 us | 1.453 us | 217.441 | 0 |
| Ring Buffer Queue (Oversubscribed 8x8) | 3.0M ops/sec | 0.178 us | 0.604 us | 1.344 us | 68.875 | 0 |
| Circular Buffer | 148.2M ops/sec | 0.005 us | 0.071 us | 0.115 us | - | 0 |
| SIMD CRLF Search (1.5KB buffer) | 45.0M ops/sec | 0.017 us | 0.268 us | 0.419 us | - | 0 |
| SIMD CRLF Search (16KB buffer) | 3.7M ops/sec | 0.196 us | 2.580 us | 4.143 us | - | 0 |
| SIMD CRLF Search (32KB buffer) | 1.8M ops/sec | 0.393 us | 5.146 us | 7.130 us | - | 0 |
| SIMD CRLF Search (64KB buffer) | 783.5K ops/sec | 0.907 us | 10.488 us | 16.500 us | - | 0 |
| SIMD CRLF Search (128KB buffer) | 415.8K ops/sec | 1.780 us | 20.684 us | 28.599 us | - | 0 |
| HTTP Parser (Complete Request) | 1.3M ops/sec | 0.581 us | 4.034 us | 5.808 us | - | 0 |
| HTTP Parser (Fragmented Request) | 1.4M ops/sec | 0.541 us | 3.773 us | 5.625 us | - | 0 |
| Arena Allocations (64B objects) | 11.9M ops/sec | - | - | - | - | 0 |
| Memory Allocations (String Queue) | 21.4M ops/sec | - | - | - | - | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | 187.4M | 19.1M | 10.20% | 16.8M | 171.5M | 178.9M | 216.7M | 224.7M |
| Ring Buffer Queue (Concurrent 2x2) | 29.3M | 2.4M | 8.24% | 2.1M | 26.1M | 30.2M | 32.2M | 32.7M |
| Ring Buffer Queue (Concurrent 4x4) | 14.1M | 6.9M | 48.80% | 6.0M | 1.9M | 14.8M | 21.7M | 23.2M |
| Ring Buffer Queue (Core Saturation 3x3) | 19.8M | 1.2M | 5.99% | 1.0M | 18.8M | 19.4M | 21.6M | 22.0M |
| Ring Buffer Queue (High Contention 8x8) | 6.6M | 7.4M | 112.93% | 6.5M | 1.1M | 1.1M | 17.9M | 19.9M |
| Ring Buffer Queue (Extreme Contention 12x12) | 8.5M | 7.5M | 87.48% | 6.5M | 412.0K | 5.3M | 17.9M | 18.2M |
| Ring Buffer Queue (Max Contention 16x16) | 4.4M | 6.9M | 159.52% | 6.1M | 329.1K | 957.7K | 14.9M | 18.2M |
| Ring Buffer Queue (Oversubscribed 8x8) | 7.3M | 6.3M | 86.13% | 5.5M | 1.9M | 3.0M | 15.9M | 16.6M |
| Circular Buffer | 155.6M | 18.3M | 11.74% | 16.0M | 138.3M | 148.2M | 183.4M | 189.9M |
| SIMD CRLF Search (1.5KB buffer) | 46.2M | 6.1M | 13.16% | 5.3M | 40.8M | 45.0M | 55.3M | 57.9M |
| SIMD CRLF Search (16KB buffer) | 3.8M | 669.7K | 17.69% | 587.0K | 3.0M | 3.7M | 4.8M | 5.0M |
| SIMD CRLF Search (32KB buffer) | 1.8M | 192.0K | 10.59% | 168.3K | 1.5M | 1.8M | 2.1M | 2.1M |
| SIMD CRLF Search (64KB buffer) | 822.3K | 87.8K | 10.67% | 76.9K | 750.2K | 783.5K | 957.1K | 993.3K |
| SIMD CRLF Search (128KB buffer) | 435.5K | 56.4K | 12.95% | 49.4K | 391.1K | 415.8K | 520.9K | 546.8K |
| HTTP Parser (Complete Request) | 1.3M | 156.6K | 11.80% | 137.3K | 1.2M | 1.3M | 1.6M | 1.6M |
| HTTP Parser (Fragmented Request) | 1.4M | 185.4K | 12.88% | 162.5K | 1.3M | 1.4M | 1.7M | 1.8M |
| Arena Allocations (64B objects) | 12.7M | 1.8M | 14.20% | 1.6M | 11.1M | 11.9M | 15.4M | 16.0M |
| Memory Allocations (String Queue) | 21.5M | 3.0M | 13.83% | 2.6M | 18.3M | 21.4M | 25.8M | 26.9M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | p99 us | 0.009 | 0.001 | 7.65% | 0.001 | 0.009 | 0.010 | 0.010 |
| Ring Buffer Queue (Concurrent 2x2) | p99 us | 0.238 | 0.036 | 14.90% | 0.031 | 0.240 | 0.284 | 0.290 |
| Ring Buffer Queue (Concurrent 4x4) | p99 us | 0.389 | 0.098 | 25.33% | 0.086 | 0.383 | 0.495 | 0.503 |
| Ring Buffer Queue (Core Saturation 3x3) | p99 us | 0.394 | 0.016 | 4.07% | 0.014 | 0.393 | 0.412 | 0.413 |
| Ring Buffer Queue (High Contention 8x8) | p99 us | 0.530 | 0.116 | 21.83% | 0.102 | 0.556 | 0.680 | 0.703 |
| Ring Buffer Queue (Extreme Contention 12x12) | p99 us | 0.460 | 0.101 | 22.06% | 0.089 | 0.478 | 0.560 | 0.569 |
| Ring Buffer Queue (Max Contention 16x16) | p99 us | 0.522 | 0.083 | 15.84% | 0.072 | 0.487 | 0.645 | 0.672 |
| Ring Buffer Queue (Oversubscribed 8x8) | p99 us | 0.564 | 0.086 | 15.27% | 0.076 | 0.604 | 0.650 | 0.660 |
| Circular Buffer | p99 us | 0.060 | 0.026 | 43.52% | 0.023 | 0.071 | 0.077 | 0.078 |
| SIMD CRLF Search (1.5KB buffer) | p99 us | 0.226 | 0.096 | 42.39% | 0.084 | 0.268 | 0.291 | 0.291 |
| SIMD CRLF Search (16KB buffer) | p99 us | 2.208 | 0.920 | 41.66% | 0.806 | 2.580 | 2.837 | 2.890 |
| SIMD CRLF Search (32KB buffer) | p99 us | 4.494 | 1.673 | 37.23% | 1.466 | 5.146 | 5.801 | 5.950 |
| SIMD CRLF Search (64KB buffer) | p99 us | 9.380 | 3.564 | 37.99% | 3.124 | 10.488 | 11.915 | 11.996 |
| SIMD CRLF Search (128KB buffer) | p99 us | 17.648 | 6.990 | 39.61% | 6.127 | 20.684 | 22.060 | 22.130 |
| HTTP Parser (Complete Request) | p99 us | 3.422 | 1.142 | 33.37% | 1.001 | 4.034 | 4.117 | 4.118 |
| HTTP Parser (Fragmented Request) | p99 us | 3.242 | 1.220 | 37.64% | 1.070 | 3.773 | 3.960 | 3.972 |

### Contention Stability (Retries/op)

| Benchmark | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|
| Ring Buffer Queue (Concurrent 2x2) | 0.170 | 0.053 | 30.98% | 0.046 | 0.195 | 0.209 | 0.210 |
| Ring Buffer Queue (Concurrent 4x4) | 21.595 | 39.092 | 181.02% | 34.266 | 2.610 | 80.775 | 99.699 |
| Ring Buffer Queue (Core Saturation 3x3) | 0.177 | 0.055 | 31.28% | 0.048 | 0.150 | 0.249 | 0.253 |
| Ring Buffer Queue (High Contention 8x8) | 124.290 | 97.703 | 78.61% | 85.641 | 198.051 | 209.650 | 211.483 |
| Ring Buffer Queue (Extreme Contention 12x12) | 129.189 | 199.702 | 154.58% | 175.047 | 28.311 | 436.968 | 522.679 |
| Ring Buffer Queue (Max Contention 16x16) | 309.084 | 250.559 | 81.06% | 219.625 | 217.441 | 636.952 | 661.408 |
| Ring Buffer Queue (Oversubscribed 8x8) | 57.130 | 45.468 | 79.59% | 39.854 | 68.875 | 105.419 | 105.979 |

---

## Codegen Quality Benchmarks

_Runs: 5 | Aggregation: median_

JSON parsing, string escaping, key dispatch

| Config | Value |
|--------|-------|
| binary | codegen_quality_benchmark |
| binary_path | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs/benchmark/codegen_quality_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Tail p95 | Tail p99 | Data Rate |
|-----------|---------|------------|----------|----------|-----------|
| parse_int64 (simple) | 9.7 ns | 103.0M ops/sec | - | - | - |
| parse_int64 (negative) | 8.9 ns | 111.9M ops/sec | - | - | - |
| parse_double | 20.6 ns | 48.7M ops/sec | - | - | - |
| parse_bool (strict validation) | 2.7 ns | 366.8M ops/sec | - | - | - |
| parse_bool (mixed true/false/quoted) | 7.6 ns | 131.2M ops/sec | - | - | - |
| parse_int64 profile (best case) | 13.9 ns | 72.2M ops/sec | 50.0 ns | 140.0 ns | 199.2M bytes/sec |
| parse_int64 profile (typical mixed) | 16.8 ns | 59.4M ops/sec | 60.0 ns | 131.0 ns | 297.9M bytes/sec |
| parse_int64 profile (hard edge) | 21.7 ns | 46.0M ops/sec | 81.0 ns | 140.0 ns | 469.5M bytes/sec |
| parse_bool profile (best case) | 6.9 ns | 144.8M ops/sec | 40.0 ns | 90.0 ns | 722.7M bytes/sec |
| parse_bool profile (typical mixed) | 16.2 ns | 61.6M ops/sec | 80.0 ns | 131.0 ns | 498.2M bytes/sec |
| parse_bool profile (hard edge) | 20.6 ns | 48.7M ops/sec | 80.0 ns | 170.0 ns | 1.05B bytes/sec |
| validate_user profile (best case) | 11.3 ns | 88.5M ops/sec | 50.0 ns | 90.0 ns | 2.71B bytes/sec |
| validate_user profile (typical mixed) | 13.0 ns | 76.7M ops/sec | 50.0 ns | 80.0 ns | 2.40B bytes/sec |
| validate_user profile (hard edge) | 14.3 ns | 70.1M ops/sec | 60.0 ns | 90.0 ns | 2.27B bytes/sec |
| needs_json_escaping (clean, 58 chars) | 11.4 ns | 87.4M ops/sec | - | - | - |
| needs_json_escaping (dirty, 25 chars) | 1.5 ns | 669.8M ops/sec | - | - | - |
| escape_json_string (no-alloc path) | 22.3 ns | 44.8M ops/sec | - | - | - |
| escape_json_string_into (append) | 9.2 ns | 109.1M ops/sec | - | - | - |
| skip_value (nested obj with strings) | 39.7 ns | 25.2M ops/sec | - | - | - |
| 3-field object parse (linear) | 73.4 ns | 13.6M ops/sec | - | - | - |
| 8-field object parse (length-switch) | 248.9 ns | 4.0M ops/sec | - | - | - |
| arena alloc+reset cycle (4KB) | 86.9 ns | 11.5M ops/sec | - | - | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| parse_int64 (simple) | 107.0M | 10.6M | 9.93% | 9.3M | 97.9M | 103.0M | 123.1M | 127.8M |
| parse_int64 (negative) | 117.7M | 13.8M | 11.71% | 12.1M | 107.1M | 111.9M | 138.7M | 145.0M |
| parse_double | 51.3M | 7.1M | 13.83% | 6.2M | 45.6M | 48.7M | 62.1M | 65.3M |
| parse_bool (strict validation) | 389.9M | 46.7M | 11.98% | 41.0M | 352.7M | 366.8M | 461.7M | 481.1M |
| parse_bool (mixed true/false/quoted) | 134.9M | 25.6M | 18.95% | 22.4M | 96.8M | 131.2M | 169.4M | 176.5M |
| parse_int64 profile (best case) | 74.0M | 13.1M | 17.70% | 11.5M | 59.5M | 72.2M | 93.3M | 98.5M |
| parse_int64 profile (typical mixed) | 60.3M | 11.6M | 19.26% | 10.2M | 44.0M | 59.4M | 76.3M | 80.3M |
| parse_int64 profile (hard edge) | 47.5M | 9.3M | 19.63% | 8.2M | 37.2M | 46.0M | 61.2M | 64.5M |
| parse_bool profile (best case) | 147.4M | 28.0M | 18.99% | 24.5M | 121.8M | 144.8M | 189.0M | 199.3M |
| parse_bool profile (typical mixed) | 64.9M | 9.3M | 14.36% | 8.2M | 58.3M | 61.6M | 79.1M | 83.3M |
| parse_bool profile (hard edge) | 52.2M | 7.1M | 13.59% | 6.2M | 48.1M | 48.7M | 63.0M | 66.4M |
| validate_user profile (best case) | 92.1M | 13.3M | 14.45% | 11.7M | 79.7M | 88.5M | 112.1M | 117.9M |
| validate_user profile (typical mixed) | 82.1M | 10.5M | 12.74% | 9.2M | 74.3M | 76.7M | 98.2M | 102.5M |
| validate_user profile (hard edge) | 73.3M | 12.9M | 17.58% | 11.3M | 58.0M | 70.1M | 92.1M | 96.9M |
| needs_json_escaping (clean, 58 chars) | 85.0M | 19.4M | 22.77% | 17.0M | 53.2M | 87.4M | 108.9M | 113.6M |
| needs_json_escaping (dirty, 25 chars) | 702.1M | 84.6M | 12.05% | 74.2M | 634.4M | 669.8M | 831.8M | 866.4M |
| escape_json_string (no-alloc path) | 48.1M | 5.9M | 12.32% | 5.2M | 44.8M | 44.8M | 57.1M | 59.9M |
| escape_json_string_into (append) | 116.4M | 13.8M | 11.82% | 12.1M | 107.4M | 109.1M | 137.7M | 143.6M |
| skip_value (nested obj with strings) | 26.6M | 3.5M | 13.14% | 3.1M | 23.8M | 25.2M | 31.9M | 33.4M |
| 3-field object parse (linear) | 14.4M | 1.6M | 11.16% | 1.4M | 13.3M | 13.6M | 16.8M | 17.6M |
| 8-field object parse (length-switch) | 4.3M | 541.5K | 12.63% | 474.6K | 3.9M | 4.0M | 5.1M | 5.4M |
| arena alloc+reset cycle (4KB) | 12.0M | 1.6M | 13.01% | 1.4M | 10.8M | 11.5M | 14.3M | 15.0M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| parse_int64 (simple) | ns | 9.420 | 0.835 | 8.87% | 0.732 | 9.700 | 10.120 | 10.200 |
| parse_int64 (negative) | ns | 8.580 | 0.857 | 9.98% | 0.751 | 8.900 | 9.240 | 9.300 |
| parse_double | ns | 19.800 | 2.319 | 11.71% | 2.033 | 20.600 | 21.720 | 21.900 |
| parse_bool (strict validation) | ns | 2.580 | 0.248 | 9.62% | 0.218 | 2.700 | 2.780 | 2.800 |
| parse_bool (mixed true/false/quoted) | ns | 7.700 | 1.493 | 19.39% | 1.308 | 7.600 | 9.800 | 10.300 |
| parse_int64 profile (best case) | tail p99 ns | 130.400 | 21.266 | 16.31% | 18.640 | 140.000 | 149.000 | 151.000 |
| parse_int64 profile (typical mixed) | tail p99 ns | 132.600 | 25.881 | 19.52% | 22.686 | 131.000 | 164.800 | 171.000 |
| parse_int64 profile (hard edge) | tail p99 ns | 132.000 | 23.152 | 17.54% | 20.293 | 140.000 | 156.000 | 160.000 |
| parse_bool profile (best case) | tail p99 ns | 86.000 | 19.596 | 22.79% | 17.177 | 90.000 | 106.000 | 110.000 |
| parse_bool profile (typical mixed) | tail p99 ns | 126.600 | 20.713 | 16.36% | 18.156 | 131.000 | 148.200 | 150.000 |
| parse_bool profile (hard edge) | tail p99 ns | 156.400 | 22.966 | 14.68% | 20.131 | 170.000 | 170.000 | 170.000 |
| validate_user profile (best case) | tail p99 ns | 84.000 | 12.000 | 14.29% | 10.518 | 90.000 | 90.000 | 90.000 |
| validate_user profile (typical mixed) | tail p99 ns | 80.000 | 10.954 | 13.69% | 9.602 | 80.000 | 90.000 | 90.000 |
| validate_user profile (hard edge) | tail p99 ns | 88.200 | 9.847 | 11.16% | 8.631 | 90.000 | 98.200 | 100.000 |
| needs_json_escaping (clean, 58 chars) | ns | 12.480 | 3.364 | 26.96% | 2.949 | 11.400 | 17.500 | 18.800 |
| needs_json_escaping (dirty, 25 chars) | ns | 1.440 | 0.136 | 9.42% | 0.119 | 1.500 | 1.580 | 1.600 |
| escape_json_string (no-alloc path) | ns | 21.060 | 2.192 | 10.41% | 1.922 | 22.300 | 22.300 | 22.300 |
| escape_json_string_into (append) | ns | 8.700 | 0.867 | 9.97% | 0.760 | 9.200 | 9.280 | 9.300 |
| skip_value (nested obj with strings) | ns | 38.220 | 4.285 | 11.21% | 3.756 | 39.700 | 41.800 | 42.000 |
| 3-field object parse (linear) | ns | 70.320 | 6.770 | 9.63% | 5.934 | 73.400 | 74.860 | 75.100 |
| 8-field object parse (length-switch) | ns | 236.520 | 25.317 | 10.70% | 22.192 | 248.900 | 253.260 | 253.700 |
| arena alloc+reset cycle (4KB) | ns | 84.880 | 9.437 | 11.12% | 8.272 | 86.900 | 92.540 | 92.900 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| parse_int64 profile (best case) | 204.3M bytes/sec | 36.1M bytes/sec | 17.69% | 31.7M bytes/sec | 164.3M bytes/sec | 199.2M bytes/sec | 257.4M bytes/sec | 271.8M bytes/sec |
| parse_int64 profile (typical mixed) | 302.3M bytes/sec | 58.2M bytes/sec | 19.26% | 51.1M bytes/sec | 220.9M bytes/sec | 297.9M bytes/sec | 382.7M bytes/sec | 402.8M bytes/sec |
| parse_int64 profile (hard edge) | 484.1M bytes/sec | 95.1M bytes/sec | 19.63% | 83.3M bytes/sec | 379.4M bytes/sec | 469.5M bytes/sec | 624.5M bytes/sec | 658.0M bytes/sec |
| parse_bool profile (best case) | 736.1M bytes/sec | 139.8M bytes/sec | 18.99% | 122.5M bytes/sec | 608.2M bytes/sec | 722.7M bytes/sec | 943.5M bytes/sec | 995.1M bytes/sec |
| parse_bool profile (typical mixed) | 524.6M bytes/sec | 75.3M bytes/sec | 14.36% | 66.0M bytes/sec | 471.5M bytes/sec | 498.2M bytes/sec | 639.2M bytes/sec | 673.3M bytes/sec |
| parse_bool profile (hard edge) | 1.12B bytes/sec | 152.5M bytes/sec | 13.59% | 133.7M bytes/sec | 1.03B bytes/sec | 1.05B bytes/sec | 1.35B bytes/sec | 1.43B bytes/sec |
| validate_user profile (best case) | 2.83B bytes/sec | 408.2M bytes/sec | 14.45% | 357.8M bytes/sec | 2.45B bytes/sec | 2.71B bytes/sec | 3.44B bytes/sec | 3.62B bytes/sec |
| validate_user profile (typical mixed) | 2.56B bytes/sec | 326.3M bytes/sec | 12.74% | 286.0M bytes/sec | 2.32B bytes/sec | 2.40B bytes/sec | 3.06B bytes/sec | 3.20B bytes/sec |
| validate_user profile (hard edge) | 2.37B bytes/sec | 416.8M bytes/sec | 17.58% | 365.4M bytes/sec | 1.88B bytes/sec | 2.27B bytes/sec | 2.98B bytes/sec | 3.14B bytes/sec |

---

## Serialization Benchmarks

_Runs: 5 | Aggregation: median_

JSON serialization, string escaping, array construction

| Config | Value |
|--------|-------|
| binary | serialize_benchmark |
| binary_path | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs/benchmark/serialize_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Data Rate |
|-----------|---------|------------|-----------|
| escape_json_string (clean, return) | 22.7 ns | 44.0M ops/sec | - |
| escape_json_string_into (clean, append) | 9.9 ns | 101.1M ops/sec | - |
| escape_json_string (dirty, return) | 105.8 ns | 9.5M ops/sec | - |
| escape_json_string_into (dirty, append) | 73.3 ns | 13.6M ops/sec | - |
| escape profile (best case) | 18.1 ns | 55.4M ops/sec | 870.2M bytes/sec |
| escape profile (typical mixed) | 472.2 ns | 2.1M ops/sec | 173.9M bytes/sec |
| escape profile (hard edge) | 5840.0 ns | 171.2K ops/sec | 152.8M bytes/sec |
| serialize object profile (best case) | 97.5 ns | 10.3M ops/sec | 290.5M bytes/sec |
| serialize object profile (typical mixed) | 692.5 ns | 1.4M ops/sec | 173.3M bytes/sec |
| serialize object profile (hard edge) | 7670.0 ns | 130.4K ops/sec | 153.3M bytes/sec |
| needs_json_escaping (16 byte clean) | 4.3 ns | 234.3M ops/sec | - |
| needs_json_escaping (64 byte clean) | 4.7 ns | 214.8M ops/sec | - |
| needs_json_escaping (256 byte clean) | 8.5 ns | 117.6M ops/sec | - |
| needs_json_escaping (64 byte, escape at end) | 3.1 ns | 325.3M ops/sec | - |
| needs_json_escaping (64 byte, escape at start) | 2.8 ns | 360.8M ops/sec | - |
| serialize 5-field obj (embedded commas) | 101.5 ns | 9.8M ops/sec | - |
| serialize array 100 ints (to_chars flat buffer) | 374.6 ns | 2.7M ops/sec | - |
| serialize array 100 ints (to_chars into string) | 758.8 ns | 1.3M ops/sec | - |
| serialize array 100 ints (lookup table into string) | 201.3 ns | 5.0M ops/sec | - |
| serialize array 1000 ints (optimized) | 2390.0 ns | 419.1K ops/sec | - |
| serialize array 10000 ints (optimized) | 54150.0 ns | 18.5K ops/sec | - |
| serialize nested obj (5 levels deep) | 28.5 ns | 35.0M ops/sec | - |
| serialize 20-field obj | 545.3 ns | 1.8M ops/sec | - |
| serialize 100 nested objects | 5790.0 ns | 172.6K ops/sec | - |
| escape_json_string (10KB clean) | 329.6 ns | 3.0M ops/sec | - |
| escape_json_string (10KB heavy escaping) | 20130.0 ns | 49.7K ops/sec | - |
| escape_json_string (escape at SIMD boundary) | 599.7 ns | 1.7M ops/sec | - |
| needs_json_escaping (1KB clean) | 24.7 ns | 40.4M ops/sec | - |
| needs_json_escaping (1KB, escape at end) | 23.5 ns | 42.6M ops/sec | - |
| escape hot (same string repeated) | 14.6 ns | 68.4M ops/sec | - |
| escape cold (different strings) | 27.8 ns | 36.0M ops/sec | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| escape_json_string (clean, return) | 44.6M | 1.4M | 3.19% | 1.2M | 42.9M | 44.0M | 46.6M | 46.9M |
| escape_json_string_into (clean, append) | 101.0M | 3.2M | 3.21% | 2.8M | 96.2M | 101.1M | 104.8M | 105.0M |
| escape_json_string (dirty, return) | 9.5M | 331.4K | 3.48% | 290.5K | 9.0M | 9.5M | 10.0M | 10.0M |
| escape_json_string_into (dirty, append) | 13.4M | 443.3K | 3.30% | 388.5K | 12.7M | 13.6M | 13.9M | 13.9M |
| escape profile (best case) | 55.6M | 1.2M | 2.12% | 1.0M | 54.0M | 55.4M | 57.1M | 57.2M |
| escape profile (typical mixed) | 2.1M | 49.1K | 2.35% | 43.0K | 2.0M | 2.1M | 2.1M | 2.1M |
| escape profile (hard edge) | 178.2K | 19.9K | 11.18% | 17.5K | 161.8K | 171.2K | 208.3K | 217.3K |
| serialize object profile (best case) | 10.9M | 1.3M | 12.34% | 1.2M | 10.0M | 10.3M | 13.0M | 13.6M |
| serialize object profile (typical mixed) | 1.5M | 246.8K | 16.54% | 216.3K | 1.2M | 1.4M | 1.8M | 1.9M |
| serialize object profile (hard edge) | 138.1K | 16.7K | 12.06% | 14.6K | 127.3K | 130.4K | 163.4K | 171.3K |
| needs_json_escaping (16 byte clean) | 235.0M | 8.2M | 3.48% | 7.2M | 224.7M | 234.3M | 245.5M | 246.5M |
| needs_json_escaping (64 byte clean) | 213.9M | 8.4M | 3.92% | 7.3M | 202.1M | 214.8M | 224.3M | 225.3M |
| needs_json_escaping (256 byte clean) | 118.0M | 4.5M | 3.82% | 3.9M | 112.9M | 117.6M | 124.5M | 126.1M |
| needs_json_escaping (64 byte, escape at end) | 330.0M | 9.7M | 2.95% | 8.5M | 320.5M | 325.3M | 342.6M | 343.2M |
| needs_json_escaping (64 byte, escape at start) | 351.4M | 14.8M | 4.21% | 13.0M | 332.1M | 360.8M | 365.0M | 365.3M |
| serialize 5-field obj (embedded commas) | 9.9M | 194.7K | 1.96% | 170.7K | 9.6M | 9.8M | 10.2M | 10.2M |
| serialize array 100 ints (to_chars flat buffer) | 2.8M | 220.2K | 7.83% | 193.0K | 2.6M | 2.7M | 3.1M | 3.2M |
| serialize array 100 ints (to_chars into string) | 1.3M | 43.6K | 3.36% | 38.2K | 1.2M | 1.3M | 1.3M | 1.3M |
| serialize array 100 ints (lookup table into string) | 5.0M | 185.2K | 3.68% | 162.4K | 4.8M | 5.0M | 5.3M | 5.3M |
| serialize array 1000 ints (optimized) | 429.1K | 26.3K | 6.12% | 23.0K | 397.4K | 419.1K | 465.4K | 469.7K |
| serialize array 10000 ints (optimized) | 18.2K | 753 | 4.14% | 660 | 17.1K | 18.5K | 19.0K | 19.1K |
| serialize nested obj (5 levels deep) | 34.6M | 1.4M | 4.08% | 1.2M | 32.5M | 35.0M | 36.4M | 36.7M |
| serialize 20-field obj | 1.9M | 134.4K | 7.13% | 117.8K | 1.7M | 1.8M | 2.1M | 2.1M |
| serialize 100 nested objects | 168.6K | 9.4K | 5.59% | 8.3K | 154.8K | 172.6K | 178.8K | 179.6K |
| escape_json_string (10KB clean) | 2.8M | 371.9K | 13.10% | 326.0K | 2.1M | 3.0M | 3.1M | 3.1M |
| escape_json_string (10KB heavy escaping) | 48.9K | 1.5K | 3.07% | 1.3K | 46.3K | 49.7K | 50.2K | 50.2K |
| escape_json_string (escape at SIMD boundary) | 1.7M | 125.6K | 7.60% | 110.1K | 1.4M | 1.7M | 1.8M | 1.8M |
| needs_json_escaping (1KB clean) | 39.4M | 1.9M | 4.88% | 1.7M | 36.7M | 40.4M | 41.4M | 41.5M |
| needs_json_escaping (1KB, escape at end) | 41.5M | 1.9M | 4.61% | 1.7M | 38.5M | 42.6M | 43.2M | 43.3M |
| escape hot (same string repeated) | 69.0M | 2.0M | 2.86% | 1.7M | 66.3M | 68.4M | 71.5M | 71.8M |
| escape cold (different strings) | 35.8M | 1.0M | 2.88% | 903.5K | 34.2M | 36.0M | 36.9M | 36.9M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| escape_json_string (clean, return) | ns | 22.440 | 0.709 | 3.16% | 0.621 | 22.700 | 23.220 | 23.300 |
| escape_json_string_into (clean, append) | ns | 9.900 | 0.329 | 3.32% | 0.288 | 9.900 | 10.340 | 10.400 |
| escape_json_string (dirty, return) | ns | 105.060 | 3.622 | 3.45% | 3.175 | 105.800 | 109.700 | 110.600 |
| escape_json_string_into (dirty, append) | ns | 74.440 | 2.521 | 3.39% | 2.210 | 73.300 | 78.200 | 78.900 |
| escape profile (best case) | ns | 18.020 | 0.371 | 2.06% | 0.325 | 18.100 | 18.460 | 18.500 |
| escape profile (typical mixed) | ns | 478.020 | 11.600 | 2.43% | 10.168 | 472.200 | 495.580 | 501.200 |
| escape profile (hard edge) | ns | 5674.000 | 553.014 | 9.75% | 484.738 | 5840.000 | 6134.000 | 6180.000 |
| serialize object profile (best case) | ns | 92.760 | 9.726 | 10.48% | 8.525 | 97.500 | 99.440 | 99.700 |
| serialize object profile (typical mixed) | ns | 687.220 | 104.939 | 15.27% | 91.983 | 692.500 | 818.000 | 844.900 |
| serialize object profile (hard edge) | ns | 7332.000 | 750.664 | 10.24% | 657.986 | 7670.000 | 7824.000 | 7860.000 |
| needs_json_escaping (16 byte clean) | ns | 4.260 | 0.136 | 3.18% | 0.119 | 4.300 | 4.400 | 4.400 |
| needs_json_escaping (64 byte clean) | ns | 4.660 | 0.185 | 3.98% | 0.163 | 4.700 | 4.880 | 4.900 |
| needs_json_escaping (256 byte clean) | ns | 8.480 | 0.337 | 3.97% | 0.295 | 8.500 | 8.860 | 8.900 |
| needs_json_escaping (64 byte, escape at end) | ns | 3.020 | 0.098 | 3.24% | 0.086 | 3.100 | 3.100 | 3.100 |
| needs_json_escaping (64 byte, escape at start) | ns | 2.840 | 0.136 | 4.78% | 0.119 | 2.800 | 3.000 | 3.000 |
| serialize 5-field obj (embedded commas) | ns | 100.880 | 1.998 | 1.98% | 1.752 | 101.500 | 103.320 | 103.700 |
| serialize array 100 ints (to_chars flat buffer) | ns | 357.640 | 27.000 | 7.55% | 23.666 | 374.600 | 384.380 | 386.500 |
| serialize array 100 ints (to_chars into string) | ns | 771.800 | 26.717 | 3.46% | 23.419 | 758.800 | 811.980 | 819.700 |
| serialize array 100 ints (lookup table into string) | ns | 198.780 | 7.176 | 3.61% | 6.290 | 201.300 | 206.200 | 206.500 |
| serialize array 1000 ints (optimized) | ns | 2340.000 | 140.855 | 6.02% | 123.464 | 2390.000 | 2502.000 | 2520.000 |
| serialize array 10000 ints (optimized) | ns | 55116.000 | 2317.029 | 4.20% | 2030.966 | 54150.000 | 58294.000 | 58620.000 |
| serialize nested obj (5 levels deep) | ns | 28.940 | 1.181 | 4.08% | 1.035 | 28.500 | 30.560 | 30.800 |
| serialize 20-field obj | ns | 532.980 | 37.537 | 7.04% | 32.903 | 545.300 | 578.080 | 583.600 |
| serialize 100 nested objects | ns | 5948.000 | 339.435 | 5.71% | 297.528 | 5790.000 | 6414.000 | 6460.000 |
| escape_json_string (10KB clean) | ns | 359.620 | 57.485 | 15.98% | 50.387 | 329.600 | 448.140 | 473.200 |
| escape_json_string (10KB heavy escaping) | ns | 20472.000 | 645.582 | 3.15% | 565.877 | 20130.000 | 21444.000 | 21620.000 |
| escape_json_string (escape at SIMD boundary) | ns | 608.360 | 50.169 | 8.25% | 43.975 | 599.700 | 683.720 | 703.000 |
| needs_json_escaping (1KB clean) | ns | 25.460 | 1.244 | 4.88% | 1.090 | 24.700 | 27.100 | 27.200 |
| needs_json_escaping (1KB, escape at end) | ns | 24.160 | 1.146 | 4.75% | 1.005 | 23.500 | 25.800 | 26.000 |
| escape hot (same string repeated) | ns | 14.500 | 0.415 | 2.86% | 0.364 | 14.600 | 15.020 | 15.100 |
| escape cold (different strings) | ns | 28.020 | 0.823 | 2.94% | 0.722 | 27.800 | 29.160 | 29.300 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| escape profile (best case) | 873.1M bytes/sec | 18.5M bytes/sec | 2.12% | 16.2M bytes/sec | 847.8M bytes/sec | 870.2M bytes/sec | 896.9M bytes/sec | 899.2M bytes/sec |
| escape profile (typical mixed) | 171.9M bytes/sec | 4.0M bytes/sec | 2.35% | 3.5M bytes/sec | 163.9M bytes/sec | 173.9M bytes/sec | 174.1M bytes/sec | 174.1M bytes/sec |
| escape profile (hard edge) | 159.0M bytes/sec | 17.8M bytes/sec | 11.18% | 15.6M bytes/sec | 144.4M bytes/sec | 152.8M bytes/sec | 185.9M bytes/sec | 193.9M bytes/sec |
| serialize object profile (best case) | 309.4M bytes/sec | 38.2M bytes/sec | 12.34% | 33.5M bytes/sec | 284.1M bytes/sec | 290.5M bytes/sec | 367.9M bytes/sec | 385.1M bytes/sec |
| serialize object profile (typical mixed) | 179.1M bytes/sec | 29.6M bytes/sec | 16.54% | 26.0M bytes/sec | 142.0M bytes/sec | 173.3M bytes/sec | 221.9M bytes/sec | 232.7M bytes/sec |
| serialize object profile (hard edge) | 162.3M bytes/sec | 19.6M bytes/sec | 12.06% | 17.2M bytes/sec | 149.6M bytes/sec | 153.3M bytes/sec | 192.0M bytes/sec | 201.4M bytes/sec |

---

## Router Benchmarks

_Runs: 5 | Aggregation: median_

HTTP routing dispatch performance

| Config | Value |
|--------|-------|
| binary | router_benchmark |
| binary_path | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs/benchmark/router_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Router dispatch (hits) | 1.5M ops/sec | 0.482 us | 5.650 us | 8.087 us | 0 |
| Router dispatch (not found) | 1.8M ops/sec | 0.400 us | 5.523 us | 8.720 us | 0 |
| Router dispatch (405) | 1.2M ops/sec | 0.598 us | 6.635 us | 10.277 us | 0 |
| Large router (hits) | 1.1M ops/sec | 0.642 us | 6.274 us | 9.251 us | 0 |
| Large router (not found) | 1.2M ops/sec | 0.605 us | 6.135 us | 8.930 us | 0 |
| Large router (405) | 952.7K ops/sec | 0.750 us | 6.782 us | 9.660 us | 0 |
| Large router (edge cases) | 791.5K ops/sec | 0.904 us | 7.209 us | 10.459 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Router dispatch (hits) | 1.5M | 111.2K | 7.61% | 97.5K | 1.3M | 1.5M | 1.6M | 1.6M |
| Router dispatch (not found) | 1.7M | 196.7K | 11.55% | 172.4K | 1.3M | 1.8M | 1.8M | 1.8M |
| Router dispatch (405) | 1.1M | 131.1K | 11.94% | 114.9K | 848.1K | 1.2M | 1.2M | 1.2M |
| Large router (hits) | 1.1M | 116.1K | 10.64% | 101.8K | 864.0K | 1.1M | 1.2M | 1.2M |
| Large router (not found) | 1.2M | 65.8K | 5.57% | 57.7K | 1.1M | 1.2M | 1.2M | 1.3M |
| Large router (405) | 952.4K | 24.9K | 2.62% | 21.9K | 914.3K | 952.7K | 984.3K | 989.0K |
| Large router (edge cases) | 807.5K | 23.0K | 2.85% | 20.2K | 787.0K | 791.5K | 836.5K | 837.1K |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Router dispatch (hits) | p99 us | 5.761 | 0.536 | 9.31% | 0.470 | 5.650 | 6.505 | 6.607 |
| Router dispatch (not found) | p99 us | 5.506 | 0.524 | 9.52% | 0.459 | 5.523 | 6.243 | 6.384 |
| Router dispatch (405) | p99 us | 6.859 | 0.511 | 7.44% | 0.448 | 6.635 | 7.637 | 7.857 |
| Large router (hits) | p99 us | 6.506 | 0.747 | 11.48% | 0.655 | 6.274 | 7.640 | 7.974 |
| Large router (not found) | p99 us | 6.223 | 0.433 | 6.97% | 0.380 | 6.135 | 6.856 | 6.976 |
| Large router (405) | p99 us | 6.736 | 0.190 | 2.83% | 0.167 | 6.782 | 6.966 | 6.990 |
| Large router (edge cases) | p99 us | 7.148 | 0.191 | 2.68% | 0.168 | 7.209 | 7.354 | 7.366 |

---

## Generated API Dispatch Benchmarks

_Runs: 5 | Aggregation: median_

Generated router bindings, param extraction, parse and serialize on ready request objects

| Config | Value |
|--------|-------|
| binary | generated_api_benchmark |
| binary_path | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs/benchmark/generated_api_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Generated API dispatch+parse | 2.1M ops/sec | 0.230 us | 0.692 us | 1.753 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Generated API dispatch+parse | 2.0M | 113.0K | 5.63% | 99.0K | 1.8M | 2.1M | 2.1M | 2.1M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Generated API dispatch+parse | p99 us | 0.699 | 0.015 | 2.09% | 0.013 | 0.692 | 0.719 | 0.721 |

---

## Generated API Mixed Workload Benchmarks

_Runs: 5 | Aggregation: median_

Generated stack mixed valid/error workload on benchmark_api spec

| Config | Value |
|--------|-------|
| binary | benchmark_api_codegen_benchmark |
| binary_path | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs/benchmark/benchmark_api_codegen_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_codegen_valid_mix | 507.5K ops/sec | 0.851 us | 5.090 us | 9.738 us | 159.380 us | 0 |
| benchmark_api_codegen_error_mix | 1.1M ops/sec | 0.551 us | 0.972 us | 1.403 us | 24.757 us | 120000 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| benchmark_api_codegen_valid_mix | 507.6K | 14.6K | 2.88% | 12.8K | 483.2K | 507.5K | 523.3K | 523.7K |
| benchmark_api_codegen_error_mix | 1.1M | 25.3K | 2.36% | 22.2K | 1.0M | 1.1M | 1.1M | 1.1M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| benchmark_api_codegen_valid_mix | p99 us | 9.864 | 0.445 | 4.51% | 0.390 | 9.738 | 10.526 | 10.660 |
| benchmark_api_codegen_error_mix | p99 us | 1.399 | 0.045 | 3.25% | 0.040 | 1.403 | 1.449 | 1.453 |

---

## Generated API Framework Path Benchmarks

_Runs: 5 | Aggregation: median_

Generated stack valid/error workload on benchmark_api spec with minimal handlers

| Config | Value |
|--------|-------|
| binary | benchmark_api_framework_benchmark |
| binary_path | /mnt/bench/katana-bench-20260314/KATANA/build/deep-bench-5runs/benchmark/benchmark_api_framework_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_framework_valid_mix | 955.8K ops/sec | 0.641 us | 1.193 us | 2.204 us | 34.775 us | 0 |
| benchmark_api_framework_error_mix | 1.1M ops/sec | 0.541 us | 0.982 us | 1.413 us | 23.284 us | 120000 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| benchmark_api_framework_valid_mix | 955.1K | 33.0K | 3.45% | 28.9K | 905.4K | 955.8K | 998.7K | 1.0M |
| benchmark_api_framework_error_mix | 1.1M | 35.6K | 3.21% | 31.2K | 1.1M | 1.1M | 1.2M | 1.2M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| benchmark_api_framework_valid_mix | p99 us | 2.178 | 0.075 | 3.46% | 0.066 | 2.204 | 2.260 | 2.264 |
| benchmark_api_framework_error_mix | p99 us | 1.375 | 0.069 | 5.02% | 0.061 | 1.413 | 1.432 | 1.432 |

---

## HTTP Load Benchmark (hello_world_server canonical pipeline via wrk)

_Runs: 5 | Aggregation: median_

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
| wrk hello_world GET / depth10 | 1.7M req/sec | 170.0M bytes/sec | 2800.000 us | 1839.000 us | 8388.000 us | 14888.000 us | 26586.000 us | 43158.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth10 | 1.7M | 33.9K | 2.04% | 29.7K | 1.6M | 1.7M | 1.7M | 1.7M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk hello_world GET / depth10 | avg us | 2798.000 | 63.372 | 2.26% | 55.548 | 2800.000 | 2884.000 | 2900.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth10 | 169.7M bytes/sec | 3.5M bytes/sec | 2.04% | 3.0M bytes/sec | 163.7M bytes/sec | 170.0M bytes/sec | 173.3M bytes/sec | 173.4M bytes/sec |

---

## HTTP Load Benchmark (compute_api canonical pipeline via wrk)

_Runs: 5 | Aggregation: median_

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
| wrk compute_api POST /compute/sum depth10 | 1.1M req/sec | 109.3M bytes/sec | 3430.000 us | 2661.000 us | 8877.000 us | 13201.000 us | 19497.000 us | 37322.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | 1.1M | 6.5K | 0.57% | 5.7K | 1.1M | 1.1M | 1.1M | 1.1M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | avg us | 3426.000 | 22.450 | 0.66% | 19.678 | 3430.000 | 3450.000 | 3450.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | 109.0M bytes/sec | 626.1K bytes/sec | 0.57% | 548.8K bytes/sec | 107.8M bytes/sec | 109.3M bytes/sec | 109.5M bytes/sec | 109.5M bytes/sec |

---

## HTTP Load Benchmark (hello_world_server peak pipeline via wrk)

_Runs: 5 | Aggregation: median_

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
| wrk hello_world GET / depth20 | 2.5M req/sec | 256.1M bytes/sec | 2850.000 us | 2153.000 us | 7468.000 us | 11558.000 us | 19518.000 us | 35415.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth20 | 2.5M | 11.5K | 0.46% | 10.0K | 2.5M | 2.5M | 2.5M | 2.5M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk hello_world GET / depth20 | avg us | 2834.000 | 36.661 | 1.29% | 32.134 | 2850.000 | 2870.000 | 2870.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth20 | 256.1M bytes/sec | 1.2M bytes/sec | 0.46% | 1.0M bytes/sec | 254.3M bytes/sec | 256.1M bytes/sec | 257.4M bytes/sec | 257.5M bytes/sec |

---

## HTTP Load Benchmark (compute_api peak pipeline via wrk)

_Runs: 5 | Aggregation: median_

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
| wrk compute_api POST /compute/sum depth40 | 1.5M req/sec | 145.8M bytes/sec | 7350.000 us | 6953.000 us | 15916.000 us | 0.000 us | 0.000 us | 63416.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | 1.5M | 24.9K | 1.62% | 21.8K | 1.5M | 1.5M | 1.6M | 1.6M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | avg us | 7380.000 | 103.923 | 1.41% | 91.093 | 7350.000 | 7534.000 | 7570.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | 147.4M bytes/sec | 2.4M bytes/sec | 1.62% | 2.1M bytes/sec | 145.2M bytes/sec | 145.8M bytes/sec | 150.7M bytes/sec | 151.0M bytes/sec |

---

## Legacy E2E HTTP Keep-Alive Benchmark

_Runs: 5 | Aggregation: median_

Legacy Python keep-alive scenario for compute_api; kept for compatibility only

| Config | Value |
|--------|-------|
| connections | 16 |
| kind | e2e_keepalive |
| port | 18082 |
| requests_per_connection | 200 |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Latency max | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|-------------|--------|
| HTTP E2E Keep-Alive (16 conn x 200 req) | 2.0K req/sec | 4871.079 us | 23527.530 us | 36622.630 us | 52307.692 us | 64039.791 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| HTTP E2E Keep-Alive (16 conn x 200 req) | 2.1K | 69 | 3.34% | 60 | 2.0K | 2.0K | 2.2K | 2.2K |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| HTTP E2E Keep-Alive (16 conn x 200 req) | p99 us | 35938.992 | 1418.393 | 3.95% | 1243.276 | 36622.630 | 37257.448 | 37360.040 |

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
