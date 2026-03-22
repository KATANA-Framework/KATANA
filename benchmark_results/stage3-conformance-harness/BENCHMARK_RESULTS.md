# KATANA Benchmark Results

> Last updated: 2026-03-22 23:14:57
> Commit: 6fdc7df

## Summary

- Stability verdict: contains noisy metrics, inspect before trusting small deltas
- Noisy metrics (CV > 20.0%): 38
- Severely noisy metrics (CV > 40.0%): 8
- E2E network stages included: yes

> **Note**: Results shown use median-of-N aggregation across default 10 run(s), overrides: stage 1=15, stage 2=10, stage 3=10, stage 4=10, stage 6=10, stage 7=10, stage 8=10, stage 9=5, stage 10=5, stage 11=5, stage 12=5. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Environment

| Key | Value |
|-----|-------|
| affinity_cpus | 12 |
| benchmark_dir | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local/benchmark |
| build_dir | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local |
| cmake | CMAKE_BUILD_TYPE=Release; CMAKE_CXX_COMPILER=/usr/bin/c++; CMAKE_CXX_FLAGS=; CMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG |
| cpu_governor | powersave |
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
- Noisy metrics: 38
- Severely noisy metrics: 8

| Top Noisy Metric | CV |
|------------------|----|
| stage12.wrk compute_api POST /compute/sum depth40.latency_p99_us | 200.00% |
| stage12.wrk compute_api POST /compute/sum depth40.latency_p95_us | 124.69% |
| stage6.Generated API dispatch+parse.latency_p999_us | 120.62% |
| stage1.Ring Buffer Queue (Core Saturation 6x6).retries_per_op_total | 115.73% |
| stage1.Ring Buffer Queue (Concurrent 4x4).retries_per_op_total | 93.97% |
| stage11.wrk hello_world GET / depth20.latency_p999_us | 86.51% |
| stage11.wrk hello_world GET / depth20.latency_p99_us | 73.90% |
| stage10.wrk compute_api POST /compute/sum depth10.latency_p999_us | 49.23% |
| stage1.Ring Buffer Queue (Core Saturation 6x6).latency_p999_us | 38.99% |
| stage9.wrk hello_world GET / depth10.latency_p999_us | 36.20% |

---

## E2E Summary

> **Note**: For pipeline stages, `wrk` latency metrics reflect one scripted batch/request() call,
> not isolated single-request service time. Throughput and error-free completion are the primary
> comparable signals across different pipeline depths.

| Stage | Profile | Benchmark | Workers | wrk t/c | Depth | Duration | Throughput | Avg Latency | p50 | p95 | p99 | Data Rate | Errors |
|-------|---------|-----------|---------|---------|-------|----------|------------|-------------|-----|-----|-----|-----------|--------|
| 9 | canonical | wrk hello_world GET / depth10 | 4 | 4/512 | 10 | 10s | 3.5M req/sec | 799.550 us | 664.000 us | 1658.000 us | 2552.000 us | 356.0M bytes/sec | 0 |
| 10 | canonical | wrk compute_api POST /compute/sum depth10 | 4 | 4/512 | 10 | 10s | 2.5M req/sec | 1310.000 us | 1152.000 us | 2678.000 us | 3898.000 us | 238.9M bytes/sec | 0 |
| 11 | peak | wrk hello_world GET / depth20 | 4 | 4/512 | 20 | 5s | 5.3M req/sec | 1140.000 us | 925.000 us | 2661.000 us | 3953.000 us | 537.3M bytes/sec | 0 |
| 12 | peak | wrk compute_api POST /compute/sum depth40 | 4 | 4/512 | 40 | 5s | 3.5M req/sec | 3140.000 us | 3396.000 us | 0.000 us | 0.000 us | 334.0M bytes/sec | 0 |

---

## Core Runtime Benchmarks

_Runs: 15 | Aggregation: median_

Ring buffer, circular buffer, SIMD operations, arena allocation

