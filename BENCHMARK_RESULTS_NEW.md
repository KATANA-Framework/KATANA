# KATANA Framework - Benchmark Results

Generated: 2025-11-08 15:15:51

**Note**: Measurements use time-boxed phases with warm-ups, steady-state sampling, and full response validation.


## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Latency samples | 15117.000 | samples |
| Latency avg | 0.774 | ms |
| Latency p50 | 0.773 | ms |
| Latency p90 | 1.282 | ms |
| Latency p95 | 1.427 | ms |
| Latency p99 | 1.752 | ms |
| Latency p999 | 2.366 | ms |
| Latency IQR | 0.572 | ms |
| Latency max | 3.100 | ms |
| Keep-alive throughput | 3059.465 | req/s |
| Keep-alive success | 2502.000 | requests |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Minimal request samples | 1500.000 | samples |
| Minimal request p50 | 0.187 | ms |
| Minimal request p99 | 0.320 | ms |
| Medium request samples | 1500.000 | samples |
| Medium request p50 | 0.196 | ms |
| Medium request p99 | 0.343 | ms |
| Large headers samples | 1500.000 | samples |
| Large headers p50 | 0.198 | ms |
| Large headers p99 | 0.348 | ms |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Throughput with 1 threads | 2618.500 | req/s |
| Throughput with 4 threads | 5586.500 | req/s |
| Throughput with 8 threads | 6943.500 | req/s |
| 32 concurrent connections | 7310.000 | req/s |
| 64 concurrent connections | 7180.400 | req/s |
| 128 concurrent connections | 7361.600 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 1901.190 | req/s |
| Total requests | 9509.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD soft limit | 20000.000 | fds |
| FD hard limit | 20000.000 | fds |
