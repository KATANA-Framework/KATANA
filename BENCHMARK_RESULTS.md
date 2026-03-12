# KATANA Benchmark Results

> Last updated: 2026-03-12 16:58:29
> Commit: e7006e3

## Summary

- Stability verdict: contains noisy metrics, inspect before trusting small deltas
- Noisy metrics (CV > 20.0%): 51
- Severely noisy metrics (CV > 40.0%): 26
- E2E network stages included: yes

> **Note**: Results shown use median-of-N aggregation across 10 run(s) per stage. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

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
- Noisy metrics: 51
- Severely noisy metrics: 26

| Top Noisy Metric | CV |
|------------------|----|
| stage12.wrk compute_api POST /compute/sum depth40.latency_p999_us | 300.00% |
| stage11.wrk hello_world GET / depth20.latency_max_us | 237.50% |
| stage9.wrk hello_world GET / depth10.latency_p95_us | 152.06% |
| stage9.wrk hello_world GET / depth10.latency_p99_us | 151.60% |
| stage10.wrk compute_api POST /compute/sum depth10.latency_p95_us | 150.49% |
| stage10.wrk compute_api POST /compute/sum depth10.latency_p99_us | 150.06% |
| stage9.wrk hello_world GET / depth10.avg_latency_us | 147.54% |
| stage10.wrk compute_api POST /compute/sum depth10.latency_p999_us | 145.57% |
| stage9.wrk hello_world GET / depth10.latency_p999_us | 144.90% |
| stage10.wrk compute_api POST /compute/sum depth10.avg_latency_us | 141.41% |

---

## E2E Summary

> **Note**: For pipeline stages, `wrk` latency metrics reflect one scripted batch/request() call,
> not isolated single-request service time. Throughput and error-free completion are the primary
> comparable signals across different pipeline depths.

| Stage | Profile | Benchmark | Workers | wrk t/c | Depth | Duration | Throughput | Avg Latency | p50 | p95 | p99 | Data Rate | Errors |
|-------|---------|-----------|---------|---------|-------|----------|------------|-------------|-----|-----|-----|-----------|--------|
| 9 | canonical | wrk hello_world GET / depth10 | 4 | 4/512 | 10 | 10s | 2.1M req/sec | 1625.000 us | 1350.500 us | 3601.500 us | 5207.500 us | 211.8M bytes/sec | 0 |
| 10 | canonical | wrk compute_api POST /compute/sum depth10 | 4 | 4/512 | 10 | 10s | 1.1M req/sec | 2780.000 us | 2582.000 us | 5294.000 us | 7943.500 us | 108.1M bytes/sec | 0 |
| 11 | peak | wrk hello_world GET / depth20 | 4 | 4/512 | 20 | 5s | 2.6M req/sec | 2270.000 us | 2029.000 us | 5002.000 us | 7329.500 us | 269.5M bytes/sec | 0 |
| 12 | peak | wrk compute_api POST /compute/sum depth40 | 4 | 4/512 | 40 | 5s | 1.2M req/sec | 8770.000 us | 8799.500 us | 16985.500 us | 0.000 us | 118.0M bytes/sec | 0 |

---

## Core Runtime Benchmarks

_Runs: 10 | Aggregation: median_

Ring buffer, circular buffer, SIMD operations, arena allocation

