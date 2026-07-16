import os
from contextlib import asynccontextmanager
from time import monotonic
from typing import Literal
from uuid import UUID

import psycopg
from psycopg.rows import dict_row
from psycopg_pool import AsyncConnectionPool
from fastapi import Cookie, FastAPI, Header, HTTPException, Query, Request
from fastapi.responses import PlainTextResponse
from pydantic import BaseModel, Field
import uvicorn


DEFAULT_PORT = 8080
DEFAULT_WORKERS = 4
MAX_ITEMS = 1024
DEFAULT_SQL_SEED_COUNT = 4096
VALID_CATEGORIES = {"electronics", "books", "clothing", "food", "tools", "other"}

CREATE_TABLE_SQL = """\
CREATE TABLE IF NOT EXISTS katana_stage4_items (
  id BIGSERIAL PRIMARY KEY,
  name TEXT NOT NULL,
  description TEXT,
  price DOUBLE PRECISION NOT NULL,
  stock BIGINT,
  category TEXT NOT NULL,
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
)
"""
CATEGORY_INDEX_SQL = """\
CREATE INDEX IF NOT EXISTS katana_stage4_items_category_id_cover_idx
ON katana_stage4_items (category, id) INCLUDE (name, description, price, stock)
"""
DISABLE_AUTOVACUUM_SQL = """\
ALTER TABLE katana_stage4_items SET (autovacuum_enabled = false, toast.autovacuum_enabled = false)
"""
RESTORE_AUTOVACUUM_SQL = """\
ALTER TABLE katana_stage4_items RESET (autovacuum_enabled, toast.autovacuum_enabled)
"""
RESET_ITEMS_SQL = "TRUNCATE katana_stage4_items RESTART IDENTITY"
ANALYZE_ITEMS_SQL = "ANALYZE katana_stage4_items"
SEED_ITEMS_SQL = """\
INSERT INTO katana_stage4_items (name, description, price, stock, category)
SELECT
  'Seed-' || gs::text,
  'Seed item',
  10.0 + (gs %% 100)::double precision,
  100 + (gs %% 50)::bigint,
  CASE (gs %% 6)
    WHEN 0 THEN 'electronics'
    WHEN 1 THEN 'books'
    WHEN 2 THEN 'clothing'
    WHEN 3 THEN 'food'
    WHEN 4 THEN 'tools'
    ELSE 'other'
  END
FROM generate_series(1, %s::bigint) AS gs
"""
LIST_ITEMS_ALL_SQL = """\
SELECT
  page.id::bigint AS id,
  page.name::text AS name,
  page.description::text AS description,
  page.price::float8 AS price,
  page.stock::bigint AS stock,
  page.category::text AS category_name,
  counts.total_count::bigint AS total_count
FROM (
  SELECT COUNT(*)::bigint AS total_count
  FROM katana_stage4_items
) AS counts
LEFT JOIN LATERAL (
  SELECT id, name, description, price, stock, category
  FROM katana_stage4_items
  ORDER BY id
  LIMIT %s::bigint OFFSET %s::bigint
) AS page ON TRUE
"""
LIST_ITEMS_BY_CATEGORY_SQL = """\
SELECT
  page.id::bigint AS id,
  page.name::text AS name,
  page.description::text AS description,
  page.price::float8 AS price,
  page.stock::bigint AS stock,
  page.category::text AS category_name,
  counts.total_count::bigint AS total_count
FROM (
  SELECT COUNT(*)::bigint AS total_count
  FROM katana_stage4_items
  WHERE category = %s::text
) AS counts
LEFT JOIN LATERAL (
  SELECT id, name, description, price, stock, category
  FROM katana_stage4_items
  WHERE category = %s::text
  ORDER BY id
  LIMIT %s::bigint OFFSET %s::bigint
) AS page ON TRUE
"""
GET_ITEM_SQL = """\
SELECT
  id::bigint AS id,
  name::text AS name,
  description::text AS description,
  price::float8 AS price,
  stock::bigint AS stock,
  category::text AS category_name
FROM katana_stage4_items
WHERE id = %s::bigint
"""
CREATE_ITEM_SQL = """\
INSERT INTO katana_stage4_items (
  name,
  description,
  price,
  stock,
  category
)
VALUES (
  %s::text,
  CASE WHEN %s::bool THEN %s::text ELSE NULL END,
  %s::float8,
  CASE WHEN %s::bool THEN %s::bigint ELSE NULL END,
  %s::text
)
RETURNING
  id::bigint AS id,
  name::text AS name,
  description::text AS description,
  price::float8 AS price,
  stock::bigint AS stock,
  category::text AS category_name
"""
UPDATE_ITEM_SQL = """\
UPDATE katana_stage4_items
SET
  name = CASE WHEN %s::bool THEN %s::text ELSE name END,
  description = CASE WHEN %s::bool THEN %s::text ELSE description END,
  price = CASE WHEN %s::bool THEN %s::float8 ELSE price END,
  stock = CASE WHEN %s::bool THEN %s::bigint ELSE stock END,
  category = CASE WHEN %s::bool THEN %s::text ELSE category END,
  updated_at = NOW()
WHERE id = %s::bigint
RETURNING
  id::bigint AS id,
  name::text AS name,
  description::text AS description,
  price::float8 AS price,
  stock::bigint AS stock,
  category::text AS category_name
"""
DELETE_ITEM_SQL = "DELETE FROM katana_stage4_items WHERE id = %s::bigint"


