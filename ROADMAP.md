# Roadmap & status

The honest state of the framework, broken into stages. The README is a short
presentation; this file is the real plan.

Blunt summary: today KATANA is a fast HTTP/1.1 server, a compile-time router, an
OpenAPI/SQL code generator, and a PostgreSQL data layer. The performance core is solid.
Most of the plumbing a real production service needs — TLS, auth, observability,
configuration, operational safety — is not built yet. The stages below go from "what
exists" through "what makes it operable in production" to "breadth".

> Note: `ARCHITECTURE.md` describes a target design (it mentions modules like
> `katana/net`, `katana/security`, `katana/metrics`). Those directories don't exist yet —
> everything real lives in `katana/core/`. This file is the source of truth for status.
>
> Priority tags inside stages: **[P0]** blocks any production use, **[P1]** needed for a
> serious service, **[P2]** important later. Stages are roughly ordered, but the P0 items
> are what actually gate a first production deployment regardless of stage number.

---

## Stages 1–4 — Done

- **Stage 1 — Runtime core.** Epoll/io_uring reactors, reactor pool, arenas and IO
  buffers, wheel timer, TCP listener/socket helpers.
- **Stage 2 — HTTP & router.** HTTP/1.1 parser and serializer, compile-time router with
  middleware, media-type registry (`Content-Type`/`Accept` for JSON, CBOR, MessagePack).
- **Stage 3 — Contract-first codegen.** OpenAPI loader (JSON/YAML, block + flow style,
  `$ref`), `katana_gen openapi` (DTOs with pmr allocators, JSON parse/serialize,
  validators, enums, handler interfaces, constexpr route tables, typed path/query/body
  parameter binding), conformance harness.
- **Stage 4 — SQL-first data layer.** `katana_gen sql` (`:one`/`:many`/`:exec`, typed
  models/repositories, PostgreSQL arrays + bulk `UNNEST`, prepared statements,
  pool/executor path, transactions), live PostgreSQL integration tests, benchmark harness.

---

## Stage 5 — Runtime policy layer + Redis (done; async Redis client optional)

Turning the declarative `x-katana-*` spec annotations into runtime behaviour, with both
in-memory and Redis-backed storage.

Done:
- Runtime enforcement of `x-katana-*` policies, with the executor chain wired into the
  server's request path (it's a no-op when a route has no policy, so it doesn't slow the
  hot path):
  - response **caching** (`x-katana-cache`) — a hit returns `X-Katana-Cache: HIT` and skips
    the handler
  - **rate limiting** (`x-katana-rate-limit`)
  - **idempotency** (`x-katana-idempotency`)
- **Redis client** + **Redis-backed policy stores** (so cache/rate-limit/idempotency state
  is shared across instances), alongside the in-memory stores
- **Per-reactor Redis pool** (`redis_pool`) so each worker uses its own connection — a single
  shared connection both serialized every request on one round-trip and raced the RESP
  stream. One-line wiring via `redis_contract_policy_executor(pool)`. (See the
  "Caching and rate limiting" section of `docs/WRITING_AN_APP.md`.)
- Unit tests for the Redis client, policy stores, and the router policy path

Done (continued):
- **In-process L1 cache in front of Redis** (`tiered_response_cache_store`). A response-cache
  HIT is now served from L1 with no Redis round-trip; on an L1 miss it reads Redis (L2) and
  promotes the entry. `redis_contract_policy_executor` wires it automatically via the
  `l1_cache_ttl` option (default 1s; set to 0 to go straight to Redis). L1 entries are capped
  to that TTL, so a cross-instance invalidation goes stale elsewhere for at most that long.
- **Neutral executor naming** — the policy executors are now `response_cache_executor` /
  `idempotency_executor` / `rate_limit_executor` (store-agnostic), with the old
  `in_memory_*_executor` names kept as back-compat aliases.

Remaining:
- **[P1/P2] Async / pipelined Redis client.** Rate-limit and idempotency still do one
  synchronous Redis round-trip per request (they need the shared counter/lock, so L1 doesn't
  apply). A non-blocking, reactor-integrated Redis client (like the PostgreSQL async path) or
  command pipelining would close that remaining gap. The cache-HIT round-trip is already
  removed by the L1 tier above.

---

## Stage 6 — Production observability & operations  [P0]

You cannot operate a service without these. This is the stage that makes a deployment
debuggable and safe to run.

**Status: every Stage 6 item — all [P0] plus the [P1] distributed tracing — has a base
implementation** (config, structured logging + access log, metrics incl. duration histogram,
health/readiness, connection timeouts + max-connections, graceful shutdown, W3C tracing). Each
entry's *Remaining* note lists the follow-on depth (config hot reload/secrets, per-route metric
labels, DB deadlines into libpq, OTLP export, handler-side log helper, …) — refinements, not
operational blockers.