| Config | Value |
|--------|-------|
| binary | performance_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/performance_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Retries/op | Errors |
|-----------|------------|-------------|-------------|--------------|------------|--------|
| Ring Buffer Queue (Single Thread) | 217.2M ops/sec | 0.004 us | 0.009 us | 0.036 us | - | 0 |
| Ring Buffer Queue (Concurrent 2x2) | 32.7M ops/sec | 0.055 us | 0.208 us | 0.506 us | 0.098 | 0 |
| Ring Buffer Queue (Concurrent 4x4) | 18.0M ops/sec | 0.198 us | 0.593 us | 2.955 us | 0.258 | 0 |
| Ring Buffer Queue (Core Saturation 6x6) | 14.8M ops/sec | 0.266 us | 1.642 us | 26.223 us | 1.430 | 0 |
| Ring Buffer Queue (High Contention 8x8) | 10.2M ops/sec | 0.242 us | 2.019 us | 130.119 us | 7.690 | 0 |
| Ring Buffer Queue (Extreme Contention 12x12) | 7.4M ops/sec | 0.248 us | 3.064 us | 311.144 us | 18.163 | 0 |
| Ring Buffer Queue (Max Contention 16x16) | 6.1M ops/sec | 0.258 us | 4.573 us | 434.990 us | 27.722 | 0 |
| Ring Buffer Queue (Oversubscribed 14x14) | 6.9M ops/sec | 0.251 us | 3.849 us | 358.959 us | 23.288 | 0 |
| Circular Buffer | 196.9M ops/sec | 0.004 us | 0.011 us | 0.033 us | - | 0 |
| SIMD CRLF Search (1.5KB buffer) | 53.9M ops/sec | 0.016 us | 0.047 us | 0.116 us | - | 0 |
| SIMD CRLF Search (16KB buffer) | 4.9M ops/sec | 0.185 us | 0.411 us | 0.931 us | - | 0 |
| SIMD CRLF Search (32KB buffer) | 2.4M ops/sec | 0.371 us | 0.787 us | 1.570 us | - | 0 |
| SIMD CRLF Search (64KB buffer) | 1.3M ops/sec | 0.727 us | 1.544 us | 3.562 us | - | 0 |
| SIMD CRLF Search (128KB buffer) | 614.9K ops/sec | 1.442 us | 3.433 us | 6.582 us | - | 0 |
| HTTP Parser (Complete Request) | 1.8M ops/sec | 0.502 us | 1.172 us | 1.781 us | - | 0 |
| HTTP Parser (Fragmented Request) | 2.1M ops/sec | 0.431 us | 1.080 us | 1.892 us | - | 0 |
| Arena Allocations (64B objects) | 15.0M ops/sec | - | - | - | - | 0 |
| Memory Allocations (String Queue) | 23.7M ops/sec | - | - | - | - | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | 214.7M | 9.4M | 4.37% | 5.8M | 190.1M | 217.2M | 223.1M | 223.5M |
| Ring Buffer Queue (Concurrent 2x2) | 32.7M | 786.7K | 2.41% | 487.6K | 31.3M | 32.7M | 33.8M | 34.0M |
| Ring Buffer Queue (Concurrent 4x4) | 18.0M | 227.3K | 1.26% | 140.9K | 17.7M | 18.0M | 18.3M | 18.4M |
| Ring Buffer Queue (Core Saturation 6x6) | 14.7M | 496.0K | 3.37% | 307.4K | 13.9M | 14.8M | 15.3M | 15.4M |
| Ring Buffer Queue (High Contention 8x8) | 10.0M | 1.1M | 10.85% | 671.0K | 8.4M | 10.2M | 11.5M | 12.2M |
| Ring Buffer Queue (Extreme Contention 12x12) | 7.5M | 1.3M | 17.18% | 793.9K | 5.2M | 7.4M | 9.4M | 10.4M |
| Ring Buffer Queue (Max Contention 16x16) | 6.2M | 421.1K | 6.80% | 261.0K | 5.6M | 6.1M | 6.9M | 6.9M |
| Ring Buffer Queue (Oversubscribed 14x14) | 6.9M | 340.3K | 4.97% | 210.9K | 6.3M | 6.9M | 7.4M | 7.5M |
| Circular Buffer | 199.1M | 7.4M | 3.72% | 4.6M | 189.4M | 196.9M | 210.7M | 210.7M |
| SIMD CRLF Search (1.5KB buffer) | 52.7M | 3.7M | 7.05% | 2.3M | 44.7M | 53.9M | 56.3M | 56.8M |
| SIMD CRLF Search (16KB buffer) | 4.9M | 170.0K | 3.50% | 105.4K | 4.5M | 4.9M | 5.1M | 5.1M |
| SIMD CRLF Search (32KB buffer) | 2.4M | 73.8K | 3.03% | 45.8K | 2.3M | 2.4M | 2.5M | 2.6M |
| SIMD CRLF Search (64KB buffer) | 1.2M | 31.7K | 2.54% | 19.6K | 1.2M | 1.3M | 1.3M | 1.3M |
| SIMD CRLF Search (128KB buffer) | 616.1K | 16.8K | 2.73% | 10.4K | 592.9K | 614.9K | 641.0K | 649.5K |
| HTTP Parser (Complete Request) | 1.8M | 37.9K | 2.14% | 23.5K | 1.7M | 1.8M | 1.8M | 1.8M |
| HTTP Parser (Fragmented Request) | 2.0M | 44.1K | 2.15% | 27.3K | 2.0M | 2.1M | 2.1M | 2.1M |
| Arena Allocations (64B objects) | 15.2M | 598.0K | 3.94% | 370.6K | 14.4M | 15.0M | 16.0M | 16.0M |
| Memory Allocations (String Queue) | 23.2M | 1.1M | 4.84% | 696.4K | 21.3M | 23.7M | 24.4M | 24.5M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | p99 us | 0.009 | 0.001 | 7.17% | 0.000 | 0.009 | 0.009 | 0.009 |
| Ring Buffer Queue (Concurrent 2x2) | p99 us | 0.202 | 0.013 | 6.34% | 0.008 | 0.208 | 0.216 | 0.219 |
| Ring Buffer Queue (Concurrent 4x4) | p99 us | 0.596 | 0.017 | 2.81% | 0.010 | 0.593 | 0.623 | 0.626 |
| Ring Buffer Queue (Core Saturation 6x6) | p99 us | 1.610 | 0.190 | 11.78% | 0.118 | 1.642 | 1.886 | 1.983 |
| Ring Buffer Queue (High Contention 8x8) | p99 us | 1.932 | 0.312 | 16.17% | 0.194 | 2.019 | 2.279 | 2.317 |
| Ring Buffer Queue (Extreme Contention 12x12) | p99 us | 3.000 | 0.425 | 14.17% | 0.264 | 3.064 | 3.562 | 3.623 |
| Ring Buffer Queue (Max Contention 16x16) | p99 us | 8.534 | 7.009 | 82.13% | 4.344 | 4.573 | 21.753 | 25.226 |
| Ring Buffer Queue (Oversubscribed 14x14) | p99 us | 6.272 | 3.852 | 61.42% | 2.387 | 3.849 | 13.364 | 13.749 |
| Circular Buffer | p99 us | 0.012 | 0.003 | 22.63% | 0.002 | 0.011 | 0.017 | 0.019 |
| SIMD CRLF Search (1.5KB buffer) | p99 us | 0.052 | 0.010 | 19.78% | 0.006 | 0.047 | 0.069 | 0.070 |
| SIMD CRLF Search (16KB buffer) | p99 us | 0.460 | 0.116 | 25.30% | 0.072 | 0.411 | 0.640 | 0.655 |
| SIMD CRLF Search (32KB buffer) | p99 us | 0.937 | 0.269 | 28.73% | 0.167 | 0.787 | 1.380 | 1.403 |
| SIMD CRLF Search (64KB buffer) | p99 us | 1.669 | 0.295 | 17.66% | 0.183 | 1.544 | 2.192 | 2.254 |
| SIMD CRLF Search (128KB buffer) | p99 us | 3.522 | 0.491 | 13.93% | 0.304 | 3.433 | 4.332 | 4.753 |
| HTTP Parser (Complete Request) | p99 us | 1.228 | 0.136 | 11.09% | 0.084 | 1.172 | 1.435 | 1.449 |
| HTTP Parser (Fragmented Request) | p99 us | 1.075 | 0.036 | 3.33% | 0.022 | 1.080 | 1.116 | 1.118 |

### Contention Stability (Retries/op)

| Benchmark | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|
| Ring Buffer Queue (Concurrent 2x2) | 0.097 | 0.026 | 27.00% | 0.016 | 0.098 | 0.133 | 0.134 |
| Ring Buffer Queue (Concurrent 4x4) | 0.244 | 0.087 | 35.49% | 0.054 | 0.258 | 0.358 | 0.379 |
| Ring Buffer Queue (Core Saturation 6x6) | 1.369 | 0.415 | 30.32% | 0.257 | 1.430 | 1.895 | 2.024 |
| Ring Buffer Queue (High Contention 8x8) | 7.884 | 2.465 | 31.27% | 1.528 | 7.690 | 11.236 | 11.873 |
| Ring Buffer Queue (Extreme Contention 12x12) | 18.652 | 5.414 | 29.03% | 3.356 | 18.163 | 27.680 | 30.201 |
| Ring Buffer Queue (Max Contention 16x16) | 27.371 | 3.493 | 12.76% | 2.165 | 27.722 | 32.363 | 32.563 |
| Ring Buffer Queue (Oversubscribed 14x14) | 23.264 | 1.927 | 8.28% | 1.194 | 23.288 | 26.398 | 27.465 |

