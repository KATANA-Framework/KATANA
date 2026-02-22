# KATANA Benchmark Results

> Last updated: 2026-02-22 07:33:53
> Commit: 6a86404

## Summary

All benchmarks show realistic, stable measurements with proper compiler optimization barriers (`do_not_optimize()` / `clobber_memory()` in `benchmark/bench_utils.hpp`). Performance-critical paths have been optimized with hand-rolled parsers, SIMD-accelerated string processing, and lock-free concurrent data structures.

> **Note**: Results shown use median-of-N aggregation across default 10 run(s), overrides: stage 1=15, stage 2=10, stage 3=10, stage 4=10. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Core Runtime Benchmarks

_Runs: 15 | Aggregation: median_

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency max | Retries/op | Errors |
|-----------|------------|-------------|-------------|-------------|-------------|------------|--------|
| Ring Buffer Queue (Single Thread) | 201.0M ops/sec | 0.005 us | - | 0.009 us | - | - | 0 |
| Ring Buffer Queue (Concurrent 2x2) | 36.9M ops/sec | 0.056 us | - | 0.160 us | - | 0.141 | 0 |
| Ring Buffer Queue (Concurrent 4x4) | 19.8M ops/sec | 0.182 us | - | 0.452 us | - | 0.434 | 0 |
| Ring Buffer Queue (Core Saturation 6x6) | 15.5M ops/sec | 0.224 us | - | 3.978 us | - | 1.848 | 0 |
| Ring Buffer Queue (High Contention 8x8) | 10.5M ops/sec | 0.218 us | - | 12.102 us | - | 7.167 | 0 |
| Ring Buffer Queue (Extreme Contention 12x12) | 6.7M ops/sec | 0.276 us | - | 27.702 us | - | 22.463 | 0 |
| Ring Buffer Queue (Max Contention 16x16) | 6.6M ops/sec | 0.282 us | - | 35.416 us | - | 27.041 | 0 |
| Ring Buffer Queue (Oversubscribed 14x14) | 6.6M ops/sec | 0.287 us | - | 23.882 us | - | 26.808 | 0 |
| Circular Buffer | 184.1M ops/sec | 0.005 us | - | 0.008 us | - | - | 0 |
| SIMD CRLF Search (1.5KB buffer) | 55.5M ops/sec | 0.014 us | - | 0.039 us | - | - | 0 |
| SIMD CRLF Search (16KB buffer) | 3.9M ops/sec | 0.197 us | - | 1.612 us | - | - | 0 |
| SIMD CRLF Search (32KB buffer) | 2.2M ops/sec | 0.394 us | - | 1.498 us | - | - | 0 |
| SIMD CRLF Search (64KB buffer) | 1.1M ops/sec | 0.784 us | - | 1.993 us | - | - | 0 |
| SIMD CRLF Search (128KB buffer) | 538.3K ops/sec | 1.562 us | - | 4.297 us | - | - | 0 |
| HTTP Parser (Complete Request) | 1.1M ops/sec | 0.750 us | - | 3.617 us | - | - | 0 |
| HTTP Parser (Fragmented Request) | 1.2M ops/sec | 0.715 us | - | 1.944 us | - | - | 0 |
| Arena Allocations (64B objects) | 6.1M ops/sec | - | - | - | - | - | 0 |
| Memory Allocations (String Queue) | 42.3M ops/sec | - | - | - | - | - | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | 187.9M | 34.9M | 18.59% | 97.7M | 201.0M | 222.4M | 225.2M |
| Ring Buffer Queue (Concurrent 2x2) | 37.0M | 1.7M | 4.60% | 34.8M | 36.9M | 39.3M | 40.6M |
| Ring Buffer Queue (Concurrent 4x4) | 19.9M | 1.2M | 6.15% | 17.9M | 19.8M | 21.7M | 23.1M |
| Ring Buffer Queue (Core Saturation 6x6) | 15.2M | 1.9M | 12.75% | 12.0M | 15.5M | 17.8M | 18.1M |
| Ring Buffer Queue (High Contention 8x8) | 10.6M | 1.2M | 11.36% | 9.1M | 10.5M | 12.6M | 13.4M |
| Ring Buffer Queue (Extreme Contention 12x12) | 7.5M | 2.1M | 28.72% | 5.6M | 6.7M | 9.2M | 14.5M |
| Ring Buffer Queue (Max Contention 16x16) | 6.9M | 1.3M | 18.29% | 5.1M | 6.6M | 9.3M | 9.5M |
| Ring Buffer Queue (Oversubscribed 14x14) | 7.3M | 2.2M | 29.88% | 5.6M | 6.6M | 9.4M | 14.6M |
| Circular Buffer | 174.0M | 32.8M | 18.86% | 72.1M | 184.1M | 202.5M | 205.6M |
| SIMD CRLF Search (1.5KB buffer) | 52.8M | 12.0M | 22.81% | 29.3M | 55.5M | 67.2M | 67.7M |
| SIMD CRLF Search (16KB buffer) | 3.9M | 312.5K | 7.95% | 3.3M | 3.9M | 4.3M | 4.4M |
| SIMD CRLF Search (32KB buffer) | 2.1M | 202.1K | 9.54% | 1.8M | 2.2M | 2.3M | 2.4M |
| SIMD CRLF Search (64KB buffer) | 1.0M | 135.5K | 13.07% | 741.2K | 1.1M | 1.2M | 1.2M |
| SIMD CRLF Search (128KB buffer) | 542.1K | 48.1K | 8.87% | 428.8K | 538.3K | 620.6K | 622.0K |
| HTTP Parser (Complete Request) | 1.1M | 98.4K | 8.83% | 883.5K | 1.1M | 1.2M | 1.2M |
| HTTP Parser (Fragmented Request) | 1.2M | 91.5K | 7.49% | 1.1M | 1.2M | 1.4M | 1.4M |
| Arena Allocations (64B objects) | 5.9M | 818.6K | 13.77% | 4.3M | 6.1M | 6.8M | 7.2M |
| Memory Allocations (String Queue) | 39.7M | 6.7M | 16.94% | 23.3M | 42.3M | 45.8M | 46.7M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | p50 | p95 | Max |
|-----------|--------|------|--------|----|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | p99 us | 0.014 | 0.023 | 166.14% | 0.008 | 0.009 | 0.098 |
| Ring Buffer Queue (Concurrent 2x2) | p99 us | 0.142 | 0.019 | 12.99% | 0.140 | 0.160 | 0.178 |
| Ring Buffer Queue (Concurrent 4x4) | p99 us | 0.534 | 0.254 | 47.56% | 0.452 | 0.623 | 1.459 |
| Ring Buffer Queue (Core Saturation 6x6) | p99 us | 3.558 | 1.262 | 35.45% | 4.226 | 4.559 | 5.370 |
| Ring Buffer Queue (High Contention 8x8) | p99 us | 11.416 | 3.351 | 29.36% | 12.036 | 15.887 | 16.347 |
| Ring Buffer Queue (Extreme Contention 12x12) | p99 us | 29.228 | 8.481 | 29.02% | 28.113 | 39.260 | 42.431 |
| Ring Buffer Queue (Max Contention 16x16) | p99 us | 37.272 | 9.127 | 24.49% | 38.263 | 50.911 | 50.966 |
| Ring Buffer Queue (Oversubscribed 14x14) | p99 us | 34.608 | 13.051 | 37.71% | 34.374 | 54.753 | 55.150 |
| Circular Buffer | p99 us | 0.034 | 0.097 | 285.80% | 0.008 | 0.009 | 0.399 |
| SIMD CRLF Search (1.5KB buffer) | p99 us | 0.051 | 0.066 | 129.92% | 0.038 | 0.046 | 0.296 |
| SIMD CRLF Search (16KB buffer) | p99 us | 0.937 | 0.460 | 49.06% | 0.695 | 1.612 | 2.199 |
| SIMD CRLF Search (32KB buffer) | p99 us | 1.661 | 0.874 | 52.64% | 1.427 | 3.377 | 3.494 |
| SIMD CRLF Search (64KB buffer) | p99 us | 3.294 | 1.700 | 51.61% | 2.913 | 6.351 | 6.627 |
| SIMD CRLF Search (128KB buffer) | p99 us | 6.211 | 2.335 | 37.59% | 5.929 | 9.293 | 11.480 |
| HTTP Parser (Complete Request) | p99 us | 2.951 | 0.827 | 28.02% | 3.208 | 3.645 | 4.705 |
| HTTP Parser (Fragmented Request) | p99 us | 2.506 | 0.837 | 33.42% | 2.363 | 3.966 | 4.082 |

