# KATANA Benchmark Results

> Last updated: 2026-02-22 08:43:38
> Commit: 89778a6

## Summary

- Stability verdict: contains noisy metrics, inspect before trusting small deltas
- Noisy metrics (CV > 20.0%): 41
- Severely noisy metrics (CV > 40.0%): 6
- E2E keep-alive stage included: no

> **Note**: Results shown use median-of-N aggregation across default 10 run(s), overrides: stage 1=15, stage 2=10, stage 3=10, stage 4=10. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Environment

| Key | Value |
|-----|-------|
| affinity_cpus | 12 |
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

---

## Quality Gates

- CV warning threshold: 20.00%
- Noisy metrics: 41
- Severely noisy metrics: 6

| Top Noisy Metric | CV |
|------------------|----|
| stage1.Ring Buffer Queue (Max Contention 16x16).latency_p99_us | 144.07% |
| stage1.Ring Buffer Queue (Core Saturation 6x6).latency_p999_us | 83.77% |
| stage1.Ring Buffer Queue (Concurrent 4x4).retries_per_op_total | 59.58% |
| stage1.Ring Buffer Queue (Single Thread).latency_p99_us | 56.18% |
| stage1.Ring Buffer Queue (Core Saturation 6x6).retries_per_op_total | 50.99% |
| stage1.Ring Buffer Queue (Single Thread).latency_p999_us | 41.30% |
| stage1.Ring Buffer Queue (Oversubscribed 14x14).latency_p99_us | 35.77% |
| stage1.HTTP Parser (Fragmented Request).latency_p999_us | 34.86% |
| stage1.SIMD CRLF Search (1.5KB buffer).latency_p99_us | 33.52% |
| stage1.SIMD CRLF Search (1.5KB buffer).latency_p999_us | 31.59% |

---

## Core Runtime Benchmarks