---

## Codegen Quality Benchmarks

_Runs: 10 | Aggregation: median_

JSON parsing, string escaping, key dispatch

| Config | Value |
|--------|-------|
| binary | codegen_quality_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/codegen_quality_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Tail p95 | Tail p99 | Data Rate |
|-----------|---------|------------|----------|----------|-----------|
| parse_int64 (simple) | 7.4 ns | 135.8M ops/sec | - | - | - |
| parse_int64 (negative) | 8.6 ns | 116.8M ops/sec | - | - | - |
| parse_double | 16.1 ns | 61.8M ops/sec | - | - | - |
| parse_bool (strict validation) | 2.0 ns | 486.2M ops/sec | - | - | - |
| parse_bool (mixed true/false/quoted) | 5.2 ns | 191.8M ops/sec | - | - | - |
| parse_int64 profile (best case) | 10.8 ns | 92.9M ops/sec | 38.0 ns | 95.5 ns | 256.3M bytes/sec |
| parse_int64 profile (typical mixed) | 13.2 ns | 75.6M ops/sec | 47.5 ns | 96.0 ns | 379.1M bytes/sec |
| parse_int64 profile (hard edge) | 16.4 ns | 61.2M ops/sec | 67.0 ns | 96.0 ns | 624.4M bytes/sec |
| parse_bool profile (best case) | 4.8 ns | 207.0M ops/sec | 29.0 ns | 66.5 ns | 1.03B bytes/sec |
| parse_bool profile (typical mixed) | 10.7 ns | 93.5M ops/sec | 57.0 ns | 91.5 ns | 755.5M bytes/sec |
| parse_bool profile (hard edge) | 13.7 ns | 73.3M ops/sec | 58.0 ns | 124.0 ns | 1.58B bytes/sec |
| validate_user profile (best case) | 8.8 ns | 113.6M ops/sec | 38.0 ns | 67.0 ns | 3.48B bytes/sec |
| validate_user profile (typical mixed) | 10.3 ns | 97.1M ops/sec | 38.5 ns | 67.0 ns | 3.03B bytes/sec |
| validate_user profile (hard edge) | 11.3 ns | 88.4M ops/sec | 39.0 ns | 67.0 ns | 2.86B bytes/sec |
| needs_json_escaping (clean, 58 chars) | 9.5 ns | 105.6M ops/sec | - | - | - |
| needs_json_escaping (dirty, 25 chars) | 1.1 ns | 897.7M ops/sec | - | - | - |
| escape_json_string (no-alloc path) | 17.6 ns | 56.6M ops/sec | - | - | - |
| escape_json_string_into (append) | 6.0 ns | 167.8M ops/sec | - | - | - |
| skip_value (nested obj with strings) | 35.5 ns | 28.2M ops/sec | - | - | - |
| 3-field object parse (linear) | 50.6 ns | 19.8M ops/sec | - | - | - |
| 8-field object parse (length-switch) | 185.9 ns | 5.4M ops/sec | - | - | - |
| arena alloc+reset cycle (4KB) | 58.5 ns | 17.1M ops/sec | - | - | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| parse_int64 (simple) | 134.2M | 3.0M | 2.24% | 1.9M | 127.6M | 135.8M | 136.9M | 136.9M |
| parse_int64 (negative) | 117.0M | 2.0M | 1.72% | 1.2M | 112.0M | 116.8M | 119.5M | 119.6M |
| parse_double | 61.9M | 947.4K | 1.53% | 587.2K | 60.5M | 61.8M | 63.4M | 63.8M |
| parse_bool (strict validation) | 481.8M | 18.9M | 3.92% | 11.7M | 433.4M | 486.2M | 497.6M | 498.0M |
| parse_bool (mixed true/false/quoted) | 192.6M | 7.1M | 3.69% | 4.4M | 179.0M | 191.8M | 202.8M | 205.3M |
| parse_int64 profile (best case) | 92.5M | 2.8M | 3.03% | 1.7M | 88.4M | 92.9M | 96.1M | 96.2M |
| parse_int64 profile (typical mixed) | 75.1M | 2.1M | 2.78% | 1.3M | 71.0M | 75.6M | 77.8M | 78.1M |
| parse_int64 profile (hard edge) | 61.0M | 1.5M | 2.49% | 941.4K | 57.5M | 61.2M | 62.7M | 62.9M |
| parse_bool profile (best case) | 206.3M | 4.9M | 2.37% | 3.0M | 197.4M | 207.0M | 212.3M | 212.7M |
| parse_bool profile (typical mixed) | 93.3M | 3.7M | 3.92% | 2.3M | 86.3M | 93.5M | 97.8M | 98.6M |
| parse_bool profile (hard edge) | 73.4M | 2.2M | 3.00% | 1.4M | 70.5M | 73.3M | 76.7M | 77.3M |
| validate_user profile (best case) | 113.9M | 3.8M | 3.36% | 2.4M | 108.8M | 113.6M | 120.3M | 122.4M |
| validate_user profile (typical mixed) | 97.5M | 2.2M | 2.27% | 1.4M | 94.8M | 97.1M | 101.0M | 102.3M |
| validate_user profile (hard edge) | 88.5M | 2.7M | 3.02% | 1.7M | 83.8M | 88.4M | 91.8M | 92.4M |
| needs_json_escaping (clean, 58 chars) | 105.7M | 2.0M | 1.91% | 1.2M | 101.9M | 105.6M | 108.3M | 109.0M |
| needs_json_escaping (dirty, 25 chars) | 892.6M | 19.4M | 2.17% | 12.0M | 838.3M | 897.7M | 908.3M | 910.5M |
| escape_json_string (no-alloc path) | 56.8M | 1.2M | 2.17% | 764.3K | 54.2M | 56.6M | 58.2M | 58.2M |
| escape_json_string_into (append) | 166.5M | 5.4M | 3.25% | 3.4M | 154.9M | 167.8M | 172.1M | 172.5M |
| skip_value (nested obj with strings) | 27.2M | 2.3M | 8.62% | 1.5M | 20.7M | 28.2M | 28.8M | 28.8M |
| 3-field object parse (linear) | 19.5M | 875.6K | 4.49% | 542.7K | 18.1M | 19.8M | 20.5M | 20.6M |
| 8-field object parse (length-switch) | 5.4M | 72.3K | 1.35% | 44.8K | 5.2M | 5.4M | 5.4M | 5.5M |
| arena alloc+reset cycle (4KB) | 16.9M | 658.1K | 3.89% | 407.9K | 15.8M | 17.1M | 17.8M | 18.0M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| parse_int64 (simple) | ns | 7.460 | 0.174 | 2.34% | 0.108 | 7.400 | 7.755 | 7.800 |
| parse_int64 (negative) | ns | 8.570 | 0.135 | 1.57% | 0.083 | 8.600 | 8.765 | 8.900 |
| parse_double | ns | 16.150 | 0.246 | 1.52% | 0.152 | 16.150 | 16.500 | 16.500 |
| parse_bool (strict validation) | ns | 2.080 | 0.098 | 4.71% | 0.061 | 2.050 | 2.255 | 2.300 |
| parse_bool (mixed true/false/quoted) | ns | 5.190 | 0.192 | 3.70% | 0.119 | 5.200 | 5.465 | 5.600 |
| parse_int64 profile (best case) | tail p99 ns | 94.800 | 4.812 | 5.08% | 2.983 | 95.500 | 100.950 | 105.000 |
| parse_int64 profile (typical mixed) | tail p99 ns | 99.600 | 4.409 | 4.43% | 2.733 | 96.000 | 105.000 | 105.000 |
| parse_int64 profile (hard edge) | tail p99 ns | 97.800 | 3.600 | 3.68% | 2.231 | 96.000 | 105.000 | 105.000 |
| parse_bool profile (best case) | tail p99 ns | 63.000 | 4.712 | 7.48% | 2.920 | 66.500 | 67.000 | 67.000 |
| parse_bool profile (typical mixed) | tail p99 ns | 92.900 | 7.395 | 7.96% | 4.584 | 91.500 | 105.000 | 105.000 |
| parse_bool profile (hard edge) | tail p99 ns | 124.000 | 6.971 | 5.62% | 4.321 | 124.000 | 133.000 | 133.000 |
| validate_user profile (best case) | tail p99 ns | 64.200 | 4.069 | 6.34% | 2.522 | 67.000 | 67.000 | 67.000 |
| validate_user profile (typical mixed) | tail p99 ns | 65.200 | 5.400 | 8.28% | 3.347 | 67.000 | 71.950 | 76.000 |
| validate_user profile (hard edge) | tail p99 ns | 68.900 | 3.807 | 5.52% | 2.359 | 67.000 | 76.550 | 77.000 |
| needs_json_escaping (clean, 58 chars) | ns | 9.460 | 0.180 | 1.90% | 0.112 | 9.500 | 9.755 | 9.800 |
| needs_json_escaping (dirty, 25 chars) | ns | 1.110 | 0.030 | 2.70% | 0.019 | 1.100 | 1.155 | 1.200 |
| escape_json_string (no-alloc path) | ns | 17.620 | 0.394 | 2.24% | 0.244 | 17.650 | 18.230 | 18.500 |
| escape_json_string_into (append) | ns | 6.020 | 0.218 | 3.62% | 0.135 | 5.950 | 6.365 | 6.500 |
| skip_value (nested obj with strings) | ns | 37.090 | 3.919 | 10.57% | 2.429 | 35.450 | 44.025 | 48.300 |
| 3-field object parse (linear) | ns | 51.390 | 2.339 | 4.55% | 1.449 | 50.600 | 54.740 | 55.100 |
| 8-field object parse (length-switch) | ns | 186.550 | 2.543 | 1.36% | 1.576 | 185.950 | 190.475 | 191.600 |
| arena alloc+reset cycle (4KB) | ns | 59.220 | 2.324 | 3.92% | 1.440 | 58.500 | 62.635 | 63.400 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| parse_int64 profile (best case) | 255.4M bytes/sec | 7.7M bytes/sec | 3.03% | 4.8M bytes/sec | 243.8M bytes/sec | 256.3M bytes/sec | 265.2M bytes/sec | 265.6M bytes/sec |
| parse_int64 profile (typical mixed) | 376.8M bytes/sec | 10.5M bytes/sec | 2.78% | 6.5M bytes/sec | 356.2M bytes/sec | 379.1M bytes/sec | 390.5M bytes/sec | 392.0M bytes/sec |
| parse_int64 profile (hard edge) | 621.8M bytes/sec | 15.5M bytes/sec | 2.49% | 9.6M bytes/sec | 586.5M bytes/sec | 624.4M bytes/sec | 639.8M bytes/sec | 641.1M bytes/sec |
| parse_bool profile (best case) | 1.03B bytes/sec | 24.4M bytes/sec | 2.37% | 15.2M bytes/sec | 985.6M bytes/sec | 1.03B bytes/sec | 1.06B bytes/sec | 1.06B bytes/sec |
| parse_bool profile (typical mixed) | 754.5M bytes/sec | 29.6M bytes/sec | 3.92% | 18.4M bytes/sec | 697.9M bytes/sec | 755.5M bytes/sec | 790.7M bytes/sec | 797.4M bytes/sec |
| parse_bool profile (hard edge) | 1.58B bytes/sec | 47.4M bytes/sec | 3.00% | 29.4M bytes/sec | 1.51B bytes/sec | 1.58B bytes/sec | 1.65B bytes/sec | 1.66B bytes/sec |
| validate_user profile (best case) | 3.49B bytes/sec | 117.4M bytes/sec | 3.36% | 72.8M bytes/sec | 3.34B bytes/sec | 3.48B bytes/sec | 3.69B bytes/sec | 3.75B bytes/sec |
| validate_user profile (typical mixed) | 3.04B bytes/sec | 69.1M bytes/sec | 2.27% | 42.8M bytes/sec | 2.96B bytes/sec | 3.03B bytes/sec | 3.15B bytes/sec | 3.19B bytes/sec |
| validate_user profile (hard edge) | 2.86B bytes/sec | 86.6M bytes/sec | 3.02% | 53.7M bytes/sec | 2.71B bytes/sec | 2.86B bytes/sec | 2.97B bytes/sec | 2.99B bytes/sec |