### Contention Stability (Retries/op)

| Benchmark | Mean | Stddev | CV | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|
| Ring Buffer Queue (Concurrent 2x2) | 0.176 | 0.058 | 32.76% | 0.154 | 0.267 | 0.282 |
| Ring Buffer Queue (Concurrent 4x4) | 0.407 | 0.350 | 86.03% | 0.265 | 0.659 | 1.617 |
| Ring Buffer Queue (Core Saturation 6x6) | 2.643 | 1.443 | 54.62% | 2.727 | 4.718 | 5.351 |
| Ring Buffer Queue (High Contention 8x8) | 7.618 | 2.278 | 29.90% | 7.852 | 10.380 | 10.578 |
| Ring Buffer Queue (Extreme Contention 12x12) | 19.846 | 6.365 | 32.07% | 22.463 | 26.081 | 28.134 |
| Ring Buffer Queue (Max Contention 16x16) | 25.226 | 6.422 | 25.46% | 27.041 | 30.978 | 39.722 |
| Ring Buffer Queue (Oversubscribed 14x14) | 23.302 | 6.591 | 28.28% | 25.978 | 29.814 | 31.100 |

---

## Codegen Quality Benchmarks

_Runs: 10 | Aggregation: median_

| Benchmark | Latency | Throughput | Data Rate |
|-----------|---------|------------|-----------|
| parse_int64 (simple) | 7.2 ns | 138.8M ops/sec | - |
| parse_int64 (negative) | 8.4 ns | 118.4M ops/sec | - |
| parse_double | 15.4 ns | 64.7M ops/sec | - |
| parse_bool (strict validation) | 2.2 ns | 445.3M ops/sec | - |
| parse_bool (mixed true/false/quoted) | 4.0 ns | 251.0M ops/sec | - |
| parse_int64 profile (best case) | 9.7 ns | 103.2M ops/sec | 284.8M bytes/sec |
| parse_int64 profile (typical mixed) | 11.8 ns | 85.1M ops/sec | 426.9M bytes/sec |
| parse_int64 profile (hard edge) | 14.8 ns | 67.6M ops/sec | 689.0M bytes/sec |
| parse_bool profile (best case) | 4.3 ns | 230.2M ops/sec | 1.15B bytes/sec |
| parse_bool profile (typical mixed) | 7.6 ns | 130.9M ops/sec | 703.9M bytes/sec |
| parse_bool profile (hard edge) | 6.2 ns | 161.7M ops/sec | 797.3M bytes/sec |
| validate_user profile (best case) | 8.8 ns | 114.0M ops/sec | 3.50B bytes/sec |
| validate_user profile (typical mixed) | 9.9 ns | 100.7M ops/sec | 3.14B bytes/sec |
| validate_user profile (hard edge) | 10.5 ns | 94.9M ops/sec | 3.07B bytes/sec |
| needs_json_escaping (clean, 58 chars) | 10.3 ns | 96.7M ops/sec | - |
| needs_json_escaping (dirty, 25 chars) | 2.2 ns | 462.0M ops/sec | - |
| escape_json_string (no-alloc path) | 18.8 ns | 53.2M ops/sec | - |
| escape_json_string_into (append) | 7.8 ns | 127.6M ops/sec | - |
| skip_value (nested obj with strings) | 35.4 ns | 28.3M ops/sec | - |
| 3-field object parse (linear) | 24.4 ns | 41.0M ops/sec | - |
| 8-field object parse (length-switch) | 80.4 ns | 12.4M ops/sec | - |
| arena alloc+reset cycle (4KB) | 54.1 ns | 18.5M ops/sec | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| parse_int64 (simple) | 134.3M | 13.5M | 10.08% | 100.3M | 138.8M | 145.5M | 146.9M |
| parse_int64 (negative) | 120.9M | 6.5M | 5.37% | 109.3M | 118.4M | 128.8M | 130.3M |
| parse_double | 65.1M | 5.8M | 8.98% | 56.4M | 64.7M | 70.7M | 74.0M |
| parse_bool (strict validation) | 446.5M | 31.5M | 7.06% | 382.0M | 445.3M | 483.8M | 487.1M |
| parse_bool (mixed true/false/quoted) | 247.0M | 21.6M | 8.75% | 183.5M | 251.0M | 259.3M | 261.1M |
| parse_int64 profile (best case) | 101.1M | 13.2M | 13.02% | 82.8M | 103.2M | 116.4M | 119.3M |
| parse_int64 profile (typical mixed) | 84.8M | 9.7M | 11.48% | 66.5M | 85.1M | 95.3M | 96.4M |
| parse_int64 profile (hard edge) | 68.1M | 5.4M | 7.92% | 59.6M | 67.6M | 74.5M | 75.0M |
| parse_bool profile (best case) | 241.9M | 25.3M | 10.45% | 201.7M | 230.2M | 272.8M | 285.3M |
| parse_bool profile (typical mixed) | 131.7M | 15.5M | 11.76% | 106.1M | 130.9M | 140.8M | 161.1M |
| parse_bool profile (hard edge) | 150.5M | 36.7M | 24.37% | 53.3M | 161.7M | 180.2M | 184.0M |
| validate_user profile (best case) | 111.1M | 19.1M | 17.17% | 67.8M | 114.0M | 134.8M | 135.8M |
| validate_user profile (typical mixed) | 102.7M | 4.8M | 4.65% | 96.5M | 100.7M | 109.1M | 111.3M |
| validate_user profile (hard edge) | 96.4M | 10.8M | 11.25% | 76.3M | 94.9M | 105.7M | 112.0M |
| needs_json_escaping (clean, 58 chars) | 99.7M | 13.3M | 13.31% | 69.4M | 96.7M | 113.6M | 114.9M |
| needs_json_escaping (dirty, 25 chars) | 475.1M | 41.0M | 8.64% | 409.5M | 462.0M | 524.2M | 531.7M |
| escape_json_string (no-alloc path) | 53.1M | 5.2M | 9.71% | 43.2M | 53.2M | 59.0M | 59.1M |
| escape_json_string_into (append) | 126.7M | 6.9M | 5.45% | 112.2M | 127.6M | 133.1M | 134.1M |
| skip_value (nested obj with strings) | 28.3M | 2.8M | 10.01% | 23.6M | 28.3M | 31.5M | 31.9M |
| 3-field object parse (linear) | 38.4M | 6.0M | 15.53% | 22.7M | 41.0M | 42.8M | 43.4M |
| 8-field object parse (length-switch) | 12.6M | 1.0M | 8.24% | 10.7M | 12.4M | 13.9M | 14.2M |
| arena alloc+reset cycle (4KB) | 18.5M | 1.0M | 5.56% | 16.6M | 18.5M | 19.6M | 20.3M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | p50 | p95 | Max |
|-----------|--------|------|--------|----|-----|-----|-----|
| parse_int64 (simple) | ns | 7.540 | 0.917 | 12.16% | 7.200 | 8.200 | 10.000 |
| parse_int64 (negative) | ns | 8.300 | 0.434 | 5.22% | 8.200 | 8.700 | 9.100 |
| parse_double | ns | 15.490 | 1.422 | 9.18% | 14.800 | 17.600 | 17.700 |
| parse_bool (strict validation) | ns | 2.260 | 0.156 | 6.91% | 2.200 | 2.400 | 2.600 |
| parse_bool (mixed true/false/quoted) | ns | 4.090 | 0.441 | 10.79% | 4.000 | 4.000 | 5.400 |
| parse_int64 profile (best case) | ns | 10.070 | 1.356 | 13.46% | 9.600 | 11.900 | 12.100 |
| parse_int64 profile (typical mixed) | ns | 11.970 | 1.489 | 12.44% | 11.500 | 14.200 | 15.000 |
| parse_int64 profile (hard edge) | ns | 14.790 | 1.210 | 8.18% | 14.500 | 16.200 | 16.800 |
| parse_bool profile (best case) | ns | 4.180 | 0.435 | 10.42% | 4.000 | 4.600 | 5.000 |
| parse_bool profile (typical mixed) | ns | 7.690 | 0.954 | 12.41% | 7.400 | 9.400 | 9.400 |
| parse_bool profile (hard edge) | ns | 7.560 | 3.828 | 50.64% | 6.100 | 8.400 | 18.800 |
| validate_user profile (best case) | ns | 9.350 | 2.047 | 21.89% | 8.600 | 10.400 | 14.800 |
| validate_user profile (typical mixed) | ns | 9.770 | 0.443 | 4.53% | 9.900 | 10.300 | 10.400 |
| validate_user profile (hard edge) | ns | 10.500 | 1.263 | 12.02% | 9.700 | 11.800 | 13.100 |
| needs_json_escaping (clean, 58 chars) | ns | 10.230 | 1.624 | 15.87% | 9.400 | 11.200 | 14.400 |
| needs_json_escaping (dirty, 25 chars) | ns | 2.110 | 0.187 | 8.85% | 2.100 | 2.300 | 2.400 |
| escape_json_string (no-alloc path) | ns | 19.020 | 1.947 | 10.24% | 18.000 | 20.800 | 23.100 |
| escape_json_string_into (append) | ns | 7.920 | 0.442 | 5.58% | 7.700 | 8.500 | 8.900 |
| skip_value (nested obj with strings) | ns | 35.760 | 3.761 | 10.52% | 35.200 | 41.300 | 42.400 |
| 3-field object parse (linear) | ns | 26.960 | 6.029 | 22.36% | 24.300 | 29.400 | 44.100 |
| 8-field object parse (length-switch) | ns | 80.140 | 6.659 | 8.31% | 80.300 | 86.600 | 93.100 |
| arena alloc+reset cycle (4KB) | ns | 54.080 | 3.036 | 5.61% | 53.700 | 57.000 | 60.100 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| parse_int64 profile (best case) | 278.9M bytes/sec | 36.3M bytes/sec | 13.02% | 228.6M bytes/sec | 284.8M bytes/sec | 321.3M bytes/sec | 329.3M bytes/sec |
| parse_int64 profile (typical mixed) | 425.4M bytes/sec | 48.8M bytes/sec | 11.48% | 333.7M bytes/sec | 426.9M bytes/sec | 477.9M bytes/sec | 483.8M bytes/sec |
| parse_int64 profile (hard edge) | 694.0M bytes/sec | 55.0M bytes/sec | 7.92% | 607.8M bytes/sec | 689.0M bytes/sec | 760.1M bytes/sec | 764.7M bytes/sec |
| parse_bool profile (best case) | 1.21B bytes/sec | 126.2M bytes/sec | 10.45% | 1.01B bytes/sec | 1.15B bytes/sec | 1.36B bytes/sec | 1.42B bytes/sec |
| parse_bool profile (typical mixed) | 708.2M bytes/sec | 83.2M bytes/sec | 11.76% | 570.3M bytes/sec | 703.9M bytes/sec | 757.0M bytes/sec | 865.9M bytes/sec |
| parse_bool profile (hard edge) | 741.9M bytes/sec | 180.8M bytes/sec | 24.37% | 262.9M bytes/sec | 797.3M bytes/sec | 888.2M bytes/sec | 906.9M bytes/sec |
| validate_user profile (best case) | 3.41B bytes/sec | 585.4M bytes/sec | 17.17% | 2.08B bytes/sec | 3.50B bytes/sec | 4.14B bytes/sec | 4.16B bytes/sec |
| validate_user profile (typical mixed) | 3.20B bytes/sec | 149.1M bytes/sec | 4.65% | 3.01B bytes/sec | 3.14B bytes/sec | 3.40B bytes/sec | 3.47B bytes/sec |
| validate_user profile (hard edge) | 3.12B bytes/sec | 350.9M bytes/sec | 11.25% | 2.47B bytes/sec | 3.07B bytes/sec | 3.42B bytes/sec | 3.63B bytes/sec |

