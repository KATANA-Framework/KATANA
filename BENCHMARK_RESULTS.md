# KATANA Benchmark Results

> Last updated: 2026-02-22 04:04:37
> Commit: ffa5bad

## Summary

All benchmarks show realistic, stable measurements with proper compiler optimization barriers (`do_not_optimize()` / `clobber_memory()` in `include/bench_utils.hpp`). Performance-critical paths have been optimized with hand-rolled parsers, SIMD-accelerated string processing, and lock-free concurrent data structures.

> **Note**: Results shown use median-of-N aggregation across 3 run(s) per stage. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Core Runtime Benchmarks

_Runs: 3 | Aggregation: median_

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| Ring Buffer Queue (Single Thread) | 183.9M ops/sec | 0.005 us | 0.005 us |
| Ring Buffer Queue (Concurrent 2x2) | 33.7M ops/sec | - | - |
| Ring Buffer Queue (Concurrent 4x4) | 18.8M ops/sec | - | - |
| Ring Buffer Queue (High Contention 8x8) | 7.7M ops/sec | - | - |
| Ring Buffer Queue (Extreme Contention 12x12) | 6.7M ops/sec | - | - |
| Ring Buffer Queue (Max Contention 16x16) | 6.5M ops/sec | - | - |
| Circular Buffer | 185.9M ops/sec | 0.004 us | 0.007 us |
| SIMD CRLF Search (1.5KB buffer) | 59.3M ops/sec | 0.014 us | 0.028 us |
| SIMD CRLF Search (16KB buffer) | 5.0M ops/sec | 0.178 us | 0.365 us |
| SIMD CRLF Search (32KB buffer) | 2.3M ops/sec | 0.361 us | 1.806 us |
| SIMD CRLF Search (64KB buffer) | 1.1M ops/sec | 0.814 us | 2.349 us |
| SIMD CRLF Search (128KB buffer) | 586.6K ops/sec | 1.384 us | 4.290 us |
| HTTP Parser (Complete Request) | 1.1M ops/sec | 0.833 us | 2.064 us |
| HTTP Parser (Fragmented Request) | 1.1M ops/sec | 0.790 us | 3.878 us |
| Arena Allocations (64B objects) | 5.0M ops/sec | - | - |
| Memory Allocations (String Queue) | 41.7M ops/sec | - | - |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| Ring Buffer Queue (Single Thread) | 176.4M | 11.2M | 6.34% | 160.6M | 183.9M | 183.9M | 184.7M |
| Ring Buffer Queue (Concurrent 2x2) | 34.9M | 1.9M | 5.56% | 33.4M | 33.7M | 33.7M | 37.7M |
| Ring Buffer Queue (Concurrent 4x4) | 18.6M | 398.8K | 2.14% | 18.1M | 18.8M | 18.8M | 19.0M |
| Ring Buffer Queue (High Contention 8x8) | 8.1M | 1.2M | 14.72% | 6.8M | 7.7M | 7.7M | 9.6M |
| Ring Buffer Queue (Extreme Contention 12x12) | 6.4M | 1.5M | 23.01% | 4.4M | 6.7M | 6.7M | 7.9M |
| Ring Buffer Queue (Max Contention 16x16) | 6.5M | 183.7K | 2.84% | 6.2M | 6.5M | 6.5M | 6.7M |
| Circular Buffer | 188.3M | 13.3M | 7.04% | 173.5M | 185.9M | 185.9M | 205.7M |
| SIMD CRLF Search (1.5KB buffer) | 54.6M | 11.2M | 20.45% | 39.2M | 59.3M | 59.3M | 65.3M |
| SIMD CRLF Search (16KB buffer) | 5.1M | 104.4K | 2.07% | 5.0M | 5.0M | 5.0M | 5.2M |
| SIMD CRLF Search (32KB buffer) | 2.4M | 198.1K | 8.22% | 2.2M | 2.3M | 2.3M | 2.7M |
| SIMD CRLF Search (64KB buffer) | 970.1K | 184.7K | 19.04% | 709.4K | 1.1M | 1.1M | 1.1M |
| SIMD CRLF Search (128KB buffer) | 601.7K | 48.1K | 7.99% | 551.9K | 586.6K | 586.6K | 666.7K |
| HTTP Parser (Complete Request) | 1.1M | 32.7K | 2.96% | 1.1M | 1.1M | 1.1M | 1.1M |
| HTTP Parser (Fragmented Request) | 1.1M | 106.0K | 9.97% | 936.0K | 1.1M | 1.1M | 1.2M |
| Arena Allocations (64B objects) | 5.2M | 731.2K | 14.13% | 4.4M | 5.0M | 5.0M | 6.2M |
| Memory Allocations (String Queue) | 40.6M | 2.0M | 4.94% | 37.8M | 41.7M | 41.7M | 42.3M |

