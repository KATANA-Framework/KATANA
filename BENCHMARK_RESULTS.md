# KATANA Framework - Benchmark Results

Generated: 2025-11-08 18:27:33

**Note**: Measurements use time-boxed phases with warm-ups, steady-state sampling, and full response validation.


## Core Performance

| Benchmark | Value | Unit |
|-----------|-------|------|
| Latency samples | 61539.000 | samples |
| Latency avg | 0.235 | ms |
| Latency p50 | 0.144 | ms |
| Latency p90 | 0.355 | ms |
| Latency p95 | 0.473 | ms |
| Latency p99 | 1.118 | ms |
| Latency p999 | 21.960 | ms |
| Latency IQR | 0.192 | ms |
| Latency max | 31.501 | ms |
| Keep-alive throughput | 14568.992 | req/s |
| Keep-alive success | 2794.000 | requests |

## HTTP Parsing

| Benchmark | Value | Unit |
|-----------|-------|------|
| Minimal request samples | 1500.000 | samples |
| Minimal request p50 | 0.036 | ms |
| Minimal request p99 | 0.100 | ms |
| Medium request samples | 1500.000 | samples |
| Medium request p50 | 0.036 | ms |
| Medium request p99 | 0.092 | ms |
| Large headers samples | 1500.000 | samples |
| Large headers p50 | 0.038 | ms |
| Large headers p99 | 0.080 | ms |

## Scalability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Throughput with 1 threads | 13959.500 | req/s |
| Throughput with 4 threads | 31007.000 | req/s |
| Throughput with 8 threads | 31985.500 | req/s |
| 32 concurrent connections | 46344.800 | req/s |
| 64 concurrent connections | 46807.200 | req/s |
| 128 concurrent connections | 49970.800 | req/s |

## Stability

| Benchmark | Value | Unit |
|-----------|-------|------|
| Sustained throughput | 4479.190 | req/s |
| Total requests | 22401.000 | requests |

## System Configuration

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD soft limit | 1048576.000 | fds |
| FD hard limit | 1048576.000 | fds |