| Config | Value |
|--------|-------|
| binary | performance_benchmark |
| binary_path | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local/benchmark/performance_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Retries/op | Errors |
|-----------|------------|-------------|-------------|--------------|------------|--------|
| Ring Buffer Queue (Single Thread) | 222.2M ops/sec | 0.004 us | 0.006 us | 0.007 us | - | 0 |
| Ring Buffer Queue (Concurrent 2x2) | 30.7M ops/sec | 0.054 us | 0.191 us | 0.256 us | 0.049 | 0 |
| Ring Buffer Queue (Concurrent 4x4) | 15.5M ops/sec | 0.238 us | 0.424 us | 0.534 us | 0.025 | 0 |
| Ring Buffer Queue (Core Saturation 6x6) | 12.4M ops/sec | 0.444 us | 0.788 us | 1.436 us | 0.227 | 0 |
| Ring Buffer Queue (High Contention 8x8) | 8.0M ops/sec | 0.384 us | 0.682 us | 163.317 us | 15.657 | 0 |
| Ring Buffer Queue (Extreme Contention 12x12) | 5.6M ops/sec | 0.274 us | 1.062 us | 438.249 us | 43.094 | 0 |
| Ring Buffer Queue (Max Contention 16x16) | 4.5M ops/sec | 0.298 us | 1.124 us | 750.705 us | 68.981 | 0 |
| Ring Buffer Queue (Oversubscribed 14x14) | 5.1M ops/sec | 0.309 us | 1.206 us | 562.907 us | 51.259 | 0 |
| Circular Buffer | 77.9M ops/sec | 0.013 us | 0.016 us | 0.017 us | - | 0 |
| SIMD CRLF Search (1.5KB buffer) | 62.7M ops/sec | 0.015 us | 0.017 us | 0.024 us | - | 0 |
| SIMD CRLF Search (16KB buffer) | 5.3M ops/sec | 0.187 us | 0.200 us | 0.375 us | - | 0 |
| SIMD CRLF Search (32KB buffer) | 2.5M ops/sec | 0.392 us | 0.400 us | 0.659 us | - | 0 |
| SIMD CRLF Search (64KB buffer) | 1.3M ops/sec | 0.776 us | 0.802 us | 1.312 us | - | 0 |
| SIMD CRLF Search (128KB buffer) | 646.4K ops/sec | 1.538 us | 1.591 us | 2.233 us | - | 0 |
| HTTP Parser (Complete Request) | 1.9M ops/sec | 0.518 us | 0.585 us | 0.914 us | - | 0 |
| HTTP Parser (Fragmented Request) | 2.2M ops/sec | 0.456 us | 0.504 us | 0.608 us | - | 0 |
| Arena Allocations (64B objects) | 21.8M ops/sec | - | - | - | - | 0 |
| Memory Allocations (String Queue) | 34.4M ops/sec | - | - | - | - | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | 221.5M | 5.1M | 2.29% | 2.6M | 213.2M | 222.2M | 228.8M | 231.7M |
| Ring Buffer Queue (Concurrent 2x2) | 30.2M | 1.5M | 4.96% | 758.0K | 26.9M | 30.7M | 32.0M | 32.8M |
| Ring Buffer Queue (Concurrent 4x4) | 15.7M | 428.1K | 2.73% | 216.6K | 15.1M | 15.5M | 16.5M | 16.7M |
| Ring Buffer Queue (Core Saturation 6x6) | 12.4M | 347.1K | 2.80% | 175.6K | 12.0M | 12.4M | 13.0M | 13.2M |
| Ring Buffer Queue (High Contention 8x8) | 8.1M | 629.5K | 7.74% | 318.6K | 7.0M | 8.0M | 9.2M | 9.4M |
| Ring Buffer Queue (Extreme Contention 12x12) | 5.5M | 416.3K | 7.63% | 210.7K | 4.7M | 5.6M | 6.0M | 6.2M |
| Ring Buffer Queue (Max Contention 16x16) | 4.6M | 649.8K | 14.08% | 328.9K | 3.7M | 4.5M | 5.9M | 6.2M |
| Ring Buffer Queue (Oversubscribed 14x14) | 5.2M | 554.2K | 10.69% | 280.4K | 4.4M | 5.1M | 6.1M | 6.5M |
| Circular Buffer | 77.8M | 516.1K | 0.66% | 261.2K | 76.7M | 77.9M | 78.6M | 78.8M |
| SIMD CRLF Search (1.5KB buffer) | 62.6M | 741.8K | 1.19% | 375.4K | 60.7M | 62.7M | 63.4M | 63.6M |
| SIMD CRLF Search (16KB buffer) | 5.2M | 128.3K | 2.47% | 64.9K | 4.9M | 5.3M | 5.3M | 5.3M |
| SIMD CRLF Search (32KB buffer) | 2.5M | 15.1K | 0.59% | 7.6K | 2.5M | 2.5M | 2.6M | 2.6M |
| SIMD CRLF Search (64KB buffer) | 1.3M | 7.7K | 0.60% | 3.9K | 1.3M | 1.3M | 1.3M | 1.3M |
| SIMD CRLF Search (128KB buffer) | 644.8K | 4.4K | 0.69% | 2.2K | 631.4K | 646.4K | 647.8K | 648.0K |
| HTTP Parser (Complete Request) | 1.9M | 15.1K | 0.79% | 7.6K | 1.9M | 1.9M | 1.9M | 1.9M |
| HTTP Parser (Fragmented Request) | 2.2M | 29.6K | 1.36% | 15.0K | 2.1M | 2.2M | 2.2M | 2.2M |
| Arena Allocations (64B objects) | 21.7M | 926.4K | 4.27% | 468.8K | 19.5M | 21.8M | 22.8M | 22.9M |
| Memory Allocations (String Queue) | 34.3M | 362.7K | 1.06% | 183.5K | 33.1M | 34.4M | 34.6M | 34.6M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | p99 us | 0.006 | 0.001 | 15.87% | 0.000 | 0.006 | 0.007 | 0.008 |
| Ring Buffer Queue (Concurrent 2x2) | p99 us | 0.187 | 0.013 | 6.98% | 0.007 | 0.191 | 0.203 | 0.219 |
| Ring Buffer Queue (Concurrent 4x4) | p99 us | 0.438 | 0.041 | 9.45% | 0.021 | 0.424 | 0.513 | 0.573 |
| Ring Buffer Queue (Core Saturation 6x6) | p99 us | 0.810 | 0.088 | 10.83% | 0.044 | 0.788 | 1.017 | 1.032 |
| Ring Buffer Queue (High Contention 8x8) | p99 us | 0.676 | 0.067 | 9.89% | 0.034 | 0.682 | 0.774 | 0.788 |
| Ring Buffer Queue (Extreme Contention 12x12) | p99 us | 1.082 | 0.122 | 11.27% | 0.062 | 1.062 | 1.237 | 1.246 |
| Ring Buffer Queue (Max Contention 16x16) | p99 us | 1.125 | 0.114 | 10.13% | 0.058 | 1.124 | 1.325 | 1.345 |
| Ring Buffer Queue (Oversubscribed 14x14) | p99 us | 1.205 | 0.134 | 11.16% | 0.068 | 1.206 | 1.404 | 1.412 |
| Circular Buffer | p99 us | 0.015 | 0.001 | 6.72% | 0.001 | 0.016 | 0.017 | 0.017 |
| SIMD CRLF Search (1.5KB buffer) | p99 us | 0.019 | 0.002 | 13.02% | 0.001 | 0.017 | 0.024 | 0.024 |
| SIMD CRLF Search (16KB buffer) | p99 us | 0.227 | 0.057 | 25.22% | 0.029 | 0.200 | 0.325 | 0.412 |
| SIMD CRLF Search (32KB buffer) | p99 us | 0.429 | 0.066 | 15.37% | 0.033 | 0.400 | 0.572 | 0.596 |
| SIMD CRLF Search (64KB buffer) | p99 us | 0.898 | 0.167 | 18.58% | 0.084 | 0.802 | 1.268 | 1.299 |
| SIMD CRLF Search (128KB buffer) | p99 us | 1.764 | 0.364 | 20.63% | 0.184 | 1.591 | 2.559 | 2.742 |
| HTTP Parser (Complete Request) | p99 us | 0.599 | 0.056 | 9.34% | 0.028 | 0.585 | 0.697 | 0.720 |
| HTTP Parser (Fragmented Request) | p99 us | 0.544 | 0.135 | 24.80% | 0.068 | 0.504 | 0.789 | 0.999 |

### Contention Stability (Retries/op)

