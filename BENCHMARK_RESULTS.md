# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-25 15:17:18

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.170 ms; throughput 9886.652 req/s
- Thread scaling: 8 threads -> 228876.000 req/s
- Fan-out: 256 conns -> 287452.400 req/s
- Connection churn (4 threads): 15506.000 req/s
- Stability: sustained 40258.969 req/s
- Contention: ring buffer 8x8 10638297.870 ops/sec
- HTTP fragmented p99 2.503 us
- SIMD scan 16KB p99 0.434 us

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
| Close-after-each-request throughput (4 threads) | 15506.000 | req/s |
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
| HTTP Parser (Complete Request) - Duration | 55.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.860 | us |
| HTTP Parser (Complete Request) - Latency p99 | 2.383 | us |
| HTTP Parser (Complete Request) - Latency p999 | 3.498 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 909090.910 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 67.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 1.531 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 2.503 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 3.870 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 746268.660 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 9886.652 | req/s |
| Latency IQR | 0.020 | ms |
| Latency avg | 0.050 | ms |
| Latency max | 10.857 | ms |
| Latency p50 | 0.028 | ms |
| Latency p90 | 0.072 | ms |
| Latency p95 | 0.094 | ms |
| Latency p99 | 0.170 | ms |
| Latency p999 | 4.002 | ms |
| Latency samples | 315910.000 | samples |
| Memory Allocations (String Queue) - Duration | 2.000 | ms |
| Memory Allocations (String Queue) - Latency p50 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p99 | 0.000 | us |
| Memory Allocations (String Queue) - Latency p999 | 0.000 | us |
| Memory Allocations (String Queue) - Operations | 100000.000 | ops |
| Memory Allocations (String Queue) - Throughput | 50000000.000 | ops/sec |
| Ring Buffer Queue (Concurrent 4x4) - Duration | 64.000 | ms |
| Ring Buffer Queue (Concurrent 4x4) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (Concurrent 4x4) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Concurrent 4x4) - Throughput | 15625000.000 | ops/sec |
| Ring Buffer Queue (High Contention 8x8) - Duration | 94.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 10638297.870 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 4.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.006 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.009 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 250000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 1.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.016 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.041 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.049 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 100000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 12.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.246 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.434 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 1.554 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 4166666.670 | ops/sec |
## Example: compute_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| 1 threads errors | 61787.000 | count |
| 1 threads throughput | 6199.505 | req/s |
| 16 threads errors | 31413.000 | count |
| 16 threads throughput | 3137.385 | req/s |
| 16t size=1 errors | 6235.000 | count |
| 16t size=1 p50 | 2.379 | ms |
| 16t size=1 p95 | 4.356 | ms |
| 16t size=1 p99 | 5.414 | ms |
| 16t size=1 status_0 | 2772.000 | count |
| 16t size=1 status_200 | 3483.000 | count |
| 16t size=1 throughput | 624.502 | req/s |
| 16t size=1024 errors | 6419.000 | count |
| 16t size=1024 p50 | 2.348 | ms |
| 16t size=1024 p95 | 4.250 | ms |
| 16t size=1024 p99 | 5.362 | ms |
| 16t size=1024 status_0 | 2844.000 | count |
| 16t size=1024 status_200 | 3485.000 | count |
| 16t size=1024 throughput | 631.890 | req/s |
| 16t size=256 errors | 6254.000 | count |
| 16t size=256 p50 | 2.390 | ms |
| 16t size=256 p95 | 4.311 | ms |
| 16t size=256 p99 | 5.345 | ms |
| 16t size=256 status_0 | 2771.000 | count |
| 16t size=256 status_200 | 3388.000 | count |
| 16t size=256 throughput | 614.917 | req/s |
| 16t size=64 errors | 6272.000 | count |
| 16t size=64 p50 | 2.385 | ms |
| 16t size=64 p95 | 4.309 | ms |
| 16t size=64 p99 | 5.507 | ms |
| 16t size=64 status_0 | 2832.000 | count |
| 16t size=64 status_200 | 3583.000 | count |
| 16t size=64 throughput | 640.476 | req/s |
| 16t size=8 errors | 6233.000 | count |
| 16t size=8 p50 | 2.375 | ms |
| 16t size=8 p95 | 4.292 | ms |
| 16t size=8 p99 | 5.432 | ms |
| 16t size=8 status_0 | 2747.000 | count |
| 16t size=8 status_200 | 3519.000 | count |
| 16t size=8 throughput | 625.600 | req/s |
| 1t size=1 errors | 12346.000 | count |
| 1t size=1 p50 | 0.096 | ms |
| 1t size=1 p95 | 0.173 | ms |
| 1t size=1 p99 | 2.326 | ms |
| 1t size=1 status_0 | 5514.000 | count |
| 1t size=1 status_200 | 6823.000 | count |
| 1t size=1 throughput | 1233.641 | req/s |
| 1t size=1024 errors | 12507.000 | count |
| 1t size=1024 p50 | 0.159 | ms |
| 1t size=1024 p95 | 0.315 | ms |
| 1t size=1024 p99 | 2.562 | ms |
| 1t size=1024 status_0 | 5459.000 | count |
| 1t size=1024 status_200 | 6905.000 | count |
| 1t size=1024 throughput | 1236.341 | req/s |
| 1t size=256 errors | 12246.000 | count |
| 1t size=256 p50 | 0.110 | ms |
| 1t size=256 p95 | 0.196 | ms |
| 1t size=256 p99 | 2.253 | ms |
| 1t size=256 status_0 | 5530.000 | count |
| 1t size=256 status_200 | 7029.000 | count |
| 1t size=256 throughput | 1255.840 | req/s |
| 1t size=64 errors | 12302.000 | count |
| 1t size=64 p50 | 0.100 | ms |
| 1t size=64 p95 | 0.183 | ms |
| 1t size=64 p99 | 2.093 | ms |
| 1t size=64 status_0 | 5540.000 | count |
| 1t size=64 status_200 | 6955.000 | count |
| 1t size=64 throughput | 1249.440 | req/s |
| 1t size=8 errors | 12386.000 | count |
| 1t size=8 p50 | 0.097 | ms |
| 1t size=8 p95 | 0.187 | ms |
| 1t size=8 p99 | 2.300 | ms |
| 1t size=8 status_0 | 5424.000 | count |
| 1t size=8 status_200 | 6819.000 | count |
| 1t size=8 throughput | 1224.242 | req/s |
| 4 threads errors | 40386.000 | count |
| 4 threads throughput | 4066.769 | req/s |
| 4t size=1 errors | 8057.000 | count |
| 4t size=1 p50 | 0.347 | ms |
| 4t size=1 p95 | 2.952 | ms |
| 4t size=1 p99 | 4.778 | ms |
| 4t size=1 status_0 | 3589.000 | count |
| 4t size=1 status_200 | 4547.000 | count |
| 4t size=1 throughput | 813.114 | req/s |
| 4t size=1024 errors | 8135.000 | count |
| 4t size=1024 p50 | 0.427 | ms |
| 4t size=1024 p95 | 3.031 | ms |
| 4t size=1024 p99 | 5.226 | ms |
| 4t size=1024 status_0 | 3549.000 | count |
| 4t size=1024 status_200 | 4642.000 | count |
| 4t size=1024 throughput | 818.611 | req/s |
| 4t size=256 errors | 8064.000 | count |
| 4t size=256 p50 | 0.370 | ms |
| 4t size=256 p95 | 2.977 | ms |
| 4t size=256 p99 | 5.135 | ms |
| 4t size=256 status_0 | 3552.000 | count |
| 4t size=256 status_200 | 4536.000 | count |
| 4t size=256 throughput | 808.317 | req/s |
| 4t size=64 errors | 8046.000 | count |
| 4t size=64 p50 | 0.357 | ms |
| 4t size=64 p95 | 3.018 | ms |
| 4t size=64 p99 | 4.962 | ms |
| 4t size=64 status_0 | 3525.000 | count |
| 4t size=64 status_200 | 4586.000 | count |
| 4t size=64 throughput | 810.615 | req/s |
| 4t size=8 errors | 8084.000 | count |
| 4t size=8 p50 | 0.363 | ms |
| 4t size=8 p95 | 3.011 | ms |
| 4t size=8 p99 | 4.758 | ms |
| 4t size=8 status_0 | 3629.000 | count |
| 4t size=8 status_200 | 4537.000 | count |
| 4t size=8 throughput | 816.112 | req/s |
| 8 threads errors | 26878.000 | count |
| 8 threads throughput | 2691.503 | req/s |
| 8t size=1 errors | 5275.000 | count |
| 8t size=1 p50 | 1.242 | ms |
| 8t size=1 p95 | 4.269 | ms |
| 8t size=1 p99 | 8.199 | ms |
| 8t size=1 status_0 | 2309.000 | count |
| 8t size=1 status_200 | 3015.000 | count |
| 8t size=1 throughput | 532.104 | req/s |
| 8t size=1024 errors | 5311.000 | count |
| 8t size=1024 p50 | 1.284 | ms |
| 8t size=1024 p95 | 4.478 | ms |
| 8t size=1024 p99 | 8.399 | ms |
| 8t size=1024 status_0 | 2345.000 | count |
| 8t size=1024 status_200 | 2997.000 | count |
| 8t size=1024 throughput | 533.903 | req/s |
| 8t size=256 errors | 5395.000 | count |
| 8t size=256 p50 | 1.289 | ms |
| 8t size=256 p95 | 4.515 | ms |
| 8t size=256 p99 | 8.294 | ms |
| 8t size=256 status_0 | 2419.000 | count |
| 8t size=256 status_200 | 3049.000 | count |
| 8t size=256 throughput | 546.496 | req/s |
| 8t size=64 errors | 5426.000 | count |
| 8t size=64 p50 | 1.281 | ms |
| 8t size=64 p95 | 4.409 | ms |
| 8t size=64 p99 | 7.811 | ms |
| 8t size=64 status_0 | 2391.000 | count |
| 8t size=64 status_200 | 3035.000 | count |
| 8t size=64 throughput | 542.298 | req/s |
| 8t size=8 errors | 5471.000 | count |
| 8t size=8 p50 | 1.275 | ms |
| 8t size=8 p95 | 4.411 | ms |
| 8t size=8 p99 | 7.567 | ms |
| 8t size=8 status_0 | 2457.000 | count |
| 8t size=8 status_200 | 2913.000 | count |
| 8t size=8 throughput | 536.702 | req/s |
## Example: validation_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| 4 threads success_rate | 50.021 | % success |
| 4 threads throughput | 5318.299 | req/s |
| 4t Invalid errors | 21063.000 | count |
| 4t Invalid p50 | 0.352 | ms |
| 4t Invalid p95 | 0.826 | ms |
| 4t Invalid p99 | 1.160 | ms |
| 4t Invalid status_0 | 10537.000 | count |
| 4t Invalid status_400 | 10569.000 | count |
| 4t Invalid throughput | 2110.163 | req/s |
| 4t Valid errors | 32118.000 | count |
| 4t Valid p50 | 0.354 | ms |
| 4t Valid p95 | 0.825 | ms |
| 4t Valid p99 | 1.141 | ms |
| 4t Valid status_0 | 16049.000 | count |
| 4t Valid status_200 | 16039.000 | count |
| 4t Valid throughput | 3208.136 | req/s |
| 8 threads success_rate | 50.018 | % success |
| 8 threads throughput | 3137.401 | req/s |
| 8t Invalid errors | 12516.000 | count |
| 8t Invalid p50 | 1.243 | ms |
| 8t Invalid p95 | 2.404 | ms |
| 8t Invalid p99 | 3.094 | ms |
| 8t Invalid status_0 | 6270.000 | count |
| 8t Invalid status_400 | 6220.000 | count |
| 8t Invalid throughput | 1248.483 | req/s |
| 8t Valid errors | 18860.000 | count |
| 8t Valid p50 | 1.227 | ms |
| 8t Valid p95 | 2.384 | ms |
| 8t Valid p99 | 3.262 | ms |
| 8t Valid status_0 | 9418.000 | count |
| 8t Valid status_200 | 9479.000 | count |
| 8t Valid throughput | 1888.918 | req/s |
## Generated API

