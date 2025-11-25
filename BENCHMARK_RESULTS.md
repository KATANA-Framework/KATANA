# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-11-24 23:16:25

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
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 10259.398 | req/s |
| Latency IQR | 0.016 | ms |
| Latency avg | 0.052 | ms |
| Latency max | 8.084 | ms |
| Latency p50 | 0.031 | ms |
| Latency p90 | 0.085 | ms |
| Latency p95 | 0.125 | ms |
| Latency p99 | 0.383 | ms |
| Latency p999 | 1.129 | ms |
| Latency samples | 308015.000 | samples |

## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 24.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.018 | us |
| Case-Insensitive Compare - Latency p99 | 0.028 | us |
| Case-Insensitive Compare - Latency p999 | 0.028 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 20833333.330 | ops/sec |
| Headers Get (3 lookups) - Duration | 9.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.018 | us |
| Headers Get (3 lookups) - Latency p99 | 0.028 | us |
| Headers Get (3 lookups) - Latency p999 | 0.028 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 22222222.220 | ops/sec |
| Headers Iteration (5 fields) - Duration | 4.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.018 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.028 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.028 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 25000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 17.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.248 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.468 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.670 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2941176.470 | ops/sec |
| Headers Set (5 standard fields) - Duration | 16.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.083 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.147 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.303 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 6250000.000 | ops/sec |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.093 | ms |
| Large headers p99 | 0.245 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.089 | ms |
| Medium request p99 | 0.211 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.090 | ms |
| Minimal request p99 | 0.258 | ms |
| Minimal request samples | 1500.000 | samples |

## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 53.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.982 | us |
| IO Buffer Append (4KB) - Latency p99 | 1.644 | us |
| IO Buffer Append (4KB) - Latency p999 | 10.727 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 943396.230 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 6.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.027 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.037 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.064 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 16666666.670 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 6.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.027 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.046 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.083 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 16666666.670 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 5.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.027 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.037 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.055 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 20000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.037 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.037 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.046 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |

## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 65.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 15384615.380 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 56.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 17857142.860 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 48.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 20833333.330 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 30.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 16666666.670 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 58.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.027 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.037 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.055 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 17241379.310 | ops/sec |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 235507.600 | req/s |
| 32 concurrent connections | 248613.600 | req/s |
| 64 concurrent connections | 260044.400 | req/s |
| Throughput with 1 threads | 11438.500 | req/s |
| Throughput with 4 threads | 39422.500 | req/s |
| Throughput with 8 threads | 147399.500 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 38726.322 | req/s |
| Total requests | 193640.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1024.000 | fds |

## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 34.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.019 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.048 | us |
| Wheel Timer (Add Operations) - Latency p999 | 5.281 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 2941176.470 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 346.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 6.630 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 19.828 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 41.942 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 144508.670 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 311.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 32154.340 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.037 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.046 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.046 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |

