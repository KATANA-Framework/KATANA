# KATANA Benchmark Results

> Last updated: 2026-02-09 00:55:02
> Commit: 8e09d63

## Summary

All benchmarks show realistic, stable measurements with proper compiler optimization barriers (`do_not_optimize()` / `clobber_memory()` in `include/bench_utils.hpp`). Performance-critical paths have been optimized with hand-rolled parsers, SIMD-accelerated string processing, and lock-free concurrent data structures.

> **Note**: Results shown are best-of-3 runs. Concurrent benchmarks are highly sensitive to system load and thread scheduling.

---

## Core Runtime Benchmarks

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| Ring Buffer Queue (Single Thread) | 166.7M ops/sec | 0.006 us | 0.008 us |
| Ring Buffer Queue (Concurrent 4x4) | 4.1M ops/sec | - | - |
| Ring Buffer Queue (High Contention 8x8) | 1.6M ops/sec | - | - |
| Circular Buffer | 250.0M ops/sec | 0.005 us | 0.007 us |
| SIMD CRLF Search (1.5KB buffer) | 100.0M ops/sec | 0.020 us | 0.026 us |
| SIMD CRLF Search (16KB buffer) | 3.6M ops/sec | 0.291 us | 0.531 us |
| HTTP Parser (Complete Request) | 1.1M ops/sec | 0.720 us | 1.672 us |
| HTTP Parser (Fragmented Request) | 1.1M ops/sec | 0.984 us | 1.583 us |
| Arena Allocations (64B objects) | 6.9M ops/sec | - | - |
| Memory Allocations (String Queue) | 50.0M ops/sec | - | - |

---

## Codegen Quality Benchmarks

| Benchmark | Latency | Throughput |
|-----------|---------|------------|
| parse_int64 (simple) | 5.3 ns | 189.1M ops/sec |
| parse_int64 (negative) | 7.0 ns | 142.9M ops/sec |
| parse_double | 11.9 ns | 84.1M ops/sec |
| parse_bool (strict validation) | 1.2 ns | 867.4M ops/sec |
| needs_json_escaping (clean, 58 chars) | 7.8 ns | 127.5M ops/sec |
| needs_json_escaping (dirty, 25 chars) | 1.3 ns | 799.9M ops/sec |
| escape_json_string (no-alloc path) | 13.3 ns | 75.4M ops/sec |
| escape_json_string_into (append) | 4.8 ns | 208.1M ops/sec |
| skip_value (nested obj with strings) | 33.5 ns | 29.8M ops/sec |
| 3-field object parse (linear) | 38.4 ns | 26.1M ops/sec |
| 8-field object parse (length-switch) | 69.4 ns | 14.4M ops/sec |
| arena alloc+reset cycle (4KB) | 36.8 ns | 27.1M ops/sec |

---

## Serialization Benchmarks

| Benchmark | Latency | Throughput |
|-----------|---------|------------|
| escape_json_string (clean, return) | 11.2 ns | 89.7M ops/sec |
| escape_json_string_into (clean, append) | 4.1 ns | 242.5M ops/sec |
| escape_json_string (dirty, return) | 35.8 ns | 28.0M ops/sec |
| escape_json_string_into (dirty, append) | 24.6 ns | 40.7M ops/sec |
| needs_json_escaping (16 byte clean) | 2.4 ns | 424.5M ops/sec |
| needs_json_escaping (64 byte clean) | 2.7 ns | 373.2M ops/sec |
| needs_json_escaping (256 byte clean) | 9.1 ns | 110.4M ops/sec |
| needs_json_escaping (64 byte, escape at end) | 2.5 ns | 396.6M ops/sec |
| needs_json_escaping (64 byte, escape at start) | 1.5 ns | 656.8M ops/sec |
| serialize 5-field obj (embedded commas) | 33.6 ns | 29.7M ops/sec |
| serialize array 100 ints (single alloc) | 205.9 ns | 4.9M ops/sec |

---

## Router Benchmarks

| Benchmark | Throughput | Latency p50 | Latency p99 |
|-----------|------------|-------------|-------------|
| Router dispatch (hits) | 2.2M ops/sec | 0.401 us | 0.892 us |
| Router dispatch (not found) | 2.6M ops/sec | 0.341 us | 0.691 us |
| Router dispatch (405) | 1.4M ops/sec | 0.591 us | 1.052 us |

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
