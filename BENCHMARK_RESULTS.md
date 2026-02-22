# KATANA Benchmark Results

> Last updated: 2026-02-22 03:41:28
> Commit: b641b3e

## Summary

All benchmarks show realistic, stable measurements with proper compiler optimization barriers (`do_not_optimize()` / `clobber_memory()` in `include/bench_utils.hpp`). Performance-critical paths have been optimized with hand-rolled parsers, SIMD-accelerated string processing, and lock-free concurrent data structures.

> **Note**: Results shown are best-of-3 runs. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Core Runtime Benchmarks

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| Ring Buffer Queue (Single Thread) | 187.4M ops/sec | 0.005 us | 0.006 us |
| Ring Buffer Queue (Concurrent 2x2) | 41.8M ops/sec | - | - |
| Ring Buffer Queue (Concurrent 4x4) | 19.3M ops/sec | - | - |
| Ring Buffer Queue (High Contention 8x8) | 10.9M ops/sec | - | - |
| Ring Buffer Queue (Extreme Contention 12x12) | 6.2M ops/sec | - | - |
| Ring Buffer Queue (Max Contention 16x16) | 5.5M ops/sec | - | - |
| Circular Buffer | 225.5M ops/sec | 0.004 us | 0.004 us |
| SIMD CRLF Search (1.5KB buffer) | 62.6M ops/sec | 0.014 us | 0.026 us |
| SIMD CRLF Search (16KB buffer) | 5.6M ops/sec | 0.170 us | 0.364 us |
| SIMD CRLF Search (32KB buffer) | 2.7M ops/sec | 0.352 us | 0.705 us |
| SIMD CRLF Search (64KB buffer) | 1.2M ops/sec | 0.798 us | 1.760 us |
| SIMD CRLF Search (128KB buffer) | 696.0K ops/sec | 1.358 us | 3.407 us |
| HTTP Parser (Complete Request) | 1.2M ops/sec | 0.754 us | 1.450 us |
| HTTP Parser (Fragmented Request) | 1.3M ops/sec | 0.702 us | 1.312 us |
| Arena Allocations (64B objects) | 6.1M ops/sec | - | - |
| Memory Allocations (String Queue) | 43.2M ops/sec | - | - |

---

## Codegen Quality Benchmarks

| Benchmark | Latency | Throughput |
|-----------|---------|------------|
| parse_int64 (simple) | 6.9 ns | 145.8M ops/sec |
| parse_int64 (negative) | 7.5 ns | 133.6M ops/sec |
| parse_double | 10.8 ns | 92.6M ops/sec |
| parse_bool (strict validation) | 0.8 ns | 1.22B ops/sec |
| needs_json_escaping (clean, 58 chars) | 7.3 ns | 137.1M ops/sec |
| needs_json_escaping (dirty, 25 chars) | 1.1 ns | 945.5M ops/sec |
| escape_json_string (no-alloc path) | 15.4 ns | 64.7M ops/sec |
| escape_json_string_into (append) | 7.0 ns | 143.6M ops/sec |
| skip_value (nested obj with strings) | 29.6 ns | 33.8M ops/sec |
| 3-field object parse (linear) | 43.5 ns | 23.0M ops/sec |
| 8-field object parse (length-switch) | 63.6 ns | 15.7M ops/sec |
| arena alloc+reset cycle (4KB) | 38.3 ns | 26.1M ops/sec |

---

## Serialization Benchmarks

| Benchmark | Latency | Throughput |
|-----------|---------|------------|
| escape_json_string (clean, return) | 15.3 ns | 65.4M ops/sec |
| escape_json_string_into (clean, append) | 5.6 ns | 178.4M ops/sec |
| escape_json_string (dirty, return) | 53.8 ns | 18.6M ops/sec |
| escape_json_string_into (dirty, append) | 37.5 ns | 26.6M ops/sec |
| needs_json_escaping (16 byte clean) | 2.4 ns | 421.3M ops/sec |
| needs_json_escaping (64 byte clean) | 2.7 ns | 369.9M ops/sec |
| needs_json_escaping (256 byte clean) | 5.3 ns | 188.7M ops/sec |
| needs_json_escaping (64 byte, escape at end) | 2.1 ns | 483.9M ops/sec |
| needs_json_escaping (64 byte, escape at start) | 1.4 ns | 691.1M ops/sec |
| serialize 5-field obj (embedded commas) | 24.9 ns | 40.1M ops/sec |
| serialize array 100 ints (to_chars) | 289.1 ns | 3.5M ops/sec |
| serialize array 100 ints (optimized) | 199.1 ns | 5.0M ops/sec |
| serialize array 1000 ints (optimized) | 2430.0 ns | 412.4K ops/sec |
| serialize array 10000 ints (optimized) | 47620.0 ns | 21.0K ops/sec |
| serialize nested obj (5 levels deep) | 21.9 ns | 45.6M ops/sec |
| serialize 20-field obj | 272.2 ns | 3.7M ops/sec |
| serialize 100 nested objects | 1700.0 ns | 588.6K ops/sec |
| escape_json_string (10KB clean) | 311.0 ns | 3.2M ops/sec |
| escape_json_string (10KB heavy escaping) | 43580.0 ns | 22.9K ops/sec |
| escape_json_string (escape at SIMD boundary) | 343.6 ns | 2.9M ops/sec |
| needs_json_escaping (1KB clean) | 23.0 ns | 43.5M ops/sec |
| needs_json_escaping (1KB, escape at end) | 23.5 ns | 42.5M ops/sec |
| escape hot (same string repeated) | 11.2 ns | 89.7M ops/sec |
| escape cold (different strings) | 9.8 ns | 102.4M ops/sec |

---

## Router Benchmarks

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| Router dispatch (hits) | 2.2M ops/sec | 0.366 us | 1.229 us |
| Router dispatch (not found) | 2.3M ops/sec | 0.403 us | 0.871 us |
| Router dispatch (405) | 1.8M ops/sec | 0.568 us | 1.174 us |
| Large router (hits) | 2.1M ops/sec | 0.394 us | 0.936 us |
| Large router (not found) | 2.1M ops/sec | 0.432 us | 0.963 us |
| Large router (405) | 1.3M ops/sec | 0.670 us | 1.504 us |
| Large router (edge cases) | 1.8M ops/sec | 0.514 us | 1.009 us |

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