_Runs: 15 | Aggregation: median_

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Retries/op | Errors |
|-----------|------------|-------------|-------------|--------------|------------|--------|
| Ring Buffer Queue (Single Thread) | 219.6M ops/sec | 0.004 us | 0.009 us | 0.098 us | - | 0 |
| Ring Buffer Queue (Concurrent 2x2) | 36.1M ops/sec | 0.044 us | 0.199 us | 0.660 us | 0.109 | 0 |
| Ring Buffer Queue (Concurrent 4x4) | 18.7M ops/sec | 0.182 us | 0.556 us | 1.473 us | 0.135 | 0 |
| Ring Buffer Queue (Core Saturation 6x6) | 14.7M ops/sec | 0.315 us | 1.140 us | 8.171 us | 0.540 | 0 |
| Ring Buffer Queue (High Contention 8x8) | 8.8M ops/sec | 0.266 us | 1.336 us | 130.680 us | 11.218 | 0 |
| Ring Buffer Queue (Extreme Contention 12x12) | 7.0M ops/sec | 0.341 us | 1.973 us | 287.246 us | 21.600 | 0 |
| Ring Buffer Queue (Max Contention 16x16) | 5.7M ops/sec | 0.328 us | 2.695 us | 500.314 us | 36.266 | 0 |
| Ring Buffer Queue (Oversubscribed 14x14) | 6.5M ops/sec | 0.312 us | 3.794 us | 314.444 us | 29.852 | 0 |
| Circular Buffer | 215.5M ops/sec | 0.004 us | 0.008 us | 0.020 us | - | 0 |
| SIMD CRLF Search (1.5KB buffer) | 64.7M ops/sec | 0.013 us | 0.077 us | 0.184 us | - | 0 |
| SIMD CRLF Search (16KB buffer) | 4.1M ops/sec | 0.220 us | 0.553 us | 1.247 us | - | 0 |
| SIMD CRLF Search (32KB buffer) | 2.3M ops/sec | 0.387 us | 1.162 us | 2.549 us | - | 0 |
| SIMD CRLF Search (64KB buffer) | 1.1M ops/sec | 0.786 us | 3.137 us | 8.657 us | - | 0 |
| SIMD CRLF Search (128KB buffer) | 570.6K ops/sec | 1.560 us | 3.820 us | 9.993 us | - | 0 |
| HTTP Parser (Complete Request) | 1.2M ops/sec | 0.748 us | 2.023 us | 5.108 us | - | 0 |
| HTTP Parser (Fragmented Request) | 1.3M ops/sec | 0.707 us | 1.617 us | 3.508 us | - | 0 |
| Arena Allocations (64B objects) | 23.0M ops/sec | - | - | - | - | 0 |
| Memory Allocations (String Queue) | 24.9M ops/sec | - | - | - | - | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | 218.5M | 12.1M | 5.54% | 6.1M | 195.2M | 219.6M | 239.7M | 245.3M |
| Ring Buffer Queue (Concurrent 2x2) | 35.4M | 1.3M | 3.61% | 645.1K | 33.2M | 36.1M | 36.8M | 37.2M |
| Ring Buffer Queue (Concurrent 4x4) | 18.7M | 486.4K | 2.60% | 246.2K | 18.0M | 18.7M | 19.5M | 19.6M |
| Ring Buffer Queue (Core Saturation 6x6) | 14.9M | 885.9K | 5.94% | 448.3K | 13.8M | 14.7M | 16.5M | 16.7M |
| Ring Buffer Queue (High Contention 8x8) | 9.1M | 1.2M | 13.52% | 621.9K | 7.5M | 8.8M | 11.1M | 11.2M |
| Ring Buffer Queue (Extreme Contention 12x12) | 7.1M | 1.0M | 14.50% | 521.2K | 5.4M | 7.0M | 9.2M | 9.4M |
| Ring Buffer Queue (Max Contention 16x16) | 5.8M | 399.5K | 6.92% | 202.2K | 5.3M | 5.7M | 6.6M | 6.7M |
| Ring Buffer Queue (Oversubscribed 14x14) | 6.6M | 666.4K | 10.04% | 337.2K | 5.7M | 6.5M | 7.9M | 8.0M |
| Circular Buffer | 213.7M | 12.8M | 6.00% | 6.5M | 175.7M | 215.5M | 226.1M | 226.4M |
| SIMD CRLF Search (1.5KB buffer) | 65.3M | 2.8M | 4.24% | 1.4M | 61.3M | 64.7M | 69.9M | 71.0M |
| SIMD CRLF Search (16KB buffer) | 4.1M | 127.6K | 3.14% | 64.6K | 3.8M | 4.1M | 4.2M | 4.2M |
| SIMD CRLF Search (32KB buffer) | 2.3M | 95.3K | 4.20% | 48.2K | 2.1M | 2.3M | 2.4M | 2.4M |
| SIMD CRLF Search (64KB buffer) | 1.1M | 41.6K | 3.67% | 21.1K | 1.0M | 1.1M | 1.2M | 1.2M |
| SIMD CRLF Search (128KB buffer) | 574.1K | 17.8K | 3.10% | 9.0K | 543.4K | 570.6K | 599.5K | 604.8K |
| HTTP Parser (Complete Request) | 1.2M | 49.8K | 4.21% | 25.2K | 1.0M | 1.2M | 1.3M | 1.3M |
| HTTP Parser (Fragmented Request) | 1.3M | 52.7K | 4.17% | 26.7K | 1.1M | 1.3M | 1.3M | 1.3M |
| Arena Allocations (64B objects) | 22.7M | 1.7M | 7.41% | 850.0K | 19.3M | 23.0M | 24.4M | 25.1M |
| Memory Allocations (String Queue) | 24.6M | 1.6M | 6.63% | 824.6K | 21.3M | 24.9M | 26.7M | 27.1M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | p99 us | 0.010 | 0.006 | 56.18% | 0.003 | 0.008 | 0.018 | 0.031 |
| Ring Buffer Queue (Concurrent 2x2) | p99 us | 0.192 | 0.012 | 6.37% | 0.006 | 0.196 | 0.207 | 0.209 |
| Ring Buffer Queue (Concurrent 4x4) | p99 us | 0.595 | 0.054 | 9.09% | 0.027 | 0.575 | 0.683 | 0.753 |
| Ring Buffer Queue (Core Saturation 6x6) | p99 us | 1.236 | 0.151 | 12.25% | 0.077 | 1.215 | 1.468 | 1.522 |
| Ring Buffer Queue (High Contention 8x8) | p99 us | 1.440 | 0.401 | 27.87% | 0.203 | 1.355 | 2.025 | 2.781 |
| Ring Buffer Queue (Extreme Contention 12x12) | p99 us | 2.493 | 0.710 | 28.48% | 0.359 | 2.476 | 3.728 | 3.897 |
| Ring Buffer Queue (Max Contention 16x16) | p99 us | 15.498 | 22.329 | 144.07% | 11.300 | 4.670 | 62.849 | 63.015 |
| Ring Buffer Queue (Oversubscribed 14x14) | p99 us | 2.779 | 0.994 | 35.77% | 0.503 | 2.242 | 4.456 | 4.814 |
| Circular Buffer | p99 us | 0.015 | 0.004 | 28.79% | 0.002 | 0.013 | 0.023 | 0.025 |
| SIMD CRLF Search (1.5KB buffer) | p99 us | 0.051 | 0.017 | 33.52% | 0.009 | 0.048 | 0.079 | 0.084 |
| SIMD CRLF Search (16KB buffer) | p99 us | 0.758 | 0.216 | 28.52% | 0.109 | 0.606 | 1.117 | 1.147 |
| SIMD CRLF Search (32KB buffer) | p99 us | 1.325 | 0.331 | 25.00% | 0.168 | 1.159 | 1.904 | 2.190 |
| SIMD CRLF Search (64KB buffer) | p99 us | 2.723 | 0.743 | 27.28% | 0.376 | 2.720 | 3.764 | 3.822 |
| SIMD CRLF Search (128KB buffer) | p99 us | 5.033 | 1.434 | 28.50% | 0.726 | 4.676 | 7.354 | 7.367 |
| HTTP Parser (Complete Request) | p99 us | 1.874 | 0.308 | 16.44% | 0.156 | 1.918 | 2.352 | 2.429 |
| HTTP Parser (Fragmented Request) | p99 us | 1.738 | 0.245 | 14.07% | 0.124 | 1.620 | 2.097 | 2.101 |

### Contention Stability (Retries/op)