---

## Codegen Quality Benchmarks

_Runs: 3 | Aggregation: median_

| Benchmark | Latency | Throughput |
|-----------|---------|------------|
| parse_int64 (simple) | 7.3 ns | 136.2M ops/sec |
| parse_int64 (negative) | 8.1 ns | 123.8M ops/sec |
| parse_double | 11.5 ns | 86.8M ops/sec |
| parse_bool (strict validation) | 1.0 ns | 1.03B ops/sec |
| needs_json_escaping (clean, 58 chars) | 8.1 ns | 123.3M ops/sec |
| needs_json_escaping (dirty, 25 chars) | 1.1 ns | 937.5M ops/sec |
| escape_json_string (no-alloc path) | 16.2 ns | 61.7M ops/sec |
| escape_json_string_into (append) | 7.6 ns | 131.6M ops/sec |
| skip_value (nested obj with strings) | 31.4 ns | 31.8M ops/sec |
| 3-field object parse (linear) | 61.5 ns | 16.2M ops/sec |
| 8-field object parse (length-switch) | 74.9 ns | 13.3M ops/sec |
| arena alloc+reset cycle (4KB) | 46.2 ns | 21.6M ops/sec |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| parse_int64 (simple) | 135.7M | 2.6M | 1.89% | 132.4M | 136.2M | 136.2M | 138.6M |
| parse_int64 (negative) | 124.7M | 1.4M | 1.09% | 123.7M | 123.8M | 123.8M | 126.7M |
| parse_double | 85.0M | 3.0M | 3.49% | 80.8M | 86.8M | 86.8M | 87.3M |
| parse_bool (strict validation) | 1.08B | 89.9M | 8.34% | 996.2M | 1.03B | 1.03B | 1.20B |
| needs_json_escaping (clean, 58 chars) | 126.0M | 4.6M | 3.62% | 122.3M | 123.3M | 123.3M | 132.4M |
| needs_json_escaping (dirty, 25 chars) | 929.3M | 29.6M | 3.18% | 889.7M | 937.5M | 937.5M | 960.7M |
| escape_json_string (no-alloc path) | 60.8M | 1.5M | 2.42% | 58.8M | 61.7M | 61.7M | 62.0M |
| escape_json_string_into (append) | 133.3M | 9.9M | 7.41% | 122.1M | 131.6M | 131.6M | 146.1M |
| skip_value (nested obj with strings) | 32.4M | 762.9K | 2.36% | 31.8M | 31.8M | 31.8M | 33.4M |
| 3-field object parse (linear) | 18.5M | 3.2M | 17.56% | 16.1M | 16.2M | 16.2M | 23.1M |
| 8-field object parse (length-switch) | 14.1M | 1.2M | 8.57% | 13.1M | 13.3M | 13.3M | 15.8M |
| arena alloc+reset cycle (4KB) | 21.6M | 3.0M | 13.67% | 18.0M | 21.6M | 21.6M | 25.2M |

---

## Serialization Benchmarks

_Runs: 3 | Aggregation: median_

