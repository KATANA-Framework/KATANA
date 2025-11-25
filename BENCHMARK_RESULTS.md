# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-11-25 02:59:01

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
| Circular Buffer - Duration | 25.000 | ms |
| Circular Buffer - Latency p50 | 0.018 | us |
| Circular Buffer - Latency p99 | 0.028 | us |
| Circular Buffer - Latency p999 | 0.045 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 20000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 43.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.713 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.569 | us |
| HTTP Parser (Complete Request) - Latency p999 | 7.133 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1162790.700 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 12114.369 | req/s |
| Latency IQR | 0.023 | ms |
| Latency avg | 0.043 | ms |
| Latency max | 6.669 | ms |
| Latency p50 | 0.029 | ms |
| Latency p90 | 0.081 | ms |
| Latency p95 | 0.100 | ms |
| Latency p99 | 0.152 | ms |
| Latency p999 | 0.342 | ms |
| Latency samples | 371463.000 | samples |
| Memory Allocations (String Queue) - Duration | 2.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 50000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 57.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 17543859.650 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 54.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.027 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.028 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.036 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 18518518.520 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 5.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.036 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.045 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.072 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 20000000.000 | ops/sec |

## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 24.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.018 | us |
| Case-Insensitive Compare - Latency p99 | 0.027 | us |
| Case-Insensitive Compare - Latency p999 | 0.028 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 20833333.330 | ops/sec |
| Headers Get (3 lookups) - Duration | 9.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.018 | us |
| Headers Get (3 lookups) - Latency p99 | 0.027 | us |
| Headers Get (3 lookups) - Latency p999 | 0.028 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 22222222.220 | ops/sec |
| Headers Iteration (5 fields) - Duration | 4.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.018 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.027 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.028 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 25000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 16.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.244 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.514 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.641 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 3125000.000 | ops/sec |
| Headers Set (5 standard fields) - Duration | 16.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.081 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.154 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.252 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 6250000.000 | ops/sec |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.080 | ms |
| Large headers p99 | 0.133 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.080 | ms |
| Medium request p99 | 0.115 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.080 | ms |
| Minimal request p99 | 0.113 | ms |
| Minimal request samples | 1500.000 | samples |

## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 52.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.965 | us |
| IO Buffer Append (4KB) - Latency p99 | 1.722 | us |
| IO Buffer Append (4KB) - Latency p999 | 8.530 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 961538.460 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 5.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.027 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.036 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.054 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 5.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.027 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.036 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.055 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 5.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.027 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.045 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.063 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 20000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.036 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.072 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.090 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |

## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 70.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 14285714.290 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 57.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 17543859.650 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 48.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 20833333.330 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 28.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 17857142.860 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 56.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.027 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.037 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.054 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 17857142.860 | ops/sec |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 275043.600 | req/s |
| 32 concurrent connections | 204839.200 | req/s |
| 64 concurrent connections | 269707.600 | req/s |
| Throughput with 1 threads | 11448.500 | req/s |
| Throughput with 4 threads | 40968.500 | req/s |
| Throughput with 8 threads | 169154.500 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 42504.563 | req/s |
| Total requests | 212535.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1024.000 | fds |

## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 32.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.018 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.046 | us |
| Wheel Timer (Add Operations) - Latency p999 | 5.032 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 3125000.000 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 348.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 6.619 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 21.208 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 40.076 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 143678.160 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 312.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 32051.280 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.036 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.046 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.054 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