| Benchmark | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|
| Ring Buffer Queue (Concurrent 2x2) | 0.116 | 0.028 | 23.94% | 0.014 | 0.113 | 0.168 | 0.174 |
| Ring Buffer Queue (Concurrent 4x4) | 0.161 | 0.096 | 59.58% | 0.049 | 0.135 | 0.302 | 0.502 |
| Ring Buffer Queue (Core Saturation 6x6) | 1.194 | 0.609 | 50.99% | 0.308 | 1.007 | 2.241 | 2.303 |
| Ring Buffer Queue (High Contention 8x8) | 10.437 | 3.122 | 29.91% | 1.580 | 11.218 | 15.145 | 15.965 |
| Ring Buffer Queue (Extreme Contention 12x12) | 21.628 | 4.490 | 20.76% | 2.272 | 22.105 | 27.416 | 28.055 |
| Ring Buffer Queue (Max Contention 16x16) | 34.851 | 4.291 | 12.31% | 2.172 | 35.079 | 40.458 | 40.548 |
| Ring Buffer Queue (Oversubscribed 14x14) | 26.564 | 4.230 | 15.92% | 2.141 | 27.529 | 31.883 | 32.376 |

---

## Codegen Quality Benchmarks

_Runs: 10 | Aggregation: median_

| Benchmark | Latency | Throughput | Tail p95 | Tail p99 | Data Rate |
|-----------|---------|------------|----------|----------|-----------|
| parse_int64 (simple) | 7.1 ns | 141.0M ops/sec | - | - | - |
| parse_int64 (negative) | 8.0 ns | 125.2M ops/sec | - | - | - |
| parse_double | 16.7 ns | 59.8M ops/sec | - | - | - |
| parse_bool (strict validation) | 1.9 ns | 517.2M ops/sec | - | - | - |
| parse_bool (mixed true/false/quoted) | 4.9 ns | 202.5M ops/sec | - | - | - |
| parse_int64 profile (best case) | 9.6 ns | 104.7M ops/sec | 36.0 ns | 90.0 ns | 288.9M bytes/sec |
| parse_int64 profile (typical mixed) | 12.0 ns | 83.0M ops/sec | 45.0 ns | 90.0 ns | 416.6M bytes/sec |
| parse_int64 profile (hard edge) | 15.3 ns | 65.3M ops/sec | 63.0 ns | 90.0 ns | 665.6M bytes/sec |
| parse_bool profile (best case) | 4.6 ns | 218.2M ops/sec | 28.0 ns | 72.0 ns | 1.09B bytes/sec |
| parse_bool profile (typical mixed) | 9.7 ns | 103.3M ops/sec | 54.0 ns | 82.0 ns | 835.5M bytes/sec |
| parse_bool profile (hard edge) | 13.0 ns | 76.8M ops/sec | 63.0 ns | 117.0 ns | 1.65B bytes/sec |
| validate_user profile (best case) | 7.8 ns | 128.4M ops/sec | 28.0 ns | 54.0 ns | 3.94B bytes/sec |
| validate_user profile (typical mixed) | 9.2 ns | 108.8M ops/sec | 36.0 ns | 63.0 ns | 3.40B bytes/sec |
| validate_user profile (hard edge) | 10.0 ns | 100.3M ops/sec | 36.0 ns | 63.0 ns | 3.25B bytes/sec |
| needs_json_escaping (clean, 58 chars) | 8.9 ns | 112.0M ops/sec | - | - | - |
| needs_json_escaping (dirty, 25 chars) | 1.1 ns | 929.6M ops/sec | - | - | - |
| escape_json_string (no-alloc path) | 16.2 ns | 61.6M ops/sec | - | - | - |
| escape_json_string_into (append) | 6.2 ns | 160.4M ops/sec | - | - | - |
| skip_value (nested obj with strings) | 29.8 ns | 33.5M ops/sec | - | - | - |
| 3-field object parse (linear) | 35.0 ns | 28.6M ops/sec | - | - | - |
| 8-field object parse (length-switch) | 170.8 ns | 5.9M ops/sec | - | - | - |
| arena alloc+reset cycle (4KB) | 54.9 ns | 18.2M ops/sec | - | - | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| parse_int64 (simple) | 141.2M | 3.7M | 2.61% | 2.3M | 133.8M | 140.8M | 145.8M | 146.1M |
| parse_int64 (negative) | 123.9M | 4.9M | 3.91% | 3.0M | 113.7M | 125.6M | 128.8M | 129.1M |
| parse_double | 58.8M | 2.4M | 4.16% | 1.5M | 53.8M | 59.3M | 61.3M | 61.3M |
| parse_bool (strict validation) | 511.3M | 11.1M | 2.17% | 6.9M | 486.0M | 515.5M | 520.1M | 521.3M |
| parse_bool (mixed true/false/quoted) | 201.3M | 5.3M | 2.63% | 3.3M | 193.9M | 201.1M | 207.9M | 208.4M |
| parse_int64 profile (best case) | 104.7M | 3.9M | 3.71% | 2.4M | 95.2M | 105.2M | 108.6M | 108.6M |
| parse_int64 profile (typical mixed) | 83.3M | 2.9M | 3.50% | 1.8M | 79.0M | 83.0M | 87.2M | 87.4M |
| parse_int64 profile (hard edge) | 65.6M | 2.6M | 3.95% | 1.6M | 59.0M | 65.9M | 68.5M | 68.5M |
| parse_bool profile (best case) | 221.3M | 6.1M | 2.77% | 3.8M | 212.2M | 220.2M | 230.9M | 231.8M |
| parse_bool profile (typical mixed) | 104.4M | 3.1M | 2.98% | 1.9M | 99.1M | 103.2M | 109.4M | 110.5M |
| parse_bool profile (hard edge) | 77.8M | 2.2M | 2.82% | 1.4M | 74.7M | 77.5M | 81.5M | 82.2M |
| validate_user profile (best case) | 127.3M | 4.5M | 3.52% | 2.8M | 120.8M | 128.7M | 132.6M | 133.1M |
| validate_user profile (typical mixed) | 109.7M | 2.4M | 2.23% | 1.5M | 106.4M | 109.4M | 113.5M | 113.9M |
| validate_user profile (hard edge) | 100.5M | 3.7M | 3.70% | 2.3M | 93.9M | 99.8M | 105.7M | 105.8M |
| needs_json_escaping (clean, 58 chars) | 112.1M | 1.7M | 1.54% | 1.1M | 109.6M | 112.4M | 114.5M | 115.4M |
| needs_json_escaping (dirty, 25 chars) | 929.2M | 14.3M | 1.54% | 8.9M | 901.3M | 928.8M | 945.9M | 947.7M |
| escape_json_string (no-alloc path) | 61.8M | 1.2M | 1.95% | 746.8K | 59.5M | 61.6M | 63.8M | 63.9M |
| escape_json_string_into (append) | 160.0M | 5.7M | 3.55% | 3.5M | 151.1M | 160.5M | 167.4M | 168.0M |
| skip_value (nested obj with strings) | 33.6M | 709.1K | 2.11% | 439.5K | 32.5M | 33.7M | 34.5M | 34.5M |
| 3-field object parse (linear) | 28.5M | 667.9K | 2.35% | 414.0K | 27.0M | 28.5M | 29.4M | 29.5M |
| 8-field object parse (length-switch) | 5.8M | 146.9K | 2.54% | 91.0K | 5.6M | 5.8M | 6.0M | 6.0M |
| arena alloc+reset cycle (4KB) | 18.5M | 729.5K | 3.95% | 452.2K | 17.5M | 18.4M | 19.5M | 19.6M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| parse_int64 (simple) | ns | 7.080 | 0.189 | 2.66% | 0.117 | 7.100 | 7.365 | 7.500 |
| parse_int64 (negative) | ns | 8.080 | 0.349 | 4.32% | 0.216 | 7.950 | 8.710 | 8.800 |
| parse_double | ns | 17.030 | 0.740 | 4.35% | 0.459 | 16.850 | 18.240 | 18.600 |
| parse_bool (strict validation) | ns | 1.940 | 0.066 | 3.42% | 0.041 | 1.900 | 2.055 | 2.100 |
| parse_bool (mixed true/false/quoted) | ns | 4.970 | 0.142 | 2.85% | 0.088 | 4.950 | 5.155 | 5.200 |
| parse_int64 profile (best case) | tail p99 ns | 85.600 | 4.409 | 5.15% | 2.733 | 86.000 | 90.000 | 90.000 |
| parse_int64 profile (typical mixed) | tail p99 ns | 90.300 | 6.856 | 7.59% | 4.250 | 90.000 | 100.350 | 108.000 |
| parse_int64 profile (hard edge) | tail p99 ns | 89.100 | 2.700 | 3.03% | 1.673 | 90.000 | 90.000 | 90.000 |
| parse_bool profile (best case) | tail p99 ns | 57.600 | 9.178 | 15.93% | 5.689 | 63.000 | 67.950 | 72.000 |
| parse_bool profile (typical mixed) | tail p99 ns | 90.200 | 7.947 | 8.81% | 4.926 | 90.000 | 103.950 | 108.000 |
| parse_bool profile (hard edge) | tail p99 ns | 114.600 | 9.718 | 8.48% | 6.023 | 113.000 | 130.950 | 135.000 |
| validate_user profile (best case) | tail p99 ns | 56.800 | 4.069 | 7.16% | 2.522 | 54.000 | 63.000 | 63.000 |
| validate_user profile (typical mixed) | tail p99 ns | 59.600 | 4.176 | 7.01% | 2.588 | 63.000 | 63.000 | 63.000 |
| validate_user profile (hard edge) | tail p99 ns | 63.100 | 0.300 | 0.48% | 0.186 | 63.000 | 63.550 | 64.000 |
| needs_json_escaping (clean, 58 chars) | ns | 8.910 | 0.130 | 1.46% | 0.081 | 8.900 | 9.100 | 9.100 |
| needs_json_escaping (dirty, 25 chars) | ns | 1.100 | 0.000 | 0.00% | 0.000 | 1.100 | 1.100 | 1.100 |
| escape_json_string (no-alloc path) | ns | 16.190 | 0.327 | 2.02% | 0.203 | 16.200 | 16.620 | 16.800 |
| escape_json_string_into (append) | ns | 6.260 | 0.220 | 3.51% | 0.136 | 6.200 | 6.600 | 6.600 |
| skip_value (nested obj with strings) | ns | 29.790 | 0.644 | 2.16% | 0.399 | 29.650 | 30.755 | 30.800 |
| 3-field object parse (linear) | ns | 35.170 | 0.846 | 2.41% | 0.524 | 35.100 | 36.560 | 37.100 |
| 8-field object parse (length-switch) | ns | 172.970 | 4.385 | 2.54% | 2.718 | 172.850 | 178.755 | 178.800 |
| arena alloc+reset cycle (4KB) | ns | 54.180 | 2.131 | 3.93% | 1.321 | 54.500 | 57.210 | 57.300 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| parse_int64 profile (best case) | 288.9M bytes/sec | 10.7M bytes/sec | 3.71% | 6.6M bytes/sec | 262.8M bytes/sec | 290.3M bytes/sec | 299.6M bytes/sec | 299.6M bytes/sec |
| parse_int64 profile (typical mixed) | 417.7M bytes/sec | 14.6M bytes/sec | 3.50% | 9.1M bytes/sec | 396.2M bytes/sec | 416.2M bytes/sec | 437.7M bytes/sec | 438.7M bytes/sec |
| parse_int64 profile (hard edge) | 669.3M bytes/sec | 26.5M bytes/sec | 3.95% | 16.4M bytes/sec | 601.2M bytes/sec | 671.6M bytes/sec | 698.2M bytes/sec | 698.8M bytes/sec |
| parse_bool profile (best case) | 1.10B bytes/sec | 30.6M bytes/sec | 2.77% | 19.0M bytes/sec | 1.06B bytes/sec | 1.10B bytes/sec | 1.15B bytes/sec | 1.16B bytes/sec |
| parse_bool profile (typical mixed) | 843.9M bytes/sec | 25.1M bytes/sec | 2.98% | 15.6M bytes/sec | 801.3M bytes/sec | 834.6M bytes/sec | 884.4M bytes/sec | 893.5M bytes/sec |
| parse_bool profile (hard edge) | 1.67B bytes/sec | 47.1M bytes/sec | 2.82% | 29.2M bytes/sec | 1.61B bytes/sec | 1.67B bytes/sec | 1.75B bytes/sec | 1.77B bytes/sec |
| validate_user profile (best case) | 3.90B bytes/sec | 137.4M bytes/sec | 3.52% | 85.1M bytes/sec | 3.71B bytes/sec | 3.95B bytes/sec | 4.07B bytes/sec | 4.08B bytes/sec |
| validate_user profile (typical mixed) | 3.42B bytes/sec | 76.3M bytes/sec | 2.23% | 47.3M bytes/sec | 3.32B bytes/sec | 3.41B bytes/sec | 3.54B bytes/sec | 3.55B bytes/sec |
| validate_user profile (hard edge) | 3.25B bytes/sec | 120.3M bytes/sec | 3.70% | 74.6M bytes/sec | 3.04B bytes/sec | 3.23B bytes/sec | 3.42B bytes/sec | 3.42B bytes/sec |

