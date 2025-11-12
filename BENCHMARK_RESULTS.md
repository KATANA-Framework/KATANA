# KATANA Framework - Benchmark Results

Generated: 2025-11-12 18:48:49

**Note**: Measurements use time-boxed phases with warm-ups, steady-state sampling, and full response validation.


## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Latency samples | 433468.000 | samples |
| Latency avg | 0.037 | ms |
| Latency p50 | 0.027 | ms |
| Latency p90 | 0.061 | ms |
| Latency p95 | 0.086 | ms |
| Latency p99 | 0.152 | ms |
| Latency p999 | 0.379 | ms |
| Latency IQR | 0.014 | ms |
| Latency max | 4.892 | ms |
| Keep-alive throughput | 11748.559 | req/s |
| Keep-alive success | 4996.000 | requests |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Minimal request samples | 1500.000 | samples |
| Minimal request p50 | 0.080 | ms |
| Minimal request p99 | 0.116 | ms |
| Medium request samples | 1500.000 | samples |
| Medium request p50 | 0.080 | ms |
| Medium request p99 | 0.120 | ms |
| Large headers samples | 1500.000 | samples |
| Large headers p50 | 0.097 | ms |
| Large headers p99 | 0.579 | ms |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Throughput with 1 threads | 11924.500 | req/s |
| Throughput with 4 threads | 42776.500 | req/s |
| Throughput with 8 threads | 128483.000 | req/s |
| 32 concurrent connections | 274285.200 | req/s |
| 64 concurrent connections | 305845.200 | req/s |
| 128 concurrent connections | 278592.400 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 40660.372 | req/s |
| Total requests | 203312.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD soft limit | 10240.000 | fds |
| FD hard limit | 1048576.000 | fds |