---

## Serialization Benchmarks

_Runs: 10 | Aggregation: median_

| Benchmark | Latency | Throughput | Data Rate |
|-----------|---------|------------|-----------|
| escape_json_string (clean, return) | 16.3 ns | 61.4M ops/sec | - |
| escape_json_string_into (clean, append) | 5.7 ns | 175.1M ops/sec | - |
| escape_json_string (dirty, return) | 68.0 ns | 14.7M ops/sec | - |
| escape_json_string_into (dirty, append) | 58.4 ns | 17.1M ops/sec | - |
| escape profile (best case) | 13.6 ns | 73.6M ops/sec | 1.16B bytes/sec |
| escape profile (typical mixed) | 128.6 ns | 7.8M ops/sec | 638.7M bytes/sec |
| escape profile (hard edge) | 1450.0 ns | 687.4K ops/sec | 613.6M bytes/sec |
| serialize object profile (best case) | 64.1 ns | 15.6M ops/sec | 441.6M bytes/sec |
| serialize object profile (typical mixed) | 217.7 ns | 4.6M ops/sec | 551.4M bytes/sec |
| serialize object profile (hard edge) | 1920.0 ns | 521.2K ops/sec | 612.4M bytes/sec |
| needs_json_escaping (16 byte clean) | 2.6 ns | 388.8M ops/sec | - |
| needs_json_escaping (64 byte clean) | 3.1 ns | 325.4M ops/sec | - |
| needs_json_escaping (256 byte clean) | 6.7 ns | 148.5M ops/sec | - |
| needs_json_escaping (64 byte, escape at end) | 2.4 ns | 413.8M ops/sec | - |
| needs_json_escaping (64 byte, escape at start) | 1.8 ns | 569.0M ops/sec | - |
| serialize 5-field obj (embedded commas) | 23.5 ns | 42.6M ops/sec | - |
| serialize array 100 ints (to_chars) | 223.3 ns | 4.5M ops/sec | - |
| serialize array 100 ints (optimized) | 247.6 ns | 4.0M ops/sec | - |
| serialize array 1000 ints (optimized) | 2530.0 ns | 395.7K ops/sec | - |
| serialize array 10000 ints (optimized) | 42710.0 ns | 23.4K ops/sec | - |
| serialize nested obj (5 levels deep) | 23.7 ns | 42.3M ops/sec | - |
| serialize 20-field obj | 297.5 ns | 3.4M ops/sec | - |
| serialize 100 nested objects | 1850.0 ns | 539.2K ops/sec | - |
| escape_json_string (10KB clean) | 362.0 ns | 2.8M ops/sec | - |
| escape_json_string (10KB heavy escaping) | 18420.0 ns | 54.3K ops/sec | - |
| escape_json_string (escape at SIMD boundary) | 299.0 ns | 3.3M ops/sec | - |
| needs_json_escaping (1KB clean) | 24.7 ns | 40.5M ops/sec | - |
| needs_json_escaping (1KB, escape at end) | 23.6 ns | 42.3M ops/sec | - |
| escape hot (same string repeated) | 10.9 ns | 91.6M ops/sec | - |
| escape cold (different strings) | 10.0 ns | 100.1M ops/sec | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| escape_json_string (clean, return) | 59.2M | 7.6M | 12.87% | 45.8M | 61.4M | 67.5M | 68.9M |
| escape_json_string_into (clean, append) | 167.8M | 22.4M | 13.37% | 129.4M | 175.1M | 185.8M | 198.1M |
| escape_json_string (dirty, return) | 14.6M | 1.5M | 10.57% | 10.6M | 14.7M | 16.1M | 16.2M |
| escape_json_string_into (dirty, append) | 17.4M | 1.8M | 10.61% | 13.6M | 17.1M | 19.5M | 20.1M |
| escape profile (best case) | 72.9M | 6.8M | 9.37% | 63.4M | 73.6M | 79.0M | 83.5M |
| escape profile (typical mixed) | 7.7M | 451.7K | 5.87% | 7.0M | 7.8M | 7.9M | 8.7M |
| escape profile (hard edge) | 680.8K | 44.2K | 6.49% | 595.1K | 687.4K | 706.6K | 776.4K |
| serialize object profile (best case) | 14.8M | 2.1M | 13.95% | 10.0M | 15.6M | 17.0M | 17.3M |
| serialize object profile (typical mixed) | 4.6M | 365.6K | 7.98% | 3.6M | 4.6M | 4.9M | 5.0M |
| serialize object profile (hard edge) | 530.6K | 56.5K | 10.65% | 433.6K | 521.2K | 604.7K | 605.7K |
| needs_json_escaping (16 byte clean) | 380.3M | 34.1M | 8.96% | 324.4M | 388.8M | 417.1M | 425.8M |
| needs_json_escaping (64 byte clean) | 307.7M | 51.8M | 16.84% | 203.4M | 325.4M | 358.3M | 375.0M |
| needs_json_escaping (256 byte clean) | 146.4M | 28.5M | 19.49% | 91.2M | 148.5M | 175.8M | 177.5M |
| needs_json_escaping (64 byte, escape at end) | 410.5M | 97.8M | 23.83% | 200.1M | 413.8M | 512.1M | 513.4M |
| needs_json_escaping (64 byte, escape at start) | 550.1M | 97.9M | 17.80% | 320.6M | 569.0M | 644.3M | 665.9M |
| serialize 5-field obj (embedded commas) | 41.4M | 4.5M | 10.77% | 32.0M | 42.6M | 45.3M | 45.7M |
| serialize array 100 ints (to_chars) | 4.5M | 315.2K | 7.03% | 3.9M | 4.5M | 4.9M | 4.9M |
| serialize array 100 ints (optimized) | 4.0M | 409.3K | 10.27% | 3.3M | 4.0M | 4.4M | 4.5M |
| serialize array 1000 ints (optimized) | 400.9K | 22.2K | 5.53% | 367.0K | 395.7K | 419.5K | 449.2K |
| serialize array 10000 ints (optimized) | 23.2K | 4.4K | 19.14% | 11.1K | 23.4K | 26.8K | 27.2K |
| serialize nested obj (5 levels deep) | 41.2M | 7.2M | 17.43% | 26.5M | 42.3M | 48.8M | 49.2M |
| serialize 20-field obj | 3.3M | 208.5K | 6.23% | 2.8M | 3.4M | 3.5M | 3.6M |
| serialize 100 nested objects | 544.3K | 27.0K | 4.96% | 507.1K | 539.2K | 570.2K | 600.9K |
| escape_json_string (10KB clean) | 2.7M | 285.2K | 10.44% | 2.2M | 2.8M | 3.0M | 3.0M |
| escape_json_string (10KB heavy escaping) | 57.4K | 5.6K | 9.85% | 47.7K | 54.3K | 63.6K | 65.1K |
| escape_json_string (escape at SIMD boundary) | 3.2M | 298.6K | 9.33% | 2.5M | 3.3M | 3.4M | 3.4M |
| needs_json_escaping (1KB clean) | 40.2M | 4.5M | 11.19% | 28.1M | 40.5M | 43.4M | 44.0M |
| needs_json_escaping (1KB, escape at end) | 41.2M | 4.7M | 11.37% | 29.1M | 42.3M | 44.9M | 45.8M |
| escape hot (same string repeated) | 88.5M | 11.3M | 12.79% | 60.1M | 91.6M | 98.5M | 98.8M |
| escape cold (different strings) | 96.3M | 13.6M | 14.16% | 64.4M | 100.1M | 108.3M | 109.1M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | p50 | p95 | Max |
|-----------|--------|------|--------|----|-----|-----|-----|
| escape_json_string (clean, return) | ns | 17.190 | 2.391 | 13.91% | 16.300 | 20.100 | 21.800 |
| escape_json_string_into (clean, append) | ns | 6.070 | 0.888 | 14.63% | 5.700 | 7.400 | 7.700 |
| escape_json_string (dirty, return) | ns | 69.560 | 8.961 | 12.88% | 65.700 | 72.800 | 94.200 |
| escape_json_string_into (dirty, append) | ns | 58.350 | 6.638 | 11.38% | 56.300 | 65.200 | 73.300 |
| escape profile (best case) | ns | 13.860 | 1.320 | 9.52% | 13.000 | 15.800 | 15.800 |
| escape profile (typical mixed) | ns | 130.470 | 7.518 | 5.76% | 128.500 | 139.400 | 143.000 |
| escape profile (hard edge) | ns | 1476.000 | 96.042 | 6.51% | 1450.000 | 1570.000 | 1680.000 |
| serialize object profile (best case) | ns | 68.990 | 11.910 | 17.26% | 63.100 | 75.700 | 100.400 |
| serialize object profile (typical mixed) | ns | 219.960 | 20.274 | 9.22% | 212.200 | 228.900 | 275.200 |
| serialize object profile (hard edge) | ns | 1907.000 | 211.237 | 11.08% | 1860.000 | 2190.000 | 2310.000 |
| needs_json_escaping (16 byte clean) | ns | 2.650 | 0.254 | 9.58% | 2.500 | 3.000 | 3.100 |
| needs_json_escaping (64 byte clean) | ns | 3.360 | 0.674 | 20.06% | 3.000 | 4.300 | 4.900 |
| needs_json_escaping (256 byte clean) | ns | 7.170 | 1.762 | 24.58% | 6.400 | 10.000 | 11.000 |
| needs_json_escaping (64 byte, escape at end) | ns | 2.640 | 0.913 | 34.60% | 2.300 | 3.600 | 5.000 |
| needs_json_escaping (64 byte, escape at start) | ns | 1.900 | 0.449 | 23.65% | 1.700 | 2.200 | 3.100 |
| serialize 5-field obj (embedded commas) | ns | 24.500 | 3.053 | 12.46% | 23.100 | 29.600 | 31.200 |
| serialize array 100 ints (to_chars) | ns | 224.050 | 16.137 | 7.20% | 220.400 | 242.000 | 255.500 |
| serialize array 100 ints (optimized) | ns | 253.810 | 27.926 | 11.00% | 239.400 | 299.100 | 301.100 |
| serialize array 1000 ints (optimized) | ns | 2504.000 | 135.218 | 5.40% | 2520.000 | 2630.000 | 2730.000 |
| serialize array 10000 ints (optimized) | ns | 45839.000 | 14987.066 | 32.70% | 41870.000 | 46290.000 | 89800.000 |
| serialize nested obj (5 levels deep) | ns | 25.180 | 5.327 | 21.16% | 22.500 | 31.900 | 37.700 |
| serialize 20-field obj | ns | 300.210 | 21.131 | 7.04% | 294.600 | 305.700 | 358.900 |
| serialize 100 nested objects | ns | 1841.000 | 90.161 | 4.90% | 1830.000 | 1940.000 | 1970.000 |
| escape_json_string (10KB clean) | ns | 370.680 | 42.887 | 11.57% | 361.800 | 446.600 | 454.400 |
| escape_json_string (10KB heavy escaping) | ns | 17606.000 | 1780.400 | 10.11% | 16750.000 | 19480.000 | 20970.000 |
| escape_json_string (escape at SIMD boundary) | ns | 315.710 | 34.125 | 10.81% | 295.000 | 352.500 | 402.000 |
| needs_json_escaping (1KB clean) | ns | 25.300 | 3.613 | 14.28% | 23.500 | 26.500 | 35.500 |
| needs_json_escaping (1KB, escape at end) | ns | 24.690 | 3.517 | 14.24% | 23.100 | 26.400 | 34.400 |
| escape hot (same string repeated) | ns | 11.520 | 1.871 | 16.24% | 10.800 | 12.700 | 16.600 |
| escape cold (different strings) | ns | 10.660 | 1.899 | 17.81% | 10.000 | 12.800 | 15.500 |

