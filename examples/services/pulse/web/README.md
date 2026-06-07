# Pulse web — Next.js frontend

A small Next.js (App Router, TypeScript) dashboard for the KATANA **Pulse** service. It talks to
the C++ backend **cross-origin** — the backend enables CORS (`server.cors()`), so the browser can
call it directly.

What it shows:

- **Catalog** — list products (polls the cached `GET /products`) and create new ones
  (`POST /products`, rate-limited server-side).
- **Analytics** — per-kind event counts (cached `GET /analytics/summary`) and an ingest form
  (`POST /events`, rate-limited + idempotent server-side).

## Run

```sh
# 1) start the backend (from the repo root) — see ../README.md
PG_DSN=postgresql://user@host:5432/db PULSE_PORT=8080 /tmp/pulse

# 2) start the frontend
cd examples/services/pulse/web
npm install
NEXT_PUBLIC_PULSE_URL=http://localhost:8080 npm run dev   # http://localhost:3000
```

Point `NEXT_PUBLIC_PULSE_URL` at wherever Pulse is listening (default `http://localhost:8080`).

The client lives in [`lib/api.ts`](lib/api.ts); the dashboard is [`app/page.tsx`](app/page.tsx).

> Note: the pinned `next` 14.2.x carries an upstream advisory for its dev server; this is a demo,
> not production. Bump to the latest `next` if you deploy it.
