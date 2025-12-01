# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-01 02:13:34

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.182 ms; throughput 11720.101 req/s
- Thread scaling: 8 threads -> 149090.500 req/s
- Fan-out: 256 conns -> 287740.400 req/s
- Connection churn (4 threads): 15341.333 req/s
- Stability: sustained 40076.136 req/s
- Contention: ring buffer 8x8 9259259.260 ops/sec
- HTTP fragmented p99 1.378 us
- SIMD scan 16KB p99 0.717 us

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
| Close-after-each-request throughput (4 threads) | 15341.333 | req/s |

## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 80.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 6250000.000 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.005 | us |
| Circular Buffer - Latency p99 | 0.008 | us |
| Circular Buffer - Latency p999 | 0.009 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 47.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.863 | us |
| HTTP Parser (Complete Request) - Latency p99 | 2.138 | us |
| HTTP Parser (Complete Request) - Latency p999 | 3.715 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1063829.790 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 41.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.812 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 1.378 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 2.068 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1219512.200 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 11720.101 | req/s |
| Latency IQR | 0.053 | ms |
| Latency avg | 0.071 | ms |
| Latency max | 6.891 | ms |
| Latency p50 | 0.066 | ms |
| Latency p90 | 0.119 | ms |
| Latency p95 | 0.139 | ms |
| Latency p99 | 0.182 | ms |
| Latency p999 | 0.264 | ms |
| Latency samples | 225736.000 | samples |
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
| Ring Buffer Queue (High Contention 8x8) - Duration | 108.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 9259259.260 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 4.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.005 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.008 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 250000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 1.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.016 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.017 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.043 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 100000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 12.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.246 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.717 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 1.422 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 4166666.670 | ops/sec |

## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 27.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.020 | us |
| Case-Insensitive Compare - Latency p99 | 0.030 | us |
| Case-Insensitive Compare - Latency p999 | 0.031 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 18518518.520 | ops/sec |
| Headers Get (3 lookups) - Duration | 10.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.020 | us |
| Headers Get (3 lookups) - Latency p99 | 0.030 | us |
| Headers Get (3 lookups) - Latency p999 | 0.031 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 20000000.000 | ops/sec |
| Headers Iteration (5 fields) - Duration | 5.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.020 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.030 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.031 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 20000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 17.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.260 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.311 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.551 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2941176.470 | ops/sec |
| Headers Set (5 standard fields) - Duration | 21.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.130 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.171 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.592 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4761904.760 | ops/sec |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.084 | ms |
| Large headers p99 | 0.154 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.084 | ms |
| Medium request p99 | 0.149 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.084 | ms |
| Minimal request p99 | 0.146 | ms |
| Minimal request samples | 1500.000 | samples |

## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 4.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.070 | us |
| IO Buffer Append (4KB) - Latency p99 | 0.071 | us |
| IO Buffer Append (4KB) - Latency p999 | 0.110 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 12500000.000 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 5.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.020 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.031 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.031 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 5.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.020 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.031 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.031 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 4.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.020 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.030 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.031 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 25000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.040 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.041 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.060 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |

## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 86.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 11627906.980 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 65.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 15384615.380 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 51.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 19607843.140 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 31.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 16129032.260 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 61.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.030 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.040 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.050 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 16393442.620 | ops/sec |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 263939.200 | req/s |
| 256 concurrent connections | 287740.400 | req/s |
| 32 concurrent connections | 252268.000 | req/s |
| 64 concurrent connections | 273847.600 | req/s |
| Throughput with 1 threads | 11737.500 | req/s |
| Throughput with 4 threads | 41157.000 | req/s |
| Throughput with 8 threads | 149090.500 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 40076.136 | req/s |
| Total requests | 200390.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1024.000 | fds |

## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 36.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.020 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.245 | us |
| Wheel Timer (Add Operations) - Latency p999 | 5.650 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 2777777.780 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 366.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 7.114 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 19.256 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 32.621 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 136612.020 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 313.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 31948.880 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.040 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.051 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.051 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
