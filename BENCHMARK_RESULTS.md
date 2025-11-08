# KATANA Framework - Benchmark Results

Generated: 2025-11-08 22:05:18

**Note**: Measurements use time-boxed phases with warm-ups, steady-state sampling, and full response validation.


## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Latency samples | 18836.000 | samples |
| Latency avg | 0.497 | ms |
| Latency p50 | 0.476 | ms |
| Latency p90 | 0.758 | ms |
| Latency p95 | 0.864 | ms |
| Latency p99 | 1.111 | ms |
| Latency p999 | 1.479 | ms |
| Latency IQR | 0.257 | ms |
| Latency max | 1.941 | ms |
| Keep-alive throughput | 3183.509 | req/s |
| Keep-alive success | 2501.000 | requests |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Minimal request samples | 1500.000 | samples |
| Minimal request p50 | 0.124 | ms |
| Minimal request p99 | 0.237 | ms |
| Medium request samples | 1500.000 | samples |
| Medium request p50 | 0.124 | ms |
| Medium request p99 | 0.229 | ms |
| Large headers samples | 1500.000 | samples |
| Large headers p50 | 0.121 | ms |
| Large headers p99 | 0.239 | ms |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Throughput with 1 threads | 3509.000 | req/s |
| Throughput with 4 threads | 6529.000 | req/s |
| Throughput with 8 threads | 8340.000 | req/s |
| 32 concurrent connections | 8888.400 | req/s |
| 64 concurrent connections | 9984.800 | req/s |
| 128 concurrent connections | 10320.000 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 1920.066 | req/s |
| Total requests | 9603.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD soft limit | 20000.000 | fds |
| FD hard limit | 20000.000 | fds |
