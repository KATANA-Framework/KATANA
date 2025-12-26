# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-26 13:38:56

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.084 ms; throughput 11382.085 req/s
- Thread scaling: 8 threads -> 252413.000 req/s
- Fan-out: 256 conns -> 258905.200 req/s
- Connection churn (4 threads): 16349.333 req/s
- Stability: sustained 42352.832 req/s
- Contention: ring buffer 8x8 11111111.110 ops/sec
- HTTP fragmented p99 1.677 us
- SIMD scan 16KB p99 0.468 us

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
| Close-after-each-request throughput (4 threads) | 16349.333 | req/s |
## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 91.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 5494505.490 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.004 | us |
| Circular Buffer - Latency p99 | 0.010 | us |
| Circular Buffer - Latency p999 | 0.086 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 44.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.835 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.951 | us |
| HTTP Parser (Complete Request) - Latency p999 | 4.001 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1136363.640 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 40.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.773 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 1.677 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 2.705 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1250000.000 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 11382.085 | req/s |
| Latency IQR | 0.006 | ms |
| Latency avg | 0.030 | ms |
| Latency max | 10.234 | ms |
| Latency p50 | 0.027 | ms |
| Latency p90 | 0.040 | ms |
| Latency p95 | 0.051 | ms |
| Latency p99 | 0.084 | ms |
| Latency p999 | 0.198 | ms |
| Latency samples | 523695.000 | samples |
| Memory Allocations (String Queue) - Duration | 2.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 50000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 65.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 15384615.380 | ops/sec |
| Ring Buffer Queue (High Contention 8x8) - Duration | 90.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 11111111.110 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 4.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.009 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.012 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 250000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 1.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.014 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.024 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.222 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 100000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 11.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.224 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.468 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 0.961 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 4545454.550 | ops/sec |
## Example: compute_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| 1 threads errors | 560.000 | count |
| 1 threads throughput | 3979.450 | req/s |
| 16 threads errors | 35181.000 | count |
| 16 threads throughput | 0.000 | req/s |
| 16t size=1 errors | 6994.000 | count |
| 16t size=1 throughput | 0.000 | req/s |
| 16t size=1024 errors | 7024.000 | count |
| 16t size=1024 throughput | 0.000 | req/s |
| 16t size=256 errors | 6940.000 | count |
| 16t size=256 throughput | 0.000 | req/s |
| 16t size=64 errors | 7112.000 | count |
| 16t size=64 throughput | 0.000 | req/s |
| 16t size=8 errors | 7111.000 | count |
| 16t size=8 throughput | 0.000 | req/s |
| 1t size=1 errors | 117.000 | count |
| 1t size=1 p50 | 0.063 | ms |
| 1t size=1 p95 | 0.099 | ms |
| 1t size=1 p99 | 0.121 | ms |
| 1t size=1 status_0 | 117.000 | count |
| 1t size=1 status_200 | 7737.000 | count |
| 1t size=1 throughput | 785.370 | req/s |
| 1t size=1024 errors | 94.000 | count |
| 1t size=1024 p50 | 0.063 | ms |
| 1t size=1024 p95 | 12.370 | ms |
| 1t size=1024 p99 | 13.099 | ms |
| 1t size=1024 status_0 | 94.000 | count |
| 1t size=1024 status_200 | 7991.000 | count |
| 1t size=1024 throughput | 808.469 | req/s |
| 1t size=256 errors | 118.000 | count |
| 1t size=256 p50 | 0.063 | ms |
| 1t size=256 p95 | 0.110 | ms |
| 1t size=256 p99 | 12.705 | ms |
| 1t size=256 status_0 | 118.000 | count |
| 1t size=256 status_200 | 7783.000 | count |
| 1t size=256 throughput | 790.070 | req/s |
| 1t size=64 errors | 103.000 | count |
| 1t size=64 p50 | 0.063 | ms |
| 1t size=64 p95 | 0.101 | ms |
| 1t size=64 p99 | 0.127 | ms |
| 1t size=64 status_0 | 103.000 | count |
| 1t size=64 status_200 | 7800.000 | count |
| 1t size=64 throughput | 790.270 | req/s |
| 1t size=8 errors | 128.000 | count |
| 1t size=8 p50 | 0.063 | ms |
| 1t size=8 p95 | 0.099 | ms |
| 1t size=8 p99 | 0.123 | ms |
| 1t size=8 status_0 | 128.000 | count |
| 1t size=8 status_200 | 7925.000 | count |
| 1t size=8 throughput | 805.270 | req/s |
| 4 threads errors | 39766.000 | count |
| 4 threads throughput | 0.000 | req/s |
| 4t size=1 errors | 7993.000 | count |
| 4t size=1 throughput | 0.000 | req/s |
| 4t size=1024 errors | 8002.000 | count |
| 4t size=1024 throughput | 0.000 | req/s |
| 4t size=256 errors | 7870.000 | count |
| 4t size=256 throughput | 0.000 | req/s |
| 4t size=64 errors | 7912.000 | count |
| 4t size=64 throughput | 0.000 | req/s |
| 4t size=8 errors | 7989.000 | count |
| 4t size=8 throughput | 0.000 | req/s |
| 8 threads errors | 33715.000 | count |
| 8 threads throughput | 0.000 | req/s |
| 8t size=1 errors | 6822.000 | count |
| 8t size=1 throughput | 0.000 | req/s |
| 8t size=1024 errors | 6772.000 | count |
| 8t size=1024 throughput | 0.000 | req/s |
| 8t size=256 errors | 6857.000 | count |
| 8t size=256 throughput | 0.000 | req/s |
| 8t size=64 errors | 6544.000 | count |
| 8t size=64 throughput | 0.000 | req/s |
| 8t size=8 errors | 6720.000 | count |
| 8t size=8 throughput | 0.000 | req/s |
## Example: validation_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| 4 threads success_rate | 100.000 | % success |
| 4 threads throughput | 10782.544 | req/s |
| 4t Invalid errors | 0.000 | count |
| 4t Invalid p50 | 0.336 | ms |
| 4t Invalid p95 | 0.713 | ms |
| 4t Invalid p99 | 0.933 | ms |
| 4t Invalid status_400 | 43238.000 | count |
| 4t Invalid throughput | 4322.855 | req/s |
| 4t Valid errors | 0.000 | count |
| 4t Valid p50 | 0.334 | ms |
| 4t Valid p95 | 0.710 | ms |
| 4t Valid p99 | 0.945 | ms |
| 4t Valid status_200 | 64611.000 | count |
| 4t Valid throughput | 6459.688 | req/s |
| 8 threads success_rate | 100.000 | % success |
| 8 threads throughput | 6908.220 | req/s |
| 8t Invalid errors | 0.000 | count |
| 8t Invalid p50 | 1.055 | ms |
| 8t Invalid p95 | 2.165 | ms |
| 8t Invalid p99 | 2.817 | ms |
| 8t Invalid status_400 | 27615.000 | count |
| 8t Invalid throughput | 2759.750 | req/s |
| 8t Valid errors | 0.000 | count |
| 8t Valid p50 | 1.073 | ms |
| 8t Valid p95 | 2.199 | ms |
| 8t Valid p99 | 2.856 | ms |
| 8t Valid status_200 | 41511.000 | count |
| 8t Valid throughput | 4148.469 | req/s |
## Generated API