| Benchmark | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|
| Ring Buffer Queue (Concurrent 2x2) | 0.050 | 0.013 | 26.30% | 0.007 | 0.049 | 0.072 | 0.084 |
| Ring Buffer Queue (Concurrent 4x4) | 0.084 | 0.079 | 93.97% | 0.040 | 0.025 | 0.198 | 0.210 |
| Ring Buffer Queue (Core Saturation 6x6) | 0.553 | 0.640 | 115.73% | 0.324 | 0.227 | 1.885 | 2.046 |
| Ring Buffer Queue (High Contention 8x8) | 15.816 | 4.733 | 29.92% | 2.395 | 15.657 | 22.540 | 23.498 |
| Ring Buffer Queue (Extreme Contention 12x12) | 45.163 | 7.572 | 16.77% | 3.832 | 43.094 | 56.657 | 56.751 |
| Ring Buffer Queue (Max Contention 16x16) | 66.818 | 13.769 | 20.61% | 6.968 | 68.981 | 86.745 | 92.542 |
| Ring Buffer Queue (Oversubscribed 14x14) | 51.581 | 8.384 | 16.25% | 4.243 | 51.259 | 63.755 | 65.537 |

---

## Codegen Quality Benchmarks

_Runs: 10 | Aggregation: median_

JSON parsing, string escaping, key dispatch