def read_port() -> int:
    for key in ("PORT", "HELLO_PORT", "COMPUTE_PORT"):
        value = os.getenv(key)
        if value:
            try:
                port = int(value)
            except ValueError:
                continue
            if 0 < port < 65536:
                return port
    return DEFAULT_PORT


def read_workers() -> int:
    for key in ("BENCH_WORKERS", "KATANA_WORKERS", "WORKERS"):
        value = os.getenv(key)
        if value:
            try:
                workers = int(value)
            except ValueError:
                continue
            if workers > 0:
                return workers
    return DEFAULT_WORKERS


def read_postgres_dsn() -> str | None:
    for key in ("KATANA_BENCHMARK_API_POSTGRES_DSN", "KATANA_TEST_POSTGRES_DSN"):
        value = os.getenv(key)
        if value and value.strip():
            return value.strip()
    return None


def read_bool_env(name: str, fallback: bool) -> bool:
    value = os.getenv(name)
    if not value:
        return fallback
    normalized = value.strip().lower()
    if normalized in {"1", "true", "yes"}:
        return True
    if normalized in {"0", "false", "no"}:
        return False
    return fallback


def read_size_env(name: str, fallback: int) -> int:
    value = os.getenv(name)
    if not value:
        return fallback
    try:
        parsed = int(value)
    except ValueError:
        return fallback
    return parsed if parsed >= 0 else fallback


def normalize_optional_text(value: str | None) -> str | None:
    if value is None:
        return None
    normalized = value.strip()
    return normalized or None


class ItemModel(BaseModel):
    id: int
    name: str
    description: str | None = None
    price: float
    stock: int | None = None
    category: Literal["electronics", "books", "clothing", "food", "tools", "other"]


class CreateItemRequest(BaseModel):
    name: str = Field(min_length=1, max_length=200)
    description: str | None = Field(default=None, max_length=2000)
    price: float = Field(ge=0.0, lt=1_000_000.0)
    stock: int | None = Field(default=None, ge=0, le=999_999)
    category: Literal["electronics", "books", "clothing", "food", "tools", "other"]


class UpdateItemRequest(BaseModel):
    name: str | None = Field(default=None, min_length=1, max_length=200)
    description: str | None = Field(default=None, max_length=2000)
    price: float | None = Field(default=None, ge=0.0)
    stock: int | None = Field(default=None, ge=0, le=999_999)
    category: Literal["electronics", "books", "clothing", "food", "tools", "other"] | None = None


class ItemListResponse(BaseModel):
    items: list[ItemModel]
    total: int
    limit: int
    offset: int


class HealthResponse(BaseModel):
    status: str
    uptime_ms: int


class JsonMessageResponse(BaseModel):
    message: str


class EchoRequest(BaseModel):
    message: str = Field(max_length=4096)
    repeat: int | None = Field(default=None, ge=1, le=100)
    uppercase: bool | None = None


