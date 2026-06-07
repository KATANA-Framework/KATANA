# Compute API (codegen)

A minimal but loaded example: POST `/compute/sum` takes a JSON array of numbers and returns the sum. No blocking and no I/O — just deserialization, validation and CPU logic.

## What it demonstrates
- DTO `array<double>` + arena allocators
- Zero-copy JSON → arena
- `minItems/maxItems` validation from OpenAPI
- Handler with no virtual hop in the hot path (`make_router` inlines the calls)
- Streaming JSON writer for the response

## Build and run
```bash
cmake --preset examples
cmake --build --preset examples --target compute_api
./build/examples/examples/codegen/compute_api/compute_api  # PORT=8080 by default
```

## Example request
```bash
curl -X POST http://localhost:8080/compute/sum \
  -H 'Content-Type: application/json' \
  -d '[1, 2, 3, 4.5]'
# => 10.5
```

## Load testing
Recommended scenarios (5 s warmup → 5 s load):
- Array sizes: 1, 8, 64, 256, 1024
- Threads: 1, 4, 8, 16

Baseline expectations on a typical x86:
- p50: 0.03–0.05 ms
- p95: 0.10–0.20 ms
- p99: 0.18–0.35 ms
- 150k–350k rps (depends on array size)

These scenarios are already integrated into `generate_benchmark_report.py` and the docker benchmark.
