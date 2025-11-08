# KATANA Benchmarks

## Building Benchmarks

```bash
cmake -B build -DENABLE_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Built-in Benchmarks

### latency_benchmark
Original latency benchmark with keep-alive support.

```bash
./build/benchmark/latency_benchmark [requests] [threads]
./build/benchmark/latency_benchmark 10000 10
```

### http_load_benchmark
Advanced benchmark with latency histograms and detailed statistics.

```bash
./build/benchmark/http_load_benchmark [requests] [threads] [keep-alive]
./build/benchmark/http_load_benchmark 100000 10 1
```

**Output includes:**
- Min/Max/Avg/Stddev latencies
- Percentiles: p50, p75, p90, p95, p99, p99.9
- Latency histogram
- Throughput (req/s)
- Success/failure rates

**Example:**

```bash
# Start server
./build/raii_http_server 8080

# Run benchmark
./build/benchmark/http_load_benchmark 100000 10 1
```

## External Load Testing Tools

### wrk

```bash
./benchmark/run_wrk.sh [host] [port] [duration] [threads] [connections]
./benchmark/run_wrk.sh 127.0.0.1 8080 30s 4 100
```

### bombardier

```bash
./benchmark/run_bombardier.sh [host] [port] [duration] [connections]
./benchmark/run_bombardier.sh 127.0.0.1 8080 30s 100
```

### hey

```bash
./benchmark/run_hey.sh [host] [port] [requests] [concurrency]
./benchmark/run_hey.sh 127.0.0.1 8080 100000 100
```

### REST API Load Test

```bash
./benchmark/run_rest_api_test.sh [host] [port]
./benchmark/run_rest_api_test.sh 127.0.0.1 8080
```

## Performance Targets

**Stage 1 Requirements:**
- p99 latency < 2.0 ms
- Single-threaded throughput > 50k req/s
- Multi-threaded linear scaling

## Installing External Tools

**wrk:**
```bash
sudo apt-get install wrk
```

**bombardier:**
```bash
go install github.com/codesenberg/bombardier@latest
```

**hey:**
```bash
go install github.com/rakyll/hey@latest
```