| Benchmark | Value | Unit |
|-----------|-------|------|
| Generated API dispatch+parse - Duration | 151.000 | ms |
| Generated API dispatch+parse - Errors | 0.000 | count |
| Generated API dispatch+parse - Latency p50 | 0.571 | us |
| Generated API dispatch+parse - Latency p99 | 1.453 | us |
| Generated API dispatch+parse - Latency p999 | 2.775 | us |
| Generated API dispatch+parse - Operations | 200000.000 | ops |
| Generated API dispatch+parse - Throughput | 1324503.310 | ops/sec |
## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 26.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.020 | us |
| Case-Insensitive Compare - Latency p99 | 0.030 | us |
| Case-Insensitive Compare - Latency p999 | 0.031 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 19230769.230 | ops/sec |
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
| Headers Set (4 custom fields) - Latency p99 | 0.521 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.722 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 2941176.470 | ops/sec |
| Headers Set (5 standard fields) - Duration | 21.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.130 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.220 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.351 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 4761904.760 | ops/sec |
## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.089 | ms |
| Large headers p99 | 0.144 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.087 | ms |
| Medium request p99 | 0.139 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.081 | ms |
| Minimal request p99 | 0.134 | ms |
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
| IO Buffer Read/Write (256B) - Latency p999 | 0.060 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 4.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.020 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.030 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.031 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 25000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 7.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.040 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.080 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.090 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 14285714.290 | ops/sec |
## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 72.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 13888888.890 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 58.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 17241379.310 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 54.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 18518518.520 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 26.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 19230769.230 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 61.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.030 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.040 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.060 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 16393442.620 | ops/sec |
## Router Dispatch