### Data Throughput Stability (bytes/sec)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| escape profile (best case) | 1.15B bytes/sec | 107.3M bytes/sec | 9.37% | 995.5M bytes/sec | 1.16B bytes/sec | 1.24B bytes/sec | 1.31B bytes/sec |
| escape profile (typical mixed) | 631.6M bytes/sec | 37.1M bytes/sec | 5.87% | 574.2M bytes/sec | 638.7M bytes/sec | 649.9M bytes/sec | 711.8M bytes/sec |
| escape profile (hard edge) | 607.7M bytes/sec | 39.4M bytes/sec | 6.49% | 531.2M bytes/sec | 613.6M bytes/sec | 630.7M bytes/sec | 693.0M bytes/sec |
| serialize object profile (best case) | 420.5M bytes/sec | 58.7M bytes/sec | 13.95% | 282.1M bytes/sec | 441.6M bytes/sec | 481.1M bytes/sec | 488.7M bytes/sec |
| serialize object profile (typical mixed) | 549.7M bytes/sec | 43.9M bytes/sec | 7.98% | 436.1M bytes/sec | 551.4M bytes/sec | 583.8M bytes/sec | 599.5M bytes/sec |
| serialize object profile (hard edge) | 623.5M bytes/sec | 66.4M bytes/sec | 10.65% | 509.5M bytes/sec | 612.4M bytes/sec | 710.5M bytes/sec | 711.8M bytes/sec |

