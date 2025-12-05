# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-05 04:18:12

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.195 ms; throughput 11355.455 req/s
- Thread scaling: 8 threads -> 173386.500 req/s
- Fan-out: 256 conns -> 256921.200 req/s
- Connection churn (4 threads): 15672.000 req/s
- Stability: sustained 39304.196 req/s
- Contention: ring buffer 8x8 8474576.270 ops/sec
- HTTP fragmented p99 1.788 us
- SIMD scan 16KB p99 0.444 us

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
| Close-after-each-request throughput (4 threads) | 15672.000 | req/s |
## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 81.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 6172839.510 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.005 | us |
| Circular Buffer - Latency p99 | 0.008 | us |
| Circular Buffer - Latency p999 | 0.009 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 45.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.862 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.811 | us |
| HTTP Parser (Complete Request) - Latency p999 | 2.800 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1111111.110 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 43.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.821 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 1.788 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 2.712 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1162790.700 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 11355.455 | req/s |
| Latency IQR | 0.057 | ms |
| Latency avg | 0.067 | ms |
| Latency max | 14.516 | ms |
| Latency p50 | 0.055 | ms |
| Latency p90 | 0.114 | ms |
| Latency p95 | 0.135 | ms |
| Latency p99 | 0.195 | ms |
| Latency p999 | 0.734 | ms |
| Latency samples | 238892.000 | samples |
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
| Ring Buffer Queue (High Contention 8x8) - Duration | 118.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 8474576.270 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 5.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.008 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.078 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 200000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 2.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.023 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.050 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.056 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 50000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 13.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.245 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.444 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 1.335 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 3846153.850 | ops/sec |
## Example: compute_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| 1 threads errors | 85058.000 | count |
| 1 threads throughput | 8518.655 | req/s |
| 16 threads errors | 29790.000 | count |
| 16 threads throughput | 2973.529 | req/s |
| 16t size=1 errors | 6039.000 | count |
| 16t size=1 p50 | 2.494 | ms |
| 16t size=1 p95 | 4.576 | ms |
| 16t size=1 p99 | 5.736 | ms |
| 16t size=1 status_0 | 2686.000 | count |
| 16t size=1 status_200 | 3330.000 | count |
| 16t size=1 throughput | 600.273 | req/s |
| 16t size=1024 errors | 5792.000 | count |
| 16t size=1024 p50 | 2.485 | ms |
| 16t size=1024 p95 | 4.661 | ms |
| 16t size=1024 p99 | 5.905 | ms |
| 16t size=1024 status_0 | 2607.000 | count |
| 16t size=1024 status_200 | 3200.000 | count |
| 16t size=1024 throughput | 579.420 | req/s |
| 16t size=256 errors | 6034.000 | count |
| 16t size=256 p50 | 2.462 | ms |
| 16t size=256 p95 | 4.651 | ms |
| 16t size=256 p99 | 5.729 | ms |
| 16t size=256 status_0 | 2713.000 | count |
| 16t size=256 status_200 | 3394.000 | count |
| 16t size=256 throughput | 609.353 | req/s |
| 16t size=64 errors | 5914.000 | count |
| 16t size=64 p50 | 2.489 | ms |
| 16t size=64 p95 | 4.505 | ms |
| 16t size=64 p99 | 5.917 | ms |
| 16t size=64 status_0 | 2593.000 | count |
| 16t size=64 status_200 | 3321.000 | count |
| 16t size=64 throughput | 590.096 | req/s |
| 16t size=8 errors | 6011.000 | count |
| 16t size=8 p50 | 2.512 | ms |
| 16t size=8 p95 | 4.583 | ms |
| 16t size=8 p99 | 6.111 | ms |
| 16t size=8 status_0 | 2700.000 | count |
| 16t size=8 status_200 | 3257.000 | count |
| 16t size=8 throughput | 594.386 | req/s |
| 1t size=1 errors | 17052.000 | count |
| 1t size=1 p50 | 0.085 | ms |
| 1t size=1 p95 | 0.120 | ms |
| 1t size=1 p99 | 0.164 | ms |
| 1t size=1 status_0 | 7565.000 | count |
| 1t size=1 status_200 | 9474.000 | count |
| 1t size=1 throughput | 1703.811 | req/s |
| 1t size=1024 errors | 16950.000 | count |
| 1t size=1024 p50 | 0.132 | ms |
| 1t size=1024 p95 | 0.185 | ms |
| 1t size=1024 p99 | 0.232 | ms |
| 1t size=1024 status_0 | 7534.000 | count |
| 1t size=1024 status_200 | 9436.000 | count |
| 1t size=1024 throughput | 1696.911 | req/s |
| 1t size=256 errors | 16940.000 | count |
| 1t size=256 p50 | 0.097 | ms |
| 1t size=256 p95 | 0.137 | ms |
| 1t size=256 p99 | 0.178 | ms |
| 1t size=256 status_0 | 7601.000 | count |
| 1t size=256 status_200 | 9472.000 | count |
| 1t size=256 throughput | 1707.211 | req/s |
| 1t size=64 errors | 17042.000 | count |
| 1t size=64 p50 | 0.088 | ms |
| 1t size=64 p95 | 0.125 | ms |
| 1t size=64 p99 | 0.161 | ms |
| 1t size=64 status_0 | 7629.000 | count |
| 1t size=64 status_200 | 9451.000 | count |
| 1t size=64 throughput | 1707.911 | req/s |
| 1t size=8 errors | 17074.000 | count |
| 1t size=8 p50 | 0.086 | ms |
| 1t size=8 p95 | 0.121 | ms |
| 1t size=8 p99 | 0.164 | ms |
| 1t size=8 status_0 | 7521.000 | count |
| 1t size=8 status_200 | 9508.000 | count |
| 1t size=8 throughput | 1702.811 | req/s |
| 4 threads errors | 47368.000 | count |
| 4 threads throughput | 4737.336 | req/s |
| 4t size=1 errors | 9479.000 | count |
| 4t size=1 p50 | 0.362 | ms |
| 4t size=1 p95 | 0.860 | ms |
| 4t size=1 p99 | 1.119 | ms |
| 4t size=1 status_0 | 4198.000 | count |
| 4t size=1 status_200 | 5177.000 | count |
| 4t size=1 throughput | 937.210 | req/s |
| 4t size=1024 errors | 9451.000 | count |
| 4t size=1024 p50 | 0.398 | ms |
| 4t size=1024 p95 | 0.872 | ms |
| 4t size=1024 p99 | 1.166 | ms |
| 4t size=1024 status_0 | 4205.000 | count |
| 4t size=1024 status_200 | 5213.000 | count |
| 4t size=1024 throughput | 941.509 | req/s |
| 4t size=256 errors | 9380.000 | count |
| 4t size=256 p50 | 0.365 | ms |
| 4t size=256 p95 | 0.846 | ms |
| 4t size=256 p99 | 1.120 | ms |
| 4t size=256 status_0 | 4187.000 | count |
| 4t size=256 status_200 | 5188.000 | count |
| 4t size=256 throughput | 937.210 | req/s |
| 4t size=64 errors | 9475.000 | count |
| 4t size=64 p50 | 0.366 | ms |
| 4t size=64 p95 | 0.854 | ms |
| 4t size=64 p99 | 1.135 | ms |
| 4t size=64 status_0 | 4207.000 | count |
| 4t size=64 status_200 | 5321.000 | count |
| 4t size=64 throughput | 952.506 | req/s |
| 4t size=8 errors | 9583.000 | count |
| 4t size=8 p50 | 0.363 | ms |
| 4t size=8 p95 | 0.862 | ms |
| 4t size=8 p99 | 1.174 | ms |
| 4t size=8 status_0 | 4280.000 | count |
| 4t size=8 status_200 | 5412.000 | count |
| 4t size=8 throughput | 968.901 | req/s |
| 8 threads errors | 28645.000 | count |
| 8 threads throughput | 2863.264 | req/s |
| 8t size=1 errors | 5754.000 | count |
| 8t size=1 p50 | 1.287 | ms |
| 8t size=1 p95 | 2.395 | ms |
| 8t size=1 p99 | 3.016 | ms |
| 8t size=1 status_0 | 2550.000 | count |
| 8t size=1 status_200 | 3171.000 | count |
| 8t size=1 throughput | 571.694 | req/s |
| 8t size=1024 errors | 5743.000 | count |
| 8t size=1024 p50 | 1.281 | ms |
| 8t size=1024 p95 | 2.407 | ms |
| 8t size=1024 p99 | 3.030 | ms |
| 8t size=1024 status_0 | 2491.000 | count |
| 8t size=1024 status_200 | 3147.000 | count |
| 8t size=1024 throughput | 563.399 | req/s |
| 8t size=256 errors | 5635.000 | count |
| 8t size=256 p50 | 1.286 | ms |
| 8t size=256 p95 | 2.418 | ms |
| 8t size=256 p99 | 3.094 | ms |
| 8t size=256 status_0 | 2574.000 | count |
| 8t size=256 status_200 | 3217.000 | count |
| 8t size=256 throughput | 578.689 | req/s |
| 8t size=64 errors | 5719.000 | count |
| 8t size=64 p50 | 1.300 | ms |
| 8t size=64 p95 | 2.386 | ms |
| 8t size=64 p99 | 2.995 | ms |
| 8t size=64 status_0 | 2544.000 | count |
| 8t size=64 status_200 | 3178.000 | count |
| 8t size=64 throughput | 571.793 | req/s |
| 8t size=8 errors | 5794.000 | count |
| 8t size=8 p50 | 1.285 | ms |
| 8t size=8 p95 | 2.408 | ms |
| 8t size=8 p99 | 3.020 | ms |
| 8t size=8 status_0 | 2591.000 | count |
| 8t size=8 status_200 | 3190.000 | count |
| 8t size=8 throughput | 577.689 | req/s |
## Example: validation_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| 4 threads success_rate | 50.013 | % success |
| 4 threads throughput | 5057.179 | req/s |
| 4t Invalid errors | 20374.000 | count |
| 4t Invalid p50 | 0.365 | ms |
| 4t Invalid p95 | 0.857 | ms |
| 4t Invalid p99 | 1.136 | ms |
| 4t Invalid status_0 | 10181.000 | count |
| 4t Invalid status_400 | 10182.000 | count |
| 4t Invalid throughput | 2035.929 | req/s |
| 4t Valid errors | 30197.000 | count |
| 4t Valid p50 | 0.363 | ms |
| 4t Valid p95 | 0.844 | ms |
| 4t Valid p99 | 1.123 | ms |
| 4t Valid status_0 | 15103.000 | count |
| 4t Valid status_200 | 15115.000 | count |
| 4t Valid throughput | 3021.250 | req/s |
| 8 threads success_rate | 50.008 | % success |
| 8 threads throughput | 3036.781 | req/s |
| 8t Invalid errors | 12101.000 | count |
| 8t Invalid p50 | 1.283 | ms |
| 8t Invalid p95 | 2.367 | ms |
| 8t Invalid p99 | 3.004 | ms |
| 8t Invalid status_0 | 6034.000 | count |
| 8t Invalid status_400 | 6084.000 | count |
| 8t Invalid throughput | 1211.115 | req/s |
| 8t Valid errors | 18277.000 | count |
| 8t Valid p50 | 1.260 | ms |
| 8t Valid p95 | 2.395 | ms |
| 8t Valid p99 | 3.024 | ms |
| 8t Valid status_0 | 9156.000 | count |
| 8t Valid status_200 | 9111.000 | count |
| 8t Valid throughput | 1825.667 | req/s |
## Generated API