---

## Serialization Benchmarks

_Runs: 10 | Aggregation: median_

| Benchmark | Latency | Throughput | Data Rate |
|-----------|---------|------------|-----------|
| escape_json_string (clean, return) | 16.0 ns | 62.6M ops/sec | - |
| escape_json_string_into (clean, append) | 7.0 ns | 143.7M ops/sec | - |
| escape_json_string (dirty, return) | 71.8 ns | 13.9M ops/sec | - |
| escape_json_string_into (dirty, append) | 50.4 ns | 19.9M ops/sec | - |
| escape profile (best case) | 13.1 ns | 76.3M ops/sec | 1.20B bytes/sec |
| escape profile (typical mixed) | 317.5 ns | 3.1M ops/sec | 258.6M bytes/sec |
| escape profile (hard edge) | 3820.0 ns | 262.1K ops/sec | 233.9M bytes/sec |
| serialize object profile (best case) | 73.5 ns | 13.6M ops/sec | 385.6M bytes/sec |
| serialize object profile (typical mixed) | 455.8 ns | 2.2M ops/sec | 263.3M bytes/sec |
| serialize object profile (hard edge) | 4870.0 ns | 205.3K ops/sec | 241.3M bytes/sec |
| needs_json_escaping (16 byte clean) | 2.7 ns | 372.7M ops/sec | - |
| needs_json_escaping (64 byte clean) | 2.8 ns | 352.2M ops/sec | - |
| needs_json_escaping (256 byte clean) | 5.5 ns | 183.3M ops/sec | - |
| needs_json_escaping (64 byte, escape at end) | 2.3 ns | 435.6M ops/sec | - |
| needs_json_escaping (64 byte, escape at start) | 1.7 ns | 581.5M ops/sec | - |
| serialize 5-field obj (embedded commas) | 70.1 ns | 14.3M ops/sec | - |
| serialize array 100 ints (to_chars flat buffer) | 263.3 ns | 3.8M ops/sec | - |
| serialize array 100 ints (to_chars into string) | 1470.0 ns | 678.3K ops/sec | - |
| serialize array 100 ints (lookup table into string) | 129.4 ns | 7.7M ops/sec | - |
| serialize array 1000 ints (optimized) | 1880.0 ns | 532.6K ops/sec | - |
| serialize array 10000 ints (optimized) | 38300.0 ns | 26.1K ops/sec | - |
| serialize nested obj (5 levels deep) | 21.3 ns | 47.0M ops/sec | - |
| serialize 20-field obj | 329.2 ns | 3.0M ops/sec | - |
| serialize 100 nested objects | 4120.0 ns | 242.8K ops/sec | - |
| escape_json_string (10KB clean) | 339.3 ns | 2.9M ops/sec | - |
| escape_json_string (10KB heavy escaping) | 14700.0 ns | 68.0K ops/sec | - |
| escape_json_string (escape at SIMD boundary) | 558.5 ns | 1.8M ops/sec | - |
| needs_json_escaping (1KB clean) | 22.8 ns | 43.9M ops/sec | - |
| needs_json_escaping (1KB, escape at end) | 22.9 ns | 43.7M ops/sec | - |
| escape hot (same string repeated) | 10.9 ns | 91.6M ops/sec | - |
| escape cold (different strings) | 20.8 ns | 48.1M ops/sec | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| escape_json_string (clean, return) | 63.4M | 1.5M | 2.39% | 939.6K | 60.8M | 63.3M | 65.1M | 65.1M |
| escape_json_string_into (clean, append) | 142.8M | 6.9M | 4.81% | 4.3M | 128.3M | 144.0M | 151.1M | 151.3M |
| escape_json_string (dirty, return) | 13.6M | 1.0M | 7.63% | 641.1K | 11.4M | 13.9M | 14.6M | 14.6M |
| escape_json_string_into (dirty, append) | 19.4M | 1.0M | 5.19% | 622.3K | 17.5M | 19.8M | 20.4M | 20.4M |
| escape profile (best case) | 75.1M | 3.1M | 4.11% | 1.9M | 70.8M | 75.2M | 78.8M | 79.0M |
| escape profile (typical mixed) | 3.2M | 87.8K | 2.77% | 54.4K | 3.0M | 3.2M | 3.3M | 3.3M |
| escape profile (hard edge) | 263.3K | 3.9K | 1.49% | 2.4K | 257.9K | 262.5K | 268.7K | 270.0K |
| serialize object profile (best case) | 13.5M | 582.8K | 4.31% | 361.2K | 12.6M | 13.5M | 14.4M | 14.6M |
| serialize object profile (typical mixed) | 2.2M | 70.3K | 3.23% | 43.6K | 2.1M | 2.2M | 2.3M | 2.3M |
| serialize object profile (hard edge) | 204.6K | 4.1K | 1.99% | 2.5K | 194.8K | 205.1K | 209.2K | 210.0K |
| needs_json_escaping (16 byte clean) | 370.1M | 15.3M | 4.13% | 9.5M | 340.7M | 374.3M | 386.3M | 388.3M |
| needs_json_escaping (64 byte clean) | 345.8M | 17.1M | 4.94% | 10.6M | 301.5M | 351.6M | 362.7M | 364.4M |
| needs_json_escaping (256 byte clean) | 180.2M | 5.5M | 3.04% | 3.4M | 173.1M | 180.7M | 187.7M | 189.4M |
| needs_json_escaping (64 byte, escape at end) | 439.2M | 11.2M | 2.56% | 7.0M | 419.0M | 437.7M | 454.2M | 457.1M |
| needs_json_escaping (64 byte, escape at start) | 571.6M | 23.4M | 4.09% | 14.5M | 512.3M | 582.8M | 587.0M | 587.8M |
| serialize 5-field obj (embedded commas) | 14.1M | 450.3K | 3.20% | 279.1K | 13.3M | 14.3M | 14.6M | 14.8M |
| serialize array 100 ints (to_chars flat buffer) | 3.8M | 190.5K | 5.04% | 118.1K | 3.3M | 3.8M | 4.0M | 4.0M |
| serialize array 100 ints (to_chars into string) | 673.6K | 20.4K | 3.03% | 12.7K | 633.5K | 670.8K | 698.9K | 701.2K |
| serialize array 100 ints (lookup table into string) | 7.8M | 207.6K | 2.65% | 128.7K | 7.6M | 7.8M | 8.2M | 8.2M |
| serialize array 1000 ints (optimized) | 533.8K | 25.2K | 4.72% | 15.6K | 494.4K | 539.4K | 564.6K | 570.6K |
| serialize array 10000 ints (optimized) | 25.9K | 968 | 3.73% | 600 | 24.3K | 26.2K | 27.1K | 27.5K |
| serialize nested obj (5 levels deep) | 47.1M | 933.7K | 1.98% | 578.7K | 45.6M | 47.1M | 48.7M | 48.8M |
| serialize 20-field obj | 3.0M | 48.5K | 1.60% | 30.1K | 2.9M | 3.0M | 3.1M | 3.1M |
| serialize 100 nested objects | 240.4K | 5.4K | 2.26% | 3.4K | 227.3K | 242.7K | 245.8K | 246.5K |
| escape_json_string (10KB clean) | 3.0M | 42.8K | 1.44% | 26.5K | 2.9M | 3.0M | 3.0M | 3.1M |
| escape_json_string (10KB heavy escaping) | 66.6K | 3.8K | 5.64% | 2.3K | 56.1K | 68.1K | 69.4K | 69.5K |
| escape_json_string (escape at SIMD boundary) | 1.8M | 42.4K | 2.36% | 26.3K | 1.7M | 1.8M | 1.9M | 1.9M |
| needs_json_escaping (1KB clean) | 43.5M | 1.4M | 3.13% | 844.5K | 41.1M | 43.8M | 45.1M | 45.1M |
| needs_json_escaping (1KB, escape at end) | 43.2M | 1.3M | 2.98% | 797.2K | 40.9M | 43.6M | 44.5M | 44.8M |
| escape hot (same string repeated) | 90.5M | 3.1M | 3.40% | 1.9M | 83.2M | 91.3M | 93.6M | 93.9M |
| escape cold (different strings) | 47.7M | 2.8M | 5.84% | 1.7M | 42.9M | 48.8M | 50.3M | 50.4M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| escape_json_string (clean, return) | ns | 15.790 | 0.370 | 2.34% | 0.229 | 15.800 | 16.310 | 16.400 |
| escape_json_string_into (clean, append) | ns | 7.030 | 0.355 | 5.05% | 0.220 | 6.950 | 7.575 | 7.800 |
| escape_json_string (dirty, return) | ns | 74.240 | 6.252 | 8.42% | 3.875 | 71.750 | 86.415 | 87.900 |
| escape_json_string_into (dirty, append) | ns | 51.840 | 2.785 | 5.37% | 1.726 | 50.450 | 56.255 | 57.200 |
| escape profile (best case) | ns | 13.330 | 0.546 | 4.10% | 0.338 | 13.300 | 14.100 | 14.100 |
| escape profile (typical mixed) | ns | 315.470 | 8.691 | 2.75% | 5.387 | 316.000 | 326.775 | 328.800 |
| escape profile (hard edge) | ns | 3800.000 | 58.992 | 1.55% | 36.563 | 3810.000 | 3875.500 | 3880.000 |
| serialize object profile (best case) | ns | 74.110 | 3.173 | 4.28% | 1.967 | 74.100 | 78.680 | 79.400 |
| serialize object profile (typical mixed) | ns | 459.400 | 15.016 | 3.27% | 9.307 | 457.200 | 482.845 | 483.700 |
| serialize object profile (hard edge) | ns | 4888.000 | 99.177 | 2.03% | 61.470 | 4875.000 | 5058.000 | 5130.000 |
| needs_json_escaping (16 byte clean) | ns | 2.710 | 0.114 | 4.19% | 0.070 | 2.700 | 2.900 | 2.900 |
| needs_json_escaping (64 byte clean) | ns | 2.890 | 0.164 | 5.68% | 0.102 | 2.800 | 3.165 | 3.300 |
| needs_json_escaping (256 byte clean) | ns | 5.560 | 0.174 | 3.14% | 0.108 | 5.550 | 5.800 | 5.800 |
| needs_json_escaping (64 byte, escape at end) | ns | 2.270 | 0.064 | 2.82% | 0.040 | 2.300 | 2.355 | 2.400 |
| needs_json_escaping (64 byte, escape at start) | ns | 1.740 | 0.092 | 5.27% | 0.057 | 1.700 | 1.910 | 2.000 |
| serialize 5-field obj (embedded commas) | ns | 71.220 | 2.306 | 3.24% | 1.429 | 70.150 | 75.120 | 75.300 |
| serialize array 100 ints (to_chars flat buffer) | ns | 265.390 | 14.667 | 5.53% | 9.091 | 262.150 | 290.100 | 305.400 |
| serialize array 100 ints (to_chars into string) | ns | 1487.000 | 44.508 | 2.99% | 27.587 | 1490.000 | 1553.000 | 1580.000 |
| serialize array 100 ints (lookup table into string) | ns | 127.550 | 3.316 | 2.60% | 2.055 | 128.550 | 130.875 | 131.100 |
| serialize array 1000 ints (optimized) | ns | 1877.000 | 90.559 | 4.82% | 56.129 | 1855.000 | 2011.000 | 2020.000 |
| serialize array 10000 ints (optimized) | ns | 38609.000 | 1464.517 | 3.79% | 907.717 | 38215.000 | 41060.500 | 41110.000 |
| serialize nested obj (5 levels deep) | ns | 21.240 | 0.415 | 1.95% | 0.257 | 21.250 | 21.810 | 21.900 |
| serialize 20-field obj | ns | 330.130 | 5.333 | 1.62% | 3.306 | 328.900 | 338.340 | 341.400 |
| serialize 100 nested objects | ns | 4161.000 | 97.000 | 2.33% | 60.121 | 4120.000 | 4328.000 | 4400.000 |
| escape_json_string (10KB clean) | ns | 336.640 | 4.805 | 1.43% | 2.978 | 338.300 | 341.755 | 342.700 |
| escape_json_string (10KB heavy escaping) | ns | 15065.000 | 967.143 | 6.42% | 599.441 | 14690.000 | 16745.500 | 17830.000 |
| escape_json_string (escape at SIMD boundary) | ns | 557.310 | 13.160 | 2.36% | 8.156 | 557.550 | 577.055 | 578.000 |
| needs_json_escaping (1KB clean) | ns | 22.990 | 0.734 | 3.19% | 0.455 | 22.850 | 24.300 | 24.300 |
| needs_json_escaping (1KB, escape at end) | ns | 23.180 | 0.726 | 3.13% | 0.450 | 22.950 | 24.500 | 24.500 |
| escape hot (same string repeated) | ns | 11.040 | 0.390 | 3.54% | 0.242 | 10.950 | 11.730 | 12.000 |
| escape cold (different strings) | ns | 21.010 | 1.293 | 6.16% | 0.802 | 20.500 | 23.120 | 23.300 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| escape profile (best case) | 1.18B bytes/sec | 48.5M bytes/sec | 4.11% | 30.0M bytes/sec | 1.11B bytes/sec | 1.18B bytes/sec | 1.24B bytes/sec | 1.24B bytes/sec |
| escape profile (typical mixed) | 260.5M bytes/sec | 7.2M bytes/sec | 2.77% | 4.5M bytes/sec | 249.7M bytes/sec | 259.9M bytes/sec | 271.4M bytes/sec | 273.2M bytes/sec |
| escape profile (hard edge) | 234.9M bytes/sec | 3.5M bytes/sec | 1.49% | 2.2M bytes/sec | 230.2M bytes/sec | 234.2M bytes/sec | 239.8M bytes/sec | 240.9M bytes/sec |
| serialize object profile (best case) | 383.0M bytes/sec | 16.5M bytes/sec | 4.31% | 10.2M bytes/sec | 356.8M bytes/sec | 382.4M bytes/sec | 409.2M bytes/sec | 412.2M bytes/sec |
| serialize object profile (typical mixed) | 261.5M bytes/sec | 8.4M bytes/sec | 3.23% | 5.2M bytes/sec | 248.1M bytes/sec | 262.5M bytes/sec | 271.1M bytes/sec | 271.5M bytes/sec |
| serialize object profile (hard edge) | 240.5M bytes/sec | 4.8M bytes/sec | 1.99% | 3.0M bytes/sec | 228.9M bytes/sec | 241.0M bytes/sec | 245.8M bytes/sec | 246.8M bytes/sec |

