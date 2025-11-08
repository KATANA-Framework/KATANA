# Load Testing

This directory contains load testing scripts and configurations for KATANA.

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

### Basic HTTP Server Test

First, start the server:
```bash
./hello_world_server
```

#### Using wrk
```bash
# Simple test - 12 threads, 400 connections, 30 seconds
wrk -t12 -c400 -d30s http://localhost:8080/

# With latency distribution
wrk -t12 -c400 -d30s --latency http://localhost:8080/

# Using Lua script for POST requests
wrk -t12 -c400 -d30s -s scripts/post.lua http://localhost:8080/
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
# GET all users
wrk -t4 -c100 -d10s http://localhost:8080/api/users

# Health check endpoint
hey -n 10000 -c 200 http://localhost:8080/api/health

# Mixed workload with Lua script
wrk -t4 -c100 -d10s -s scripts/rest_api.lua http://localhost:8080/
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
| Throughput | > 100k req/s | Single-threaded on modern CPU |
| P50 Latency | < 1ms | Under normal load |
| P99 Latency | < 5ms | Under normal load |
| P99.9 Latency | < 20ms | Under normal load |
| Max Connections | > 10k | Concurrent connections |

## Custom Lua Scripts

See the `scripts/` directory for example Lua scripts for wrk.

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
