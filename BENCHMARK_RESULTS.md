# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-26 20:35:25

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.082 ms; throughput 12284.867 req/s
- Thread scaling: 8 threads -> 221055.500 req/s
- Fan-out: 256 conns -> 0.000 req/s
- Connection churn (4 threads): 16243.333 req/s
- Stability: sustained 0.000 req/s
- Contention: ring buffer 8x8 11494252.870 ops/sec
- HTTP fragmented p99 1.728 us
- SIMD scan 16KB p99 0.401 us

## Table of Contents

- [Connection Churn](#connection-churn)
- [Core Performance](#core-performance)
- [Example: compute_api](#example:-compute_api)
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
| Close-after-each-request throughput (4 threads) | 16243.333 | req/s |
## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 75.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 6666666.670 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.004 | us |
| Circular Buffer - Latency p99 | 0.006 | us |
| Circular Buffer - Latency p999 | 0.006 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 43.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.831 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.740 | us |
| HTTP Parser (Complete Request) - Latency p999 | 3.766 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1162790.700 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 41.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.772 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 1.728 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 2.652 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1219512.200 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 12284.867 | req/s |
| Latency IQR | 0.006 | ms |
| Latency avg | 0.030 | ms |
| Latency max | 7.396 | ms |
| Latency p50 | 0.027 | ms |
| Latency p90 | 0.040 | ms |
| Latency p95 | 0.050 | ms |
| Latency p99 | 0.082 | ms |
| Latency p999 | 0.220 | ms |
| Latency samples | 525295.000 | samples |
| Memory Allocations (String Queue) - Duration | 2.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 50000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 59.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 16949152.540 | ops/sec |
| Ring Buffer Queue (High Contention 8x8) - Duration | 87.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 11494252.870 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 4.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.008 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.012 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 250000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 1.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.014 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.023 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.031 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 100000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 11.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.222 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.401 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 1.031 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 4545454.550 | ops/sec |
## Example: compute_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| wrk size=1 throughput | 186276.590 | req/s |
| wrk size=1024 throughput | 13467.090 | req/s |
| wrk size=256 throughput | 62282.160 | req/s |
| wrk size=64 throughput | 156336.100 | req/s |
| wrk size=8 socket_errors | 2.000 | count |
| wrk size=8 throughput | 163319.460 | req/s |
## Example: validation_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| wrk invalid only non_2xx_3xx | 2148574.000 | count |
| wrk invalid only socket_errors | 30.000 | count |
| wrk invalid only throughput | 139045.980 | req/s |
| wrk mix (60% valid / 40% invalid) non_2xx_3xx | 306777.000 | count |
| wrk mix (60% valid / 40% invalid) throughput | 63643.190 | req/s |
| wrk valid only throughput | 187042.110 | req/s |
## Generated API

| Benchmark | Value | Unit |
|-----------|-------|------|
| Generated API dispatch+parse - Duration | 121.000 | ms |
| Generated API dispatch+parse - Errors | 0.000 | count |
| Generated API dispatch+parse - Latency p50 | 0.429 | us |
| Generated API dispatch+parse - Latency p99 | 1.168 | us |
| Generated API dispatch+parse - Latency p999 | 2.867 | us |
| Generated API dispatch+parse - Operations | 200000.000 | ops |
| Generated API dispatch+parse - Throughput | 1652892.560 | ops/sec |
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
| Headers Set (4 custom fields) - Duration | 20.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.311 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.576 | us |
| Headers Set (4 custom fields) - Latency p999 | 1.187 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2500000.000 | ops/sec |
| Headers Set (5 standard fields) - Duration | 21.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.137 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.228 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.319 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4761904.760 | ops/sec |
## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.085 | ms |
| Large headers p99 | 0.126 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.082 | ms |
| Medium request p99 | 0.125 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.078 | ms |
| Minimal request p99 | 0.118 | ms |
| Minimal request samples | 1500.000 | samples |
## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 51.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.932 | us |
| IO Buffer Append (4KB) - Latency p99 | 1.707 | us |
| IO Buffer Append (4KB) - Latency p999 | 14.866 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 980392.160 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 6.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.028 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.046 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.064 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 16666666.670 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 7.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.046 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.055 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.074 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 14285714.290 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 6.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.037 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.073 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.092 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 16666666.670 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.036 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.037 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.055 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |
## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 67.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 14925373.130 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 57.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 17543859.650 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 52.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 19230769.230 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 28.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 17857142.860 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 56.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.027 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.037 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.055 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 17857142.860 | ops/sec |
## Router Dispatch

| Benchmark | Value | Unit |
|-----------|-------|------|
| Router dispatch (405) - Duration | 200.000 | ms |
| Router dispatch (405) - Errors | 166667.000 | count |
| Router dispatch (405) - Latency p50 | 0.621 | us |
| Router dispatch (405) - Latency p99 | 1.296 | us |
| Router dispatch (405) - Latency p999 | 3.406 | us |
| Router dispatch (405) - Operations | 200000.000 | ops |
| Router dispatch (405) - Throughput | 1000000.000 | ops/sec |
| Router dispatch (hits) - Duration | 166.000 | ms |
| Router dispatch (hits) - Errors | 33333.000 | count |
| Router dispatch (hits) - Latency p50 | 0.384 | us |
| Router dispatch (hits) - Latency p99 | 0.822 | us |
| Router dispatch (hits) - Latency p999 | 2.986 | us |
| Router dispatch (hits) - Operations | 200000.000 | ops |
| Router dispatch (hits) - Throughput | 1204819.280 | ops/sec |
| Router dispatch (not found) - Duration | 173.000 | ms |
| Router dispatch (not found) - Errors | 200000.000 | count |
| Router dispatch (not found) - Latency p50 | 0.457 | us |
| Router dispatch (not found) - Latency p99 | 0.968 | us |
| Router dispatch (not found) - Latency p999 | 2.967 | us |
| Router dispatch (not found) - Operations | 200000.000 | ops |
| Router dispatch (not found) - Throughput | 1156069.360 | ops/sec |
## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 273856.400 | req/s |
| 256 concurrent connections | 0.000 | req/s |
| 32 concurrent connections | 249509.200 | req/s |
| 64 concurrent connections | 267733.600 | req/s |
| Throughput with 1 threads | 12227.000 | req/s |
| Throughput with 4 threads | 46539.500 | req/s |
| Throughput with 8 threads | 221055.500 | req/s |
## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 0.000 | req/s |
| Total requests | 0.000 | requests |
## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1048576.000 | fds |
## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 16.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.027 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.091 | us |
| Wheel Timer (Add Operations) - Latency p999 | 5.908 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 6250000.000 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 2.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 0.018 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 0.028 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 0.037 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 25000000.000 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 312.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 32051.280 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.037 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.046 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.064 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