| Benchmark | Latency | Throughput |
|-----------|---------|------------|
| escape_json_string (clean, return) | 17.1 ns | 58.3M ops/sec |
| escape_json_string_into (clean, append) | 6.2 ns | 160.1M ops/sec |
| escape_json_string (dirty, return) | 59.4 ns | 16.8M ops/sec |
| escape_json_string_into (dirty, append) | 39.9 ns | 25.0M ops/sec |
| needs_json_escaping (16 byte clean) | 3.1 ns | 327.3M ops/sec |
| needs_json_escaping (64 byte clean) | 3.1 ns | 320.9M ops/sec |
| needs_json_escaping (256 byte clean) | 6.1 ns | 164.1M ops/sec |
| needs_json_escaping (64 byte, escape at end) | 2.3 ns | 442.4M ops/sec |
| needs_json_escaping (64 byte, escape at start) | 1.5 ns | 659.7M ops/sec |
| serialize 5-field obj (embedded commas) | 27.9 ns | 35.8M ops/sec |
| serialize array 100 ints (to_chars) | 289.6 ns | 3.5M ops/sec |
| serialize array 100 ints (optimized) | 211.8 ns | 4.7M ops/sec |
| serialize array 1000 ints (optimized) | 2410.0 ns | 414.4K ops/sec |
| serialize array 10000 ints (optimized) | 52820.0 ns | 18.9K ops/sec |
| serialize nested obj (5 levels deep) | 22.1 ns | 45.2M ops/sec |
| serialize 20-field obj | 287.1 ns | 3.5M ops/sec |
| serialize 100 nested objects | 1780.0 ns | 561.9K ops/sec |
| escape_json_string (10KB clean) | 357.1 ns | 2.8M ops/sec |
| escape_json_string (10KB heavy escaping) | 44950.0 ns | 22.2K ops/sec |
| escape_json_string (escape at SIMD boundary) | 348.1 ns | 2.9M ops/sec |
| needs_json_escaping (1KB clean) | 23.8 ns | 42.0M ops/sec |
| needs_json_escaping (1KB, escape at end) | 24.6 ns | 40.6M ops/sec |
| escape hot (same string repeated) | 12.0 ns | 83.6M ops/sec |
| escape cold (different strings) | 12.8 ns | 78.3M ops/sec |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| escape_json_string (clean, return) | 59.9M | 3.0M | 4.98% | 57.3M | 58.3M | 58.3M | 64.1M |
| escape_json_string_into (clean, append) | 161.2M | 10.7M | 6.65% | 148.6M | 160.1M | 160.1M | 174.8M |
| escape_json_string (dirty, return) | 16.8M | 359.9K | 2.14% | 16.4M | 16.8M | 16.8M | 17.2M |
| escape_json_string_into (dirty, append) | 23.5M | 2.9M | 12.51% | 19.4M | 25.0M | 25.0M | 26.0M |
| needs_json_escaping (16 byte clean) | 322.6M | 84.8M | 26.27% | 216.5M | 327.3M | 327.3M | 424.0M |
| needs_json_escaping (64 byte clean) | 319.2M | 30.9M | 9.67% | 280.5M | 320.9M | 320.9M | 356.0M |
| needs_json_escaping (256 byte clean) | 163.4M | 11.4M | 6.97% | 149.2M | 164.1M | 164.1M | 177.1M |
| needs_json_escaping (64 byte, escape at end) | 433.6M | 34.6M | 7.99% | 387.4M | 442.4M | 442.4M | 470.9M |
| needs_json_escaping (64 byte, escape at start) | 553.9M | 163.0M | 29.43% | 323.6M | 659.7M | 659.7M | 678.4M |
| serialize 5-field obj (embedded commas) | 36.6M | 1.4M | 3.90% | 35.4M | 35.8M | 35.8M | 38.6M |
| serialize array 100 ints (to_chars) | 3.5M | 14.4K | 0.42% | 3.4M | 3.5M | 3.5M | 3.5M |
| serialize array 100 ints (optimized) | 4.7M | 214.9K | 4.54% | 4.5M | 4.7M | 4.7M | 5.0M |
| serialize array 1000 ints (optimized) | 365.3K | 70.3K | 19.25% | 265.8K | 414.4K | 414.4K | 415.7K |
| serialize array 10000 ints (optimized) | 20.1K | 1.8K | 8.97% | 18.8K | 18.9K | 18.9K | 22.7K |
| serialize nested obj (5 levels deep) | 44.7M | 942.9K | 2.11% | 43.4M | 45.2M | 45.2M | 45.5M |
| serialize 20-field obj | 3.5M | 40.0K | 1.15% | 3.4M | 3.5M | 3.5M | 3.5M |
| serialize 100 nested objects | 569.3K | 23.2K | 4.07% | 545.3K | 561.9K | 561.9K | 600.7K |
| escape_json_string (10KB clean) | 2.8M | 256.7K | 9.15% | 2.5M | 2.8M | 2.8M | 3.1M |
| escape_json_string (10KB heavy escaping) | 21.8K | 620 | 2.84% | 20.9K | 22.2K | 22.2K | 22.3K |
| escape_json_string (escape at SIMD boundary) | 2.7M | 290.5K | 10.73% | 2.3M | 2.9M | 2.9M | 3.0M |
| needs_json_escaping (1KB clean) | 41.0M | 3.0M | 7.34% | 36.9M | 42.0M | 42.0M | 44.1M |
| needs_json_escaping (1KB, escape at end) | 39.7M | 2.9M | 7.39% | 35.7M | 40.6M | 40.6M | 42.7M |
| escape hot (same string repeated) | 71.4M | 19.4M | 27.11% | 44.1M | 83.6M | 83.6M | 86.6M |
| escape cold (different strings) | 77.5M | 14.9M | 19.22% | 58.9M | 78.3M | 78.3M | 95.4M |