class EchoResponse(BaseModel):
    message: str
    length: int


class StatsRequest(BaseModel):
    values: list[float] = Field(min_length=1, max_length=10_000)
    include_median: bool | None = None


class StatsResponse(BaseModel):
    min: float
    max: float
    mean: float
    sum: float
    count: int
    median: float | None = None


def row_to_item(row: dict) -> ItemModel:
    return ItemModel(
        id=int(row["id"]),
        name=str(row["name"]),
        description=row["description"],
        price=float(row["price"]),
        stock=None if row["stock"] is None else int(row["stock"]),
        category=str(row["category_name"]),
    )


def bootstrap_database(dsn: str) -> None:
    with psycopg.connect(dsn, autocommit=True) as conn:
        with conn.cursor() as cur:
            cur.execute(CREATE_TABLE_SQL)
            cur.execute(CATEGORY_INDEX_SQL)

            if read_bool_env("KATANA_BENCHMARK_API_DISABLE_AUTOVACUUM", False):
                cur.execute(DISABLE_AUTOVACUUM_SQL)
            else:
                cur.execute(RESTORE_AUTOVACUUM_SQL)

            if read_bool_env("KATANA_BENCHMARK_API_RESET", True):
                cur.execute(RESET_ITEMS_SQL)

            seed_count = read_size_env("KATANA_BENCHMARK_API_SEED_COUNT", DEFAULT_SQL_SEED_COUNT)
            if seed_count > 0:
                with conn.transaction():
                    cur.execute(SEED_ITEMS_SQL, (seed_count,))
                cur.execute(ANALYZE_ITEMS_SQL)


@asynccontextmanager
async def lifespan(app: FastAPI):
    app.state.started_at = monotonic()
    dsn = read_postgres_dsn()
    app.state.pool = None
    if dsn:
        workers = read_workers()
        pool = AsyncConnectionPool(
            conninfo=dsn,
            min_size=1,
            max_size=max(4, min(workers * 4, 64)),
            open=False,
            kwargs={"row_factory": dict_row},
        )
        await pool.open()
        app.state.pool = pool

    try:
        yield
    finally:
        pool = getattr(app.state, "pool", None)
        if pool is not None:
            await pool.close()


app = FastAPI(docs_url=None, redoc_url=None, openapi_url=None, lifespan=lifespan)


@app.get("/", response_class=PlainTextResponse)
async def hello() -> str:
    return "Hello, World!"


@app.post("/compute/sum")
async def compute_sum(nums: list[float]) -> float:
    if not 1 <= len(nums) <= MAX_ITEMS:
        raise HTTPException(status_code=400, detail="payload must contain 1..=1024 numbers")
    return float(sum(nums))


@app.get("/json", response_model=JsonMessageResponse)
async def json_message() -> JsonMessageResponse:
    return JsonMessageResponse(message="Hello, World!")


@app.post("/echo", response_model=EchoResponse)
async def echo(body: EchoRequest) -> EchoResponse:
    repeat = body.repeat if body.repeat is not None else 1
    payload = body.message * repeat
    if body.uppercase:
        payload = payload.upper()
    return EchoResponse(message=payload, length=len(payload))