---

## Serialization Benchmarks

_Runs: 10 | Aggregation: median_

JSON serialization, string escaping, array construction

| Config | Value |
|--------|-------|
| binary | serialize_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/serialize_benchmark |
| kind | microbenchmark |

| Benchmark | Latency | Throughput | Data Rate |
|-----------|---------|------------|-----------|
| escape_json_string (clean, return) | 16.9 ns | 59.0M ops/sec | - |
| escape_json_string_into (clean, append) | 7.5 ns | 134.0M ops/sec | - |
| escape_json_string (dirty, return) | 77.6 ns | 12.9M ops/sec | - |
| escape_json_string_into (dirty, append) | 55.1 ns | 18.1M ops/sec | - |
| escape profile (best case) | 13.8 ns | 72.3M ops/sec | 1.14B bytes/sec |
| escape profile (typical mixed) | 332.7 ns | 3.0M ops/sec | 246.8M bytes/sec |
| escape profile (hard edge) | 4085.0 ns | 244.6K ops/sec | 218.3M bytes/sec |
| serialize object profile (best case) | 75.9 ns | 13.2M ops/sec | 373.3M bytes/sec |
| serialize object profile (typical mixed) | 478.9 ns | 2.1M ops/sec | 250.6M bytes/sec |
| serialize object profile (hard edge) | 5220.0 ns | 191.7K ops/sec | 225.3M bytes/sec |
| needs_json_escaping (16 byte clean) | 2.8 ns | 363.0M ops/sec | - |
| needs_json_escaping (64 byte clean) | 3.1 ns | 325.7M ops/sec | - |
| needs_json_escaping (256 byte clean) | 5.8 ns | 170.8M ops/sec | - |
| needs_json_escaping (64 byte, escape at end) | 2.4 ns | 419.3M ops/sec | - |
| needs_json_escaping (64 byte, escape at start) | 1.8 ns | 556.2M ops/sec | - |
| serialize 5-field obj (embedded commas) | 75.0 ns | 13.3M ops/sec | - |
| serialize array 100 ints (to_chars flat buffer) | 266.7 ns | 3.7M ops/sec | - |
| serialize array 100 ints (to_chars into string) | 1550.0 ns | 646.2K ops/sec | - |
| serialize array 100 ints (lookup table into string) | 137.9 ns | 7.3M ops/sec | - |
| serialize array 1000 ints (optimized) | 1975.0 ns | 506.1K ops/sec | - |
| serialize array 10000 ints (optimized) | 40610.0 ns | 24.6K ops/sec | - |
| serialize nested obj (5 levels deep) | 21.8 ns | 46.0M ops/sec | - |
| serialize 20-field obj | 349.0 ns | 2.9M ops/sec | - |
| serialize 100 nested objects | 4330.0 ns | 230.9K ops/sec | - |
| escape_json_string (10KB clean) | 352.2 ns | 2.8M ops/sec | - |
| escape_json_string (10KB heavy escaping) | 15910.0 ns | 62.9K ops/sec | - |
| escape_json_string (escape at SIMD boundary) | 600.0 ns | 1.7M ops/sec | - |
| needs_json_escaping (1KB clean) | 24.6 ns | 40.6M ops/sec | - |
| needs_json_escaping (1KB, escape at end) | 24.2 ns | 41.2M ops/sec | - |
| escape hot (same string repeated) | 11.9 ns | 84.5M ops/sec | - |
| escape cold (different strings) | 22.2 ns | 45.0M ops/sec | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| escape_json_string (clean, return) | 58.0M | 2.5M | 4.37% | 1.6M | 51.3M | 59.0M | 59.9M | 60.1M |
| escape_json_string_into (clean, append) | 131.4M | 10.8M | 8.23% | 6.7M | 99.9M | 134.0M | 139.2M | 139.4M |
| escape_json_string (dirty, return) | 12.9M | 406.8K | 3.15% | 252.1K | 12.2M | 12.9M | 13.6M | 13.6M |
| escape_json_string_into (dirty, append) | 18.2M | 269.2K | 1.48% | 166.8K | 17.8M | 18.1M | 18.6M | 18.6M |
| escape profile (best case) | 71.9M | 1.8M | 2.49% | 1.1M | 66.8M | 72.3M | 73.4M | 73.7M |
| escape profile (typical mixed) | 3.0M | 29.3K | 0.98% | 18.2K | 3.0M | 3.0M | 3.0M | 3.1M |
| escape profile (hard edge) | 243.8K | 6.6K | 2.73% | 4.1K | 230.9K | 244.6K | 251.1K | 251.2K |
| serialize object profile (best case) | 12.9M | 655.6K | 5.10% | 406.4K | 11.5M | 13.2M | 13.5M | 13.5M |
| serialize object profile (typical mixed) | 2.1M | 34.0K | 1.64% | 21.1K | 2.0M | 2.1M | 2.1M | 2.1M |
| serialize object profile (hard edge) | 188.8K | 7.1K | 3.75% | 4.4K | 168.7K | 191.7K | 193.5K | 194.0K |
| needs_json_escaping (16 byte clean) | 359.5M | 12.6M | 3.49% | 7.8M | 337.8M | 363.0M | 373.3M | 375.6M |
| needs_json_escaping (64 byte clean) | 328.3M | 9.2M | 2.82% | 5.7M | 311.5M | 325.7M | 341.0M | 342.1M |
| needs_json_escaping (256 byte clean) | 170.2M | 2.7M | 1.58% | 1.7M | 166.0M | 170.8M | 173.5M | 173.6M |
| needs_json_escaping (64 byte, escape at end) | 419.2M | 6.9M | 1.65% | 4.3M | 406.5M | 419.3M | 428.6M | 429.5M |
| needs_json_escaping (64 byte, escape at start) | 554.4M | 9.2M | 1.66% | 5.7M | 528.5M | 556.2M | 563.2M | 564.7M |
| serialize 5-field obj (embedded commas) | 13.3M | 236.3K | 1.78% | 146.5K | 12.9M | 13.3M | 13.6M | 13.6M |
| serialize array 100 ints (to_chars flat buffer) | 3.8M | 84.2K | 2.24% | 52.2K | 3.6M | 3.7M | 3.9M | 3.9M |
| serialize array 100 ints (to_chars into string) | 648.4K | 11.8K | 1.83% | 7.3K | 623.8K | 646.2K | 665.0K | 665.5K |
| serialize array 100 ints (lookup table into string) | 7.3M | 276.2K | 3.79% | 171.2K | 6.8M | 7.3M | 7.6M | 7.7M |
| serialize array 1000 ints (optimized) | 497.5K | 18.8K | 3.78% | 11.7K | 459.1K | 506.1K | 516.7K | 519.3K |
| serialize array 10000 ints (optimized) | 24.4K | 511 | 2.09% | 317 | 23.2K | 24.6K | 24.9K | 24.9K |
| serialize nested obj (5 levels deep) | 45.9M | 528.9K | 1.15% | 327.8K | 45.0M | 46.0M | 46.7M | 46.8M |
| serialize 20-field obj | 2.9M | 46.2K | 1.61% | 28.6K | 2.8M | 2.9M | 2.9M | 3.0M |
| serialize 100 nested objects | 230.5K | 2.8K | 1.21% | 1.7K | 226.3K | 230.9K | 234.2K | 235.0K |
| escape_json_string (10KB clean) | 2.8M | 58.6K | 2.08% | 36.3K | 2.7M | 2.8M | 2.9M | 2.9M |
| escape_json_string (10KB heavy escaping) | 62.9K | 1.2K | 1.94% | 755 | 61.3K | 62.9K | 64.6K | 64.8K |
| escape_json_string (escape at SIMD boundary) | 1.6M | 49.8K | 3.02% | 30.9K | 1.6M | 1.7M | 1.7M | 1.7M |
| needs_json_escaping (1KB clean) | 40.6M | 562.8K | 1.39% | 348.8K | 39.6M | 40.6M | 41.4M | 41.4M |
| needs_json_escaping (1KB, escape at end) | 41.1M | 963.3K | 2.35% | 597.1K | 39.6M | 41.2M | 42.3M | 42.4M |
| escape hot (same string repeated) | 84.8M | 1.5M | 1.76% | 923.1K | 82.3M | 84.5M | 87.1M | 87.3M |
| escape cold (different strings) | 44.8M | 1.2M | 2.69% | 747.6K | 42.4M | 45.0M | 46.2M | 46.3M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| escape_json_string (clean, return) | ns | 17.280 | 0.832 | 4.81% | 0.515 | 16.950 | 18.780 | 19.500 |
| escape_json_string_into (clean, append) | ns | 7.670 | 0.787 | 10.27% | 0.488 | 7.500 | 8.920 | 10.000 |
| escape_json_string (dirty, return) | ns | 77.570 | 2.443 | 3.15% | 1.514 | 77.600 | 81.210 | 82.200 |
| escape_json_string_into (dirty, append) | ns | 54.950 | 0.809 | 1.47% | 0.501 | 55.100 | 56.030 | 56.300 |
| escape profile (best case) | ns | 13.910 | 0.378 | 2.72% | 0.234 | 13.800 | 14.550 | 15.000 |
| escape profile (typical mixed) | ns | 333.130 | 3.266 | 0.98% | 2.024 | 332.700 | 337.585 | 337.900 |
| escape profile (hard edge) | ns | 4104.000 | 113.860 | 2.77% | 70.571 | 4085.000 | 4303.000 | 4330.000 |
| serialize object profile (best case) | ns | 77.960 | 4.168 | 5.35% | 2.583 | 75.900 | 85.310 | 87.200 |
| serialize object profile (typical mixed) | ns | 481.750 | 8.034 | 1.67% | 4.979 | 478.900 | 496.530 | 499.500 |
| serialize object profile (hard edge) | ns | 5305.000 | 217.405 | 4.10% | 134.749 | 5220.000 | 5682.500 | 5930.000 |
| needs_json_escaping (16 byte clean) | ns | 2.780 | 0.098 | 3.52% | 0.061 | 2.750 | 2.955 | 3.000 |
| needs_json_escaping (64 byte clean) | ns | 3.050 | 0.092 | 3.02% | 0.057 | 3.100 | 3.155 | 3.200 |
| needs_json_escaping (256 byte clean) | ns | 5.880 | 0.087 | 1.48% | 0.054 | 5.850 | 6.000 | 6.000 |
| needs_json_escaping (64 byte, escape at end) | ns | 2.380 | 0.060 | 2.52% | 0.037 | 2.400 | 2.455 | 2.500 |
| needs_json_escaping (64 byte, escape at start) | ns | 1.810 | 0.030 | 1.66% | 0.019 | 1.800 | 1.855 | 1.900 |
| serialize 5-field obj (embedded commas) | ns | 75.200 | 1.339 | 1.78% | 0.830 | 75.000 | 77.285 | 77.600 |
| serialize array 100 ints (to_chars flat buffer) | ns | 265.780 | 5.943 | 2.24% | 3.683 | 266.700 | 273.875 | 276.800 |
| serialize array 100 ints (to_chars into string) | ns | 1543.000 | 28.302 | 1.83% | 17.542 | 1550.000 | 1582.000 | 1600.000 |
| serialize array 100 ints (lookup table into string) | ns | 137.570 | 5.306 | 3.86% | 3.289 | 137.950 | 146.430 | 147.600 |
| serialize array 1000 ints (optimized) | ns | 2013.000 | 77.981 | 3.87% | 48.333 | 1975.000 | 2148.500 | 2180.000 |
| serialize array 10000 ints (optimized) | ns | 40955.000 | 878.991 | 2.15% | 544.804 | 40610.000 | 42502.500 | 43020.000 |
| serialize nested obj (5 levels deep) | ns | 21.780 | 0.264 | 1.21% | 0.164 | 21.750 | 22.200 | 22.200 |
| serialize 20-field obj | ns | 348.560 | 5.587 | 1.60% | 3.463 | 349.050 | 356.540 | 359.600 |
| serialize 100 nested objects | ns | 4340.000 | 52.536 | 1.21% | 32.562 | 4330.000 | 4415.500 | 4420.000 |
| escape_json_string (10KB clean) | ns | 354.950 | 7.556 | 2.13% | 4.683 | 352.200 | 369.235 | 370.000 |
| escape_json_string (10KB heavy escaping) | ns | 15895.000 | 306.309 | 1.93% | 189.852 | 15910.000 | 16269.500 | 16310.000 |
| escape_json_string (escape at SIMD boundary) | ns | 607.500 | 18.672 | 3.07% | 11.573 | 600.050 | 636.215 | 643.100 |
| needs_json_escaping (1KB clean) | ns | 24.660 | 0.356 | 1.44% | 0.220 | 24.650 | 25.255 | 25.300 |
| needs_json_escaping (1KB, escape at end) | ns | 24.360 | 0.578 | 2.37% | 0.358 | 24.250 | 25.255 | 25.300 |
| escape hot (same string repeated) | ns | 11.790 | 0.197 | 1.67% | 0.122 | 11.850 | 12.055 | 12.100 |
| escape cold (different strings) | ns | 22.360 | 0.620 | 2.77% | 0.384 | 22.250 | 23.330 | 23.600 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| escape profile (best case) | 1.13B bytes/sec | 28.1M bytes/sec | 2.49% | 17.4M bytes/sec | 1.05B bytes/sec | 1.14B bytes/sec | 1.15B bytes/sec | 1.16B bytes/sec |
| escape profile (typical mixed) | 246.6M bytes/sec | 2.4M bytes/sec | 0.98% | 1.5M bytes/sec | 243.1M bytes/sec | 246.8M bytes/sec | 249.8M bytes/sec | 250.7M bytes/sec |
| escape profile (hard edge) | 217.6M bytes/sec | 5.9M bytes/sec | 2.73% | 3.7M bytes/sec | 206.0M bytes/sec | 218.3M bytes/sec | 224.1M bytes/sec | 224.2M bytes/sec |
| serialize object profile (best case) | 364.3M bytes/sec | 18.6M bytes/sec | 5.10% | 11.5M bytes/sec | 324.8M bytes/sec | 373.3M bytes/sec | 381.5M bytes/sec | 383.3M bytes/sec |
| serialize object profile (typical mixed) | 249.2M bytes/sec | 4.1M bytes/sec | 1.64% | 2.5M bytes/sec | 240.3M bytes/sec | 250.6M bytes/sec | 253.4M bytes/sec | 254.1M bytes/sec |
| serialize object profile (hard edge) | 221.9M bytes/sec | 8.3M bytes/sec | 3.75% | 5.2M bytes/sec | 198.3M bytes/sec | 225.3M bytes/sec | 227.4M bytes/sec | 228.0M bytes/sec |

