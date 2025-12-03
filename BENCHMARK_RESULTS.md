# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-03 08:49:55

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.160 ms; throughput 11509.050 req/s
- Thread scaling: 8 threads -> 205334.500 req/s
- Fan-out: 256 conns -> 262782.400 req/s
- Connection churn (4 threads): 15590.000 req/s
- Stability: sustained 39676.866 req/s
- Contention: ring buffer 8x8 9009009.010 ops/sec
- HTTP fragmented p99 2.032 us
- SIMD scan 16KB p99 1.009 us

## Table of Contents

- [Connection Churn](#connection-churn)
- [Core Performance](#core-performance)
- [Generated API](#generated-api)
- [HTTP Headers](#http-headers)
- [HTTP Parsing](#http-parsing)
- [IO Buffer](#io-buffer)
- [MPSC Queue](#mpsc-queue)
- [Router Dispatch](#router-dispatch)
- [Scalability](#scalability)
- [Stability](#stability)
- [System Configuration](#system-configuration)
- [Timer System](#timer-system)

---

## Connection Churn

| Benchmark | Value | Unit |
|-----------|-------|------|
| Close-after-each-request throughput (4 threads) | 15590.000 | req/s |

## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 83.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 6024096.390 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.005 | us |
| Circular Buffer - Latency p99 | 0.005 | us |
| Circular Buffer - Latency p999 | 0.005 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 43.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.838 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.796 | us |
| HTTP Parser (Complete Request) - Latency p999 | 3.057 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1162790.700 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 42.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.797 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 2.032 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 3.399 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1190476.190 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 11509.050 | req/s |
| Latency IQR | 0.037 | ms |
| Latency avg | 0.049 | ms |
| Latency max | 6.486 | ms |
| Latency p50 | 0.035 | ms |
| Latency p90 | 0.093 | ms |
| Latency p95 | 0.111 | ms |
| Latency p99 | 0.160 | ms |
| Latency p999 | 0.298 | ms |
| Latency samples | 321905.000 | samples |
| Memory Allocations (String Queue) - Duration | 2.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 50000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 62.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 16129032.260 | ops/sec |
| Ring Buffer Queue (High Contention 8x8) - Duration | 111.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 9009009.010 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 5.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.012 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.025 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 200000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 2.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.022 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.023 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.238 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 50000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 14.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.241 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 1.009 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 5.677 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 3571428.570 | ops/sec |

## Generated API

| Benchmark | Value | Unit |
|-----------|-------|------|
| Generated API dispatch+parse - Duration | 133.000 | ms |
| Generated API dispatch+parse - Errors | 0.000 | count |
| Generated API dispatch+parse - Latency p50 | 0.518 | us |
| Generated API dispatch+parse - Latency p99 | 1.183 | us |
| Generated API dispatch+parse - Latency p999 | 2.670 | us |
| Generated API dispatch+parse - Operations | 200000.000 | ops |
| Generated API dispatch+parse - Throughput | 1503759.400 | ops/sec |

## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 26.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.020 | us |
| Case-Insensitive Compare - Latency p99 | 0.030 | us |
| Case-Insensitive Compare - Latency p999 | 0.030 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 19230769.230 | ops/sec |
| Headers Get (3 lookups) - Duration | 10.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.020 | us |
| Headers Get (3 lookups) - Latency p99 | 0.030 | us |
| Headers Get (3 lookups) - Latency p999 | 0.030 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 20000000.000 | ops/sec |
| Headers Iteration (5 fields) - Duration | 4.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.020 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.030 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.030 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 25000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 17.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.254 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.528 | us |
| Headers Set (4 custom fields) - Latency p999 | 1.555 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2941176.470 | ops/sec |
| Headers Set (5 standard fields) - Duration | 21.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.127 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.215 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.372 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4761904.760 | ops/sec |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.088 | ms |
| Large headers p99 | 0.159 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.088 | ms |
| Medium request p99 | 0.142 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.083 | ms |
| Minimal request p99 | 0.133 | ms |
| Minimal request samples | 1500.000 | samples |

## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 5.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.068 | us |
| IO Buffer Append (4KB) - Latency p99 | 0.088 | us |
| IO Buffer Append (4KB) - Latency p999 | 0.147 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 10000000.000 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 5.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.020 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.030 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.058 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 5.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.020 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.030 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.049 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 4.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.020 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.030 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.030 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 25000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 7.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.039 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.078 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.098 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 14285714.290 | ops/sec |

## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 75.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 13333333.330 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 60.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 16666666.670 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 51.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 19607843.140 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 30.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 16666666.670 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 61.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.029 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.039 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.049 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 16393442.620 | ops/sec |

## Router Dispatch

| Benchmark | Value | Unit |
|-----------|-------|------|
| Router dispatch (405) - Duration | 230.000 | ms |
| Router dispatch (405) - Errors | 166667.000 | count |
| Router dispatch (405) - Latency p50 | 0.734 | us |
| Router dispatch (405) - Latency p99 | 1.496 | us |
| Router dispatch (405) - Latency p999 | 5.095 | us |
| Router dispatch (405) - Operations | 200000.000 | ops |
| Router dispatch (405) - Throughput | 869565.220 | ops/sec |
| Router dispatch (hits) - Duration | 204.000 | ms |
| Router dispatch (hits) - Errors | 33333.000 | count |
| Router dispatch (hits) - Latency p50 | 0.528 | us |
| Router dispatch (hits) - Latency p99 | 1.516 | us |
| Router dispatch (hits) - Latency p999 | 5.340 | us |
| Router dispatch (hits) - Operations | 200000.000 | ops |
| Router dispatch (hits) - Throughput | 980392.160 | ops/sec |
| Router dispatch (not found) - Duration | 196.000 | ms |
| Router dispatch (not found) - Errors | 200000.000 | count |
| Router dispatch (not found) - Latency p50 | 0.518 | us |
| Router dispatch (not found) - Latency p99 | 1.076 | us |
| Router dispatch (not found) - Latency p999 | 6.337 | us |
| Router dispatch (not found) - Operations | 200000.000 | ops |
| Router dispatch (not found) - Throughput | 1020408.160 | ops/sec |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 260648.400 | req/s |
| 256 concurrent connections | 262782.400 | req/s |
| 32 concurrent connections | 170358.400 | req/s |
| 64 concurrent connections | 235155.600 | req/s |
| Throughput with 1 threads | 11684.500 | req/s |
| Throughput with 4 threads | 41758.500 | req/s |
| Throughput with 8 threads | 205334.500 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 39676.866 | req/s |
| Total requests | 198395.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1024.000 | fds |

## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 34.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.020 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.171 | us |
| Wheel Timer (Add Operations) - Latency p999 | 5.252 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 2941176.470 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 2.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 0.020 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 0.030 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 0.157 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 25000000.000 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 313.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 31948.880 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.039 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.049 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.049 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
