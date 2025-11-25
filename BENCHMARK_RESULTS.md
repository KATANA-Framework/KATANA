# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-11-25 05:19:30

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
| Keep-alive throughput | 35092.926 | req/s |
| Latency IQR | 0.021 | ms |
| Latency avg | 0.037 | ms |
| Latency max | 3.048 | ms |
| Latency p50 | 0.030 | ms |
| Latency p90 | 0.063 | ms |
| Latency p95 | 0.081 | ms |
| Latency p99 | 0.140 | ms |
| Latency p999 | 0.307 | ms |
| Latency samples | 431829.000 | samples |

## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 21.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.017 | us |
| Case-Insensitive Compare - Latency p99 | 0.018 | us |
| Case-Insensitive Compare - Latency p999 | 0.018 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 23809523.810 | ops/sec |
| Headers Get (3 lookups) - Duration | 8.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.017 | us |
| Headers Get (3 lookups) - Latency p99 | 0.018 | us |
| Headers Get (3 lookups) - Latency p999 | 0.018 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 25000000.000 | ops/sec |
| Headers Iteration (5 fields) - Duration | 4.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.017 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.018 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.018 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 25000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 18.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.290 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.315 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.550 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2777777.780 | ops/sec |
| Headers Set (5 standard fields) - Duration | 18.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.117 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.136 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.154 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 5555555.560 | ops/sec |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.025 | ms |
| Large headers p99 | 0.031 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.025 | ms |
| Medium request p99 | 0.032 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.030 | ms |
| Minimal request p99 | 0.038 | ms |
| Minimal request samples | 1500.000 | samples |

## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 7.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.109 | us |
| IO Buffer Append (4KB) - Latency p99 | 0.120 | us |
| IO Buffer Append (4KB) - Latency p999 | 0.176 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 7142857.140 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 6.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.030 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.038 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.059 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 16666666.670 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 6.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.032 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.047 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.081 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 16666666.670 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 5.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.029 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.031 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.074 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 20000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 7.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.048 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.051 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.062 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 14285714.290 | ops/sec |

## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 152.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 6578947.370 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 100.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 10000000.000 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 99.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 10101010.100 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 92.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 5434782.610 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 60.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.034 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.035 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.115 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 16666666.670 | ops/sec |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 217142.400 | req/s |
| 32 concurrent connections | 221531.600 | req/s |
| 64 concurrent connections | 216588.000 | req/s |
| Throughput with 1 threads | 36219.500 | req/s |
| Throughput with 4 threads | 208905.000 | req/s |
| Throughput with 8 threads | 216703.500 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 151537.919 | req/s |
| Total requests | 757722.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 65536.000 | fds |
| FD soft limit | 65536.000 | fds |

## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 14.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.023 | us |
| Wheel Timer (Add Operations) - Latency p99 | 0.872 | us |
| Wheel Timer (Add Operations) - Latency p999 | 1.501 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 7142857.140 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 500.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 10.031 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 19.644 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 23.585 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 100000.000 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 313.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 31948.880 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.035 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.036 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.037 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |

