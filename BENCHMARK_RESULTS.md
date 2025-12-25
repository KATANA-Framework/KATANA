# KATANA Framework - Comprehensive Benchmark Results

Generated: 2025-12-26 02:22:40

This report includes results from all benchmark suites.

## Summary

- Core: p99 0.093 ms; throughput 11765.683 req/s
- Thread scaling: 8 threads -> 240454.500 req/s
- Fan-out: 256 conns -> 261286.400 req/s
- Connection churn (4 threads): 15899.333 req/s
- Stability: sustained 40963.940 req/s
- Contention: ring buffer 8x8 9523809.520 ops/sec
- HTTP fragmented p99 1.740 us
- SIMD scan 16KB p99 0.406 us

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
| Close-after-each-request throughput (4 threads) | 15899.333 | req/s |
## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Arena Allocations (64B objects) - Duration | 76.000 | ms |
| Arena Allocations (64B objects) - Latency p50 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p99 | 0.000 | us |
| Arena Allocations (64B objects) - Latency p999 | 0.000 | us |
| Arena Allocations (64B objects) - Operations | 500000.000 | ops |
| Arena Allocations (64B objects) - Throughput | 6578947.370 | ops/sec |
| Circular Buffer - Duration | 2.000 | ms |
| Circular Buffer - Latency p50 | 0.005 | us |
| Circular Buffer - Latency p99 | 0.007 | us |
| Circular Buffer - Latency p999 | 0.011 | us |
| Circular Buffer - Operations | 500000.000 | ops |
| Circular Buffer - Throughput | 250000000.000 | ops/sec |
| HTTP Parser (Complete Request) - Duration | 48.000 | ms |
| HTTP Parser (Complete Request) - Latency p50 | 0.916 | us |
| HTTP Parser (Complete Request) - Latency p99 | 1.865 | us |
| HTTP Parser (Complete Request) - Latency p999 | 3.664 | us |
| HTTP Parser (Complete Request) - Operations | 50000.000 | ops |
| HTTP Parser (Complete Request) - Throughput | 1041666.670 | ops/sec |
| HTTP Parser (Fragmented Request) - Duration | 44.000 | ms |
| HTTP Parser (Fragmented Request) - Latency p50 | 0.860 | us |
| HTTP Parser (Fragmented Request) - Latency p99 | 1.740 | us |
| HTTP Parser (Fragmented Request) - Latency p999 | 3.064 | us |
| HTTP Parser (Fragmented Request) - Operations | 50000.000 | ops |
| HTTP Parser (Fragmented Request) - Throughput | 1136363.640 | ops/sec |
| Keep-alive success | 4996.000 | requests |
| Keep-alive throughput | 11765.683 | req/s |
| Latency IQR | 0.007 | ms |
| Latency avg | 0.033 | ms |
| Latency max | 10.413 | ms |
| Latency p50 | 0.028 | ms |
| Latency p90 | 0.045 | ms |
| Latency p95 | 0.057 | ms |
| Latency p99 | 0.093 | ms |
| Latency p999 | 0.315 | ms |
| Latency samples | 484589.000 | samples |
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
| Ring Buffer Queue (High Contention 8x8) - Duration | 105.000 | ms |
| Ring Buffer Queue (High Contention 8x8) - Latency p50 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p99 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Latency p999 | 0.000 | us |
| Ring Buffer Queue (High Contention 8x8) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (High Contention 8x8) - Throughput | 9523809.520 | ops/sec |
| Ring Buffer Queue (Single Thread) - Duration | 4.000 | ms |
| Ring Buffer Queue (Single Thread) - Latency p50 | 0.004 | us |
| Ring Buffer Queue (Single Thread) - Latency p99 | 0.007 | us |
| Ring Buffer Queue (Single Thread) - Latency p999 | 0.008 | us |
| Ring Buffer Queue (Single Thread) - Operations | 1000000.000 | ops |
| Ring Buffer Queue (Single Thread) - Throughput | 250000000.000 | ops/sec |
| SIMD CRLF Search (1.5KB buffer) - Duration | 2.000 | ms |
| SIMD CRLF Search (1.5KB buffer) - Latency p50 | 0.022 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p99 | 0.047 | us |
| SIMD CRLF Search (1.5KB buffer) - Latency p999 | 0.054 | us |
| SIMD CRLF Search (1.5KB buffer) - Operations | 100000.000 | ops |
| SIMD CRLF Search (1.5KB buffer) - Throughput | 50000000.000 | ops/sec |
| SIMD CRLF Search (16KB buffer) - Duration | 12.000 | ms |
| SIMD CRLF Search (16KB buffer) - Latency p50 | 0.232 | us |
| SIMD CRLF Search (16KB buffer) - Latency p99 | 0.406 | us |
| SIMD CRLF Search (16KB buffer) - Latency p999 | 1.237 | us |
| SIMD CRLF Search (16KB buffer) - Operations | 50000.000 | ops |
| SIMD CRLF Search (16KB buffer) - Throughput | 4166666.670 | ops/sec |
## Example: compute_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| 1 threads errors | 84109.000 | count |
| 1 threads throughput | 8417.342 | req/s |
| 16 threads errors | 31823.000 | count |
| 16 threads throughput | 3179.120 | req/s |
| 16t size=1 errors | 6366.000 | count |
| 16t size=1 p50 | 2.328 | ms |
| 16t size=1 p95 | 4.121 | ms |
| 16t size=1 p99 | 4.967 | ms |
| 16t size=1 status_0 | 2794.000 | count |
| 16t size=1 status_200 | 3546.000 | count |
| 16t size=1 throughput | 633.128 | req/s |
| 16t size=1024 errors | 6329.000 | count |
| 16t size=1024 p50 | 2.313 | ms |
| 16t size=1024 p95 | 4.094 | ms |
| 16t size=1024 p99 | 5.093 | ms |
| 16t size=1024 status_0 | 2795.000 | count |
| 16t size=1024 status_200 | 3569.000 | count |
| 16t size=1024 throughput | 635.524 | req/s |
| 16t size=256 errors | 6292.000 | count |
| 16t size=256 p50 | 2.297 | ms |
| 16t size=256 p95 | 4.060 | ms |
| 16t size=256 p99 | 5.091 | ms |
| 16t size=256 status_0 | 2791.000 | count |
| 16t size=256 status_200 | 3546.000 | count |
| 16t size=256 throughput | 632.828 | req/s |
| 16t size=64 errors | 6450.000 | count |
| 16t size=64 p50 | 2.311 | ms |
| 16t size=64 p95 | 4.010 | ms |
| 16t size=64 p99 | 5.067 | ms |
| 16t size=64 status_0 | 2911.000 | count |
| 16t size=64 status_200 | 3472.000 | count |
| 16t size=64 throughput | 637.422 | req/s |
| 16t size=8 errors | 6386.000 | count |
| 16t size=8 p50 | 2.304 | ms |
| 16t size=8 p95 | 4.069 | ms |
| 16t size=8 p99 | 4.934 | ms |
| 16t size=8 status_0 | 2839.000 | count |
| 16t size=8 status_200 | 3572.000 | count |
| 16t size=8 throughput | 640.218 | req/s |
| 1t size=1 errors | 16819.000 | count |
| 1t size=1 p50 | 0.088 | ms |
| 1t size=1 p95 | 0.121 | ms |
| 1t size=1 p99 | 0.161 | ms |
| 1t size=1 status_0 | 7543.000 | count |
| 1t size=1 status_200 | 9177.000 | count |
| 1t size=1 throughput | 1671.929 | req/s |
| 1t size=1024 errors | 17014.000 | count |
| 1t size=1024 p50 | 0.144 | ms |
| 1t size=1024 p95 | 0.194 | ms |
| 1t size=1024 p99 | 0.235 | ms |
| 1t size=1024 status_0 | 7660.000 | count |
| 1t size=1024 status_200 | 9195.000 | count |
| 1t size=1024 throughput | 1685.428 | req/s |
| 1t size=256 errors | 16933.000 | count |
| 1t size=256 p50 | 0.099 | ms |
| 1t size=256 p95 | 0.134 | ms |
| 1t size=256 p99 | 0.166 | ms |
| 1t size=256 status_0 | 7746.000 | count |
| 1t size=256 status_200 | 9224.000 | count |
| 1t size=256 throughput | 1696.928 | req/s |
| 1t size=64 errors | 16656.000 | count |
| 1t size=64 p50 | 0.091 | ms |
| 1t size=64 p95 | 0.124 | ms |
| 1t size=64 p99 | 0.161 | ms |
| 1t size=64 status_0 | 7603.000 | count |
| 1t size=64 status_200 | 9133.000 | count |
| 1t size=64 throughput | 1673.529 | req/s |
| 1t size=8 errors | 16687.000 | count |
| 1t size=8 p50 | 0.088 | ms |
| 1t size=8 p95 | 0.122 | ms |
| 1t size=8 p99 | 0.162 | ms |
| 1t size=8 status_0 | 7662.000 | count |
| 1t size=8 status_200 | 9234.000 | count |
| 1t size=8 throughput | 1689.528 | req/s |
| 4 threads errors | 53684.000 | count |
| 4 threads throughput | 5367.767 | req/s |
| 4t size=1 errors | 10832.000 | count |
| 4t size=1 p50 | 0.305 | ms |
| 4t size=1 p95 | 0.729 | ms |
| 4t size=1 p99 | 1.023 | ms |
| 4t size=1 status_0 | 4827.000 | count |
| 4t size=1 status_200 | 5941.000 | count |
| 4t size=1 throughput | 1076.553 | req/s |
| 4t size=1024 errors | 10729.000 | count |
| 4t size=1024 p50 | 0.359 | ms |
| 4t size=1024 p95 | 0.768 | ms |
| 4t size=1024 p99 | 1.020 | ms |
| 4t size=1024 status_0 | 4760.000 | count |
| 4t size=1024 status_200 | 5872.000 | count |
| 4t size=1024 throughput | 1062.956 | req/s |
| 4t size=256 errors | 10683.000 | count |
| 4t size=256 p50 | 0.320 | ms |
| 4t size=256 p95 | 0.747 | ms |
| 4t size=256 p99 | 1.023 | ms |
| 4t size=256 status_0 | 4749.000 | count |
| 4t size=256 status_200 | 6041.000 | count |
| 4t size=256 throughput | 1078.752 | req/s |
| 4t size=64 errors | 10688.000 | count |
| 4t size=64 p50 | 0.306 | ms |
| 4t size=64 p95 | 0.746 | ms |
| 4t size=64 p99 | 1.029 | ms |
| 4t size=64 status_0 | 4734.000 | count |
| 4t size=64 status_200 | 5935.000 | count |
| 4t size=64 throughput | 1066.655 | req/s |
| 4t size=8 errors | 10752.000 | count |
| 4t size=8 p50 | 0.302 | ms |
| 4t size=8 p95 | 0.753 | ms |
| 4t size=8 p99 | 1.012 | ms |
| 4t size=8 status_0 | 4821.000 | count |
| 4t size=8 status_200 | 6010.000 | count |
| 4t size=8 throughput | 1082.851 | req/s |
| 8 threads errors | 30705.000 | count |
| 8 threads throughput | 3068.794 | req/s |
| 8t size=1 errors | 6101.000 | count |
| 8t size=1 p50 | 1.194 | ms |
| 8t size=1 p95 | 2.130 | ms |
| 8t size=1 p99 | 2.682 | ms |
| 8t size=1 status_0 | 2676.000 | count |
| 8t size=1 status_200 | 3416.000 | count |
| 8t size=1 throughput | 608.782 | req/s |
| 8t size=1024 errors | 6066.000 | count |
| 8t size=1024 p50 | 1.198 | ms |
| 8t size=1024 p95 | 2.173 | ms |
| 8t size=1024 p99 | 2.766 | ms |
| 8t size=1024 status_0 | 2696.000 | count |
| 8t size=1024 status_200 | 3372.000 | count |
| 8t size=1024 throughput | 606.384 | req/s |
| 8t size=256 errors | 6078.000 | count |
| 8t size=256 p50 | 1.181 | ms |
| 8t size=256 p95 | 2.150 | ms |
| 8t size=256 p99 | 2.605 | ms |
| 8t size=256 status_0 | 2743.000 | count |
| 8t size=256 status_200 | 3416.000 | count |
| 8t size=256 throughput | 615.478 | req/s |
| 8t size=64 errors | 6171.000 | count |
| 8t size=64 p50 | 1.189 | ms |
| 8t size=64 p95 | 2.121 | ms |
| 8t size=64 p99 | 2.644 | ms |
| 8t size=64 status_0 | 2734.000 | count |
| 8t size=64 status_200 | 3443.000 | count |
| 8t size=64 throughput | 617.276 | req/s |
| 8t size=8 errors | 6289.000 | count |
| 8t size=8 p50 | 1.201 | ms |
| 8t size=8 p95 | 2.142 | ms |
| 8t size=8 p99 | 2.809 | ms |
| 8t size=8 status_0 | 2818.000 | count |
| 8t size=8 status_200 | 3395.000 | count |
| 8t size=8 throughput | 620.874 | req/s |
## Example: validation_api

| Benchmark | Value | Unit |
|-----------|-------|------|
| 4 threads success_rate | 49.992 | % success |
| 4 threads throughput | 5720.121 | req/s |
| 4t Invalid errors | 22995.000 | count |
| 4t Invalid p50 | 0.319 | ms |
| 4t Invalid p95 | 0.758 | ms |
| 4t Invalid p99 | 1.056 | ms |
| 4t Invalid status_0 | 11496.000 | count |
| 4t Invalid status_400 | 11434.000 | count |
| 4t Invalid throughput | 2292.527 | req/s |
| 4t Valid errors | 34203.000 | count |
| 4t Valid p50 | 0.317 | ms |
| 4t Valid p95 | 0.765 | ms |
| 4t Valid p99 | 1.062 | ms |
| 4t Valid status_0 | 17115.000 | count |
| 4t Valid status_200 | 17168.000 | count |
| 4t Valid throughput | 3427.593 | req/s |
| 8 threads success_rate | 50.020 | % success |
| 8 threads throughput | 3072.184 | req/s |
| 8t Invalid errors | 12318.000 | count |
| 8t Invalid p50 | 1.244 | ms |
| 8t Invalid p95 | 2.473 | ms |
| 8t Invalid p99 | 3.451 | ms |
| 8t Invalid status_0 | 6159.000 | count |
| 8t Invalid status_400 | 6189.000 | count |
| 8t Invalid throughput | 1234.151 | req/s |
| 8t Valid errors | 18407.000 | count |
| 8t Valid p50 | 1.247 | ms |
| 8t Valid p95 | 2.441 | ms |
| 8t Valid p99 | 3.361 | ms |
| 8t Valid status_0 | 9204.000 | count |
| 8t Valid status_200 | 9186.000 | count |
| 8t Valid throughput | 1838.033 | req/s |
## Generated API

| Benchmark | Value | Unit |
|-----------|-------|------|
| Generated API dispatch+parse - Duration | 144.000 | ms |
| Generated API dispatch+parse - Errors | 0.000 | count |
| Generated API dispatch+parse - Latency p50 | 0.550 | us |
| Generated API dispatch+parse - Latency p99 | 1.262 | us |
| Generated API dispatch+parse - Latency p999 | 7.973 | us |
| Generated API dispatch+parse - Operations | 200000.000 | ops |
| Generated API dispatch+parse - Throughput | 1388888.890 | ops/sec |
## HTTP Headers

| Benchmark | Value | Unit |
|-----------|-------|------|
| Case-Insensitive Compare - Duration | 25.000 | ms |
| Case-Insensitive Compare - Latency p50 | 0.019 | us |
| Case-Insensitive Compare - Latency p99 | 0.029 | us |
| Case-Insensitive Compare - Latency p999 | 0.029 | us |
| Case-Insensitive Compare - Operations | 500000.000 | ops |
| Case-Insensitive Compare - Throughput | 20000000.000 | ops/sec |
| Headers Get (3 lookups) - Duration | 10.000 | ms |
| Headers Get (3 lookups) - Latency p50 | 0.019 | us |
| Headers Get (3 lookups) - Latency p99 | 0.029 | us |
| Headers Get (3 lookups) - Latency p999 | 0.029 | us |
| Headers Get (3 lookups) - Operations | 200000.000 | ops |
| Headers Get (3 lookups) - Throughput | 20000000.000 | ops/sec |
| Headers Iteration (5 fields) - Duration | 5.000 | ms |
| Headers Iteration (5 fields) - Latency p50 | 0.019 | us |
| Headers Iteration (5 fields) - Latency p99 | 0.029 | us |
| Headers Iteration (5 fields) - Latency p999 | 0.029 | us |
| Headers Iteration (5 fields) - Operations | 100000.000 | ops |
| Headers Iteration (5 fields) - Throughput | 20000000.000 | ops/sec |
| Headers Set (4 custom fields) - Duration | 16.000 | ms |
| Headers Set (4 custom fields) - Latency p50 | 0.266 | us |
| Headers Set (4 custom fields) - Latency p99 | 0.437 | us |
| Headers Set (4 custom fields) - Latency p999 | 0.645 | us |
| Headers Set (4 custom fields) - Operations | 50000.000 | ops |
| Headers Set (4 custom fields) - Throughput | 3125000.000 | ops/sec |
| Headers Set (5 standard fields) - Duration | 18.000 | ms |
| Headers Set (5 standard fields) - Latency p50 | 0.114 | us |
| Headers Set (5 standard fields) - Latency p99 | 0.190 | us |
| Headers Set (5 standard fields) - Latency p999 | 0.275 | us |
| Headers Set (5 standard fields) - Operations | 100000.000 | ops |
| Headers Set (5 standard fields) - Throughput | 5555555.560 | ops/sec |
## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Large headers p50 | 0.088 | ms |
| Large headers p99 | 0.141 | ms |
| Large headers samples | 1500.000 | samples |
| Medium request p50 | 0.085 | ms |
| Medium request p99 | 0.131 | ms |
| Medium request samples | 1500.000 | samples |
| Minimal request p50 | 0.080 | ms |
| Minimal request p99 | 0.121 | ms |
| Minimal request samples | 1500.000 | samples |
## IO Buffer

| Benchmark | Value | Unit |
|-----------|-------|------|
| IO Buffer Append (4KB) - Duration | 4.000 | ms |
| IO Buffer Append (4KB) - Latency p50 | 0.057 | us |
| IO Buffer Append (4KB) - Latency p99 | 0.076 | us |
| IO Buffer Append (4KB) - Latency p999 | 0.132 | us |
| IO Buffer Append (4KB) - Operations | 50000.000 | ops |
| IO Buffer Append (4KB) - Throughput | 12500000.000 | ops/sec |
| IO Buffer Append (64 bytes) - Duration | 5.000 | ms |
| IO Buffer Append (64 bytes) - Latency p50 | 0.028 | us |
| IO Buffer Append (64 bytes) - Latency p99 | 0.038 | us |
| IO Buffer Append (64 bytes) - Latency p999 | 0.056 | us |
| IO Buffer Append (64 bytes) - Operations | 100000.000 | ops |
| IO Buffer Append (64 bytes) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Read/Write (256B) - Duration | 5.000 | ms |
| IO Buffer Read/Write (256B) - Latency p50 | 0.028 | us |
| IO Buffer Read/Write (256B) - Latency p99 | 0.038 | us |
| IO Buffer Read/Write (256B) - Latency p999 | 0.066 | us |
| IO Buffer Read/Write (256B) - Operations | 100000.000 | ops |
| IO Buffer Read/Write (256B) - Throughput | 20000000.000 | ops/sec |
| IO Buffer Writable/Commit (128B) - Duration | 4.000 | ms |
| IO Buffer Writable/Commit (128B) - Latency p50 | 0.019 | us |
| IO Buffer Writable/Commit (128B) - Latency p99 | 0.029 | us |
| IO Buffer Writable/Commit (128B) - Latency p999 | 0.038 | us |
| IO Buffer Writable/Commit (128B) - Operations | 100000.000 | ops |
| IO Buffer Writable/Commit (128B) - Throughput | 25000000.000 | ops/sec |
| Scatter/Gather Write (3 buffers) - Duration | 6.000 | ms |
| Scatter/Gather Write (3 buffers) - Latency p50 | 0.038 | us |
| Scatter/Gather Write (3 buffers) - Latency p99 | 0.066 | us |
| Scatter/Gather Write (3 buffers) - Latency p999 | 0.076 | us |
| Scatter/Gather Write (3 buffers) - Operations | 100000.000 | ops |
| Scatter/Gather Write (3 buffers) - Throughput | 16666666.670 | ops/sec |
## MPSC Queue

