# Pulse — comprehensive KATANA service

A single binary that exercises (nearly) the whole framework, and serves **two independently
generated OpenAPI contracts** at once:

| Contract (namespace) | Endpoints | Policies |
|---|---|---|
| `catalog` | `POST /products`, `GET /products`, `GET /products/{id}` | rate-limit writes, cache list |
| `analytics` | `POST /events`, `GET /analytics/summary` | rate-limit + idempotency, cache summary |

Both contracts are generated with `katana_gen --namespace …` into `generated/<contract>/`, so they
link into one binary with no symbol collisions, and are mounted on one server through the core
`katana::http::composite_router` (per-route metrics span all contracts via globalised indices).

## Framework features wired here

- **Multi-contract codegen** — two OpenAPI + two SQL schemas, namespaced (`catalog`, `analytics`,
  `katana::sql::catalog`, `katana::sql::analytics`).
- **Two PostgreSQL pools** with independent `statement_timeout`s, generated SQL repositories.
- **x-katana policies on Redis** — response cache (with an in-process L1 tier), rate limit,
  idempotency (falls back to in-memory if Redis is down).
- **Config** — defaults < `PULSE_CONFIG` file < `PULSE_*` env < `--flags`, `*_FILE` secrets,
  `SIGHUP` hot-reload hook.
- **Observability** — structured access log with sampling, W3C distributed tracing (spans per
  request), Prometheus `/metrics` (request-duration histogram, connection + reactor gauges),
  `/healthz` + `/readyz` (pings both DBs and Redis).
- **Edge** — CORS for a SPA/Next.js frontend, connection read/idle timeouts, a max-connections
  cap, graceful shutdown.

## Run

```sh
# from the repo root, build the framework + generator once:
cmake --build --preset debug --target katana_gen katana_core

# create the schema (any reachable Postgres):
psql "$PG_DSN" -f examples/services/pulse/schema.sql

# generate + compile the service:
KATANA_ROOT="$PWD" examples/services/pulse/build.sh /tmp/pulse

# run it:
PG_DSN=postgresql://user@host:5432/db PULSE_PORT=8080 PULSE_WORKERS=4 /tmp/pulse
```

Then e.g. `curl -X POST localhost:8080/products -d '{"sku":"A","name":"X","price_cents":100}'`,
`curl localhost:8080/analytics/summary`, `curl localhost:8080/metrics`.

> Two real framework bugs were found and fixed while building this service: an idempotency
> null-key dereference that segfaulted under load, and rate-limit units (`/second`) being parsed
> as `unknown` and silently disabling enforcement.
