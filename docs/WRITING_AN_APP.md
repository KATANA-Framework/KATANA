# Writing an application on KATANA

This guide walks through building a real SQL-backed JSON API from scratch. The
example is a small shop: customers, products, orders, and order items, with a few
endpoints that do joins, aggregates and a write. The full source lives in
`examples/codegen/shop_api/`.

The idea behind KATANA is contract-first. You describe two things:

1. your SQL queries (in `.sql` files),
2. your HTTP API (in an OpenAPI YAML file),

and the code generator produces the typed C++ in between: a repository for the
queries and request parsing, validation, routing and handler interfaces for the
API. You write the database schema, the queries, the spec, and the handler bodies
that glue a query result to a response. Everything else is generated.

You need: CMake ≥ 3.20, Ninja, GCC ≥ 12 or Clang ≥ 16, and `libpq` for PostgreSQL.

## 1. The database schema

Start with plain SQL. KATANA does not invent its own schema language — you own the
tables.

```sql
-- examples/codegen/shop_api/schema.sql
CREATE TABLE shop_customers (
  id bigserial PRIMARY KEY, name text NOT NULL, email text NOT NULL UNIQUE,
  country text NOT NULL, created_at timestamptz NOT NULL DEFAULT now());
CREATE TABLE shop_products (
  id bigserial PRIMARY KEY, name text NOT NULL, category text NOT NULL,
  price float8 NOT NULL, stock bigint NOT NULL DEFAULT 0);
CREATE TABLE shop_orders (
  id bigserial PRIMARY KEY, customer_id bigint NOT NULL REFERENCES shop_customers(id),
  status text NOT NULL, created_at timestamptz NOT NULL DEFAULT now());
CREATE TABLE shop_order_items (
  id bigserial PRIMARY KEY, order_id bigint NOT NULL REFERENCES shop_orders(id),
  product_id bigint NOT NULL REFERENCES shop_products(id),
  qty bigint NOT NULL, unit_price float8 NOT NULL);
```

Apply it to your database with `psql`. The generator never touches your live DB; it
only reads the `.sql` query files.

## 2. The queries

Put one query per file in a directory. Each file starts with a name comment that
tells the generator the function name and how many rows come back:

- `:one` — returns zero or one row (`std::optional<Row>`),
- `:many` — returns many rows (`std::vector<Row>`),
- `:exec` — runs a statement, returns affected-row count.

Every selected column needs an explicit `::type AS name` cast. That is how the
generator learns the column's type and name without connecting to the database.

```sql
-- examples/codegen/shop_api/sql/customer_revenue_ranked.sql
-- name: customer_revenue_ranked :many
SELECT
  c.id::bigint AS customer_id,
  c.name::text AS name,
  c.country::text AS country,
  COALESCE(SUM(oi.qty * oi.unit_price), 0)::float8 AS revenue,
  COUNT(DISTINCT o.id)::bigint AS order_count,
  RANK() OVER (ORDER BY COALESCE(SUM(oi.qty * oi.unit_price), 0) DESC)::bigint AS revenue_rank
FROM shop_customers c
LEFT JOIN shop_orders o ON o.customer_id = c.id
LEFT JOIN shop_order_items oi ON oi.order_id = o.id
WHERE c.country = $1::text
GROUP BY c.id, c.name, c.country
ORDER BY revenue DESC
LIMIT $2::bigint OFFSET $3::bigint;
```

Joins, `GROUP BY`, window functions (`RANK() OVER (...)`), CTEs (`WITH`), and bulk
inserts with `UNNEST($1::bigint[], ...)` all work. Parameters are `$1`, `$2`, … with
a `::type` cast that decides the C++ argument type.

Type mapping: `bigint → int64_t`, `float8 → double`, `text → std::string`,
`bool → bool`, and array types like `bigint[] → std::vector<int64_t>`.

## 3. Generate the repository

```bash
katana_gen sql -i examples/codegen/shop_api/sql -o examples/codegen/shop_api/generated
```

You get `generated_sql_models.hpp` (a `Row` struct per query) and
`generated_sql_repository.hpp` (one method per query). The methods look like this:

```cpp
namespace katana::sql::generated {
class generated_repository {
public:
    explicit generated_repository(katana::sql::executor& executor) noexcept;

    katana::result<std::vector<CustomerRevenueRankedRow>>
    customer_revenue_ranked(std::string_view p1, int64_t p2, int64_t p3) const;

    katana::result<std::optional<OrderDetailRow>> order_detail(int64_t p1) const;
    // ...
};
}
```

`result<T>` carries either the value or an error, so you check it instead of
catching exceptions. Note the parameters are positional (`p1, p2, p3`) in the order
of `$1, $2, $3`.

## 4. The HTTP contract

Describe the endpoints in OpenAPI 3.0. Keep the response schemas close to your query
rows so the wiring is short.

```yaml
# examples/codegen/shop_api/api.yaml
paths:
  /shop/customers/revenue:
    get:
      operationId: customer_revenue
      parameters:
        - name: country
          in: query
          required: true
          schema: { type: string, minLength: 2, maxLength: 2 }
        - name: limit
          in: query
          required: true
          schema: { type: integer, minimum: 1, maximum: 200 }
        - name: offset
          in: query
          required: true
          schema: { type: integer, minimum: 0 }
      responses:
        '200':
          description: Ranked customers
          content:
            application/json:
              schema: { $ref: '#/components/schemas/CustomerRevenueList' }
```

Both indentation (block) style and inline (`{ ... }`) flow style are supported, and
you can mix them.

## 5. Generate the API