- **[P0] Configuration system** — **base done.** `katana::config` merges layered sources in
  precedence order — programmatic defaults < config file (`key = value`, `#` comments) < env
  vars (`from_env("KATANA")` → `KATANA_DB_DSN` maps to `db_dsn`) < `--flags` — into one
  normalized snake_case key space, with typed accessors (`get_u16`/`get_int`/`get_bool`/
  `get_or`), load-error capture, and `validate({required...})` returning clear messages. Wired
  into the fullstack demo (`KATANA_PORT=… ./notes --workers 8`). *Remaining:* secrets handling,
  hot reload without restart, typed-schema binding to a struct.
- **[P0] Structured logging** — **base done.** `katana::log` emits one JSON object per line
  with level, timestamp, message and typed key/value fields (`info("...").field(k, v)`),
  thread-safe, with a configurable min level and sink. The server has an opt-in built-in
  access log (`server.access_log()`) that logs method/path/status/bytes/correlation-id per
  request, plus `X-Request-Id` correlation — echoed from the client or generated, reflected on
  the response, and exposed to handlers via `request_context::request_id`. The access log
  supports 1-in-N sampling (`access_log(enable, sample_every)`) for high-volume services.
  Handlers get `ctx.log_info("...")` (and `log_warn`/`log_error`/`log_debug`) which auto-tag the
  line with the request's `request_id` and `trace_id`, so handler logs join the access log and
  spans. *Remaining:* nothing major — async-safe sink batching is a future optimization.
- **[P0] Metrics** — **base done.** Built-in Prometheus `/metrics` endpoint (on by default,
  served before the router) exposing `katana_http_requests_total` by status class, a
  `katana_http_requests_in_flight` gauge, `katana_http_connection_timeouts_total`, and a
  `katana_http_request_duration_seconds` histogram (the RED "Duration"; latency is also added to
  the access log as `duration_us`). *Remaining:* per-route labels, connection/pool gauges,
  reactor/allocator stats, OTLP push.
- **[P0] Health & readiness** — **done.** Built-in `/healthz` (liveness) and `/readyz`
  (readiness) served before the user router, on by default. `/readyz` returns 503 while the
  server is shutting down or a registered `readiness_check(probe)` reports not-ready (wire it
  to the DB pool / dependencies); paths are configurable and the endpoints can be disabled.
- **[P0] Timeouts & deadlines** — **base done.** Opt-in per-connection read/write/idle timeouts
  via `server.connection_timeout(read, write, idle)`: a connection that makes no progress within
  the window is closed by the reactor (slowloris protection), surfaced as
  `katana_http_connection_timeouts_total`. Activity refreshes the timer so actively-fed
  connections survive (verified under load: 0 timeouts, 0 errors at ~305K req/s). This also
  fixed a latent unit bug in the reactor's timeout comparison (nanoseconds vs milliseconds) that
  made any timed fd fire almost immediately and ignore refreshes. A `server.max_connections(n)`
  cap sheds load when the simultaneous-connection count is reached (over-cap accepts are closed,
  counted in `katana_http_connections_rejected_total`; live count in `katana_http_connections_active`).
  Header/body/URI/header-count size limits are enforced by the parser (8KB headers, 10MB body,
  2KB URI, 100 headers). *Remaining:* handler/DB deadlines that propagate into libpq; making the
  size limits configurable with proper 413/431 responses.
- **[P0] Complete graceful shutdown** — **base done.** On SIGTERM each reactor deregisters +
  closes its listener fd (stops accepting), then the existing drain loop — now seeing only live
  connections, not the listeners — lets in-flight requests finish and exits as soon as they
  drain, rather than always waiting the full deadline; the deadline remains a hard cap that
  force-closes stragglers. During shutdown every response also carries `Connection: close`, so
  kept-alive clients stop reusing the connection and drain as their in-flight request finishes.
  Covered by integration tests (exits well under an 8s deadline on SIGTERM; responses carry
  `Connection: close` during shutdown) and verified on the demo. *Remaining:* proactively close
  fully-idle keep-alive connections that send nothing during shutdown (today they're bounded by
  the drain deadline, or `connection_timeout` if set).
- **[P1] Distributed tracing** — **base done.** W3C Trace Context (`katana::tracing`): the server
  parses an inbound `traceparent` and continues the trace (new span id, inbound span as parent)
  or starts a fresh root, exposed to handlers via `request_context::trace` (with
  `to_traceparent()` for downstream propagation). Per sampled request it emits a span (via
  `katana::log`, msg `"span"`: trace/span/parent ids, name, status, duration) and adds `trace_id`
  to the access log. Opt-in via `server.tracing()`. *Remaining:* OTLP/gRPC exporter to a
  collector, automatic propagation into the SQL layer (sqlcommenter / span per query),
  `tracestate` passthrough.

---

