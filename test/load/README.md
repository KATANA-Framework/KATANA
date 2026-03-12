# Load Testing

This directory contains load testing scripts and configurations for KATANA. The maintained entrypoint is `scripts/run_benchmarks.py`; files here are the `wrk` Lua payloads used by the E2E stages.

## Tools

We recommend using one of the following tools:
- **wrk** - Modern HTTP benchmarking tool
- **bombardier** - Fast cross-platform HTTP benchmarking tool
- **hey** - HTTP load generator

## Installation

### wrk (Linux/macOS)
```bash
git clone https://github.com/wg/wrk.git
cd wrk
make
sudo cp wrk /usr/local/bin/
```

### bombardier
```bash
go install github.com/codesenberg/bombardier@latest
```

### hey
```bash
go install github.com/rakyll/hey@latest
```

## Running Load Tests

### Recommended: Unified Runner

```bash
# Build benchmark layout
cmake --preset bench
cmake --build --preset bench

# Run microbenchmarks + maintained E2E wrk stages
python3 scripts/run_benchmarks.py --build-dir build/bench --include-e2e

# WSL example
python3 scripts/run_benchmarks.py --build-dir build/bench-wsl --no-build --include-e2e
```

Current maintained E2E stages:
- `hello_world_server` canonical pipeline: `w4/t4/c512/depth10`
- `compute_api` canonical pipeline: `w4/t4/c512/depth10`
- `hello_world_server` peak pipeline: `w4/t4/c512/depth20`
- `compute_api` peak pipeline: `w4/t4/c512/depth40`

### Basic HTTP Server Test

First, start the server:
```bash
./hello_world_server
```

#### Using wrk
```bash
# Simple GET test
wrk -t4 -c256 -d10s http://localhost:8080/

# With latency distribution
wrk -t4 -c256 -d10s --latency http://localhost:8080/

# Maintained pipeline profile
KATANA_PIPELINE_DEPTH=10 wrk -t4 -c512 -d10s --latency -s scripts/hello_pipeline.lua http://localhost:8080/
```

#### Using bombardier
```bash
# Simple test - 125 connections, 10 seconds
bombardier -c 125 -d 10s http://localhost:8080/

# With latency histogram
bombardier -c 125 -d 10s --print r --format json http://localhost:8080/ > results.json

# Custom rate limiting
bombardier -c 125 -r 10000 -d 10s http://localhost:8080/
```

#### Using hey
```bash
# Simple test - 200 workers, 10000 requests
hey -n 10000 -c 200 http://localhost:8080/

# With custom headers
hey -n 10000 -c 200 -H "User-Agent: LoadTest" http://localhost:8080/

# POST request
hey -n 1000 -c 50 -m POST -d '{"test":"data"}' http://localhost:8080/
```

### REST API Test

Start the REST API server:
```bash
./simple_rest_api
```

Then run:
```bash
# Compute API canonical pipeline
KATANA_PIPELINE_DEPTH=10 wrk -t4 -c512 -d10s --latency -s scripts/compute_sum_pipeline.lua http://localhost:8080/

# Compute API non-pipelined request script
wrk -t4 -c256 -d10s --latency -s scripts/compute_sum.lua http://localhost:8080/
```

## Analyzing Results

### wrk Output
```
Running 30s test @ http://localhost:8080/
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.15ms    1.23ms  45.67ms   89.23%
    Req/Sec    15.23k     2.34k   23.45k    78.90%
  Latency Distribution
     50%    1.98ms
     75%    2.34ms
     90%    3.12ms
     99%    6.78ms
  5467890 requests in 30.00s, 1.23GB read
Requests/sec: 182263.00
Transfer/sec:     42.15MB
```

### bombardier Output (JSON)
```json
{
  "spec": { "numberOfConnections": 125, "testDuration": 10 },
  "result": {
    "rps": { "mean": 182000, "stddev": 5000 },
    "latencies": {
      "mean": 685.2,
      "stddev": 123.4,
      "max": 12345,
      "percentiles": {
        "50": 650,
        "75": 780,
        "90": 890,
        "95": 1020,
        "99": 1450
      }
    }
  }
}
```

## Performance Targets

| Metric | Target | Notes |
|--------|--------|-------|
| Throughput | profile-dependent | Use canonical and peak pipeline stages from `scripts/run_benchmarks.py` |
| Avg/P50 Latency | profile-dependent | For pipeline scripts this is batch latency per `request()` call |
| P99 Latency | watch for regressions | Compare against committed benchmark reports, not hardcoded generic numbers |
| Error Rate | 0 | Canonical and peak E2E stages should stay error-free |
| Max Connections | > 10k | Concurrent connections |

## Custom Lua Scripts

Maintained scripts:
- `scripts/hello_get.lua` and `scripts/compute_sum.lua` for non-pipelined probes
- `scripts/hello_pipeline.lua` and `scripts/compute_sum_pipeline.lua` for benchmark pipeline stages

Legacy ad hoc scripts may still exist for manual experimentation, but the benchmark runner is the source of truth.

## Monitoring

Use system tools to monitor during load tests:
```bash
# CPU and memory
top -p $(pgrep hello_world_server)

# Network stats
ss -s

# File descriptors
lsof -p $(pgrep hello_world_server) | wc -l
```
