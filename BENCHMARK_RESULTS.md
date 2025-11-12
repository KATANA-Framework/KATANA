# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-11-12 17:49:23

This report includes results from all benchmark suites.

## Table of Contents

- [Core Performance](#core-performance)
- [HTTP Headers](#http-headers)
- [HTTP Parsing](#http-parsing)
- [IO Buffer](#io-buffer)
- [MPSC Queue](#mpsc-queue)
- [Scalability](#scalability)
- [Stability](#stability)
- [System Configuration](#system-configuration)
- [Timer System](#timer-system)

---

## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Circular Buffer - Duration | 18.000 | ms |
| Circular Buffer - Latency p50 | 0.000 | us |
| Circular Buffer - Latency p99 | 0.042 | us |
| Circular Buffer - Latency p999 | 0.042 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 27777777.780 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 35.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.625 | us |
| HTTP Parser (Complete Request) - Latency p99 | 0.792 | us |
| HTTP Parser (Complete Request) - Latency p999 | 4.833 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1428571.430 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 28492.570 | req/s |
| Latency IQR | 0.043 | ms |
| Latency avg | 0.052 | ms |
| Latency max | 3.609 | ms |
| Latency p50 | 0.050 | ms |
| Latency p90 | 0.096 | ms |
| Latency p95 | 0.113 | ms |
| Latency p99 | 0.149 | ms |
| Latency p999 | 0.202 | ms |
| Latency samples | 305143.000 | samples |
| Memory Allocations (String Queue) - Duration | 1.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 100000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 183.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 5464480.870 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 38.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.042 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.042 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 26315789.470 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 29.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.250 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.333 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.375 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 3448275.860 | ops/sec |

## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 19.000 | ms |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 26315789.470 | ops/sec |
| Headers Get (3 lookups) - Duration | 10.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.041 | us |
| Headers Get (3 lookups) - Latency p99 | 0.042 | us |
| Headers Get (3 lookups) - Latency p999 | 0.042 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 20000000.000 | ops/sec |
| Headers Iteration (5 fields) - Duration | 3.000 | ms |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 33333333.330 | ops/sec |
| Headers Set (4 custom fields) - Duration | 22.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.375 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.458 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.500 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2272727.270 | ops/sec |
| Headers Set (5 standard fields) - Duration | 19.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.084 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.167 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.209 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 5263157.890 | ops/sec |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.036 | ms |
| Large headers p99 | 0.045 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.035 | ms |
| Medium request p99 | 0.041 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.034 | ms |
| Minimal request p99 | 0.042 | ms |
| Minimal request samples | 1500.000 | samples |

## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 6.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.125 | us |
| IO Buffer Append (4KB) - Latency p99 | 0.166 | us |
| IO Buffer Append (4KB) - Latency p999 | 0.167 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 8333333.330 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 4.000 | ms |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 25000000.000 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 4.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.041 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.042 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.042 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 25000000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 4.000 | ms |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 25000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 5.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.041 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.042 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.042 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 20000000.000 | ops/sec |

## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 112.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 8928571.430 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 110.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 9090909.090 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 100.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 10000000.000 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 66.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 7575757.580 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 41.000 | ms |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 24390243.900 | ops/sec |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 1198861.200 | req/s |
| 32 concurrent connections | 353679.600 | req/s |
| 64 concurrent connections | 645324.800 | req/s |
| Throughput with 1 threads | 28912.000 | req/s |
| Throughput with 4 threads | 79793.000 | req/s |
| Throughput with 8 threads | 147226.000 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 77844.375 | req/s |
| Total requests | 389306.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1048576.000 | fds |

## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 6.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.041 | us |
| Wheel Timer (Add Operations) - Latency p99 | 0.250 | us |
| Wheel Timer (Add Operations) - Latency p999 | 0.583 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 16666666.670 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 323.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 6.417 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 12.916 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 19.875 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 154798.760 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 321.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 31152.650 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 2.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.041 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.042 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.042 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 25000000.000 | ops/sec |