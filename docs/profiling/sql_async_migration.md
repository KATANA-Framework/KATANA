# SQL Async Migration Notes

## Current state

Stage 4 SQL HTTP benchmark path is now split:

- generated async repository calls can run on a reactor-bound nonblocking PostgreSQL path inside HTTP handler scope;
- default durable write-heavy flows still spend most of their time in PostgreSQL commit/WAL work, and mixed CRUD latency still includes queueing pressure from the remaining sync path.

Recent perf evidence on the benchmark fixture shows:

- app-server CPU is not the main bottleneck anymore;
- PostgreSQL dominates system cycles under mixed CRUD;
- Docker port publishing adds measurable overhead and should not be used for reference SQL numbers when host-network or native Postgres is available.

What is already in place:

- HTTP runtime now supports deferred completion on the owning reactor;
- generated OpenAPI routers can dispatch into optional `*_async(..., async_response_writer)` handlers;
- generated SQL repositories already expose `*_async(...)` methods in parallel with the sync API.

What is still missing:

- a default async write path that actually improves durable mixed CRUD instead of only shifting where the wait happens;
- better separation between reactor I/O progress and heavy completion work where that matters for tail latency.

## What will not work

These changes do not solve the fundamental issue on their own:

- adding more string micro-optimizations in handlers;
- moving sync DB work to another thread pool and immediately blocking on a future from the same HTTP worker;
- replacing prepared statements with ad-hoc SQL strings;
- adding more HTTP workers without a matching DB strategy.

They may shift p50 a bit, but they do not remove head-of-line blocking from the request path.

## Practical migration path

### Phase 1: benchmark hygiene

Keep benchmark results honest and reproducible before touching runtime shape:

- use `cpu_governor=performance`;
- use host-network or native Postgres for reference SQL runs;
- keep explicit seed + `ANALYZE`;
- keep Stage 17 and Stage 18 on repeated median-of-N runner policy.

### Phase 2: async HTTP completion path

Status: implemented.

Before async SQL is useful, HTTP needs a way to finish a response after the initial dispatch frame returns.

Minimal runtime capability needed:

- detach request processing from the current worker frame;
- resume completion on the owning reactor safely;
- support timeout/cancel/connection-close handling;
- preserve arena/request-context lifetime until completion.

Without this layer, an async PostgreSQL client still collapses back into sync behavior at the handler boundary.

### Phase 3: nonblocking PostgreSQL executor

Status: implemented for handler-scoped generated `query_async/exec_async`, still being hardened/perf-tuned.

Async executor shape is built around nonblocking `libpq` primitives:

- `PQconnectStart` / `PQconnectPoll`;
- `PQsetnonblocking`;
- `PQsocket`;
- `PQsendQueryPrepared` or `PQsendQueryParams`;
- `PQflush`;
- `PQconsumeInput`;
- `PQisBusy`;
- `PQgetResult`.

Executor shape should stay reactor-friendly:

- one async connection object bound to one reactor;
- fd readiness drives progress;
- no global mutex around a connection;
- statement preparation cached per connection as today.

### Phase 4: generated async repository API

Status: implemented.

Once async executor exists, generated repositories should expose async methods in parallel with the sync API:

- `get_item_async(...)`
- `list_items_page_async(...)`
- `create_item_async(...)`
- `update_item_async(...)`
- `delete_item_async(...)`

The sync repository can stay as a compatibility adapter over the same SQL contract.

### Phase 5: write-side batching and pipelining

Async I/O alone will improve queueing, but mixed CRUD throughput is still write-heavy and commit-sensitive.

After async executor is stable, the next real wins are:

- pipeline multiple write requests per connection when safe;
- keep transaction-scoped batching explicit;
- optionally support benchmark-only or service-level commit coalescing policies where semantics allow it.

## Recommended implementation order

1. Keep the current async HTTP completion and generated async handler contract stable.
2. Introduce or harden a dedicated `sql::async_executor` interface without breaking the current sync executor.
3. Keep hardening the reactor-driven nonblocking Postgres connection and remove unnecessary fd-watch churn.
4. Keep generated async repository emission aligned with the executor contract.
5. Convert `benchmark_api` read path first, then re-evaluate write path only if it improves Stage 18.
6. Re-benchmark Stage 17 and Stage 18 in both default cold mode and steady-state reuse-server mode before touching further SQL ergonomics.

## Success criteria

The migration is paying off only if it changes the observed shape of the benchmarks:

- Stage 17 p95/p99 should fall without needing more worker oversubscription.
- Stage 18 throughput should increase with lower tail latency at the same CRUD mix.
- Mixed CRUD should stop being dominated by HTTP worker queueing.
- Perf profiles should show less time attributed to blocked request threads and less sensitivity to worker-count tuning.