| Benchmark | Value | Unit |
|-----------|-------|------|
| Generated API dispatch+parse - Duration | 118.000 | ms |
| Generated API dispatch+parse - Errors | 0.000 | count |
| Generated API dispatch+parse - Latency p50 | 0.431 | us |
| Generated API dispatch+parse - Latency p99 | 0.991 | us |
| Generated API dispatch+parse - Latency p999 | 2.654 | us |
| Generated API dispatch+parse - Operations | 200000.000 | ops |
| Generated API dispatch+parse - Throughput | 1694915.250 | ops/sec |
## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 24.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.018 | us |
| Case-Insensitive Compare - Latency p99 | 0.028 | us |
| Case-Insensitive Compare - Latency p999 | 0.028 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 20833333.330 | ops/sec |
| Headers Get (3 lookups) - Duration | 10.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.018 | us |
| Headers Get (3 lookups) - Latency p99 | 0.028 | us |
| Headers Get (3 lookups) - Latency p999 | 0.028 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 20000000.000 | ops/sec |
| Headers Iteration (5 fields) - Duration | 4.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.018 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.028 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.028 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 25000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 20.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.312 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.569 | us |
| Headers Set (4 custom fields) - Latency p999 | 1.020 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2500000.000 | ops/sec |
| Headers Set (5 standard fields) - Duration | 21.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.138 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.220 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.303 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4761904.760 | ops/sec |
## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.085 | ms |
| Large headers p99 | 0.122 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.084 | ms |
| Medium request p99 | 0.123 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.079 | ms |
| Minimal request p99 | 0.112 | ms |
| Minimal request samples | 1500.000 | samples |
## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 52.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.974 | us |
| IO Buffer Append (4KB) - Latency p99 | 1.616 | us |
| IO Buffer Append (4KB) - Latency p999 | 8.715 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 961538.460 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 7.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.028 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.046 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.147 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 14285714.290 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 8.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.046 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.083 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.110 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 12500000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 6.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.037 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.046 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.083 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 16666666.670 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.037 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.065 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.083 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |
## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 77.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 12987012.990 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 60.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 16666666.670 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 48.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 20833333.330 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 29.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 17241379.310 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 56.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.027 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.037 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.055 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 17857142.860 | ops/sec |
## Router Dispatch