| Config | Value |
|--------|-------|
| binary | codegen_quality_benchmark |
| binary_path | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local/benchmark/codegen_quality_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Tail p95 | Tail p99 | Data Rate |
|-----------|---------|------------|----------|----------|-----------|
| parse_int64 (simple) | 7.4 ns | 134.3M ops/sec | - | - | - |
| parse_int64 (negative) | 7.7 ns | 129.8M ops/sec | - | - | - |
| parse_double | 14.2 ns | 70.3M ops/sec | - | - | - |
| parse_bool (strict validation) | 1.6 ns | 633.6M ops/sec | - | - | - |
| parse_bool (mixed true/false/quoted) | 4.3 ns | 233.0M ops/sec | - | - | - |
| parse_int64 profile (best case) | 10.8 ns | 93.0M ops/sec | 40.0 ns | 40.0 ns | 256.7M bytes/sec |
| parse_int64 profile (typical mixed) | 11.7 ns | 85.8M ops/sec | 40.0 ns | 41.0 ns | 430.4M bytes/sec |
| parse_int64 profile (hard edge) | 15.1 ns | 66.3M ops/sec | 40.0 ns | 41.0 ns | 676.4M bytes/sec |
| parse_bool profile (best case) | 3.9 ns | 259.3M ops/sec | 30.0 ns | 31.0 ns | 1.29B bytes/sec |
| parse_bool profile (typical mixed) | 8.4 ns | 119.0M ops/sec | 30.0 ns | 31.0 ns | 961.9M bytes/sec |
| parse_bool profile (hard edge) | 12.6 ns | 79.3M ops/sec | 30.0 ns | 31.0 ns | 1.70B bytes/sec |
| validate_user profile (best case) | 7.7 ns | 129.4M ops/sec | 30.0 ns | 31.0 ns | 3.97B bytes/sec |
| validate_user profile (typical mixed) | 9.5 ns | 105.0M ops/sec | 30.0 ns | 31.0 ns | 3.28B bytes/sec |
| validate_user profile (hard edge) | 8.0 ns | 125.0M ops/sec | 30.0 ns | 31.0 ns | 4.05B bytes/sec |
| needs_json_escaping (clean, 58 chars) | 10.0 ns | 100.2M ops/sec | - | - | - |
| needs_json_escaping (dirty, 25 chars) | 2.3 ns | 442.9M ops/sec | - | - | - |
| escape_json_string (no-alloc path) | 12.6 ns | 79.1M ops/sec | - | - | - |
| escape_json_string_into (append) | 7.7 ns | 130.3M ops/sec | - | - | - |
| skip_value (nested obj with strings) | 34.7 ns | 28.8M ops/sec | - | - | - |
| 3-field object parse (linear) | 36.9 ns | 27.1M ops/sec | - | - | - |
| 8-field object parse (length-switch) | 116.7 ns | 8.6M ops/sec | - | - | - |
| arena alloc+reset cycle (4KB) | 58.7 ns | 17.0M ops/sec | - | - | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| parse_int64 (simple) | 134.3M | 99.5K | 0.07% | 61.7K | 134.2M | 134.3M | 134.5M | 134.5M |
| parse_int64 (negative) | 129.7M | 352.2K | 0.27% | 218.3K | 129.1M | 129.8M | 130.3M | 130.5M |
| parse_double | 70.2M | 285.2K | 0.41% | 176.8K | 69.5M | 70.3M | 70.4M | 70.5M |
| parse_bool (strict validation) | 633.8M | 544.0K | 0.09% | 337.2K | 633.1M | 633.6M | 634.7M | 635.2M |
| parse_bool (mixed true/false/quoted) | 232.8M | 805.3K | 0.35% | 499.2K | 230.5M | 233.0M | 233.3M | 233.4M |
| parse_int64 profile (best case) | 93.0M | 196.0K | 0.21% | 121.5K | 92.5M | 93.0M | 93.2M | 93.2M |
| parse_int64 profile (typical mixed) | 85.7M | 162.2K | 0.19% | 100.5K | 85.4M | 85.8M | 86.0M | 86.0M |
| parse_int64 profile (hard edge) | 66.4M | 227.2K | 0.34% | 140.8K | 66.1M | 66.3M | 66.7M | 66.8M |
| parse_bool profile (best case) | 258.7M | 1.9M | 0.75% | 1.2M | 255.1M | 259.3M | 261.0M | 261.3M |
| parse_bool profile (typical mixed) | 119.3M | 1.2M | 1.00% | 738.2K | 117.8M | 119.0M | 121.4M | 122.0M |
| parse_bool profile (hard edge) | 79.1M | 912.1K | 1.15% | 565.3K | 77.7M | 79.3M | 80.3M | 80.4M |
| validate_user profile (best case) | 128.1M | 3.4M | 2.67% | 2.1M | 122.8M | 129.4M | 131.7M | 132.2M |
| validate_user profile (typical mixed) | 104.4M | 1.5M | 1.43% | 923.8K | 101.4M | 105.0M | 105.9M | 106.0M |
| validate_user profile (hard edge) | 124.9M | 2.0M | 1.61% | 1.2M | 120.6M | 125.0M | 127.9M | 128.1M |
| needs_json_escaping (clean, 58 chars) | 100.1M | 1.1M | 1.09% | 674.5K | 98.4M | 100.2M | 101.7M | 102.3M |
| needs_json_escaping (dirty, 25 chars) | 443.1M | 533.5K | 0.12% | 330.7K | 442.2M | 442.9M | 444.0M | 444.1M |
| escape_json_string (no-alloc path) | 79.0M | 453.6K | 0.57% | 281.1K | 77.7M | 79.1M | 79.3M | 79.4M |
| escape_json_string_into (append) | 130.4M | 144.0K | 0.11% | 89.3K | 130.2M | 130.3M | 130.6M | 130.7M |
| skip_value (nested obj with strings) | 29.2M | 907.3K | 3.10% | 562.4K | 28.2M | 28.8M | 30.6M | 30.6M |
| 3-field object parse (linear) | 27.1M | 71.4K | 0.26% | 44.3K | 26.9M | 27.1M | 27.2M | 27.2M |
| 8-field object parse (length-switch) | 8.5M | 163.5K | 1.93% | 101.3K | 8.2M | 8.6M | 8.7M | 8.7M |
| arena alloc+reset cycle (4KB) | 17.0M | 181.0K | 1.06% | 112.2K | 16.5M | 17.0M | 17.2M | 17.2M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| parse_int64 (simple) | ns | 7.420 | 0.040 | 0.54% | 0.025 | 7.400 | 7.500 | 7.500 |
| parse_int64 (negative) | ns | 7.700 | 0.000 | 0.00% | 0.000 | 7.700 | 7.700 | 7.700 |
| parse_double | ns | 14.230 | 0.064 | 0.45% | 0.040 | 14.200 | 14.355 | 14.400 |
| parse_bool (strict validation) | ns | 1.600 | 0.000 | 0.00% | 0.000 | 1.600 | 1.600 | 1.600 |
| parse_bool (mixed true/false/quoted) | ns | 4.300 | 0.000 | 0.00% | 0.000 | 4.300 | 4.300 | 4.300 |
| parse_int64 profile (best case) | tail p99 ns | 40.400 | 0.490 | 1.21% | 0.304 | 40.000 | 41.000 | 41.000 |
| parse_int64 profile (typical mixed) | tail p99 ns | 40.600 | 0.490 | 1.21% | 0.304 | 41.000 | 41.000 | 41.000 |
| parse_int64 profile (hard edge) | tail p99 ns | 40.600 | 0.490 | 1.21% | 0.304 | 41.000 | 41.000 | 41.000 |
| parse_bool profile (best case) | tail p99 ns | 30.600 | 0.490 | 1.60% | 0.304 | 31.000 | 31.000 | 31.000 |
| parse_bool profile (typical mixed) | tail p99 ns | 30.700 | 0.458 | 1.49% | 0.284 | 31.000 | 31.000 | 31.000 |
| parse_bool profile (hard edge) | tail p99 ns | 34.700 | 4.540 | 13.08% | 2.814 | 31.000 | 40.550 | 41.000 |
| validate_user profile (best case) | tail p99 ns | 33.500 | 6.185 | 18.46% | 3.833 | 31.000 | 45.500 | 50.000 |
| validate_user profile (typical mixed) | tail p99 ns | 31.600 | 2.835 | 8.97% | 1.757 | 31.000 | 35.950 | 40.000 |
| validate_user profile (hard edge) | tail p99 ns | 31.600 | 2.835 | 8.97% | 1.757 | 31.000 | 35.950 | 40.000 |
| needs_json_escaping (clean, 58 chars) | ns | 10.000 | 0.118 | 1.18% | 0.073 | 10.000 | 10.200 | 10.200 |
| needs_json_escaping (dirty, 25 chars) | ns | 2.300 | 0.000 | 0.00% | 0.000 | 2.300 | 2.300 | 2.300 |
| escape_json_string (no-alloc path) | ns | 12.630 | 0.090 | 0.71% | 0.056 | 12.600 | 12.765 | 12.900 |
| escape_json_string_into (append) | ns | 7.700 | 0.000 | 0.00% | 0.000 | 7.700 | 7.700 | 7.700 |
| skip_value (nested obj with strings) | ns | 34.230 | 1.065 | 3.11% | 0.660 | 34.700 | 35.310 | 35.400 |
| 3-field object parse (linear) | ns | 36.890 | 0.094 | 0.26% | 0.058 | 36.900 | 37.055 | 37.100 |
| 8-field object parse (length-switch) | ns | 117.830 | 2.299 | 1.95% | 1.425 | 116.650 | 121.420 | 121.600 |
| arena alloc+reset cycle (4KB) | ns | 58.820 | 0.629 | 1.07% | 0.390 | 58.650 | 59.860 | 60.400 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| parse_int64 profile (best case) | 256.6M bytes/sec | 540.8K bytes/sec | 0.21% | 335.2K bytes/sec | 255.3M bytes/sec | 256.7M bytes/sec | 257.2M bytes/sec | 257.2M bytes/sec |
| parse_int64 profile (typical mixed) | 430.2M bytes/sec | 813.5K bytes/sec | 0.19% | 504.2K bytes/sec | 428.4M bytes/sec | 430.4M bytes/sec | 431.2M bytes/sec | 431.3M bytes/sec |
| parse_int64 profile (hard edge) | 677.1M bytes/sec | 2.3M bytes/sec | 0.34% | 1.4M bytes/sec | 673.6M bytes/sec | 676.4M bytes/sec | 680.7M bytes/sec | 681.2M bytes/sec |
| parse_bool profile (best case) | 1.29B bytes/sec | 9.7M bytes/sec | 0.75% | 6.0M bytes/sec | 1.27B bytes/sec | 1.29B bytes/sec | 1.30B bytes/sec | 1.30B bytes/sec |
| parse_bool profile (typical mixed) | 964.6M bytes/sec | 9.6M bytes/sec | 1.00% | 6.0M bytes/sec | 952.3M bytes/sec | 961.9M bytes/sec | 981.7M bytes/sec | 986.5M bytes/sec |
| parse_bool profile (hard edge) | 1.70B bytes/sec | 19.6M bytes/sec | 1.15% | 12.1M bytes/sec | 1.67B bytes/sec | 1.70B bytes/sec | 1.73B bytes/sec | 1.73B bytes/sec |
| validate_user profile (best case) | 3.93B bytes/sec | 104.8M bytes/sec | 2.67% | 64.9M bytes/sec | 3.77B bytes/sec | 3.97B bytes/sec | 4.04B bytes/sec | 4.05B bytes/sec |
| validate_user profile (typical mixed) | 3.26B bytes/sec | 46.5M bytes/sec | 1.43% | 28.8M bytes/sec | 3.16B bytes/sec | 3.28B bytes/sec | 3.31B bytes/sec | 3.31B bytes/sec |
| validate_user profile (hard edge) | 4.04B bytes/sec | 65.1M bytes/sec | 1.61% | 40.4M bytes/sec | 3.91B bytes/sec | 4.05B bytes/sec | 4.14B bytes/sec | 4.15B bytes/sec |

