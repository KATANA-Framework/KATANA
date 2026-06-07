# KATANA Architecture

A detailed description of the framework's architecture, design principles, and technical decisions.

---

## Goals and principles

### Performance by default

Reactor-per-core architecture with no global queues or locks, minimal allocations, zero-copy where possible.

### Code safety

Strict RAII, no `new`/`delete`, static analysis checks (clang-tidy), sanitizers in CI (ASan/UBSan/TSan).

### Deterministic project structure

API-first approach: OpenAPI and SQL as the single sources of truth, automatic code generation.

### Observability

p50/p95/p99 metrics, distributed tracing, structured logs correlated by trace_id/span_id.

### Extensibility

Layered architecture, plugins, clean contracts between components, loose dependencies.

> Status: this document describes the target architecture. For the authoritative,
> up-to-date implementation status see [ROADMAP.md](ROADMAP.md). In short, the
> current codebase has working core (epoll/io_uring reactors + reactor pool,
> arenas/IO buffers, HTTP/1.1 parser/serializer, wheel timer, TCP helpers),
> a compile-time router with middleware and a media-type registry
> (JSON/CBOR/MessagePack), OpenAPI codegen, and SQL-first codegen. Runtime
> enforcement of `x-katana-*` policies and a Redis client are in progress;
> OpenTelemetry, Prometheus, and structured logging are planned.

---

## Repository tree

```
KATANA/
├── cmake/                  # toolchains, presets
├── third_party/            # pinned dependencies (if needed)
├── tools/
│   └── katana_gen/        # generators: OpenAPI→routes, SQL→models/repos
├── katana/
│   ├── core/              # runtime core (event loop, scheduler, allocators, time)
│   ├── net/               # TCP/UDP, TLS, DNS (Asio), connectors/acceptors
│   ├── http/              # HTTP/1.1 + HTTP/2 (Boost.Beast), routing, middleware
│   ├── rpc/               # gRPC/Cap'n Proto (optional)
│   ├── sql/               # DAL: drivers, pool, migrations, repos (generated)
│   ├── cache/             # Redis (redis-plus-plus), in-process cache
│   ├── config/            # config loading/validation (TOML/YAML), hot-reload
│   ├── logging/           # spdlog/fmt + trace_id correlation
│   ├── tracing/           # OpenTelemetry OTLP exporter
│   ├── metrics/           # Prometheus pull/OTLP push, system & business metrics
│   ├── security/          # TLS (OpenSSL/BoringSSL), JWT (jwt-cpp), RBAC/ABAC hooks
│   └── util/              # gsl, expected, outcome, small_vec, ring_buffer
├── examples/
│   ├── mid_service/       # "mid layer" service from OpenAPI/SQL
│   └── low_service/       # "low layer" with manual control
├── test/
│   ├── unit/
│   ├── integration/
│   └── fuzz/
├── benchmarks/
├── .clang-format
├── .clang-tidy
└── CMakeLists.txt
```

---

## External dependencies (policy)

### Stages 1-3: Zero-Dependency Foundation

Stages 1-3 follow a **no external dependencies** approach, using only the C++23 stdlib and direct Linux syscalls. Benefits:

- Full control over performance
- Simpler builds and deployment
- Reduced attack surface
- Fast compilation

**Core implementation (stages 1-3):**
- Custom epoll-based reactor (no Asio)
- Custom HTTP/1.1 parser (no Boost.Beast)
- std::pmr for memory management
- std::expected for error handling
- Direct syscalls for I/O

### Stage 4+: Selective dependencies

External dependencies will be added selectively in later stages:

**Async and networking** (stage 7+)

Optional: **standalone Asio** / **Boost.Asio**, **Boost.Beast** for HTTP/2, WebSocket

**Formatting and logging** (stage 5+)

**fmt**, **spdlog**

**Serialization** (stage 4+)

**nlohmann::json**, **yaml-cpp**, **toml++**

**SQL** (stage 4+)

**PostgreSQL** (libpq), **SQLite3**

**Cache** (stage 4+)

**redis-plus-plus**

**Security** (stage 7+)

**OpenSSL**/**BoringSSL**, **jwt-cpp**

**Observability** (stage 5-6)

**OpenTelemetry C++ SDK**, **Prometheus-cpp**

**Testing**

**GoogleTest**, **libFuzzer**/**LLVM** (fuzzing)

**Code quality**

**clang-tidy**, **sanitizers** (ASan/UBSan/TSan/LSan)

---

## Benchmark harness

`benchmark/simple_benchmark.cpp` runs quality control for the reactor and HTTP stack. The harness uses a timeboxed methodology:

