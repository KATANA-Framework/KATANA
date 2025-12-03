# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-01 03:40:31

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.139 ms; throughput 11559.780 req/s
- Thread scaling: 8 threads -> 197238.000 req/s
- Fan-out: 256 conns -> 271004.000 req/s
- Connection churn (4 threads): 16166.667 req/s
- Stability: sustained 40257.787 req/s
- Contention: ring buffer 8x8 10869565.220 ops/sec
- HTTP fragmented p99 1.398 us
- SIMD scan 16KB p99 0.414 us

## Table of Contents

- [Connection Churn](#connection-churn)
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

## Connection Churn

| Benchmark | Value | Unit |
|-----------|-------|------|
| Close-after-each-request throughput (4 threads) | 16166.667 | req/s |

## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 78.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 6410256.410 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.005 | us |
| Circular Buffer - Latency p99 | 0.005 | us |
| Circular Buffer - Latency p999 | 0.008 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 43.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.854 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.455 | us |
| HTTP Parser (Complete Request) - Latency p999 | 2.624 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1162790.700 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 41.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.803 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 1.398 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 2.092 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1219512.200 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 11559.780 | req/s |
| Latency IQR | 0.036 | ms |
| Latency avg | 0.047 | ms |
| Latency max | 6.507 | ms |
| Latency p50 | 0.033 | ms |
| Latency p90 | 0.089 | ms |
| Latency p95 | 0.104 | ms |
| Latency p99 | 0.139 | ms |
| Latency p999 | 0.200 | ms |
| Latency samples | 339692.000 | samples |
| Memory Allocations (String Queue) - Duration | 4.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 25000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 56.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 17857142.860 | ops/sec |
| Ring Buffer Queue (High Contention 8x8) - Duration | 92.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 10869565.220 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 4.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.005 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.009 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 250000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 2.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.023 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.024 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.024 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 50000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 12.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.244 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.414 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 1.031 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 4166666.670 | ops/sec |

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
| Headers Iteration (5 fields) - Duration | 5.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.020 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.030 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.030 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 20000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 17.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.258 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.328 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.546 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2941176.470 | ops/sec |
| Headers Set (5 standard fields) - Duration | 21.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.129 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.179 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.238 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4761904.760 | ops/sec |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.083 | ms |
| Large headers p99 | 0.120 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.082 | ms |
| Medium request p99 | 0.124 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.083 | ms |
| Minimal request p99 | 0.128 | ms |
| Minimal request samples | 1500.000 | samples |

## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 4.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.069 | us |
| IO Buffer Append (4KB) - Latency p99 | 0.070 | us |
| IO Buffer Append (4KB) - Latency p999 | 0.109 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 12500000.000 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 5.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.020 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.030 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.040 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 5.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.020 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.030 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.040 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 4.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.020 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.030 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.030 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 25000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.040 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.040 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.080 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |

## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 77.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 12987012.990 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 64.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 15625000.000 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 51.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 19607843.140 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 31.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 16129032.260 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 60.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.030 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.040 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.060 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 16666666.670 | ops/sec |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 271322.400 | req/s |
| 256 concurrent connections | 271004.000 | req/s |
| 32 concurrent connections | 268220.800 | req/s |
| 64 concurrent connections | 267775.600 | req/s |
| Throughput with 1 threads | 11941.500 | req/s |
| Throughput with 4 threads | 42015.500 | req/s |
| Throughput with 8 threads | 197238.000 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 40257.787 | req/s |
| Total requests | 201300.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1024.000 | fds |

## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 33.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.020 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.095 | us |
| Wheel Timer (Add Operations) - Latency p999 | 5.202 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 3030303.030 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 2.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 0.020 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 0.030 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 0.030 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 25000000.000 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 312.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 32051.280 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.040 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.050 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.050 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
