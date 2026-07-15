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

- **Multi-contract codegen** — two OpenAPI + two SQL schemas, each contract's DTOs and SQL
  rows/repository sharing one namespace (`catalog`, `analytics`).
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

This is also the **reference fullstack template**: a Next.js frontend (`web/`) drives the C++
backend through a **generated** TypeScript client, and the backend maps SQL rows to API DTOs with
the **generated Row↔DTO bridge** — no hand-written glue on either side.

## Run — one command

From the repo root (needs Docker, CMake+Ninja, and Node/npm):

```sh
make dev            # or: ./tools/katana dev
```

That brings up Postgres + Redis (via `docker-compose.yml`, schema auto-applied), regenerates the
contracts and builds the backend, then starts both the API (`:8080`) and the web console (`:3000`),
tailing both. `Ctrl-C` tears everything down. `make up` / `make down` manage just the deps.

## Run — manual

```sh
cmake -S examples/services/pulse -B examples/services/pulse/build && \
  cmake --build examples/services/pulse/build      # auto-finds the KATANA repo; generates everything

psql "$PG_DSN" -f examples/services/pulse/schema.sql
PG_DSN=postgresql://user@host:5432/db examples/services/pulse/build/pulse

# frontend (separate terminal):
cd examples/services/pulse/web && npm install && npm run dev   # NEXT_PUBLIC_PULSE_URL → :8080
```

Then e.g. `curl -X POST localhost:8080/products -d '{"sku":"A","name":"X","price_cents":100}'`,
`curl localhost:8080/analytics/summary`, `curl localhost:8080/metrics`.

## Frontend

`web/` is a Next.js console (both light and dark themes). Its API layer is the generated client —
the build regenerates `web/lib/catalog/generated_client.ts` and `web/lib/analytics/generated_client.ts`
from the OpenAPI contracts (`katana_add_typescript`); `web/lib/api.ts` is only a thin facade that
points them at one base URL. Don't hand-edit the generated files.

> Two real framework bugs were found and fixed while building this service: an idempotency
> null-key dereference that segfaulted under load, and rate-limit units (`/second`) being parsed
> as `unknown` and silently disabling enforcement.