| Benchmark | Value | Unit |
|-----------|-------|------|
| Router dispatch (405) - Duration | 201.000 | ms |
| Router dispatch (405) - Errors | 166667.000 | count |
| Router dispatch (405) - Latency p50 | 0.624 | us |
| Router dispatch (405) - Latency p99 | 1.322 | us |
| Router dispatch (405) - Latency p999 | 5.373 | us |
| Router dispatch (405) - Operations | 200000.000 | ops |
| Router dispatch (405) - Throughput | 995024.880 | ops/sec |
| Router dispatch (hits) - Duration | 164.000 | ms |
| Router dispatch (hits) - Errors | 33333.000 | count |
| Router dispatch (hits) - Latency p50 | 0.385 | us |
| Router dispatch (hits) - Latency p99 | 0.790 | us |
| Router dispatch (hits) - Latency p999 | 2.810 | us |
| Router dispatch (hits) - Operations | 200000.000 | ops |
| Router dispatch (hits) - Throughput | 1219512.200 | ops/sec |
| Router dispatch (not found) - Duration | 169.000 | ms |
| Router dispatch (not found) - Errors | 200000.000 | count |
| Router dispatch (not found) - Latency p50 | 0.459 | us |
| Router dispatch (not found) - Latency p99 | 0.900 | us |
| Router dispatch (not found) - Latency p999 | 2.296 | us |
| Router dispatch (not found) - Operations | 200000.000 | ops |
| Router dispatch (not found) - Throughput | 1183431.950 | ops/sec |
## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 268700.000 | req/s |
| 256 concurrent connections | 258905.200 | req/s |
| 32 concurrent connections | 257747.200 | req/s |
| 64 concurrent connections | 253528.400 | req/s |
| Throughput with 1 threads | 12119.500 | req/s |
| Throughput with 4 threads | 45727.500 | req/s |
| Throughput with 8 threads | 252413.000 | req/s |
## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 42352.832 | req/s |
| Total requests | 211774.000 | requests |
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
| Wheel Timer (Add Operations) - Latency p99 | 2.287 | us |
| Wheel Timer (Add Operations) - Latency p999 | 6.474 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 5882352.940 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 2.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 0.018 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 0.028 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 0.045 | us |
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
