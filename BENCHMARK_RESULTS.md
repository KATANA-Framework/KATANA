# KATANA Framework — Performance Benchmarks

> Test Environment: WSL2 Ubuntu on Windows 11  
> CPU Cores: 8 cores  
> Target: 127.0.0.1:8080  
> Date: 9 November 2025

---

### Core performance

| Metric | Value |
|---|---|
| Samples | 363,953 |
| Avg | 0.044 ms |
| p50 | 0.031 ms |
| p90 | 0.084 ms |
| p95 | 0.103 ms |
| p99 | 0.146 ms |
| p999 | 0.237 ms |
| IQR | 0.023 ms |
| Max | 5.878 ms |

#### Keep-alive
| Metric | Value |
|---|---|
| Throughput | 10,291 req/s |
| Success | 4,996 requests |

---

### HTTP parsing

| Request | Samples | p50 | p99 |
|---|---:|---:|---:|
| Minimal | 1,500 | 0.088 ms | 0.158 ms |
| Medium | 1,500 | 0.090 ms | 0.173 ms |
| Large headers | 1,500 | 0.084 ms | 0.205 ms |

---

### Scalability

#### Threads
| Threads | Throughput |
|---:|---:|
| 1 | 10,895 req/s |
| 4 | 40,094 req/s |
| 8 | 188,877 req/s |

- 4× threads → 3.68× throughput  
- 8× threads → 17.34× throughput

#### Connections
| Concurrency | Throughput |
|---:|---:|
| 32 | 253,583 req/s |
| 64 | 265,173 req/s |
| 128 | 277,932 req/s |

---

### Stability

| Metric | Value |
|---|---|
| Sustained throughput | 39,014 req/s |
| Total requests | 195,078 |

---

### Component benchmarks

#### Data structures
| Component | Ops | Duration | Throughput | p50 | p99 |
|---|---:|---:|---:|---:|---:|
| Ring buffer (single) | 1,000,000 | 55 ms | 18.2M ops/s | 0.028 μs | 0.029 μs |
| Ring buffer (4×4) | 1,000,000 | 110 ms | 9.1M ops/s | 0.000 μs | 0.000 μs |
| Circular buffer | 500,000 | 25 ms | 20.0M ops/s | 0.019 μs | 0.029 μs |

#### SIMD
| Component | Buffer | Ops | Throughput | p50 | p99 |
|---|---:|---:|---:|---:|---:|
| CRLF search (SIMD) | 1.5 KB | 100,000 | 20.0M ops/s | 0.028 μs | 0.038 μs |

#### HTTP
| Component | Ops | Duration | Throughput | p50 | p99 | p999 |
|---|---:|---:|---:|---:|---:|---:|
| Parser (complete request) | 50,000 | 70 ms | 714K ops/s | 1.234 μs | 2.059 μs | 11.305 μs |

#### Memory
| Component | Ops | Duration | Throughput |
|---|---:|---:|---:|
| String queue allocations | 100,000 | 2 ms | 50.0M ops/s |

---

### System configuration

| Parameter | Value |
|---|---|
| FD soft limit | 10,240 |
| FD hard limit | 1,048,576 |
| Hardware threads | 8 |

---

### Highlights

- Субмиллисекундные задержки: p50 0.031 ms, p99 0.146 ms
- Масштабирование по потокам до ~17× на 8 потоках
- Пиковая пропускная способность: 277,932 req/s при 128 соединениях
- SIMD CRLF: 20M ops/s; Circular buffer: 20M ops/s
- Стабильность под нагрузкой: 39K req/s, 195K запросов за прогон