| Benchmark | Value | Unit |
|-----------|-------|------|
| MPSC Queue (2 Producers) - Duration | 66.000 | ms |
| MPSC Queue (2 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (2 Producers) - Throughput | 15151515.150 | ops/sec |
| MPSC Queue (4 Producers) - Duration | 61.000 | ms |
| MPSC Queue (4 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (4 Producers) - Throughput | 16393442.620 | ops/sec |
| MPSC Queue (8 Producers) - Duration | 51.000 | ms |
| MPSC Queue (8 Producers) - Operations | 1000000.000 | ops |
| MPSC Queue (8 Producers) - Throughput | 19607843.140 | ops/sec |
| MPSC Queue (Bounded 1024) - Duration | 29.000 | ms |
| MPSC Queue (Bounded 1024) - Operations | 500000.000 | ops |
| MPSC Queue (Bounded 1024) - Throughput | 17241379.310 | ops/sec |
| MPSC Queue (Single Producer) - Duration | 58.000 | ms |
| MPSC Queue (Single Producer) - Latency p50 | 0.028 | us |
| MPSC Queue (Single Producer) - Latency p99 | 0.038 | us |
| MPSC Queue (Single Producer) - Latency p999 | 0.057 | us |
| MPSC Queue (Single Producer) - Operations | 1000000.000 | ops |
| MPSC Queue (Single Producer) - Throughput | 17241379.310 | ops/sec |
## Router Dispatch

| Benchmark | Value | Unit |
|-----------|-------|------|
| Router dispatch (405) - Duration | 226.000 | ms |
| Router dispatch (405) - Errors | 166667.000 | count |
| Router dispatch (405) - Latency p50 | 0.730 | us |
| Router dispatch (405) - Latency p99 | 1.537 | us |
| Router dispatch (405) - Latency p999 | 5.714 | us |
| Router dispatch (405) - Operations | 200000.000 | ops |
| Router dispatch (405) - Throughput | 884955.750 | ops/sec |
| Router dispatch (hits) - Duration | 193.000 | ms |
| Router dispatch (hits) - Errors | 33333.000 | count |
| Router dispatch (hits) - Latency p50 | 0.522 | us |
| Router dispatch (hits) - Latency p99 | 1.035 | us |
| Router dispatch (hits) - Latency p999 | 3.133 | us |
| Router dispatch (hits) - Operations | 200000.000 | ops |
| Router dispatch (hits) - Throughput | 1036269.430 | ops/sec |
| Router dispatch (not found) - Duration | 185.000 | ms |
| Router dispatch (not found) - Errors | 200000.000 | count |
| Router dispatch (not found) - Latency p50 | 0.503 | us |
| Router dispatch (not found) - Latency p99 | 1.053 | us |
| Router dispatch (not found) - Latency p999 | 2.810 | us |
| Router dispatch (not found) - Operations | 200000.000 | ops |
| Router dispatch (not found) - Throughput | 1081081.080 | ops/sec |
## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| 128 concurrent connections | 262348.800 | req/s |
| 256 concurrent connections | 261286.400 | req/s |
| 32 concurrent connections | 248869.200 | req/s |
| 64 concurrent connections | 254415.600 | req/s |
| Throughput with 1 threads | 11801.000 | req/s |
| Throughput with 4 threads | 44484.500 | req/s |
| Throughput with 8 threads | 240454.500 | req/s |
## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 40963.940 | req/s |
| Total requests | 204830.000 | requests |
## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD hard limit | 1048576.000 | fds |
| FD soft limit | 1048576.000 | fds |
## Timer System

| Benchmark | Value | Unit |
|-----------|-------|------|
| Wheel Timer (Add Operations) - Duration | 17.000 | ms |
| Wheel Timer (Add Operations) - Latency p50 | 0.029 | us |
| Wheel Timer (Add Operations) - Latency p99 | 2.202 | us |
| Wheel Timer (Add Operations) - Latency p999 | 6.502 | us |
| Wheel Timer (Add Operations) - Operations | 100000.000 | ops |
| Wheel Timer (Add Operations) - Throughput | 5882352.940 | ops/sec |
| Wheel Timer (Cancel Operations) - Duration | 2.000 | ms |
| Wheel Timer (Cancel Operations) - Latency p50 | 0.019 | us |
| Wheel Timer (Cancel Operations) - Latency p99 | 0.029 | us |
| Wheel Timer (Cancel Operations) - Latency p999 | 0.143 | us |
| Wheel Timer (Cancel Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Cancel Operations) - Throughput | 25000000.000 | ops/sec |
| Wheel Timer (Execution 10k) - Duration | 312.000 | ms |
| Wheel Timer (Execution 10k) - Operations | 10000.000 | ops |
| Wheel Timer (Execution 10k) - Throughput | 32051.280 | ops/sec |
| Wheel Timer (Tick Operations) - Duration | 3.000 | ms |
| Wheel Timer (Tick Operations) - Latency p50 | 0.038 | us |
| Wheel Timer (Tick Operations) - Latency p99 | 0.048 | us |
| Wheel Timer (Tick Operations) - Latency p999 | 0.048 | us |
| Wheel Timer (Tick Operations) - Operations | 50000.000 | ops |
| Wheel Timer (Tick Operations) - Throughput | 16666666.670 | ops/sec |