---

## Router Benchmarks

_Runs: 10 | Aggregation: median_

HTTP routing dispatch performance

| Config | Value |
|--------|-------|
| binary | router_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/router_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Router dispatch (hits) | 3.1M ops/sec | 0.293 us | 0.689 us | 1.822 us | 0 |
| Router dispatch (not found) | 3.3M ops/sec | 0.277 us | 0.621 us | 1.470 us | 0 |
| Router dispatch (405) | 1.9M ops/sec | 0.475 us | 1.191 us | 2.216 us | 0 |
| Large router (hits) | 2.5M ops/sec | 0.365 us | 0.829 us | 1.860 us | 0 |
| Large router (not found) | 2.5M ops/sec | 0.355 us | 0.857 us | 1.954 us | 0 |
| Large router (405) | 1.4M ops/sec | 0.611 us | 1.728 us | 3.718 us | 0 |
| Large router (edge cases) | 1.9M ops/sec | 0.475 us | 1.289 us | 2.379 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Router dispatch (hits) | 3.1M | 84.7K | 2.76% | 52.5K | 2.9M | 3.1M | 3.2M | 3.2M |
| Router dispatch (not found) | 3.2M | 132.9K | 4.12% | 82.4K | 3.0M | 3.3M | 3.4M | 3.4M |
| Router dispatch (405) | 1.9M | 67.2K | 3.61% | 41.7K | 1.7M | 1.9M | 1.9M | 1.9M |
| Large router (hits) | 2.5M | 165.0K | 6.71% | 102.3K | 2.0M | 2.5M | 2.6M | 2.6M |
| Large router (not found) | 2.5M | 120.3K | 4.84% | 74.6K | 2.3M | 2.5M | 2.6M | 2.6M |
| Large router (405) | 1.4M | 72.7K | 5.15% | 45.0K | 1.3M | 1.4M | 1.5M | 1.5M |
| Large router (edge cases) | 1.8M | 123.8K | 6.77% | 76.8K | 1.5M | 1.9M | 1.9M | 2.0M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Router dispatch (hits) | p99 us | 0.786 | 0.212 | 26.96% | 0.131 | 0.689 | 1.186 | 1.303 |
| Router dispatch (not found) | p99 us | 0.772 | 0.266 | 34.43% | 0.165 | 0.621 | 1.254 | 1.328 |
| Router dispatch (405) | p99 us | 1.264 | 0.165 | 13.06% | 0.102 | 1.191 | 1.579 | 1.609 |
| Large router (hits) | p99 us | 0.964 | 0.271 | 28.11% | 0.168 | 0.829 | 1.438 | 1.602 |
| Large router (not found) | p99 us | 0.933 | 0.217 | 23.29% | 0.135 | 0.857 | 1.326 | 1.456 |
| Large router (405) | p99 us | 1.726 | 0.309 | 17.91% | 0.192 | 1.728 | 2.236 | 2.415 |
| Large router (edge cases) | p99 us | 1.265 | 0.226 | 17.84% | 0.140 | 1.289 | 1.592 | 1.756 |

