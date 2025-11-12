# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-11-12 17:39:51

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
| Circular Buffer - Duration | 34.000 | ms |
| Circular Buffer - Latency p50 | 0.029 | us |
| Circular Buffer - Latency p99 | 0.048 | us |
| Circular Buffer - Latency p999 | 0.065 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 14705882.350 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 59.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.969 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.977 | us |
| HTTP Parser (Complete Request) - Latency p999 | 7.947 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 847457.630 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 9982.149 | req/s |
| Latency IQR | 0.094 | ms |
| Latency avg | 0.236 | ms |
| Latency max | 1.203 | ms |
| Latency p50 | 0.229 | ms |
| Latency p90 | 0.327 | ms |
| Latency p95 | 0.364 | ms |
| Latency p99 | 0.473 | ms |
| Latency p999 | 0.717 | ms |
| Latency samples | 67481.000 | samples |
| Memory Allocations (String Queue) - Duration | 3.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 33333333.330 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 503.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 1988071.570 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 78.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.039 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.053 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.090 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 12820512.820 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 8.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.050 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.093 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.148 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 12500000.000 | ops/sec |

## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 31.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.026 | us |
| Case-Insensitive Compare - Latency p99 | 0.027 | us |
| Case-Insensitive Compare - Latency p999 | 0.033 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 16129032.260 | ops/sec |
| Headers Get (3 lookups) - Duration | 12.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.026 | us |
| Headers Get (3 lookups) - Latency p99 | 0.034 | us |
| Headers Get (3 lookups) - Latency p999 | 0.037 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 16666666.670 | ops/sec |
| Headers Iteration (5 fields) - Duration | 6.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.026 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.034 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.038 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 16666666.670 | ops/sec |
| Headers Set (4 custom fields) - Duration | 24.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.368 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.705 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.844 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2083333.330 | ops/sec |
| Headers Set (5 standard fields) - Duration | 27.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.120 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.265 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.322 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 3703703.700 | ops/sec |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.085 | ms |
| Large headers p99 | 0.124 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.088 | ms |
| Medium request p99 | 0.132 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.090 | ms |
| Minimal request p99 | 0.181 | ms |
| Minimal request samples | 1500.000 | samples |

## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 8.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.120 | us |
| IO Buffer Append (4KB) - Latency p99 | 0.246 | us |
| IO Buffer Append (4KB) - Latency p999 | 0.277 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 6250000.000 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 7.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.034 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.067 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.074 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 14285714.290 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 7.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.035 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.072 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.088 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 14285714.290 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 7.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.034 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.058 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.068 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 14285714.290 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 9.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.051 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.099 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.121 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 11111111.110 | ops/sec |

## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 273.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 3663003.660 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 172.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 5813953.490 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 162.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 6172839.510 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 169.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 2958579.880 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 79.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.040 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.070 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.082 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 12658227.850 | ops/sec |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 64769.200 | req/s |
| 32 concurrent connections | 64429.600 | req/s |
| 64 concurrent connections | 64456.800 | req/s |
| Throughput with 1 threads | 11308.000 | req/s |
| Throughput with 4 threads | 24196.000 | req/s |
| Throughput with 8 threads | 37688.000 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 21056.009 | req/s |
| Total requests | 105295.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 20000.000 | fds |
| FD soft limit | 20000.000 | fds |

## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 36.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.030 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.248 | us |
| Wheel Timer (Add Operations) - Latency p999 | 7.415 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 2777777.780 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 531.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 10.478 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 24.807 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 37.031 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 94161.960 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 313.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 31948.880 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 4.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.052 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.067 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.074 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 12500000.000 | ops/sec |