## Stage 7 — Developer experience & packaging (from the CaseCore trial)

The fullstack CaseCore trial surfaced concrete DX gaps. This stage closes the distance
between "the primitives exist" and "it's pleasant to build a separate product on it".

- **[P0] Standalone consumer build** — depend on KATANA via a clean `add_subdirectory()`/
  install/export, without relying on `CMAKE_SOURCE_DIR` or hand-setting
  `KATANA_USE_EPOLL`/`KATANA_USE_IO_URING`.
- **[P0] CORS middleware** — correct preflight and CORS headers out of the box, so a separate
  browser frontend works without hand-rolled glue.
- **[P1] Generated TypeScript client** from the OpenAPI spec, so frontends don't hand-write
  a fetch layer over the same contract.
- **[P1] Reference fullstack template** — backend + frontend + OpenAPI + SQL catalog + run
  scripts, proving a real product builds without patching framework internals.
- **[P1] `katana dev`** — one command to bring up backend + frontend + deps and watch/reload.
- **[P2] Codegen ergonomics** — named SQL parameters instead of positional `p1/p2/p3`; a
  `Row → DTO` bridge; consistent namespaces between SQL (`katana::sql::generated`) and
  OpenAPI (`generated`) output; file/line/column on codegen errors; a loud error instead of
  a silent drop when a spec construct isn't understood.

---

## Stage 8 — Security & transport  [P0/P1]

- **[P0] TLS / HTTPS** — termination (OpenSSL/BoringSSL or kTLS), ALPN, SNI, certificate
  loading and hot reload, modern cipher defaults. The single biggest single gap; pull it
  earlier if a public deployment is needed before the rest of this stage.
- **[P0] AuthN / AuthZ** — JWT and API-key authentication, plus a per-route authorization
  hook (scopes/roles).
- **[P1] Response compression** — gzip/brotli/zstd, with content negotiation; request
  decompression.
- **[P1] Edge rate limiting & load shedding** — per-client and global, with admission control
  so the server sheds load instead of falling over (builds on Stage 5's policy layer).
- **[P2] Security headers & CSRF** — HSTS, CSP, X-Frame-Options, etc.
- **[P2] mTLS, OAuth2/OIDC** — beyond basic JWT/API-key.

---

## Stage 9 — Protocol & API breadth  [P1/P2]

- **[P1] Multipart / file uploads** — streaming `multipart/form-data`, large bodies without
  buffering everything in memory.
- **[P1] Server-Sent Events & WebSocket** — upgrade handshake, framing, ping/pong,
  backpressure.
- **[P1] Static file serving** — range requests, caching headers, `sendfile`/`splice`.
- **[P1] Problem+JSON (RFC 7807) everywhere** — consistent typed error bodies (partly there
  via `problem_details`).
- **[P1] Pagination / filtering / sorting** — conventions and codegen support.
- **[P2] HTTP/2** — multiplexing, HPACK, flow control (and a base for gRPC).
- **[P2] True async io_uring** — the backend is currently poll-based (readiness + the same
  recv/send syscalls). Real `IORING_OP_RECV/SEND`, multishot accept, registered buffers,
  batched submit.
- **[P2] HTTP/3 / QUIC**, **gRPC / protobuf**, **ETag / conditional requests**.

---

## Stage 10 — Data layer depth  [P1/P2]

- **[P1] Migrations** — schema versioning, up/down, CI integration.
- **[P1] DB resilience** — connection failover, read/write split and replica routing,
  retry-on-serialization-failure, pool health checks.
- **[P1] Retries, circuit breakers, idempotency keys** for DB and downstream calls (backoff +
  jitter); deadline propagation.
- **[P2] More drivers** — MySQL, SQLite.
- **[P2] Query result caching**, outbox/CDC hooks.

---

## Stage 11 — Distribution & deployment  [P1/P2]

- **[P1] Packaging** — official multi-arch Docker images (distroless), CMake install/export
  targets, static/musl builds.
- **[P1] Releases** — semantic versioning, a stable public API/ABI surface, a changelog.
- **[P2] Distribution** — vcpkg/conan packages, signed releases, SBOM, reproducible builds.
- **[P2] Deployment** — Helm chart / k8s manifests, systemd units, zero-downtime rolling
  deploys (socket handoff or SO_REUSEPORT rolling restart).

---

## Cross-cutting — Testing & quality (every stage)

- Contract tests, property-based tests, load tests as code, fault/chaos injection.
- Golden-file tests for codegen output so generator changes are reviewable.
- Coverage gates in CI; keep unit/integration/fuzz/conformance green.

## Notes

Performance is the current strength — see `BENCHMARK_RESULTS.md` and
`comparisons/http_frameworks/RESULTS_2026-06-07.md`. The gap to "production framework" is
breadth (Stages 6–11), not core speed. Anything not under **Done** or **Stage 5** is not
implemented yet.
