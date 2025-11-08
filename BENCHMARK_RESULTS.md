# KATANA Framework - Benchmark Results

Generated: 2025-11-08 14:01:25

**Note**: Measurements use time-boxed phases with warm-ups, steady-state sampling, and full response validation.

_Run executed at 17:00 MSK (container clock 2025-11-08 14:01:25 UTC)._ 


## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Latency samples | 27239.000 | samples |
| Latency avg | 0.541 | ms |
| Latency p50 | 0.502 | ms |
| Latency p90 | 0.733 | ms |
| Latency p95 | 0.813 | ms |
| Latency p99 | 1.090 | ms |
| Latency p999 | 7.404 | ms |
| Latency IQR | 0.226 | ms |
| Latency max | 35.202 | ms |
| Keep-alive throughput | 1936.368 | req/s |
| Keep-alive success | 2609.000 | requests |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Minimal request samples | 1500.000 | samples |
| Minimal request p50 | 0.462 | ms |
| Minimal request p99 | 0.807 | ms |
| Medium request samples | 1500.000 | samples |
| Medium request p50 | 0.514 | ms |
| Medium request p99 | 0.796 | ms |
| Large headers samples | 1500.000 | samples |
| Large headers p50 | 0.502 | ms |
| Large headers p99 | 0.736 | ms |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Throughput with 1 threads | 1468.500 | req/s |
| Throughput with 4 threads | 4082.000 | req/s |
| Throughput with 8 threads | 7193.500 | req/s |
| 32 concurrent connections | 13616.000 | req/s |
| 64 concurrent connections | 11325.200 | req/s |
| 128 concurrent connections | 14488.400 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 867.279 | req/s |
| Total requests | 4337.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD soft limit | 1048576.000 | fds |
| FD hard limit | 1048576.000 | fds |
