# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-27 13:49:26

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.095 ms; throughput 11747.917 req/s
- Thread scaling: 8 threads -> 271700.500 req/s
- Fan-out: 128 conns -> 222313.200 req/s
- Connection churn (4 threads): 15509.333 req/s
- Stability: sustained 38076.050 req/s
- Contention: ring buffer 8x8 12195121.950 ops/sec
- HTTP fragmented p99 1.862 us
- SIMD scan 16KB p99 0.402 us

## Table of Contents

- [Connection Churn](#connection-churn)
- [Core Performance](#core-performance)
- [Example: compute_api](#example:-compute_api)
- [Example: text_api](#example:-text_api)
- [Example: validation_api](#example:-validation_api)
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
| Close-after-each-request throughput (4 threads) | 15509.333 | req/s |
## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 87.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 5747126.440 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.004 | us |
| Circular Buffer - Latency p99 | 0.005 | us |
| Circular Buffer - Latency p999 | 0.008 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 46.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.844 | us |
| HTTP Parser (Complete Request) - Latency p99 | 2.517 | us |
| HTTP Parser (Complete Request) - Latency p999 | 5.516 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1086956.520 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 42.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.790 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 1.862 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 3.465 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1190476.190 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 11747.917 | req/s |
| Latency IQR | 0.006 | ms |
| Latency avg | 0.032 | ms |
| Latency max | 6.440 | ms |
| Latency p50 | 0.028 | ms |
| Latency p90 | 0.044 | ms |
| Latency p95 | 0.056 | ms |
| Latency p99 | 0.095 | ms |
| Latency p999 | 0.300 | ms |
| Latency samples | 492364.000 | samples |
| Memory Allocations (String Queue) - Duration | 2.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 50000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 58.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 17241379.310 | ops/sec |
| Ring Buffer Queue (High Contention 8x8) - Duration | 82.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 12195121.950 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 4.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.005 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.006 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 250000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 1.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.014 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.015 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.016 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 100000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 11.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.224 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.402 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 1.442 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 4545454.550 | ops/sec |
## Example: compute_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| wrk size=1 throughput | 199899.730 | req/s |
| wrk size=1024 throughput | 17314.860 | req/s |
| wrk size=256 socket_errors | 32.000 | count |
| wrk size=256 throughput | 50037.930 | req/s |
| wrk size=64 throughput | 145728.480 | req/s |
| wrk size=8 socket_errors | 32.000 | count |
| wrk size=8 throughput | 149003.760 | req/s |
## Example: text_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| avg_latency | 0.570 | ms |
| status | OK - Generated code works correctly |  |
| validation_tests_passed | 2.000 | tests |
## Example: validation_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| wrk invalid only non_2xx_3xx | 2092858.000 | count |
| wrk invalid only throughput | 172939.830 | req/s |
| wrk mix (60% valid / 40% invalid) non_2xx_3xx | 773906.000 | count |
| wrk mix (60% valid / 40% invalid) throughput | 161125.420 | req/s |
| wrk valid only socket_errors | 31.000 | count |
| wrk valid only throughput | 145826.790 | req/s |
## Generated API

| Benchmark | Value | Unit |
|-----------|-------|------|
| Generated API dispatch+parse - Duration | 125.000 | ms |
| Generated API dispatch+parse - Errors | 0.000 | count |
| Generated API dispatch+parse - Latency p50 | 0.432 | us |
| Generated API dispatch+parse - Latency p99 | 1.213 | us |
| Generated API dispatch+parse - Latency p999 | 3.977 | us |
| Generated API dispatch+parse - Operations | 200000.000 | ops |
| Generated API dispatch+parse - Throughput | 1600000.000 | ops/sec |
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
| Headers Set (4 custom fields) - Duration | 21.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.312 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.615 | us |
| Headers Set (4 custom fields) - Latency p999 | 1.534 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2380952.380 | ops/sec |
| Headers Set (5 standard fields) - Duration | 23.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.138 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.267 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.551 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4347826.090 | ops/sec |
## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.085 | ms |
| Large headers p99 | 0.124 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.085 | ms |
| Medium request p99 | 0.122 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.081 | ms |
| Minimal request p99 | 0.119 | ms |
| Minimal request samples | 1500.000 | samples |
## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 51.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.937 | us |
| IO Buffer Append (4KB) - Latency p99 | 1.671 | us |
| IO Buffer Append (4KB) - Latency p999 | 9.827 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 980392.160 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 6.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.028 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.037 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.056 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 16666666.670 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 8.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.046 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.083 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.110 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 12500000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 7.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.037 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.083 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.156 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 14285714.290 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.037 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.073 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.083 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |
## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 72.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 13888888.890 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 59.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 16949152.540 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 51.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 19607843.140 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 30.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 16666666.670 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 57.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.028 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.037 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.055 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 17543859.650 | ops/sec |
## Router Dispatch

| Benchmark | Value | Unit |
|-----------|-------|------|
| Router dispatch (405) - Duration | 218.000 | ms |
| Router dispatch (405) - Errors | 166667.000 | count |
| Router dispatch (405) - Latency p50 | 0.634 | us |
| Router dispatch (405) - Latency p99 | 1.442 | us |
| Router dispatch (405) - Latency p999 | 7.770 | us |
| Router dispatch (405) - Operations | 200000.000 | ops |
| Router dispatch (405) - Throughput | 917431.190 | ops/sec |
| Router dispatch (hits) - Duration | 189.000 | ms |
| Router dispatch (hits) - Errors | 33333.000 | count |
| Router dispatch (hits) - Latency p50 | 0.405 | us |
| Router dispatch (hits) - Latency p99 | 1.378 | us |
| Router dispatch (hits) - Latency p999 | 3.279 | us |
| Router dispatch (hits) - Operations | 200000.000 | ops |
| Router dispatch (hits) - Throughput | 1058201.060 | ops/sec |
| Router dispatch (not found) - Duration | 178.000 | ms |
| Router dispatch (not found) - Errors | 200000.000 | count |
| Router dispatch (not found) - Latency p50 | 0.478 | us |
| Router dispatch (not found) - Latency p99 | 0.973 | us |
| Router dispatch (not found) - Latency p999 | 2.682 | us |
| Router dispatch (not found) - Operations | 200000.000 | ops |
| Router dispatch (not found) - Throughput | 1123595.510 | ops/sec |
## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 222313.200 | req/s |
| 32 concurrent connections | 197422.800 | req/s |
| 64 concurrent connections | 196324.800 | req/s |
| Throughput with 1 threads | 12095.500 | req/s |
| Throughput with 4 threads | 45584.500 | req/s |
| Throughput with 8 threads | 271700.500 | req/s |
## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 38076.050 | req/s |
| Total requests | 190390.000 | requests |
## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1048576.000 | fds |
## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 17.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.028 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.324 | us |
| Wheel Timer (Add Operations) - Latency p999 | 6.521 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 5882352.940 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 2.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 0.019 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 0.037 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 0.248 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 25000000.000 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 312.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 32051.280 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.037 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.046 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.046 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