---

## Serialization Benchmarks

_Runs: 10 | Aggregation: median_

JSON serialization, string escaping, array construction

| Config | Value |
|--------|-------|
| binary | serialize_benchmark |
| binary_path | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local/benchmark/serialize_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Data Rate |
|-----------|---------|------------|-----------|
| escape_json_string (clean, return) | 13.0 ns | 76.7M ops/sec | - |
| escape_json_string_into (clean, append) | 7.5 ns | 133.1M ops/sec | - |
| escape_json_string (dirty, return) | 74.1 ns | 13.5M ops/sec | - |
| escape_json_string_into (dirty, append) | 57.8 ns | 17.3M ops/sec | - |
| escape profile (best case) | 14.0 ns | 71.5M ops/sec | 1.12B bytes/sec |
| escape profile (typical mixed) | 352.9 ns | 2.8M ops/sec | 232.7M bytes/sec |
| escape profile (hard edge) | 4405.0 ns | 227.1K ops/sec | 202.6M bytes/sec |
| serialize object profile (best case) | 71.4 ns | 14.0M ops/sec | 396.8M bytes/sec |
| serialize object profile (typical mixed) | 428.5 ns | 2.3M ops/sec | 280.1M bytes/sec |
| serialize object profile (hard edge) | 4615.0 ns | 216.7K ops/sec | 254.7M bytes/sec |
| needs_json_escaping (16 byte clean) | 2.7 ns | 368.7M ops/sec | - |
| needs_json_escaping (64 byte clean) | 3.2 ns | 316.2M ops/sec | - |
| needs_json_escaping (256 byte clean) | 6.8 ns | 146.5M ops/sec | - |
| needs_json_escaping (64 byte, escape at end) | 2.3 ns | 442.6M ops/sec | - |
| needs_json_escaping (64 byte, escape at start) | 0.7 ns | 1.48B ops/sec | - |
| serialize 5-field obj (embedded commas) | 64.5 ns | 15.5M ops/sec | - |
| serialize array 100 ints (to_chars flat buffer) | 216.7 ns | 4.6M ops/sec | - |
| serialize array 100 ints (to_chars into string) | 789.0 ns | 1.3M ops/sec | - |
| serialize array 100 ints (lookup table into string) | 194.4 ns | 5.1M ops/sec | - |
| serialize array 1000 ints (optimized) | 1930.0 ns | 518.6K ops/sec | - |
| serialize array 10000 ints (optimized) | 45280.0 ns | 22.1K ops/sec | - |
| serialize nested obj (5 levels deep) | 24.1 ns | 41.4M ops/sec | - |
| serialize 20-field obj | 351.9 ns | 2.8M ops/sec | - |
| serialize 100 nested objects | 3460.0 ns | 288.9K ops/sec | - |
| escape_json_string (10KB clean) | 241.4 ns | 4.1M ops/sec | - |
| escape_json_string (10KB heavy escaping) | 14970.0 ns | 66.8K ops/sec | - |
| escape_json_string (escape at SIMD boundary) | 294.4 ns | 3.4M ops/sec | - |
| needs_json_escaping (1KB clean) | 17.9 ns | 56.0M ops/sec | - |
| needs_json_escaping (1KB, escape at end) | 16.1 ns | 62.0M ops/sec | - |
| escape hot (same string repeated) | 12.8 ns | 78.3M ops/sec | - |
| escape cold (different strings) | 21.1 ns | 47.5M ops/sec | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| escape_json_string (clean, return) | 76.5M | 1.2M | 1.54% | 730.2K | 74.2M | 76.7M | 77.8M | 77.8M |
| escape_json_string_into (clean, append) | 132.0M | 3.1M | 2.32% | 1.9M | 123.9M | 133.1M | 134.4M | 134.4M |
| escape_json_string (dirty, return) | 13.3M | 899.7K | 6.78% | 557.6K | 10.6M | 13.5M | 13.9M | 13.9M |
| escape_json_string_into (dirty, append) | 17.4M | 275.4K | 1.58% | 170.7K | 16.9M | 17.3M | 17.8M | 17.9M |
| escape profile (best case) | 71.4M | 1.1M | 1.54% | 679.9K | 68.4M | 71.5M | 72.5M | 72.6M |
| escape profile (typical mixed) | 2.8M | 50.6K | 1.78% | 31.4K | 2.7M | 2.8M | 2.9M | 2.9M |
| escape profile (hard edge) | 227.8K | 3.7K | 1.61% | 2.3K | 223.0K | 227.1K | 232.4K | 232.4K |
| serialize object profile (best case) | 14.0M | 264.2K | 1.89% | 163.8K | 13.3M | 14.0M | 14.2M | 14.2M |
| serialize object profile (typical mixed) | 2.3M | 40.1K | 1.72% | 24.9K | 2.3M | 2.3M | 2.4M | 2.4M |
| serialize object profile (hard edge) | 214.7K | 5.0K | 2.33% | 3.1K | 204.7K | 216.7K | 220.7K | 222.3K |
| needs_json_escaping (16 byte clean) | 368.4M | 974.7K | 0.26% | 604.1K | 366.2M | 368.7M | 369.3M | 369.4M |
| needs_json_escaping (64 byte clean) | 315.1M | 3.0M | 0.94% | 1.8M | 306.3M | 316.2M | 316.6M | 316.6M |
| needs_json_escaping (256 byte clean) | 145.1M | 3.5M | 2.41% | 2.2M | 138.5M | 146.5M | 149.2M | 149.9M |
| needs_json_escaping (64 byte, escape at end) | 442.1M | 1.7M | 0.37% | 1.0M | 437.2M | 442.6M | 443.0M | 443.1M |
| needs_json_escaping (64 byte, escape at start) | 1.47B | 21.2M | 1.44% | 13.1M | 1.41B | 1.48B | 1.48B | 1.48B |
| serialize 5-field obj (embedded commas) | 15.4M | 333.9K | 2.16% | 207.0K | 14.7M | 15.5M | 15.8M | 15.9M |
| serialize array 100 ints (to_chars flat buffer) | 4.6M | 66.2K | 1.44% | 41.0K | 4.5M | 4.6M | 4.7M | 4.7M |
| serialize array 100 ints (to_chars into string) | 1.1M | 246.2K | 22.19% | 152.6K | 716.6K | 1.3M | 1.3M | 1.3M |
| serialize array 100 ints (lookup table into string) | 5.2M | 50.4K | 0.98% | 31.3K | 5.1M | 5.1M | 5.2M | 5.2M |
| serialize array 1000 ints (optimized) | 517.3K | 5.6K | 1.08% | 3.5K | 505.1K | 518.6K | 523.6K | 524.4K |
| serialize array 10000 ints (optimized) | 21.8K | 847 | 3.88% | 525 | 19.9K | 22.1K | 22.6K | 22.6K |
| serialize nested obj (5 levels deep) | 41.8M | 909.6K | 2.18% | 563.8K | 41.1M | 41.4M | 43.6M | 43.8M |
| serialize 20-field obj | 2.8M | 125.2K | 4.48% | 77.6K | 2.4M | 2.8M | 2.9M | 2.9M |
| serialize 100 nested objects | 277.4K | 29.8K | 10.74% | 18.5K | 190.3K | 288.9K | 293.3K | 293.4K |
| escape_json_string (10KB clean) | 4.1M | 58.6K | 1.42% | 36.3K | 3.9M | 4.1M | 4.2M | 4.2M |
| escape_json_string (10KB heavy escaping) | 66.6K | 1.5K | 2.31% | 952 | 63.1K | 66.8K | 68.2K | 68.6K |
| escape_json_string (escape at SIMD boundary) | 3.4M | 37.0K | 1.09% | 22.9K | 3.3M | 3.4M | 3.4M | 3.4M |
| needs_json_escaping (1KB clean) | 55.8M | 879.5K | 1.58% | 545.1K | 53.6M | 56.0M | 56.7M | 56.8M |
| needs_json_escaping (1KB, escape at end) | 62.0M | 978.1K | 1.58% | 606.2K | 59.6M | 62.0M | 63.3M | 63.3M |
| escape hot (same string repeated) | 78.0M | 1.5M | 1.98% | 958.2K | 73.8M | 78.3M | 79.3M | 79.6M |
| escape cold (different strings) | 47.5M | 632.2K | 1.33% | 391.9K | 46.1M | 47.5M | 48.2M | 48.3M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| escape_json_string (clean, return) | ns | 13.080 | 0.199 | 1.52% | 0.123 | 13.000 | 13.455 | 13.500 |
| escape_json_string_into (clean, append) | ns | 7.590 | 0.192 | 2.53% | 0.119 | 7.550 | 7.920 | 8.100 |
| escape_json_string (dirty, return) | ns | 75.840 | 6.165 | 8.13% | 3.821 | 74.100 | 85.900 | 94.000 |
| escape_json_string_into (dirty, append) | ns | 57.510 | 0.912 | 1.59% | 0.565 | 57.800 | 58.640 | 59.000 |
| escape profile (best case) | ns | 14.010 | 0.217 | 1.55% | 0.134 | 14.000 | 14.375 | 14.600 |
| escape profile (typical mixed) | ns | 352.720 | 6.299 | 1.79% | 3.904 | 352.900 | 362.010 | 363.900 |
| escape profile (hard edge) | ns | 4390.000 | 69.714 | 1.59% | 43.209 | 4405.000 | 4480.000 | 4480.000 |
| serialize object profile (best case) | ns | 71.580 | 1.413 | 1.97% | 0.876 | 71.400 | 73.985 | 75.200 |
| serialize object profile (typical mixed) | ns | 428.530 | 7.361 | 1.72% | 4.562 | 428.500 | 439.255 | 441.100 |
| serialize object profile (hard edge) | ns | 4662.000 | 110.345 | 2.37% | 68.392 | 4615.000 | 4827.000 | 4890.000 |
| needs_json_escaping (16 byte clean) | ns | 2.700 | 0.000 | 0.00% | 0.000 | 2.700 | 2.700 | 2.700 |
| needs_json_escaping (64 byte clean) | ns | 3.210 | 0.030 | 0.93% | 0.019 | 3.200 | 3.255 | 3.300 |
| needs_json_escaping (256 byte clean) | ns | 6.890 | 0.164 | 2.38% | 0.102 | 6.800 | 7.155 | 7.200 |
| needs_json_escaping (64 byte, escape at end) | ns | 2.300 | 0.000 | 0.00% | 0.000 | 2.300 | 2.300 | 2.300 |
| needs_json_escaping (64 byte, escape at start) | ns | 0.700 | 0.000 | 0.00% | 0.000 | 0.700 | 0.700 | 0.700 |
| serialize 5-field obj (embedded commas) | ns | 64.810 | 1.449 | 2.24% | 0.898 | 64.550 | 67.435 | 68.200 |
| serialize array 100 ints (to_chars flat buffer) | ns | 217.710 | 3.179 | 1.46% | 1.970 | 216.700 | 223.395 | 223.800 |
| serialize array 100 ints (to_chars into string) | ns | 960.880 | 266.582 | 27.74% | 165.229 | 788.950 | 1395.500 | 1400.000 |
| serialize array 100 ints (lookup table into string) | ns | 193.900 | 1.891 | 0.98% | 1.172 | 194.450 | 196.365 | 196.500 |
| serialize array 1000 ints (optimized) | ns | 1932.000 | 20.881 | 1.08% | 12.942 | 1930.000 | 1966.500 | 1980.000 |
| serialize array 10000 ints (optimized) | ns | 45859.000 | 1869.259 | 4.08% | 1158.579 | 45280.000 | 49405.500 | 50130.000 |
| serialize nested obj (5 levels deep) | ns | 23.910 | 0.507 | 2.12% | 0.314 | 24.150 | 24.300 | 24.300 |
| serialize 20-field obj | ns | 358.260 | 17.912 | 5.00% | 11.102 | 351.850 | 389.825 | 410.300 |
| serialize 100 nested objects | ns | 3662.000 | 535.963 | 14.64% | 332.193 | 3460.000 | 4543.500 | 5250.000 |
| escape_json_string (10KB clean) | ns | 242.590 | 3.569 | 1.47% | 2.212 | 241.450 | 248.385 | 253.200 |
| escape_json_string (10KB heavy escaping) | ns | 15021.000 | 355.034 | 2.36% | 220.052 | 14970.000 | 15647.500 | 15850.000 |
| escape_json_string (escape at SIMD boundary) | ns | 295.110 | 3.232 | 1.10% | 2.003 | 294.350 | 300.005 | 300.500 |
| needs_json_escaping (1KB clean) | ns | 17.920 | 0.279 | 1.55% | 0.173 | 17.900 | 18.375 | 18.600 |
| needs_json_escaping (1KB, escape at end) | ns | 16.150 | 0.269 | 1.67% | 0.167 | 16.150 | 16.575 | 16.800 |
| escape hot (same string repeated) | ns | 12.850 | 0.269 | 2.10% | 0.167 | 12.800 | 13.330 | 13.600 |
| escape cold (different strings) | ns | 21.060 | 0.280 | 1.33% | 0.174 | 21.050 | 21.520 | 21.700 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| escape profile (best case) | 1.12B bytes/sec | 17.2M bytes/sec | 1.54% | 10.7M bytes/sec | 1.07B bytes/sec | 1.12B bytes/sec | 1.14B bytes/sec | 1.14B bytes/sec |
| escape profile (typical mixed) | 232.9M bytes/sec | 4.2M bytes/sec | 1.78% | 2.6M bytes/sec | 225.7M bytes/sec | 232.7M bytes/sec | 238.4M bytes/sec | 239.9M bytes/sec |
| escape profile (hard edge) | 203.3M bytes/sec | 3.3M bytes/sec | 1.61% | 2.0M bytes/sec | 199.0M bytes/sec | 202.6M bytes/sec | 207.4M bytes/sec | 207.4M bytes/sec |
| serialize object profile (best case) | 395.9M bytes/sec | 7.5M bytes/sec | 1.89% | 4.6M bytes/sec | 376.9M bytes/sec | 396.8M bytes/sec | 403.0M bytes/sec | 403.2M bytes/sec |
| serialize object profile (typical mixed) | 280.1M bytes/sec | 4.8M bytes/sec | 1.72% | 3.0M bytes/sec | 272.0M bytes/sec | 280.1M bytes/sec | 286.7M bytes/sec | 288.5M bytes/sec |
| serialize object profile (hard edge) | 252.3M bytes/sec | 5.9M bytes/sec | 2.33% | 3.6M bytes/sec | 240.6M bytes/sec | 254.7M bytes/sec | 259.3M bytes/sec | 261.2M bytes/sec |