---

## Router Benchmarks

_Runs: 10 | Aggregation: median_

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Router dispatch (hits) | 1.6M ops/sec | 0.579 us | 1.578 us | 3.457 us | 0 |
| Router dispatch (not found) | 1.2M ops/sec | 0.699 us | 2.476 us | 5.455 us | 0 |
| Router dispatch (405) | 1.0M ops/sec | 0.852 us | 2.136 us | 3.736 us | 0 |
| Large router (hits) | 1.3M ops/sec | 0.687 us | 1.686 us | 2.714 us | 0 |
| Large router (not found) | 1.1M ops/sec | 0.762 us | 2.447 us | 3.914 us | 0 |
| Large router (405) | 910.2K ops/sec | 0.972 us | 2.916 us | 4.902 us | 0 |
| Large router (edge cases) | 1.1M ops/sec | 0.788 us | 2.489 us | 4.999 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Router dispatch (hits) | 1.5M | 61.5K | 4.00% | 38.1K | 1.5M | 1.5M | 1.6M | 1.6M |
| Router dispatch (not found) | 1.2M | 68.8K | 5.54% | 42.6K | 1.1M | 1.2M | 1.3M | 1.4M |
| Router dispatch (405) | 1.0M | 43.8K | 4.22% | 27.2K | 968.0K | 1.1M | 1.1M | 1.1M |
| Large router (hits) | 1.3M | 44.4K | 3.48% | 27.5K | 1.2M | 1.3M | 1.3M | 1.3M |
| Large router (not found) | 1.1M | 52.3K | 4.62% | 32.4K | 1.0M | 1.1M | 1.2M | 1.2M |
| Large router (405) | 901.8K | 53.0K | 5.87% | 32.8K | 798.9K | 904.7K | 964.7K | 967.9K |
| Large router (edge cases) | 1.1M | 51.5K | 4.78% | 31.9K | 990.7K | 1.1M | 1.2M | 1.2M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Router dispatch (hits) | p99 us | 1.779 | 0.386 | 21.72% | 0.240 | 1.554 | 2.396 | 2.411 |
| Router dispatch (not found) | p99 us | 2.322 | 0.519 | 22.35% | 0.322 | 2.505 | 2.960 | 3.060 |
| Router dispatch (405) | p99 us | 2.357 | 0.565 | 23.97% | 0.350 | 2.183 | 3.289 | 3.328 |
| Large router (hits) | p99 us | 2.181 | 0.413 | 18.92% | 0.256 | 2.265 | 2.677 | 2.704 |
| Large router (not found) | p99 us | 2.480 | 0.453 | 18.26% | 0.281 | 2.482 | 3.151 | 3.261 |
| Large router (405) | p99 us | 2.751 | 0.594 | 21.58% | 0.368 | 2.859 | 3.606 | 3.699 |
| Large router (edge cases) | p99 us | 2.753 | 0.474 | 17.20% | 0.293 | 2.788 | 3.401 | 3.461 |

---

## Running Benchmarks

```bash
# Build benchmark targets
cmake --preset bench
cmake --build build/bench -j$(nproc)

# Run all benchmarks
./scripts/run_benchmarks.py

# Recommended repeat policy with explicit quality gate
./scripts/run_benchmarks.py --aggregation median --stage-repeat 1=20 --cv-threshold-pct 15

# Include E2E keep-alive scenario
./scripts/run_benchmarks.py --include-e2e

# Collect perf counters (requires perf permissions)
./scripts/run_benchmarks.py --perf-stat

# Compare against a baseline
./scripts/run_benchmarks.py --compare benchmarks/baseline.json --fail-on-regression

# Run specific stages
./scripts/run_benchmarks.py --stage 1 2

# Run individual benchmarks
./build/bench/benchmark/codegen_quality_benchmark
./build/bench/benchmark/serialize_benchmark
./build/bench/benchmark/router_benchmark
./build/bench/benchmark/performance_benchmark
```