* Each scenario uses a warmup phase and a fixed measurement window (`steady_clock`), so results can be compared across machines.
* The client reads responses to completion: it parses headers, checks `Content-Length`, reuses keep-alive, and recreates the socket on any error.
* Percentiles are computed from a sorted array with linear interpolation, which avoids the p95=p99=p999 "steps".
* Scaling tests (threads and fan-out 32/64/128) run for 2-2.5 s rather than a fixed request count, which removed the artificial dip at 64 connections.
* Latency, keep-alive, and HTTP-parsing collect thousands of samples; the aggregator reports IQR and sample count, and results are committed automatically to `BENCHMARK_RESULTS.md`.

Honest, reproducible numbers live in [BENCHMARK_RESULTS.md](BENCHMARK_RESULTS.md) and
[comparisons/http_frameworks/RESULTS_2026-06-07.md](comparisons/http_frameworks/RESULTS_2026-06-07.md).
KATANA leads on JSON compute workloads and is competitive with actix-web on a bare
hello response. The harness runs from `build/benchmark/simple_benchmark`, and the
server from `build/hello_world_server`. The `benchmark.sh` script is still available
for wrk profiles.

---

## Layers (logical architecture)

### 1. katana/core (Runtime)

**Reactor-per-core**: each CPU core gets an independent event loop (epoll- or io_uring-based reactor).

**Complete Isolation**: each reactor has its own state with no shared data — this guarantees correctness and freedom from races.

**Thread Pinning (Optional)**: an optional performance optimization to improve CPU cache locality and NUMA-aware placement. Not required for correctness.

**Scheduler**: task scheduler, lock-free MPSC queues for cross-thread communication (when needed).

**Event Loop**: edge-triggered epoll and an io_uring reactor, vectored I/O (readv/writev), EPOLLONESHOT support.

**Allocators**: arena-per-request (std::pmr::monotonic_buffer_resource), zero-copy where possible.

**Timers/Clock**: monotonic timers, wheel-timers (512 slots, 100ms granularity) with dense handles, generations, and strict synchronization against `steady_clock`. Callbacks run only when time actually advances; cancellation is idempotent.
**Timeout API**: a single helper manages deadlines (auto-reset, chunk-sleep), keeps activity state on file descriptors, and is used in the reactor and benchmarks.

**Safety**: strict RAII, std::expected for errors, no raw `new`/`delete`.

**Note**: stages 1-3 use the custom implementation. Future stages may optionally integrate Asio/coroutines.

### 2. katana/net

**Connect/Accept**: TCP/TLS, UDP/QUIC (optional), DNS resolution.

**Backpressure**: limit on in-flight requests, flow-control handles.

**Zero-copy**: buffers as `asio::const_buffer`/`mutable_buffer`, reused through pools.

### 3. katana/http

**Server**: custom HTTP/1.1 parser with chunked encoding, keep-alive, and security limits.

**Parser**: zero-copy parsing, strict RFC 7230 compliance, configurable size limits.

**Serializer**: support for Content-Length and chunked transfer encoding.

**Error Mapping**: std::expected → Problem Details (RFC 7807).

**Router**: compile-time route table, static dispatch, content negotiation (415/406).

**Middleware**: middleware chain (logging, authentication, rate-limit, CORS, tracing).

**HTTP/2**: (stage 7+) via nghttp2 or a custom implementation.

### 4. katana/sql

**Drivers**: PostgreSQL via libpq with prepared statements, non-blocking operations via a per-core connection pool.

**Codegen**: `sql/*.sql` (`:one`/`:many`/`:exec`) → typed repositories/DTO/RowMapper, including PostgreSQL arrays and bulk UNNEST inserts.

**Transactions**: scoped transactions (RAII), retry policy, idempotency keys.

**Migrations**: built-in runner, versioning, checksum.

### 5. katana/cache

**Redis**: per-core connection pools, TTL cache, write-behind/write-through optional.

**Local Cache**: lock-free LRU/LFU (folly-like), sharded per core.

### 6. katana/config

**Sources**: file, env, CLI; merge strategy, schema validation.

**Hot reload**: signal + validated rebuild of runtime configs.

### 7. katana/tracing + metrics + logging

**Tracing**: OpenTelemetry (spans around handlers), propagate trace_id via headers.

**Metrics**: Prometheus endpoint `/metrics`; system (CPU, RSS), network (accepts, RPS), latency (p50/p95/p99), errors by class.

**Logging**: structured, correlated by trace_id/span_id, levels/sampling.

### 8. katana/security

**TLS**: context, keys, ciphers, OCSP stapling (when needed).

**AuthN/AuthZ**: JWT verify, roles/rules (RBAC/ABAC), policies as middleware.

---

## Code generation (OpenAPI/SQL → code)

### OpenAPI (Implemented)

**katana_gen** — a tool to generate C++ code from OpenAPI 3.0 specifications.

**Supported OpenAPI features**:
- Parsing JSON and YAML specifications, including both block and inline flow style
  (e.g. `- { name: x, in: query, schema: {...} }`). Anchors/tags are not supported,
  and duplicate keys are an error.