---

## Router Benchmarks

_Runs: 10 | Aggregation: median_

HTTP routing dispatch performance

| Config | Value |
|--------|-------|
| binary | router_benchmark |
| binary_path | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local/benchmark/router_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Router dispatch (hits) | 3.6M ops/sec | 0.264 us | 0.501 us | 0.644 us | 0 |
| Router dispatch (not found) | 3.7M ops/sec | 0.255 us | 0.536 us | 0.687 us | 0 |
| Router dispatch (405) | 2.3M ops/sec | 0.424 us | 0.732 us | 0.929 us | 0 |
| Large router (hits) | 2.8M ops/sec | 0.346 us | 0.635 us | 0.700 us | 0 |
| Large router (not found) | 3.0M ops/sec | 0.326 us | 0.587 us | 0.701 us | 0 |
| Large router (405) | 1.7M ops/sec | 0.566 us | 1.000 us | 1.171 us | 0 |
| Large router (edge cases) | 2.0M ops/sec | 0.476 us | 0.801 us | 0.944 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Router dispatch (hits) | 3.5M | 215.0K | 6.13% | 133.3K | 3.1M | 3.6M | 3.7M | 3.7M |
| Router dispatch (not found) | 3.6M | 289.3K | 8.08% | 179.3K | 3.0M | 3.7M | 3.9M | 3.9M |
| Router dispatch (405) | 2.2M | 106.5K | 4.75% | 66.0K | 2.0M | 2.3M | 2.3M | 2.3M |
| Large router (hits) | 2.8M | 83.5K | 3.01% | 51.8K | 2.6M | 2.8M | 2.9M | 2.9M |
| Large router (not found) | 2.9M | 182.5K | 6.34% | 113.1K | 2.5M | 3.0M | 3.0M | 3.1M |
| Large router (405) | 1.7M | 137.7K | 8.24% | 85.4K | 1.3M | 1.7M | 1.8M | 1.8M |
| Large router (edge cases) | 2.0M | 81.0K | 4.02% | 50.2K | 1.9M | 2.0M | 2.1M | 2.1M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Router dispatch (hits) | p99 us | 0.508 | 0.096 | 18.94% | 0.060 | 0.501 | 0.656 | 0.722 |
| Router dispatch (not found) | p99 us | 0.535 | 0.118 | 22.01% | 0.073 | 0.536 | 0.694 | 0.724 |
| Router dispatch (405) | p99 us | 0.719 | 0.180 | 25.04% | 0.112 | 0.732 | 0.968 | 1.028 |
| Large router (hits) | p99 us | 0.620 | 0.057 | 9.25% | 0.036 | 0.635 | 0.683 | 0.686 |
| Large router (not found) | p99 us | 0.603 | 0.151 | 25.12% | 0.094 | 0.587 | 0.819 | 0.870 |
| Large router (405) | p99 us | 1.027 | 0.165 | 16.09% | 0.102 | 1.000 | 1.284 | 1.307 |
| Large router (edge cases) | p99 us | 0.800 | 0.147 | 18.41% | 0.091 | 0.801 | 0.993 | 1.000 |