@app.post("/compute/stats", response_model=StatsResponse, response_model_exclude_none=True)
async def compute_stats(body: StatsRequest) -> StatsResponse:
    values = body.values
    total = 0.0
    min_value = values[0]
    max_value = values[0]
    for value in values:
        total += value
        if value < min_value:
            min_value = value
        if value > max_value:
            max_value = value
    mean = total / len(values)

    median: float | None = None
    if body.include_median:
        sorted_values = sorted(values)
        size = len(sorted_values)
        if size % 2 == 0:
            median = (sorted_values[size // 2 - 1] + sorted_values[size // 2]) * 0.5
        else:
            median = sorted_values[size // 2]

    return StatsResponse(
        min=min_value,
        max=max_value,
        mean=mean,
        sum=total,
        count=len(values),
        median=median,
    )


def require_pool(request: Request) -> AsyncConnectionPool:
    pool = getattr(request.app.state, "pool", None)
    if pool is None:
        raise HTTPException(status_code=503, detail="postgres is not configured for this target")
    return pool


@app.get("/health", response_model=HealthResponse)
async def health(request: Request) -> HealthResponse:
    uptime_ms = int((monotonic() - request.app.state.started_at) * 1000)
    return HealthResponse(status="ok", uptime_ms=uptime_ms)


@app.get("/items", response_model=ItemListResponse)
async def list_items(
    request: Request,
    limit: int = Query(default=10, ge=1, le=100),
    offset: int = Query(default=0, ge=0),
    category: Literal["electronics", "books", "clothing", "food", "tools", "other"] | None = None,
) -> ItemListResponse:
    pool = require_pool(request)
    async with pool.connection() as conn:
        async with conn.cursor() as cur:
            if category is None:
                await cur.execute(LIST_ITEMS_ALL_SQL, (limit, offset))
            else:
                await cur.execute(LIST_ITEMS_BY_CATEGORY_SQL, (category, category, limit, offset))
            rows = await cur.fetchall()

    total = 0
    items: list[ItemModel] = []
    for row in rows:
        if row["total_count"] is not None:
            total = int(row["total_count"])
        if row["id"] is None:
            continue
        items.append(row_to_item(row))
    return ItemListResponse(items=items, total=total, limit=limit, offset=offset)


@app.get("/items/{item_id}", response_model=ItemModel)
async def get_item(request: Request, item_id: int) -> ItemModel:
    pool = require_pool(request)
    async with pool.connection() as conn:
        async with conn.cursor() as cur:
            await cur.execute(GET_ITEM_SQL, (item_id,))
            row = await cur.fetchone()
    if row is None:
        raise HTTPException(status_code=404, detail="item not found")
    return row_to_item(row)


@app.post("/items", response_model=ItemModel, status_code=201)
async def create_item(
    request: Request,
    body: CreateItemRequest,
    x_request_id: UUID = Header(alias="X-Request-Id"),
    session: str | None = Cookie(default=None),
) -> ItemModel:
    del x_request_id
    del session
    pool = require_pool(request)
    description = normalize_optional_text(body.description)
    stock = body.stock if body.stock is not None else 0
    async with pool.connection() as conn:
        async with conn.cursor() as cur:
            await cur.execute(
                CREATE_ITEM_SQL,
                (
                    body.name,
                    description is not None,
                    description or "",
                    body.price,
                    body.stock is not None,
                    stock,
                    body.category,
                ),
            )
            row = await cur.fetchone()
    if row is None:
        raise HTTPException(status_code=500, detail="failed to create item")
    return row_to_item(row)


@app.put("/items/{item_id}", response_model=ItemModel)
async def update_item(request: Request, item_id: int, body: UpdateItemRequest) -> ItemModel:
    pool = require_pool(request)
    description = normalize_optional_text(body.description)
    async with pool.connection() as conn:
        async with conn.cursor() as cur:
            await cur.execute(
                UPDATE_ITEM_SQL,
                (
                    body.name is not None,
                    body.name or "",
                    description is not None,
                    description or "",
                    body.price is not None,
                    body.price or 0.0,
                    body.stock is not None,
                    body.stock or 0,
                    body.category is not None,
                    body.category or "",
                    item_id,
                ),
            )
            row = await cur.fetchone()
    if row is None:
        raise HTTPException(status_code=404, detail="item not found")
    return row_to_item(row)


@app.delete("/items/{item_id}", status_code=204)
async def delete_item(request: Request, item_id: int) -> None:
    pool = require_pool(request)
    async with pool.connection() as conn:
        async with conn.cursor() as cur:
            await cur.execute(DELETE_ITEM_SQL, (item_id,))
            if cur.rowcount == 0:
                raise HTTPException(status_code=404, detail="item not found")


if __name__ == "__main__":
    port = read_port()
    workers = read_workers()
    dsn = read_postgres_dsn()
    if dsn and read_bool_env("KATANA_BENCHMARK_API_BOOTSTRAP", True):
        bootstrap_database(dsn)
    print(f"fastapi comparison server listening on :{port} with {workers} workers")
    uvicorn.run(
        "app:app",
        host="0.0.0.0",
        port=port,
        workers=workers,
        access_log=False,
        log_level="warning",
    )
