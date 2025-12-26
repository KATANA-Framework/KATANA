# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-26 20:51:04

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.087 ms; throughput 12398.843 req/s
- Thread scaling: 8 threads -> 229853.000 req/s
- Fan-out: 128 conns -> 270534.800 req/s
- Connection churn (4 threads): 16327.333 req/s
- Stability: sustained 42697.308 req/s
- Contention: ring buffer 8x8 10752688.170 ops/sec
- HTTP fragmented p99 1.720 us
- SIMD scan 16KB p99 0.424 us

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
| Close-after-each-request throughput (4 threads) | 16327.333 | req/s |
## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 77.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 6493506.490 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.004 | us |
| Circular Buffer - Latency p99 | 0.007 | us |
| Circular Buffer - Latency p999 | 0.010 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 43.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.826 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.794 | us |
| HTTP Parser (Complete Request) - Latency p999 | 2.815 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1162790.700 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 39.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.759 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 1.720 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 2.571 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1282051.280 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 12398.843 | req/s |
| Latency IQR | 0.006 | ms |
| Latency avg | 0.031 | ms |
| Latency max | 7.512 | ms |
| Latency p50 | 0.026 | ms |
| Latency p90 | 0.041 | ms |
| Latency p95 | 0.054 | ms |
| Latency p99 | 0.087 | ms |
| Latency p999 | 0.230 | ms |
| Latency samples | 519235.000 | samples |
| Memory Allocations (String Queue) - Duration | 2.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 50000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 56.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 17857142.860 | ops/sec |
| Ring Buffer Queue (High Contention 8x8) - Duration | 93.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 10752688.170 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 4.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.005 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.024 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 250000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 1.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.014 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.014 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.016 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 100000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 11.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.220 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.424 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 2.151 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 4545454.550 | ops/sec |
## Example: compute_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| wrk size=1 throughput | 208810.700 | req/s |
| wrk size=1024 throughput | 17598.670 | req/s |
| wrk size=256 socket_errors | 32.000 | count |
| wrk size=256 throughput | 48191.690 | req/s |
| wrk size=64 throughput | 151360.460 | req/s |
| wrk size=8 socket_errors | 28.000 | count |
| wrk size=8 throughput | 158438.720 | req/s |
## Example: validation_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| wrk invalid only non_2xx_3xx | 2291125.000 | count |
| wrk invalid only throughput | 189312.370 | req/s |
| wrk mix (60% valid / 40% invalid) non_2xx_3xx | 928136.000 | count |
| wrk mix (60% valid / 40% invalid) throughput | 191397.500 | req/s |
| wrk valid only socket_errors | 26.000 | count |
| wrk valid only throughput | 154911.850 | req/s |
## Generated API

| Benchmark | Value | Unit |
|-----------|-------|------|
| Generated API dispatch+parse - Duration | 121.000 | ms |
| Generated API dispatch+parse - Errors | 0.000 | count |
| Generated API dispatch+parse - Latency p50 | 0.433 | us |
| Generated API dispatch+parse - Latency p99 | 1.127 | us |
| Generated API dispatch+parse - Latency p999 | 3.138 | us |
| Generated API dispatch+parse - Operations | 200000.000 | ops |
| Generated API dispatch+parse - Throughput | 1652892.560 | ops/sec |
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
| Headers Set (4 custom fields) - Duration | 19.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.306 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.577 | us |
| Headers Set (4 custom fields) - Latency p999 | 1.118 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2631578.950 | ops/sec |
| Headers Set (5 standard fields) - Duration | 21.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.135 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.297 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.360 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4761904.760 | ops/sec |
## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.082 | ms |
| Large headers p99 | 0.122 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.083 | ms |
| Medium request p99 | 0.118 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.078 | ms |
| Minimal request p99 | 0.112 | ms |
| Minimal request samples | 1500.000 | samples |
## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 49.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.920 | us |
| IO Buffer Append (4KB) - Latency p99 | 1.569 | us |
| IO Buffer Append (4KB) - Latency p999 | 7.358 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 1020408.160 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 6.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.027 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.045 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.063 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 16666666.670 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 7.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.045 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.081 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.099 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 14285714.290 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 6.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.036 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.073 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.118 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 16666666.670 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.036 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.063 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.090 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |
## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 60.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 16666666.670 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 57.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 17543859.650 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 48.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 20833333.330 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 29.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 17241379.310 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 55.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.027 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.036 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.054 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 18181818.180 | ops/sec |
## Router Dispatch

| Benchmark | Value | Unit |
|-----------|-------|------|
| Router dispatch (405) - Duration | 196.000 | ms |
| Router dispatch (405) - Errors | 166667.000 | count |
| Router dispatch (405) - Latency p50 | 0.613 | us |
| Router dispatch (405) - Latency p99 | 1.271 | us |
| Router dispatch (405) - Latency p999 | 3.408 | us |
| Router dispatch (405) - Operations | 200000.000 | ops |
| Router dispatch (405) - Throughput | 1020408.160 | ops/sec |
| Router dispatch (hits) - Duration | 164.000 | ms |
| Router dispatch (hits) - Errors | 33333.000 | count |
| Router dispatch (hits) - Latency p50 | 0.370 | us |
| Router dispatch (hits) - Latency p99 | 0.839 | us |
| Router dispatch (hits) - Latency p999 | 3.021 | us |
| Router dispatch (hits) - Operations | 200000.000 | ops |
| Router dispatch (hits) - Throughput | 1219512.200 | ops/sec |
| Router dispatch (not found) - Duration | 167.000 | ms |
| Router dispatch (not found) - Errors | 200000.000 | count |
| Router dispatch (not found) - Latency p50 | 0.451 | us |
| Router dispatch (not found) - Latency p99 | 0.893 | us |
| Router dispatch (not found) - Latency p999 | 2.552 | us |
| Router dispatch (not found) - Operations | 200000.000 | ops |
| Router dispatch (not found) - Throughput | 1197604.790 | ops/sec |
## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 270534.800 | req/s |
| 32 concurrent connections | 251638.800 | req/s |
| 64 concurrent connections | 266250.400 | req/s |
| Throughput with 1 threads | 12505.500 | req/s |
| Throughput with 4 threads | 46281.500 | req/s |
| Throughput with 8 threads | 229853.000 | req/s |
## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 42697.308 | req/s |
| Total requests | 213497.000 | requests |
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
| Wheel Timer (Add Operations) - Latency p99 | 2.137 | us |
| Wheel Timer (Add Operations) - Latency p999 | 6.302 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 6250000.000 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 2.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 0.018 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 0.027 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 0.028 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 25000000.000 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 312.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 32051.280 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.045 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.055 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.055 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
