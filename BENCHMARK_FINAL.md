# KATANA Framework - Benchmark Results

Generated: 2025-11-08 15:30:34

**Note**: Measurements use time-boxed phases with warm-ups, steady-state sampling, and full response validation.


## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Latency samples | 12362.000 | samples |
| Latency avg | 1.024 | ms |
| Latency p50 | 1.024 | ms |
| Latency p90 | 1.268 | ms |
| Latency p95 | 1.384 | ms |
| Latency p99 | 1.942 | ms |
| Latency p999 | 2.650 | ms |
| Latency IQR | 0.234 | ms |
| Latency max | 3.084 | ms |
| Keep-alive throughput | 3496.305 | req/s |
| Keep-alive success | 2501.000 | requests |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Minimal request samples | 1500.000 | samples |
| Minimal request p50 | 0.118 | ms |
| Minimal request p99 | 0.300 | ms |
| Medium request samples | 1500.000 | samples |
| Medium request p50 | 0.127 | ms |
| Medium request p99 | 0.352 | ms |
| Large headers samples | 1500.000 | samples |
| Large headers p50 | 0.137 | ms |
| Large headers p99 | 0.377 | ms |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Throughput with 1 threads | 3302.500 | req/s |
| Throughput with 4 threads | 5428.500 | req/s |
| Throughput with 8 threads | 5796.000 | req/s |
| 32 concurrent connections | 5984.400 | req/s |
| 64 concurrent connections | 7219.200 | req/s |
| 128 concurrent connections | 6532.400 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 1667.113 | req/s |
| Total requests | 8338.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD soft limit | 20000.000 | fds |
| FD hard limit | 20000.000 | fds |