| Benchmark | Value | Unit |
|-----------|-------|------|
| Router dispatch (405) - Duration | 230.000 | ms |
| Router dispatch (405) - Errors | 166667.000 | count |
| Router dispatch (405) - Latency p50 | 0.741 | us |
| Router dispatch (405) - Latency p99 | 1.533 | us |
| Router dispatch (405) - Latency p999 | 3.467 | us |
| Router dispatch (405) - Operations | 200000.000 | ops |
| Router dispatch (405) - Throughput | 869565.220 | ops/sec |
| Router dispatch (hits) - Duration | 203.000 | ms |
| Router dispatch (hits) - Errors | 33333.000 | count |
| Router dispatch (hits) - Latency p50 | 0.531 | us |
| Router dispatch (hits) - Latency p99 | 1.313 | us |
| Router dispatch (hits) - Latency p999 | 3.327 | us |
| Router dispatch (hits) - Operations | 200000.000 | ops |
| Router dispatch (hits) - Throughput | 985221.670 | ops/sec |
| Router dispatch (not found) - Duration | 196.000 | ms |
| Router dispatch (not found) - Errors | 200000.000 | count |
| Router dispatch (not found) - Latency p50 | 0.531 | us |
| Router dispatch (not found) - Latency p99 | 1.122 | us |
| Router dispatch (not found) - Latency p999 | 2.655 | us |
| Router dispatch (not found) - Operations | 200000.000 | ops |
| Router dispatch (not found) - Throughput | 1020408.160 | ops/sec |
## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 281326.000 | req/s |
| 256 concurrent connections | 287452.400 | req/s |
| 32 concurrent connections | 253727.200 | req/s |
| 64 concurrent connections | 272190.000 | req/s |
| Throughput with 1 threads | 11671.500 | req/s |
| Throughput with 4 threads | 43972.000 | req/s |
| Throughput with 8 threads | 228876.000 | req/s |
## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 40258.969 | req/s |
| Total requests | 201307.000 | requests |
## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1048576.000 | fds |
## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 17.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.020 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.294 | us |
| Wheel Timer (Add Operations) - Latency p999 | 6.171 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 5882352.940 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 2.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 0.020 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 0.030 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 0.090 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 25000000.000 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 313.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 31948.880 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.040 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.051 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.051 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