- Resolving `$ref` references to schemas (`#/components/schemas/...`)
- Merging `allOf` compositions with conflict checks
- Spec validation (duplicate operationId, invalid HTTP codes)
- Detecting circular dependencies in `$ref`

**Generated artifacts**:

1. **DTOs** (`generated_dtos.hpp`):
   - Structs for all schemas with arena allocators (pmr)
   - Support for primitives: string, integer, number, boolean, array, object
   - Required/optional fields
   - Enums

2. **Validators** (`generated_validators.hpp`):
   - `validate_<Schema>()` functions for each schema
   - Constraint checks:
     - String: minLength, maxLength, pattern, enum
     - Number: minimum, maximum, exclusiveMinimum, exclusiveMaximum, multipleOf
     - Array: minItems, maxItems, uniqueItems
   - Returns `std::optional<ValidationError>` with field + message

3. **JSON Parsers/Serializers** (`generated_json.hpp`):
   - `parse_<Schema>()` — zero-copy parsing JSON → DTO
   - `serialize_<Schema>()` — serialize DTO → JSON string
   - Support for string escape sequences

4. **Route Table** (`generated_routes.hpp`):
   - Constexpr route table with path, method, operationId
   - Content negotiation info: consumes (Content-Type), produces (Accept)
   - Constant table for O(1) lookup

5. **Handlers + Bindings** (`generated_handlers.hpp`, `generated_router_bindings.hpp`):
   - Typed signatures with optional for non-required parameters
   - Glue for parsing path/query/header/cookie, Content-Type/Accept negotiation
   - Typed path/query/body parameter binding by the negotiated media type

**Command**:

```bash
./katana_gen openapi -i api/openapi.yaml -o gen/ --emit all
./katana_gen openapi -i api/openapi.yaml -o gen/ --emit dto,validator
./katana_gen openapi -i api/openapi.yaml -o gen/ --emit router --dump-ast
```

**Options**:
- `--emit <targets>`: dto, validator, serdes, router, all
- `--alloc <type>`: pmr (arena), std (standard allocator)
- `--strict`: fail on any validation error
- `--dump-ast`: save AST summary to JSON

### SQL (Implemented)

SQL-first codegen reads `.sql` files annotated with `:one`/`:many`/`:exec` and
generates models, row mapping, and type-safe repositories with typed parameters.
It supports PostgreSQL arrays and bulk inserts via UNNEST. The runtime uses libpq
with prepared statements, a connection pool, and transactions.

### Boundaries

All generated code goes to the output directory (`-o`), **not edited by hand**; hand-written code lives in `src/`.

---

## Request processing pipeline (typical)

### Stage 1 (current)

1. **Accept connection** → edge-triggered epoll → register FD with timeouts
2. **HTTP parse**: custom incremental parser → Request object with arena allocation
3. **Handler**: synchronous handler → generate Response
4. **Response serialization**: HTTP/1.1 serializer → vectored I/O write
5. **Keep-alive**: reuse the connection or graceful close
6. **Metrics**: atomic counters (tasks, events, timers, exceptions)

### Stage 2+ (planned)

1. **Accept connection** → HTTP parse → **Router dispatch** (from OpenAPI)
2. **Middleware chain**: tracing → authentication → rate-limit → etc.
3. **Handler** (optionally `co_await`): validation → business logic → SQL/Cache calls
4. **Response serialization**, write to socket
5. **Metrics**: p50/p95/p99 histograms; **Tracing**: OpenTelemetry spans

---

## Code standards and "no unsafe"

### RAII only

Resources are wrapped in types with deterministic lifetimes.

### No raw `new`/`delete`

Enforced via a clang-tidy check + grep in CI.

### Null pointers

`gsl::not_null`, `std::optional`/`tl::optional` instead of raw nullable pointers.

### Boundaries

`std::span` for buffers/views, explicit `string_view`.

### Casts

`narrow`/`narrow_cast` (GSL) instead of C-style casts.

### Concurrency

No "shared" mutable state, per-core sharding, lock-free structures — only when justified.

### Exceptions

In the business layer — `expected`/`Outcome`; in infrastructure — allowed, but mapping and `noexcept` guarantees at boundaries are mandatory.

### Build

```
-Wall -Wextra -Werror -Wconversion -Wshadow -Wpedantic
```

LTO (Link-Time Optimization) in release.

On GCC 15 / CMake 3.31, configure with `-DCMAKE_CXX_SCAN_FOR_MODULES=OFF`: the
project uses no C++20 modules and CMake's module scanning is broken with that
toolchain.

### ABI/visibility

Hide symbols by default (`visibility=hidden`), export explicitly.

---

## Memory and allocations

### Arena-per-request