---

## Generated API Dispatch Benchmarks

_Runs: 10 | Aggregation: median_

Generated router bindings, param extraction, parse and serialize on ready request objects

| Config | Value |
|--------|-------|
| binary | generated_api_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/generated_api_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|--------------|--------|
| Generated API dispatch+parse | 3.4M ops/sec | 0.153 us | 0.560 us | 0.848 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| Generated API dispatch+parse | 3.3M | 194.0K | 5.84% | 120.3K | 2.9M | 3.4M | 3.5M | 3.5M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| Generated API dispatch+parse | p99 us | 0.549 | 0.060 | 11.00% | 0.037 | 0.560 | 0.619 | 0.623 |

---

## Generated API Mixed Workload Benchmarks

_Runs: 10 | Aggregation: median_

Generated stack mixed valid/error workload on benchmark_api spec

| Config | Value |
|--------|-------|
| binary | benchmark_api_codegen_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/benchmark_api_codegen_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_codegen_valid_mix | 715.5K ops/sec | 0.736 us | 4.750 us | 8.387 us | 20.919 us | 0 |
| benchmark_api_codegen_error_mix | 1.6M ops/sec | 0.478 us | 0.765 us | 1.037 us | 2.405 us | 120000 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| benchmark_api_codegen_valid_mix | 710.3K | 20.3K | 2.86% | 12.6K | 680.1K | 715.5K | 734.0K | 734.3K |
| benchmark_api_codegen_error_mix | 1.6M | 94.1K | 6.05% | 58.3K | 1.3M | 1.6M | 1.6M | 1.6M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| benchmark_api_codegen_valid_mix | p99 us | 8.368 | 0.425 | 5.07% | 0.263 | 8.387 | 8.958 | 9.014 |
| benchmark_api_codegen_error_mix | p99 us | 1.047 | 0.073 | 6.93% | 0.045 | 1.037 | 1.168 | 1.223 |