---

## Generated API Dispatch Benchmarks

_Runs: 10 | Aggregation: median_

Generated router bindings, param extraction, parse and serialize on ready request objects

| Config | Value |
|--------|-------|
| binary | generated_api_benchmark |
| binary_path | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local/benchmark/generated_api_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Generated API dispatch+parse | 3.7M ops/sec | 0.150 us | 0.411 us | 0.580 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Generated API dispatch+parse | 3.8M | 220.5K | 5.88% | 136.7K | 3.4M | 3.7M | 4.0M | 4.1M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Generated API dispatch+parse | p99 us | 0.404 | 0.095 | 23.55% | 0.059 | 0.411 | 0.536 | 0.541 |

---

## Generated API Mixed Workload Benchmarks

_Runs: 10 | Aggregation: median_

Generated stack mixed valid/error workload on benchmark_api spec

| Config | Value |
|--------|-------|
| binary | benchmark_api_codegen_benchmark |
| binary_path | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local/benchmark/benchmark_api_codegen_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_codegen_valid_mix | 797.5K ops/sec | 0.710 us | 4.871 us | 5.368 us | 9.809 us | 0 |
| benchmark_api_codegen_error_mix | 1.7M ops/sec | 0.466 us | 0.518 us | 0.548 us | 0.974 us | 120000 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| benchmark_api_codegen_valid_mix | 798.1K | 21.3K | 2.67% | 13.2K | 762.9K | 797.5K | 826.0K | 826.9K |
| benchmark_api_codegen_error_mix | 1.8M | 26.3K | 1.50% | 16.3K | 1.7M | 1.7M | 1.8M | 1.8M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| benchmark_api_codegen_valid_mix | p99 us | 5.520 | 0.588 | 10.64% | 0.364 | 5.368 | 6.452 | 6.566 |
| benchmark_api_codegen_error_mix | p99 us | 0.647 | 0.146 | 22.57% | 0.090 | 0.548 | 0.884 | 0.902 |