Everything tied to a request is allocated from the arena and freed in a single operation.

### Monotonic allocator

Fixed blocks, reset after the request completes.

### std::pmr

Use of `std::pmr::memory_resource` to customize the allocator.

### Modes

- `arena` (default)
- `std::pmr` with a configurable `memory_resource`
- standard `new`/`delete` (`--no-arena` flag in dev)

---

## Concurrency and isolation

### Reactor-per-core with full isolation

Each CPU core gets an independent event loop **with no shared state in the data-plane between reactors** (only service counters for shutdown/thread-id assignment remain shared).

**Isolation guarantees**:
- No global mutable variables in the data-plane; only service shared counters (shutdown, thread-id assignment) remain in the control-plane
- No mutexes/locks in the critical path
- No race conditions on request state
- Each reactor is a self-contained processing unit

### Per-core resources

DB connections, Redis, cache, arena allocators — all isolated per core:
- **DB connection pool**: each reactor owns its own connections
- **Cache**: local cache with no synchronization between cores
- **Arena allocators**: independent allocators with no contention

### No handoff between threads

A request is processed start to finish on one core — this key architectural property provides:
- No cross-thread races
- Predictable latencies (no waiting on locks)
- CPU cache locality (L1/L2 stay hot)

### Thread pinning — an optional optimization

**Correctness does NOT depend on pinning**: reactor isolation guarantees no race conditions regardless of whether a thread migrates between cores.

**Pinning as an optimization**:
- Improves CPU cache locality (reduces L1/L2 invalidation)
- Important for NUMA systems (multi-socket servers)
- Stabilizes tail latencies (p99/p999)

**When to use**:
- Production deployments with strict latency SLAs
- Multi-socket NUMA systems
- High-load scenarios

**When not to use**:
- Development environments (`--no-pin` flag)
- Platforms without `sched_setaffinity` (macOS, Windows require different APIs)
- Containerized environments (affinity managed externally)

---

## Testing

### Unit tests

GoogleTest for components, mocks for external dependencies.

### Integration tests

Testcontainers (PostgreSQL, Redis), real dependencies.

### Property-based tests

RapidCheck for validators and serializers.

### Fuzzing

libFuzzer for the HTTP parser and input data.

### E2E tests

Auto-generated from OpenAPI, contract checks.

### Performance budget

p99 regression > 10% → build fails.

The current suite (unit, integration, fuzz, conformance) is green.

---

## Observability

### Metrics (Prometheus)

- HTTP: `http_requests_total`, `http_request_duration_seconds`
- SQL: `db_query_duration_seconds`, `db_connections_active`
- Redis: `redis_commands_total`, `redis_command_duration_seconds`
- System: CPU per-core, memory, arena usage
- Backpressure: `reactor_queue_length`, `reactor_processing_delay`

### Tracing (OpenTelemetry)

- Span per HTTP request
- Span per SQL query (with query text)
- Span per Redis operation
- W3C Trace Context propagation

### Logging (structured JSON)

- Format: timestamp, level, message, trace_id, span_id
- Context fields: request_id, user_id, endpoint
- Levels: DEBUG, INFO, WARN, ERROR

---

## Security

### TLS

BoringSSL/OpenSSL, kTLS offload (Linux), OCSP stapling.

### JWT

jwt-cpp for token validation.

### RBAC/ABAC

Policies as middleware, declarative permission definitions.

### Injection protection

Prepared statements only, no string concatenation for SQL.

### Limits

Header/body size, timeouts, rate limiting.

---

## Dev Experience

### Hot-reload

Rebuild only changed controllers, dynamic loading of `.so`.

### Fast builds

clang + lld, ccache, precompiled headers.

### Automatic dependency spin-up

Docker Compose with PostgreSQL, Redis, Prometheus, Grafana, Jaeger.

### Mocks

`--mock-db`, `--mock-cache` for development without external dependencies.

### Debug mode

`--no-arena`, `--no-pin`, AddressSanitizer.

---

## Production

### Profiles

`dev`, `staging`, `prod`, `prod-high-throughput`, `prod-low-latency`

### Settings

Number of reactors, pool sizes, cache TTL, TCP parameters.

### Deployment

Dockerfile (multi-stage), Kubernetes manifests, health checks.

### Monitoring

Alerts on p99 degradation, error rate spike, connection pool exhaustion.

---

## Links

- [README.md](README.md) — framework overview, quick start
- [ROADMAP.md](ROADMAP.md) — authoritative implementation status
- [docs/WRITING_AN_APP.md](docs/WRITING_AN_APP.md) — build-your-own-app guide
- [BENCHMARK_RESULTS.md](BENCHMARK_RESULTS.md) — reproducible benchmark numbers
- `/docs/RFCs` — Core/Codegen/Lint specifications
- `/docs/Conformance` — standards conformance tests
