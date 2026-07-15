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

**Status: COMPLETE.** Every Stage 6 item — all [P0] plus the [P1] distributed tracing — is
implemented and tested: layered config with hot-reload + `*_FILE` secrets; structured logging
with access log, 1-in-N sampling and a correlation/trace-aware handler helper; metrics with a
duration histogram, per-route (template-labelled) counters, connection and reactor gauges;
health/readiness; connection read/write/idle timeouts, a max-connections cap, and a DB
statement-timeout; graceful shutdown that stops accepting, drains, and sends `Connection: close`;
oversized requests answered with 413/431/414; and W3C distributed tracing with `tracestate`
passthrough and a pluggable span exporter (the OTLP/Zipkin seam). The only deliberately-deferred
piece is a *bundled* OTLP wire exporter — it would pull in gRPC/protobuf, so the framework ships
the exporter hook instead and leaves the transport to the application.

- **[P0] Configuration system** — **base done.** `katana::config` merges layered sources in
  precedence order — programmatic defaults < config file (`key = value`, `#` comments) < env
  vars (`from_env("KATANA")` → `KATANA_DB_DSN` maps to `db_dsn`) < `--flags` — into one
  normalized snake_case key space, with typed accessors (`get_u16`/`get_int`/`get_bool`/
  `get_or`), load-error capture, and `validate({required...})` returning clear messages. Wired
  into the fullstack demo (`KATANA_PORT=… ./notes --workers 8`). `resolve_file_secrets()` adds the
  Docker/k8s `*_FILE` convention (a `db_password_file` path is read into `db_password`, keeping
  secrets out of env/args), and `reload()` re-applies every recorded source in order so a changed
  file / rotated secret / new env is picked up without a restart (wire it to SIGHUP or a watcher).
  *Remaining:* nothing major — typed-struct binding is covered by the typed getters (C++ has no
  reflection to auto-bind).
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
  the access log as `duration_us`). Per-route counters (`katana_http_route_requests_total` and
  `..._route_duration_seconds_sum`) are labelled with the route **template** (`GET /notes/{id}`,
  not raw ids) so cardinality stays bounded. Opt-in `server.reactor_metrics()` exposes worker/
  reactor gauges (`katana_http_workers`, `katana_reactor_*` for tasks/fd-events/timeouts/
  rejections/exceptions); connection gauges (`katana_http_connections_active`) are always on.
  *Remaining:* allocator/arena stats, OTLP push (see the exporter hook below).
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
  2KB URI, 100 headers). A DB deadline propagates into libpq: `postgres_config.statement_timeout_ms`
  folds a server-side `statement_timeout` into the connection string (sync + async paths), so a
  runaway query is cancelled by Postgres (57014) instead of pinning a reactor. Oversized
  requests now return a specific status — 414 (URI), 431 (header fields), 413 (body) — instead
  of a bare connection close. *Remaining:* making the size limits themselves runtime-configurable
  (they're sensible compile-time defaults today).
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
  to the access log. Opt-in via `server.tracing()`. Inbound `tracestate` is carried verbatim on
  `request_context::trace.tracestate` for downstream propagation. A pluggable
  `server.span_exporter(fn)` replaces the default log sink with any backend (the seam for OTLP/
  Zipkin — the framework hands you `tracing::span_record`, you own the wire protocol, so no
  gRPC/protobuf dependency is bundled). *Remaining:* a bundled OTLP/HTTP exporter implementation,
  automatic span-per-SQL-query.

---

## Stage 7 — Developer experience & packaging (from the CaseCore trial)

The fullstack CaseCore trial surfaced concrete DX gaps. This stage closes the distance
between "the primitives exist" and "it's pleasant to build a separate product on it".

- **[P0] Standalone consumer build** — **done.** KATANA now exports a proper CMake package:
  `install(TARGETS … EXPORT)` + `configure_package_config_file` produce `katanaConfig.cmake`,
  so a consumer just does `find_package(katana)` and links `katana::core` (library) /
  `katana::gen` (generator). The backend macro (`KATANA_USE_EPOLL`/`KATANA_USE_IO_URING`) and
  `KATANA_HAS_LIBPQ` are now PUBLIC usage requirements on `katana_core` instead of directory-scope
  defines, so consumers never hand-set them; public include dirs use `BUILD_INTERFACE`/
  `INSTALL_INTERFACE`. The `katana_add_openapi`/`katana_add_sql` helpers ship with the package and
  resolve the generator as either the in-tree `katana_gen` or the imported `katana::gen`. Verified
  end-to-end both ways: a `find_package(katana)` consumer generates from a contract, compiles and
  runs; and the CaseCore trial builds via `add_subdirectory` with the manual `KATANA_USE_EPOLL` and
  the hardcoded `katana_gen` path both removed. *Remaining:* nothing major (multi-arch/static
  packaging is Stage 11).
- **[P0] CORS middleware** — **done.** `server.cors(cors_config)` handles preflight `OPTIONS`
  (204 + `Access-Control-Allow-Methods/Headers/Max-Age`, 403 for a disallowed Origin) before
  routing, and `finalize_response` decorates every actual (non-preflight) response from an allowed
  Origin with `Access-Control-Allow-Origin` (+ `Allow-Credentials`/`Expose-Headers`/`Vary` per
  policy). Config covers origin allow-list (empty/`*` = any), methods, headers (`*` echoes the
  requested set), exposed headers, credentials, and max-age. Covered by integration tests for
  preflight (permissive + allow-list + reject) and for actual responses on a *fresh* connection
  (so the actual-response path is verified independently of preflight header reuse).
- **[P1] Generated TypeScript client** — **done.** `katana_gen openapi --emit typescript` writes a
  self-contained `generated_client.ts`: `interface`s/enum unions for the schemas and a typed
  `ApiClient` with one async method per operation (path-param interpolation, query object, JSON body,
  2xx return type), plus an `ApiError`. Opt-in (not part of `--emit all`, which feeds the C++ dirs);
  the `katana_add_typescript()` CMake helper regenerates it for a frontend build. Output is
  `tsc --strict` clean; covered by a codegen snapshot test.
- **[P1] Reference fullstack template** — **done.** `examples/services/pulse` is now a copy-me
  template: a `CMakeLists.txt` (find_package(katana), else in-repo add_subdirectory) that generates
  both contracts + SQL + the Row↔DTO bridge + the TypeScript clients, a Next.js `web/` console driven
  by the generated client, a `docker-compose.yml` (Postgres+Redis, schema auto-applied), and the
  exemplary backend wiring (config layering, two PG pools, Redis policies, built-in
  CORS/`/healthz`/`/readyz`, graceful shutdown). It builds and runs without patching the framework;
  verified end-to-end (CRUD + analytics through the bridge and generated client).
- **[P1] `katana dev`** — **done.** `tools/katana` (with `make dev`/`up`/`down`) brings up the
  template's deps via docker compose, regenerates + builds the backend, starts the API and the
  frontend dev server with prefixed logs, and tears it all down on `Ctrl-C`. `[service-dir]` targets
  any KATANA service, defaulting to the pulse template.
- **[P2] Codegen ergonomics** — **done.**
  - *Named SQL parameters* — write `@name` placeholders and the generated method takes readable
    `arg_<name>` arguments (repeats reuse one slot; the SQL sent to Postgres stays positional
    `$N`). Raw `$N` queries keep their `pN` names.
  - *Row→DTO bridge* — `katana_gen sql --openapi <spec>` emits `generated_bridge.hpp` with
    `to_<Dto>(row, arena)` / `to_<Row>(dto)` converters for every row that exactly matches a DTO
    (normalized field names + compatible scalar/string types); ambiguous or partial matches are
    skipped with a warning, never mis-mapped.
  - *Consistent namespaces* — the SQL generator now honours `--namespace` as a bare namespace
    (default `generated`), the same as the OpenAPI generator, so a contract's DTOs and its SQL
    rows/repository share one namespace (`katana::sql::` prefix removed).
  - *Louder errors* — the OpenAPI loader warns (`<file>:<line>: warning: …`) on an unsupported
    construct instead of silently coercing it, and `--strict` turns that into an error; SQL parse
    failures name the offending file and query.

---

## Stage 8 — Security & transport  [P0/P1]

- **[P0] TLS / HTTPS** — **done.** OpenSSL termination integrated with the non-blocking reactor
  (`katana/core/tls.{hpp,cpp}`): `tls_config`/`tls_context`/`ssl_session`, a handshake phase before
  the parse loop, want-read/want-write interest re-arming, ALPN (negotiates `http/1.1`), modern
  cipher/version defaults (no compression/renegotiation, server preference), cert/key load, and
  `server.reload_tls()` hot-reload (atomic context swap). `server.tls(cfg)` builder; verified with
  a real `curl` HTTPS/TLS1.3 round-trip + an integration test. mTLS config (`ca_file`,
  `require_client_cert`) is supported; *remaining:* SNI multi-cert selection, a bundled cert-rotation
  watcher, and keeping `-DENABLE_TLS=OFF` buildable.
- **[P0] AuthN / AuthZ** — **done.** JWT (HS256/RS256/ES256 + JWKS, `katana/core/jwt.{hpp,cpp}`) and
  API-key auth with an `authenticator` + `principal` (`auth.{hpp,cpp}`), delivered **both** ways: a
  contract-first `x-katana-auth` annotation → `route_policy_view` → `auth_executor` (401 missing/
  invalid, 403 insufficient scope, principal exposed to handlers), and an attachable
  `require_auth(scope)` middleware. `server.jwt_auth()/api_key_auth()` builders compose the auth
  executor ahead of the Stage-5 policy chain. Covered by jwt/auth unit tests and an end-to-end
  scope-enforcement integration test.
- **[P1] Response compression** — **done.** `server.compression()` negotiates gzip/brotli/zstd from
  `Accept-Encoding` (server preference br > zstd > gzip), compresses a compressible body above a size
  threshold in `finalize_response`, and sets `Content-Encoding` + `Vary`. `katana/core/compression.{hpp,cpp}`
  (zlib/libzstd/libbrotli behind `ENABLE_COMPRESSION`). Verified end-to-end (gzip round-trip, codec
  preference) with an integration test. *Remaining:* request decompression (`Content-Encoding` on
  inbound bodies).
- **[P1] Edge rate limiting & load shedding** — **done.** `server.load_shedding()` adds a global
  in-flight cap (sheds with **503 + Retry-After** before routing) and a per-client-IP fixed-window
  rate limit (**429 + Retry-After**). The peer IP is captured at accept and exposed on
  `request_context.client_ip` (also useful for access logs). Covered by an integration test (3 pass,
  then 429). *Remaining:* pluggable/Redis-backed shared counters and bucket eviction for unbounded
  keyspaces.
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