---

## Generated API Framework Path Benchmarks

_Runs: 10 | Aggregation: median_

Generated stack valid/error workload on benchmark_api spec with minimal handlers

| Config | Value |
|--------|-------|
| binary | benchmark_api_framework_benchmark |
| binary_path | /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl/benchmark/benchmark_api_framework_benchmark |
| kind | microbenchmark |

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency p999 | Errors |
|-----------|------------|-------------|-------------|-------------|--------------|--------|
| benchmark_api_framework_valid_mix | 1.4M ops/sec | 0.512 us | 0.994 us | 1.692 us | 3.737 us | 0 |
| benchmark_api_framework_error_mix | 1.6M ops/sec | 0.497 us | 0.807 us | 1.051 us | 3.255 us | 120000 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| benchmark_api_framework_valid_mix | 1.4M | 45.4K | 3.36% | 28.2K | 1.2M | 1.4M | 1.4M | 1.4M |
| benchmark_api_framework_error_mix | 1.6M | 35.0K | 2.24% | 21.7K | 1.5M | 1.6M | 1.6M | 1.6M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| benchmark_api_framework_valid_mix | p99 us | 1.703 | 0.080 | 4.72% | 0.050 | 1.692 | 1.829 | 1.855 |
| benchmark_api_framework_error_mix | p99 us | 1.067 | 0.040 | 3.76% | 0.025 | 1.051 | 1.135 | 1.156 |