---

## Router Benchmarks

_Runs: 3 | Aggregation: median_

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| Router dispatch (hits) | 2.0M ops/sec | 0.358 us | 1.301 us |
| Router dispatch (not found) | 2.2M ops/sec | 0.403 us | 0.889 us |
| Router dispatch (405) | 1.6M ops/sec | 0.559 us | 1.320 us |
| Large router (hits) | 2.1M ops/sec | 0.385 us | 0.944 us |
| Large router (not found) | 2.0M ops/sec | 0.431 us | 0.972 us |
| Large router (405) | 1.2M ops/sec | 0.660 us | 1.650 us |
| Large router (edge cases) | 1.8M ops/sec | 0.514 us | 0.972 us |

### Throughput Stability (Across Repeated Runs)

| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |
|-----------|------|--------|----|-----|-----|-----|-----|
| Router dispatch (hits) | 2.1M | 163.2K | 7.83% | 1.9M | 2.0M | 2.0M | 2.3M |
| Router dispatch (not found) | 2.2M | 75.3K | 3.44% | 2.1M | 2.2M | 2.2M | 2.3M |
| Router dispatch (405) | 1.6M | 144.3K | 9.27% | 1.4M | 1.6M | 1.6M | 1.7M |
| Large router (hits) | 2.1M | 126.2K | 5.91% | 2.0M | 2.1M | 2.1M | 2.3M |
| Large router (not found) | 1.9M | 90.2K | 4.63% | 1.8M | 2.0M | 2.0M | 2.0M |
| Large router (405) | 1.2M | 105.1K | 9.01% | 1.0M | 1.2M | 1.2M | 1.3M |
| Large router (edge cases) | 1.8M | 61.7K | 3.51% | 1.7M | 1.8M | 1.8M | 1.8M |

---

## Running Benchmarks

```bash
# Build benchmark targets
cmake --preset bench
cmake --build build/bench -j$(nproc)

# Run all benchmarks
./scripts/run_benchmarks.py

# Run with repeat aggregation
./scripts/run_benchmarks.py --repeats 5 --aggregation median

# Run specific stages
./scripts/run_benchmarks.py --stage 1 2

# Run individual benchmarks
./build/bench/benchmark/codegen_quality_benchmark
./build/bench/benchmark/serialize_benchmark
./build/bench/benchmark/router_benchmark
./build/bench/benchmark/performance_benchmark
```