---

## Generated API Framework Path Benchmarks

_Runs: 10 | Aggregation: median_

Generated stack valid/error workload on benchmark_api spec with minimal handlers

| Config | Value |
|--------|-------|
| binary | benchmark_api_framework_benchmark |
| binary_path | /run/media/visage/344E3FC84E3F81A2/Users/Ya/OneDrive/Desktop/KATANA/build/bench-local/benchmark/benchmark_api_framework_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_framework_valid_mix | 1.5M ops/sec | 0.470 us | 0.888 us | 0.919 us | 1.494 us | 0 |
| benchmark_api_framework_error_mix | 1.8M ops/sec | 0.444 us | 0.495 us | 0.671 us | 0.988 us | 120000 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| benchmark_api_framework_valid_mix | 1.5M | 27.3K | 1.77% | 16.9K | 1.5M | 1.5M | 1.6M | 1.6M |
| benchmark_api_framework_error_mix | 1.8M | 51.8K | 2.88% | 32.1K | 1.7M | 1.8M | 1.8M | 1.8M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| benchmark_api_framework_valid_mix | p99 us | 0.987 | 0.168 | 17.00% | 0.104 | 0.919 | 1.265 | 1.473 |
| benchmark_api_framework_error_mix | p99 us | 0.695 | 0.196 | 28.16% | 0.121 | 0.671 | 0.969 | 1.059 |

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
| wrk hello_world GET / depth10 | 3.5M req/sec | 356.0M bytes/sec | 799.550 us | 664.000 us | 1658.000 us | 2552.000 us | 12249.000 us | 35259.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth10 | 3.5M | 43.4K | 1.24% | 38.0K | 3.4M | 3.5M | 3.5M | 3.5M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk hello_world GET / depth10 | avg us | 803.660 | 23.624 | 2.94% | 20.707 | 799.550 | 835.472 | 841.080 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth10 | 356.3M bytes/sec | 4.4M bytes/sec | 1.24% | 3.9M bytes/sec | 350.6M bytes/sec | 356.0M bytes/sec | 361.5M bytes/sec | 361.6M bytes/sec |

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
| wrk compute_api POST /compute/sum depth10 | 2.5M req/sec | 238.9M bytes/sec | 1310.000 us | 1152.000 us | 2678.000 us | 3898.000 us | 13845.000 us | 48068.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | 2.5M | 18.1K | 0.73% | 15.8K | 2.5M | 2.5M | 2.5M | 2.5M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | avg us | 1314.000 | 18.547 | 1.41% | 16.257 | 1310.000 | 1338.000 | 1340.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | 238.6M bytes/sec | 1.7M bytes/sec | 0.73% | 1.5M bytes/sec | 235.5M bytes/sec | 238.9M bytes/sec | 240.5M bytes/sec | 240.8M bytes/sec |

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
| wrk hello_world GET / depth20 | 5.3M req/sec | 537.3M bytes/sec | 1140.000 us | 925.000 us | 2661.000 us | 3953.000 us | 26354.000 us | 43864.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth20 | 5.3M | 13.7K | 0.26% | 12.0K | 5.3M | 5.3M | 5.3M | 5.3M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk hello_world GET / depth20 | avg us | 1162.000 | 56.356 | 4.85% | 49.398 | 1140.000 | 1248.000 | 1270.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth20 | 537.6M bytes/sec | 1.4M bytes/sec | 0.26% | 1.2M bytes/sec | 536.3M bytes/sec | 537.3M bytes/sec | 539.7M bytes/sec | 540.3M bytes/sec |

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
| wrk compute_api POST /compute/sum depth40 | 3.5M req/sec | 334.0M bytes/sec | 3140.000 us | 3396.000 us | 0.000 us | 0.000 us | 0.000 us | 44142.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | 3.5M | 21.2K | 0.61% | 18.5K | 3.4M | 3.5M | 3.5M | 3.5M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | avg us | 3154.000 | 32.000 | 1.01% | 28.049 | 3140.000 | 3196.000 | 3200.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | 333.0M bytes/sec | 2.0M bytes/sec | 0.61% | 1.8M bytes/sec | 329.1M bytes/sec | 334.0M bytes/sec | 334.6M bytes/sec | 334.6M bytes/sec |

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