| Benchmark | Value | Unit |
|-----------|-------|------|
| Generated API dispatch+parse - Duration | 162.000 | ms |
| Generated API dispatch+parse - Errors | 80000.000 | count |
| Generated API dispatch+parse - Latency p50 | 0.558 | us |
| Generated API dispatch+parse - Latency p99 | 1.694 | us |
| Generated API dispatch+parse - Latency p999 | 5.611 | us |
| Generated API dispatch+parse - Operations | 200000.000 | ops |
| Generated API dispatch+parse - Throughput | 1234567.900 | ops/sec |
## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 27.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.020 | us |
| Case-Insensitive Compare - Latency p99 | 0.030 | us |
| Case-Insensitive Compare - Latency p999 | 0.030 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 18518518.520 | ops/sec |
| Headers Get (3 lookups) - Duration | 11.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.020 | us |
| Headers Get (3 lookups) - Latency p99 | 0.030 | us |
| Headers Get (3 lookups) - Latency p999 | 0.030 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 18181818.180 | ops/sec |
| Headers Iteration (5 fields) - Duration | 5.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.020 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.030 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.030 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 20000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 18.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.259 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.529 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.757 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2777777.780 | ops/sec |
| Headers Set (5 standard fields) - Duration | 21.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.130 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.219 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.319 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4761904.760 | ops/sec |
## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.089 | ms |
| Large headers p99 | 0.147 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.090 | ms |
| Medium request p99 | 0.154 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.084 | ms |
| Minimal request p99 | 0.137 | ms |
| Minimal request samples | 1500.000 | samples |
## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 5.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.070 | us |
| IO Buffer Append (4KB) - Latency p99 | 0.169 | us |
| IO Buffer Append (4KB) - Latency p999 | 0.349 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 10000000.000 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 5.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.020 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.030 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.030 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 5.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.020 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.030 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.050 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 4.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.020 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.030 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.030 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 25000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 7.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.040 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.080 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.100 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 14285714.290 | ops/sec |
## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 78.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 12820512.820 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 63.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 15873015.870 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 55.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 18181818.180 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 41.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 12195121.950 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 61.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.030 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.040 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.060 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 16393442.620 | ops/sec |
## Router Dispatch

