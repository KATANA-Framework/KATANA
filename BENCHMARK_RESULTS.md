# KATANA Framework - Benchmark Results

Generated: 2025-11-08 03:05:04

**Note**: These benchmarks were run after fixing critical timeout management bottleneck.

## Core Performance

| Benchmark | Value | Unit | Notes |
|-----------|-------|------|-------|
| Latency p50 | 0.59 | ms | Under load |
| Latency p95 | 1.34 | ms | Under load |
| Latency p99 | 1.34 | ms | Under load |
| Latency p999 | 1.34 | ms | Under load |
| Keep-alive throughput | 531,572 | req/s | Single connection |
| Parsing minimal p50 | 0.30 | ms | |
| Parsing medium p50 | 0.13 | ms | |
| Parsing large headers p50 | 0.13 | ms | |

## Scalability

| Benchmark | Value | Unit | Notes |
|-----------|-------|------|-------|
| Plaintext throughput (1 reactor) | 267 | req/s | |
| Plaintext throughput (2 reactors) | 413 | req/s | |
| Plaintext throughput (4 reactors) | 574 | req/s | |
| Plaintext throughput (8 reactors) | 558 | req/s | |
| 100 concurrent connections | 2,938 | req/s | |
| 1000 concurrent connections | 1,770 | req/s | |
| Connection accept latency p50 | 0.27 | ms | |
| Connection accept latency p99 | 0.62 | ms | |

## Network I/O

| Benchmark | Value | Unit | Notes |
|-----------|-------|------|-------|
| TCP_NODELAY enabled p50 | 0.16 | ms | |
| TCP_NODELAY disabled p50 | 0.15 | ms | |
| epoll latency p50 | 0.33 | ms | 4 threads |
| epoll latency p99 | 0.46 | ms | 4 threads |

## HTTP Protocol

| Benchmark | Value | Unit | Notes |
|-----------|-------|------|-------|
| Pipelining 3 requests | 0.71 | ms | |
| Large headers p50 | 0.29 | ms | 50 headers |
| Large headers p99 | 0.48 | ms | 50 headers |

## System Limits

| Benchmark | Value | Unit |
|-----------|-------|------|
| FD soft limit | 20,000 | fds |
| FD hard limit | 20,000 | fds |

## Performance Improvement Summary

After fixing the timeout management bottleneck:
- **Keep-alive throughput**: 117 req/s → 531,572 req/s (+452,500%)
- **100 concurrent connections**: N/A → 2,938 req/s
- **1000 concurrent connections**: N/A → 1,770 req/s
- **Latency p50**: 0.65ms → 0.59ms (improved)

The main bottleneck was O(n) cancel/recreate timer operations on every socket event.
Changed to lazy timeout checking with timestamp updates only.