---

## HTTP Load Benchmark (hello_world_server canonical pipeline via wrk)

_Runs: 10 | Aggregation: median_

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
| wrk hello_world GET / depth10 | 2.1M req/sec | 211.8M bytes/sec | 1625.000 us | 1350.500 us | 3601.500 us | 5207.500 us | 31570.000 us | 60661.500 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth10 | 2.0M | 204.6K | 10.38% | 126.8K | 1.5M | 2.1M | 2.1M | 2.1M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk hello_world GET / depth10 | avg us | 36164.000 | 53357.689 | 147.54% | 33071.438 | 1625.000 | 126329.000 | 134330.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth10 | 201.1M bytes/sec | 20.9M bytes/sec | 10.38% | 12.9M bytes/sec | 150.0M bytes/sec | 211.8M bytes/sec | 217.4M bytes/sec | 218.7M bytes/sec |

---

## HTTP Load Benchmark (compute_api canonical pipeline via wrk)

_Runs: 10 | Aggregation: median_

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
| wrk compute_api POST /compute/sum depth10 | 1.1M req/sec | 108.1M bytes/sec | 2780.000 us | 2582.000 us | 5294.000 us | 7943.500 us | 27579.000 us | 74687.500 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | 1.1M | 75.0K | 6.88% | 46.5K | 971.9K | 1.1M | 1.2M | 1.2M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | avg us | 34129.000 | 48261.581 | 141.41% | 29912.838 | 2780.000 | 114875.500 | 116230.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth10 | 104.6M bytes/sec | 7.2M bytes/sec | 6.88% | 4.5M bytes/sec | 93.3M bytes/sec | 108.1M bytes/sec | 111.2M bytes/sec | 111.5M bytes/sec |

---

## HTTP Load Benchmark (hello_world_server peak pipeline via wrk)

_Runs: 10 | Aggregation: median_

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
| wrk hello_world GET / depth20 | 2.6M req/sec | 269.5M bytes/sec | 2270.000 us | 2029.000 us | 5002.000 us | 7329.500 us | 0.000 us | 42750.500 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth20 | 2.6M | 180.4K | 6.94% | 111.8K | 2.1M | 2.6M | 2.7M | 2.7M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk hello_world GET / depth20 | avg us | 2779.000 | 1524.805 | 54.87% | 945.084 | 2270.000 | 5122.500 | 7350.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk hello_world GET / depth20 | 265.3M bytes/sec | 18.4M bytes/sec | 6.94% | 11.4M bytes/sec | 211.5M bytes/sec | 269.5M bytes/sec | 277.6M bytes/sec | 278.0M bytes/sec |

---

## HTTP Load Benchmark (compute_api peak pipeline via wrk)

_Runs: 10 | Aggregation: median_

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
| wrk compute_api POST /compute/sum depth40 | 1.2M req/sec | 118.0M bytes/sec | 8770.000 us | 8799.500 us | 16985.500 us | 0.000 us | 0.000 us | 60388.000 us | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | 1.1M | 142.8K | 12.41% | 88.5K | 926.5K | 1.2M | 1.3M | 1.3M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |
|-----------|--------|------|--------|----|--------|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | avg us | 65155.000 | 86436.608 | 132.66% | 53573.965 | 8770.000 | 204726.500 | 208070.000 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |
|-----------|------|--------|----|--------|-----|-----|-----|-----|
| wrk compute_api POST /compute/sum depth40 | 110.4M bytes/sec | 13.7M bytes/sec | 12.41% | 8.5M bytes/sec | 89.0M bytes/sec | 118.0M bytes/sec | 121.6M bytes/sec | 121.9M bytes/sec |

---

## Baseline Comparison

- Compared metrics: 60
- Hard regressions: 0
- Noisy regressions (ignored by CV gate): 6
- Improvements: 26
- Policy-skipped metrics: 14
- Thresholds: delta=5.00% | CV gate=10.00%

- Policy: wrk_http compares throughput/errors only and requires repeated runs on both current and baseline.

| Policy-Skipped Metric | Reason | Current Samples | Baseline Samples |
|-----------------------|--------|-----------------|------------------|
| stage10.wrk_compute_api_post_computesum_depth10.avg_latency_us | wrk_informational_only | 10 | 5 |
| stage10.wrk_compute_api_post_computesum_depth10.bytes_per_sec | wrk_informational_only | 10 | 5 |
| stage10.wrk_compute_api_post_computesum_depth10.latency_max_us | wrk_informational_only | 10 | 5 |
| stage10.wrk_compute_api_post_computesum_depth10.latency_p50_us | wrk_informational_only | 10 | 5 |
| stage10.wrk_compute_api_post_computesum_depth10.latency_p95_us | wrk_informational_only | 10 | 5 |
| stage10.wrk_compute_api_post_computesum_depth10.latency_p999_us | wrk_informational_only | 10 | 5 |
| stage10.wrk_compute_api_post_computesum_depth10.latency_p99_us | wrk_informational_only | 10 | 5 |
| stage12.wrk_compute_api_post_computesum_depth40.avg_latency_us | wrk_informational_only | 10 | 5 |
| stage12.wrk_compute_api_post_computesum_depth40.bytes_per_sec | wrk_informational_only | 10 | 5 |
| stage12.wrk_compute_api_post_computesum_depth40.latency_max_us | wrk_informational_only | 10 | 5 |

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