| Benchmark | Value | Unit |
|-----------|-------|------|
| Router dispatch (405) - Duration | 242.000 | ms |
| Router dispatch (405) - Errors | 166667.000 | count |
| Router dispatch (405) - Latency p50 | 0.738 | us |
| Router dispatch (405) - Latency p99 | 1.695 | us |
| Router dispatch (405) - Latency p999 | 10.334 | us |
| Router dispatch (405) - Operations | 200000.000 | ops |
| Router dispatch (405) - Throughput | 826446.280 | ops/sec |
| Router dispatch (hits) - Duration | 206.000 | ms |
| Router dispatch (hits) - Errors | 33333.000 | count |
| Router dispatch (hits) - Latency p50 | 0.538 | us |
| Router dispatch (hits) - Latency p99 | 1.425 | us |
| Router dispatch (hits) - Latency p999 | 9.349 | us |
| Router dispatch (hits) - Operations | 200000.000 | ops |
| Router dispatch (hits) - Throughput | 970873.790 | ops/sec |
| Router dispatch (not found) - Duration | 201.000 | ms |
| Router dispatch (not found) - Errors | 200000.000 | count |
| Router dispatch (not found) - Latency p50 | 0.528 | us |
| Router dispatch (not found) - Latency p99 | 1.146 | us |
| Router dispatch (not found) - Latency p999 | 3.099 | us |
| Router dispatch (not found) - Operations | 200000.000 | ops |
| Router dispatch (not found) - Throughput | 995024.880 | ops/sec |
## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 247742.000 | req/s |
| 256 concurrent connections | 256921.200 | req/s |
| 32 concurrent connections | 223155.600 | req/s |
| 64 concurrent connections | 236594.800 | req/s |
| Throughput with 1 threads | 11446.500 | req/s |
| Throughput with 4 threads | 43416.000 | req/s |
| Throughput with 8 threads | 173386.500 | req/s |
## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 39304.196 | req/s |
| Total requests | 196532.000 | requests |
## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 65535.000 | fds |
## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 38.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.020 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.273 | us |
| Wheel Timer (Add Operations) - Latency p999 | 5.412 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 2631578.950 | ops/sec |
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
