# KATANA Framework - Benchmark Results

Generated: 2025-11-08 15:30:34

**Note**: Measurements use time-boxed phases with warm-ups, steady-state sampling, and full response validation.

Core Performance
| Benchmark | Value | Unit | |-----------|-------|------| | Latency samples | 68010.000 | samples | | Latency avg | 0.161 | ms | | Latency p50 | 0.150 | ms | | Latency p90 | 0.248 | ms | | Latency p95 | 0.286 | ms | | Latency p99 | 0.380 | ms | | Latency p999 | 0.583 | ms | | Latency IQR | 0.086 | ms | | Latency max | 1.872 | ms | | Keep-alive throughput | 5132.279 | req/s | | Keep-alive success | 2502.000 | requests |

HTTP Parsing
| Benchmark | Value | Unit | |-----------|-------|------| | Minimal request samples | 1500.000 | samples | | Minimal request p50 | 0.107 | ms | | Minimal request p99 | 0.168 | ms | | Medium request samples | 1500.000 | samples | | Medium request p50 | 0.106 | ms | | Medium request p99 | 0.177 | ms | | Large headers samples | 1500.000 | samples | | Large headers p50 | 0.112 | ms | | Large headers p99 | 0.205 | ms |

Scalability
| Benchmark | Value | Unit | |-----------|-------|------| | Throughput with 1 threads | 4937.500 | req/s | | Throughput with 4 threads | 19331.500 | req/s | | Throughput with 8 threads | 32952.500 | req/s | | 32 concurrent connections | 53755.200 | req/s | | 64 concurrent connections | 94526.000 | req/s | | 128 concurrent connections | 125090.400 | req/s |

Stability
| Benchmark | Value | Unit | |-----------|-------|------| | Sustained throughput | 3236.726 | req/s | | Total requests | 16187.000 | requests |

System Configuration
| Benchmark | Value | Unit | |-----------|-------|------| | FD soft limit | 10240.000 | fds | | FD hard limit | 1048576.000 | fds |