---

## Router Benchmarks

_Runs: 10 | Aggregation: median_

| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency max | Errors |
|-----------|------------|-------------|-------------|-------------|-------------|--------|
| Router dispatch (hits) | 2.8M ops/sec | 0.315 us | - | 0.604 us | - | 0 |
| Router dispatch (not found) | 2.2M ops/sec | 0.406 us | - | 0.901 us | - | 0 |
| Router dispatch (405) | 1.6M ops/sec | 0.568 us | - | 1.163 us | - | 0 |
| Large router (hits) | 2.2M ops/sec | 0.379 us | - | 0.965 us | - | 0 |
| Large router (not found) | 1.8M ops/sec | 0.478 us | - | 1.082 us | - | 0 |
| Large router (405) | 1.3M ops/sec | 0.658 us | - | 1.380 us | - | 0 |
| Large router (edge cases) | 1.8M ops/sec | 0.496 us | - | 1.001 us | - | 0 |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| Router dispatch (hits) | 2.8M | 107.7K | 3.85% | 2.7M | 2.8M | 2.8M | 3.1M |
| Router dispatch (not found) | 2.2M | 91.0K | 4.17% | 2.0M | 2.2M | 2.3M | 2.3M |
| Router dispatch (405) | 1.6M | 51.6K | 3.20% | 1.5M | 1.6M | 1.7M | 1.7M |
| Large router (hits) | 2.3M | 124.8K | 5.54% | 2.0M | 2.2M | 2.4M | 2.4M |
| Large router (not found) | 1.8M | 96.8K | 5.35% | 1.6M | 1.8M | 1.9M | 1.9M |
| Large router (405) | 1.3M | 56.4K | 4.21% | 1.3M | 1.3M | 1.4M | 1.4M |
| Large router (edge cases) | 1.8M | 51.0K | 2.77% | 1.8M | 1.8M | 1.9M | 1.9M |

