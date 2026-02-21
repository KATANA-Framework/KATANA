# KATANA Benchmark Results

> Last updated: 2026-02-22 02:30:12
> Commit: 579cb42

## Summary

All benchmarks show realistic, stable measurements with proper compiler optimization barriers (`do_not_optimize()` / `clobber_memory()` in `include/bench_utils.hpp`). Performance-critical paths have been optimized with hand-rolled parsers, SIMD-accelerated string processing, and lock-free concurrent data structures.

> **Note**: Results shown are best-of-3 runs. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Core Runtime Benchmarks

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| Ring Buffer Queue (Single Thread) | 166.7M ops/sec | 0.005 us | 0.012 us |
| Ring Buffer Queue (Concurrent 2x2) | 33.3M ops/sec | - | - |
| Ring Buffer Queue (Concurrent 4x4) | 20.4M ops/sec | - | - |
| Ring Buffer Queue (High Contention 8x8) | 9.5M ops/sec | - | - |
| Ring Buffer Queue (Extreme Contention 12x12) | 7.8M ops/sec | - | - |
| Ring Buffer Queue (Max Contention 16x16) | 5.1M ops/sec | - | - |
| Circular Buffer | 166.7M ops/sec | 0.008 us | 0.008 us |
| SIMD CRLF Search (1.5KB buffer) | 100.0M ops/sec | 0.016 us | 0.040 us |
| SIMD CRLF Search (16KB buffer) | 4.5M ops/sec | 0.224 us | 0.494 us |
| SIMD CRLF Search (32KB buffer) | 2.0M ops/sec | 0.441 us | 1.454 us |
| SIMD CRLF Search (64KB buffer) | 1.1M ops/sec | 0.891 us | 1.883 us |
| SIMD CRLF Search (128KB buffer) | 526.3K ops/sec | 1.731 us | 7.202 us |
| HTTP Parser (Complete Request) | 1.0M ops/sec | 0.834 us | 2.887 us |
| HTTP Parser (Fragmented Request) | 1.0M ops/sec | 0.778 us | 3.151 us |
| Arena Allocations (64B objects) | 4.6M ops/sec | - | - |
| Memory Allocations (String Queue) | 50.0M ops/sec | - | - |

---

## Codegen Quality Benchmarks

| Benchmark | Latency | Throughput |
|-----------|---------|------------|
| parse_int64 (simple) | 7.5 ns | 133.7M ops/sec |
| parse_int64 (negative) | 8.6 ns | 116.6M ops/sec |
| parse_double | 15.1 ns | 66.2M ops/sec |
| parse_bool (strict validation) | 0.9 ns | 1.13B ops/sec |
| needs_json_escaping (clean, 58 chars) | 8.3 ns | 119.8M ops/sec |
| needs_json_escaping (dirty, 25 chars) | 1.2 ns | 820.6M ops/sec |
| escape_json_string (no-alloc path) | 20.0 ns | 50.0M ops/sec |
| escape_json_string_into (append) | 7.5 ns | 132.6M ops/sec |
| skip_value (nested obj with strings) | 37.7 ns | 26.5M ops/sec |
| 3-field object parse (linear) | 53.2 ns | 18.8M ops/sec |
| 8-field object parse (length-switch) | 76.7 ns | 13.0M ops/sec |
| arena alloc+reset cycle (4KB) | 36.9 ns | 27.1M ops/sec |

---

## Serialization Benchmarks

| Benchmark | Latency | Throughput |
|-----------|---------|------------|
| escape_json_string (clean, return) | 17.0 ns | 58.7M ops/sec |
| escape_json_string_into (clean, append) | 6.2 ns | 161.8M ops/sec |
| escape_json_string (dirty, return) | 61.8 ns | 16.2M ops/sec |
| escape_json_string_into (dirty, append) | 41.5 ns | 24.1M ops/sec |
| needs_json_escaping (16 byte clean) | 2.8 ns | 353.5M ops/sec |
| needs_json_escaping (64 byte clean) | 3.3 ns | 307.2M ops/sec |
| needs_json_escaping (256 byte clean) | 5.9 ns | 170.0M ops/sec |
| needs_json_escaping (64 byte, escape at end) | 2.3 ns | 443.5M ops/sec |
| needs_json_escaping (64 byte, escape at start) | 1.5 ns | 660.5M ops/sec |
| serialize 5-field obj (embedded commas) | 28.8 ns | 34.8M ops/sec |
| serialize array 100 ints (to_chars) | 322.3 ns | 3.1M ops/sec |
| serialize array 100 ints (optimized) | 207.8 ns | 4.8M ops/sec |
| serialize array 1000 ints (optimized) | 2490.0 ns | 401.7K ops/sec |
| serialize array 10000 ints (optimized) | 46810.0 ns | 21.4K ops/sec |
| serialize nested obj (5 levels deep) | 23.3 ns | 43.0M ops/sec |
| serialize 20-field obj | 286.7 ns | 3.5M ops/sec |
| serialize 100 nested objects | 1860.0 ns | 537.5K ops/sec |
| escape_json_string (10KB clean) | 359.1 ns | 2.8M ops/sec |
| escape_json_string (10KB heavy escaping) | 45230.0 ns | 22.1K ops/sec |
| escape_json_string (escape at SIMD boundary) | 332.3 ns | 3.0M ops/sec |
| needs_json_escaping (1KB clean) | 22.3 ns | 44.8M ops/sec |
| needs_json_escaping (1KB, escape at end) | 23.0 ns | 43.4M ops/sec |
| escape hot (same string repeated) | 11.7 ns | 85.6M ops/sec |
| escape cold (different strings) | 10.5 ns | 95.0M ops/sec |

---

## Router Benchmarks

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| Router dispatch (hits) | 2.0M ops/sec | 0.375 us | 1.399 us |
| Router dispatch (not found) | 2.2M ops/sec | 0.404 us | 0.902 us |
| Router dispatch (405) | 1.5M ops/sec | 0.582 us | 1.391 us |
| Large router (hits) | 1.9M ops/sec | 0.432 us | 1.052 us |
| Large router (not found) | 1.7M ops/sec | 0.470 us | 1.118 us |
| Large router (405) | 1.1M ops/sec | 0.686 us | 1.757 us |
| Large router (edge cases) | 1.5M ops/sec | 0.555 us | 1.118 us |

---

## Running Benchmarks

```bash
# Build benchmark targets
cmake --preset bench
cmake --build build/bench -j$(nproc)

# Run all benchmarks
./scripts/run_benchmarks.py

# Run specific stages
./scripts/run_benchmarks.py --stage 1 2

# Run individual benchmarks
./build/bench/benchmark/codegen_quality_benchmark
./build/bench/benchmark/serialize_benchmark
./build/bench/benchmark/router_benchmark
./build/bench/benchmark/performance_benchmark
```
