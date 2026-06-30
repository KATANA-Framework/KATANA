<div align="center">

# KATANA

**A C++ server framework for high-load APIs with tight tail-latency budgets.**

Contract-first: you write SQL and an OpenAPI spec, the generator produces the typed
C++ in between. Zero-allocation hot path, epoll/io_uring reactor, and an HTTP/1.1
stack built for predictable p99/p999 under load.

[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/23)
[![Build: CMake](https://img.shields.io/badge/build-CMake%20%2B%20Ninja-064F8C?logo=cmake)](CMakePresets.json)
[![License: Apache 2.0](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](LICENSE)
[![Platform: Linux](https://img.shields.io/badge/platform-Linux-333?logo=linux)](#getting-started)
[![PostgreSQL](https://img.shields.io/badge/SQL-PostgreSQL-336791?logo=postgresql&logoColor=white)](docs/WRITING_AN_APP.md)

[Quick start](#getting-started) · [Write your own app](docs/WRITING_AN_APP.md) · [Benchmarks](BENCHMARK_RESULTS.md) · [Architecture](ARCHITECTURE.md) · [Roadmap](ROADMAP.md)

</div>

<!-- LATEST_UPDATE_START -->
* 29.06 08:17 — chore: sync README with benchmark results (18b5cad)
<!-- LATEST_UPDATE_END -->

---

## What it is

KATANA closes the gap between two kinds of frameworks:

- the fast-to-write ones (Python, Node) with unpredictable performance,
- the raw high-performance ones (C++, Rust) that are painful to operate.

You describe your contract — the SQL queries and the HTTP API — and the code generator
emits typed DTOs, JSON parsers and serializers, validators, routing, handler interfaces,
and a SQL repository. You write the schema, the queries, the spec, and the handler bodies.
The framework fixes the execution model, the memory model, and the API contract so that
latency stays stable without complicating the domain logic.

## Performance

Measured on a Ryzen 5 5600 (6c/12t), all servers pinned to the same cores, all competitors
built with full optimization (LTO + `target-cpu=native`). These are honest, reproducible
numbers — see [the full run](comparisons/http_frameworks/RESULTS_2026-06-07.md).

On JSON compute work (parse + validate + serialize), KATANA leads the field:

| Throughput (req/sec) | KATANA | actix-web | axum | drogon | ntex |
|---|---|---|---|---|---|
| Compute, pipeline depth 10 | **2.81M** | 1.85M | 1.44M | 0.71M | 0.51M |
| Compute, pipeline depth 40 | **4.05M** | 2.36M | 1.86M | 0.79M | 0.52M |

On a bare `Hello, World!` response KATANA is neck-and-neck with actix-web at the top of the
pack (actix edges it at depth 10, KATANA edges it at depth 20), and ahead of axum/drogon/ntex.

Micro-benchmarks worth calling out: SIMD CRLF scan ~103M ops/sec, arena allocation
~715M ops/sec, HTTP parse ~2.3M req/sec single-thread.

A complex SQL stress test (4-table schema, 500K rows, joins + aggregates + window functions
under concurrent load) ran with **zero errors** and sub-200 µs medians on light queries.

## Getting started

Dependencies: CMake ≥ 3.20, Ninja, GCC ≥ 12 or Clang ≥ 16, `liburing-dev` (for io_uring
presets), `libpq` (for PostgreSQL).

```bash
cmake --preset debug          # presets: debug, release, asan, tsan, ubsan, bench, examples, io_uring-*
cmake --build --preset debug
ctest --preset debug          # run the test suite
```

Run an example server:

```bash
cmake --build --preset examples && ./build/examples/hello_world_server
```

Run the benchmarks:

```bash
cmake --preset bench && cmake --build --preset bench
python3 scripts/run_benchmarks.py --include-e2e
```

Or use the Makefile shortcuts: `make build`, `make test`, `make ci`, `make bench`.

## Writing your own application

The full walkthrough is in **[docs/WRITING_AN_APP.md](docs/WRITING_AN_APP.md)** — from an
empty database to a running SQL API. The short version:

```sql
-- queries/get_customer.sql
-- name: get_customer :one
SELECT id::bigint AS id, name::text AS name, country::text AS country
FROM customers WHERE id = $1::bigint;
```

```yaml
# api.yaml
paths:
  /customers/{id}:
    get:
      operationId: get_customer
      parameters:
        - { name: id, in: query, required: true, schema: { type: integer, minimum: 1 } }
      responses:
        '200': { description: ok, content: { application/json: { schema: { $ref: '#/components/schemas/Customer' } } } }
```

```bash
katana_gen sql     -i queries  -o generated      # -> typed repository
katana_gen openapi -i api.yaml -o generated_api  # -> DTOs, parsing, validation, routing, handlers
```

You implement the handler — path and query parameters arrive as typed, validated arguments;
the body arrives as a parsed DTO — call the repository, fill the response, done.

A worked example with joins, aggregates, window functions and bulk inserts is in
`examples/codegen/shop_api/`.

## Documentation

| Doc | What's in it |
|---|---|
| [WRITING_AN_APP.md](docs/WRITING_AN_APP.md) | Step-by-step: build your own SQL-backed API |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Design, execution model, memory model |
| [CODEGEN.md](docs/CODEGEN.md) | The code generator |
| [OPENAPI.md](docs/OPENAPI.md) | Supported OpenAPI subset |
| [ROUTER.md](docs/ROUTER.md) | Router internals and usage |
| [HTTP_SERVER.md](docs/HTTP_SERVER.md) | Server, reactors, connections |
| [BENCHMARKING.md](docs/BENCHMARKING.md) | How the benchmarks work |
| [X_KATANA_EXTENSIONS.md](docs/X_KATANA_EXTENSIONS.md) | `x-katana-*` spec extensions |
| [ROADMAP.md](ROADMAP.md) | What works, what's in progress, what's planned |

<!-- BENCH_SUMMARY_START -->

<!-- BENCH_SUMMARY_END -->

## License

Apache License 2.0 — see [LICENSE](LICENSE).