### Latency Stability (Across Repeated Runs)

| Benchmark | Metric | Mean | Stddev | CV | p50 | p95 | Max |
|-----------|--------|------|--------|----|-----|-----|-----|
| Router dispatch (hits) | p99 us | 0.642 | 0.039 | 6.10% | 0.649 | 0.676 | 0.677 |
| Router dispatch (not found) | p99 us | 0.843 | 0.077 | 9.17% | 0.857 | 0.901 | 0.902 |
| Router dispatch (405) | p99 us | 1.164 | 0.115 | 9.85% | 1.163 | 1.245 | 1.371 |
| Large router (hits) | p99 us | 0.829 | 0.131 | 15.85% | 0.767 | 0.974 | 1.073 |
| Large router (not found) | p99 us | 1.063 | 0.069 | 6.52% | 1.055 | 1.118 | 1.208 |
| Large router (405) | p99 us | 1.448 | 0.096 | 6.66% | 1.425 | 1.569 | 1.569 |
| Large router (edge cases) | p99 us | 0.988 | 0.046 | 4.69% | 0.974 | 1.037 | 1.082 |

---

## Running Benchmarks

```bash
# Build benchmark targets
cmake --preset bench
cmake --build build/bench -j$(nproc)

# Run all benchmarks
./scripts/run_benchmarks.py

# Recommended repeat policy (core runtime gets more samples)
./scripts/run_benchmarks.py --aggregation median --stage-repeat 1=20

# Include E2E keep-alive scenario
./scripts/run_benchmarks.py --include-e2e

# Collect perf counters (requires perf permissions)
./scripts/run_benchmarks.py --perf-stat

# Run specific stages
./scripts/run_benchmarks.py --stage 1 2

# Run individual benchmarks
./build/bench/benchmark/codegen_quality_benchmark
./build/bench/benchmark/serialize_benchmark
./build/bench/benchmark/router_benchmark
./build/bench/benchmark/performance_benchmark
```