```bash
katana_gen openapi -i examples/codegen/shop_api/api.yaml -o examples/codegen/shop_api/generated_api --emit all
```

This produces DTO structs, JSON parsers and serializers, validators, the route
table, and the handler interface. The handler interface is what you implement:

```cpp
struct api_handler {
    // GET /shop/customers/revenue
    virtual katana::result<void>
    customer_revenue(std::string_view country, int64_t limit, int64_t offset,
                     response& out) = 0;

    // GET /shop/orders/{id}
    virtual katana::result<void> order_detail(int64_t id, response& out) = 0;
    // ...
};
```

Path and query parameters arrive as typed arguments, already parsed and validated
against the `minimum`/`maxLength`/etc. constraints from the spec. The request body,
if any, arrives as a parsed DTO. You do not write any parsing or validation code.

## 6. Implement the handlers

A handler takes the typed arguments, calls the repository, turns the rows into the
response DTO, and serializes. For example:

```cpp
katana::result<void>
shop_handlers::customer_revenue(std::string_view country, int64_t limit,
                                int64_t offset, response& out) {
    auto rows = repo_.customer_revenue_ranked(country, limit, offset);
    if (!rows) {
        out.assign_error(katana::problem_details::internal_error("query failed"));
        return {};
    }

    CustomerRevenueList dto;
    for (const auto& r : *rows) {
        CustomerRevenue item;
        item.customer_id = r.customer_id;
        item.name = r.name;
        item.country = r.country;
        item.revenue = r.revenue;
        item.order_count = r.order_count;
        item.revenue_rank = r.revenue_rank;
        dto.items.push_back(std::move(item));
    }
    out.assign_json(serialize_CustomerRevenueList(dto));
    return {};
}
```

The repository row and the response DTO are separate structs today, so you copy
field by field. They usually have the same fields, so it is mechanical.

## 7. Wire up the database and the server

The SQL runtime uses a connection pool. Build it once at startup and hand each
reactor thread its own executor:

```cpp
#include "katana/sql/postgres.hpp"

katana::sql::postgres_pool pool({
    .postgres = { .connection_string = "postgresql://katana:katana@127.0.0.1:5432/shop" },
    .executor_count = 4,
    .eager_connect = true,
});

// one executor per worker thread
auto& executor = pool.for_reactor(thread_index);
katana::sql::generated::generated_repository repo(executor);
```

The executor is single-connection and not shared between threads — `for_reactor`
gives each thread its own, so there is no locking on the query path. Then construct
the generated router with your handler and run the HTTP server (see
`examples/codegen/benchmark_api/main.cpp` for a complete server setup).

## 8. Build it

Add the example to CMake the same way the other examples do, or compile a standalone
binary linking `katana_core` and `libpq`:

```bash
c++ -O3 -std=c++23 -march=native -DKATANA_HAS_LIBPQ -DKATANA_USE_EPOLL \
    -Ikatana/core/include -Iexamples/codegen/shop_api/generated \
    my_server.cpp build/<preset>/libkatana_core.a -lpq -pthread -o my_server
```

## Caching and rate limiting (policies)

You can mark routes for response caching, rate limiting, or idempotency right in the
OpenAPI spec with `x-katana-*` annotations on an operation:

```yaml
  /products/top:
    get:
      operationId: top_products
      x-katana-cache: true            # cache the response
      x-katana-rate-limit: "1000/second"
      responses:
        '200': { description: ok, content: { application/json: { schema: { $ref: '#/components/schemas/TopProductList' } } } }
```

The generator attaches these to the route. At runtime they do nothing until you give the
server a policy executor. For a single instance, the in-memory one is enough:

```cpp
#include "katana/core/contract_policies.hpp"

in_memory_contract_policy_executor policies;
server(router).policy_executor(policies).listen(port).run();
```

To share cache / rate-limit / idempotency state across several instances, back it with
Redis. Use a pool so each worker thread gets its own connection (parallel and
thread-safe) — a single shared connection would serialize every request on one
round-trip and is not safe across threads:

```cpp
#include "katana/core/contract_policies.hpp"

redis_pool pool(workers, redis_sync_client::options{.host = "127.0.0.1", .port = 6379});
if (auto c = pool.connect_all(); !c) { /* handle error */ }

redis_contract_policy_executor policies(pool);   // owns the Redis-backed stores
server(router).policy_executor(policies).listen(port).workers(workers).run();
```

A cached route returns `X-Katana-Cache: HIT` on a hit and skips the handler. Note that a
cache hit or rate-limit check still does one synchronous Redis round-trip per request, so
a Redis-backed route is slower than one with no policy — size the pool to your worker
count and keep policies on the routes that need them.

## Things worth knowing

- Every selected column must have a `::type AS name` cast. If you forget it, the
  generator can't infer the type.
- Keep response schemas aligned with query rows. The closer they are, the less glue
  you write in the handler.
- The SQL runtime is single-connection-per-executor. Always go through `for_reactor`
  or `current_executor`; do not share one executor across threads.
- `result<T>` is checked, not thrown. Handle the error branch in every handler.
- Use `katana_gen ... --dump-ast --json` to inspect exactly what the generator
  parsed (columns, types, parameters). With `--json` the JSON goes to stdout and the
  log lines go to stderr, so you can pipe it straight into a JSON tool.

## Where to look next

- `examples/codegen/benchmark_api/` — a full SQL-backed server with CRUD, used in the
  framework benchmarks.
- `docs/CODEGEN.md` — the code generator in more detail.
- `docs/OPENAPI.md` — the supported subset of OpenAPI.
- `docs/HTTP_SERVER.md` — the server, reactors and connection handling.
