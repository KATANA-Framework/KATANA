use deadpool_postgres::{Manager, ManagerConfig, Pool, RecyclingMethod};
use ntex::http::StatusCode;
use ntex::web::{self, App, HttpRequest, HttpResponse, HttpServer};
use serde::{Deserialize, Serialize};
use std::env;
use std::io;
use std::str::FromStr;
use std::time::Instant;
use tokio_postgres::NoTls;
use uuid::Uuid;

const DEFAULT_PORT: u16 = 8080;
const DEFAULT_WORKERS: usize = 4;
const MAX_ITEMS: usize = 1024;
const DEFAULT_SQL_SEED_COUNT: i64 = 4096;
const VALID_CATEGORIES: &[&str] = &[
    "electronics",
    "books",
    "clothing",
    "food",
    "tools",
    "other",
];

const CREATE_TABLE_SQL: &str = "CREATE TABLE IF NOT EXISTS katana_stage4_items (\
id BIGSERIAL PRIMARY KEY, \
name TEXT NOT NULL, \
description TEXT, \
price DOUBLE PRECISION NOT NULL, \
stock BIGINT, \
category TEXT NOT NULL, \
created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), \
updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW())";
const CATEGORY_INDEX_SQL: &str = "CREATE INDEX IF NOT EXISTS katana_stage4_items_category_id_cover_idx \
ON katana_stage4_items (category, id) INCLUDE (name, description, price, stock)";
const DISABLE_AUTOVACUUM_SQL: &str =
    "ALTER TABLE katana_stage4_items SET (autovacuum_enabled = false, toast.autovacuum_enabled = false)";
const RESTORE_AUTOVACUUM_SQL: &str =
    "ALTER TABLE katana_stage4_items RESET (autovacuum_enabled, toast.autovacuum_enabled)";
const RESET_ITEMS_SQL: &str = "TRUNCATE katana_stage4_items RESTART IDENTITY";
const ANALYZE_ITEMS_SQL: &str = "ANALYZE katana_stage4_items";
const SEED_ITEMS_SQL: &str = "INSERT INTO katana_stage4_items (name, description, price, stock, category) \
SELECT \
'Seed-' || gs::text, \
'Seed item', \
10.0 + (gs % 100)::double precision, \
100 + (gs % 50)::bigint, \
CASE (gs % 6) \
  WHEN 0 THEN 'electronics' \
  WHEN 1 THEN 'books' \
  WHEN 2 THEN 'clothing' \
  WHEN 3 THEN 'food' \
  WHEN 4 THEN 'tools' \
  ELSE 'other' \
END \
FROM generate_series(1, $1::bigint) AS gs";
const LIST_ITEMS_ALL_SQL: &str = "SELECT \
page.id::bigint AS id, \
page.name::text AS name, \
page.description::text AS description, \
page.price::float8 AS price, \
page.stock::bigint AS stock, \
page.category::text AS category_name, \
counts.total_count::bigint AS total_count \
FROM ( \
  SELECT COUNT(*)::bigint AS total_count \
  FROM katana_stage4_items \
) AS counts \
LEFT JOIN LATERAL ( \
  SELECT id, name, description, price, stock, category \
  FROM katana_stage4_items \
  ORDER BY id \
  LIMIT $1::bigint OFFSET $2::bigint \
) AS page ON TRUE";
const LIST_ITEMS_BY_CATEGORY_SQL: &str = "SELECT \
page.id::bigint AS id, \
page.name::text AS name, \
page.description::text AS description, \
page.price::float8 AS price, \
page.stock::bigint AS stock, \
page.category::text AS category_name, \
counts.total_count::bigint AS total_count \
FROM ( \
  SELECT COUNT(*)::bigint AS total_count \
  FROM katana_stage4_items \
  WHERE category = $3::text \
) AS counts \
LEFT JOIN LATERAL ( \
  SELECT id, name, description, price, stock, category \
  FROM katana_stage4_items \
  WHERE category = $3::text \
  ORDER BY id \
  LIMIT $1::bigint OFFSET $2::bigint \
) AS page ON TRUE";
const GET_ITEM_SQL: &str = "SELECT \
id::bigint AS id, \
name::text AS name, \
description::text AS description, \
price::float8 AS price, \
stock::bigint AS stock, \
category::text AS category_name \
FROM katana_stage4_items \
WHERE id = $1::bigint";
const CREATE_ITEM_SQL: &str = "INSERT INTO katana_stage4_items ( \
name, description, price, stock, category \
) VALUES ( \
$1::text, \
CASE WHEN $2::bool THEN $3::text ELSE NULL END, \
$4::float8, \
CASE WHEN $5::bool THEN $6::bigint ELSE NULL END, \
$7::text \
) RETURNING \
id::bigint AS id, \
name::text AS name, \
description::text AS description, \
price::float8 AS price, \
stock::bigint AS stock, \
category::text AS category_name";
const UPDATE_ITEM_SQL: &str = "UPDATE katana_stage4_items \
SET \
name = CASE WHEN $2::bool THEN $3::text ELSE name END, \
description = CASE WHEN $4::bool THEN $5::text ELSE description END, \
price = CASE WHEN $6::bool THEN $7::float8 ELSE price END, \
stock = CASE WHEN $8::bool THEN $9::bigint ELSE stock END, \
category = CASE WHEN $10::bool THEN $11::text ELSE category END, \
updated_at = NOW() \
WHERE id = $1::bigint \
RETURNING \
id::bigint AS id, \
name::text AS name, \
description::text AS description, \
price::float8 AS price, \
stock::bigint AS stock, \
category::text AS category_name";
const DELETE_ITEM_SQL: &str = "DELETE FROM katana_stage4_items WHERE id = $1::bigint";

fn read_port() -> u16 {
    for key in ["PORT", "HELLO_PORT", "COMPUTE_PORT"] {
        if let Ok(value) = env::var(key) {
            if let Ok(port) = value.parse::<u16>() {
                if port > 0 {
                    return port;
                }
            }
        }
    }
    DEFAULT_PORT
}

fn read_workers() -> usize {
    for key in ["BENCH_WORKERS", "KATANA_WORKERS", "WORKERS"] {
        if let Ok(value) = env::var(key) {
            if let Ok(workers) = value.parse::<usize>() {
                if workers > 0 {
                    return workers;
                }
            }
        }
    }
    DEFAULT_WORKERS
}

fn read_size_env(name: &str, fallback: i64) -> i64 {
    match env::var(name) {
        Ok(value) => value
            .parse::<i64>()
            .ok()
            .filter(|parsed| *parsed >= 0)
            .unwrap_or(fallback),
        Err(_) => fallback,
    }
}

fn read_bool_env(name: &str, fallback: bool) -> bool {
    match env::var(name) {
        Ok(value) => match value.as_str() {
            "1" | "true" | "yes" => true,
            "0" | "false" | "no" => false,
            _ => fallback,
        },
        Err(_) => fallback,
    }
}

fn read_postgres_dsn() -> Option<String> {
    for key in [
        "KATANA_BENCHMARK_API_POSTGRES_DSN",
        "KATANA_TEST_POSTGRES_DSN",
    ] {
        if let Ok(value) = env::var(key) {
            let trimmed = value.trim();
            if !trimmed.is_empty() {
                return Some(trimmed.to_string());
            }
        }
    }
    None
}

fn normalize_optional_text(value: &str) -> Option<String> {
    let trimmed = value.trim();
    if trimmed.is_empty() {
        None
    } else {
        Some(trimmed.to_string())
    }
}

fn validate_category(value: &str) -> bool {
    VALID_CATEGORIES.iter().any(|candidate| *candidate == value)
}

type HttpError = (StatusCode, String);

#[derive(Clone)]
struct AppState {
    started_at: Instant,
    pool: Option<Pool>,
}

#[derive(Deserialize)]
struct ListItemsQuery {
    limit: Option<i64>,
    offset: Option<i64>,
    category: Option<String>,
}

#[derive(Deserialize)]
struct CreateItemRequest {
    name: String,
    description: Option<String>,
    price: f64,
    stock: Option<i64>,
    category: String,
}

#[derive(Deserialize)]
struct UpdateItemRequest {
    name: Option<String>,
    description: Option<String>,
    price: Option<f64>,
    stock: Option<i64>,
    category: Option<String>,
}

#[derive(Serialize)]
struct ItemResponse {
    id: i64,
    name: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    description: Option<String>,
    price: f64,
    #[serde(skip_serializing_if = "Option::is_none")]
    stock: Option<i64>,
    category: String,
}

#[derive(Serialize)]
struct ItemListResponse {
    items: Vec<ItemResponse>,
    total: i64,
    limit: i64,
    offset: i64,
}

#[derive(Serialize)]
struct HealthResponse {
    status: &'static str,
    uptime_ms: i64,
}

fn text_response(status: StatusCode, body: impl Into<String>) -> HttpResponse {
    HttpResponse::build(status)
        .content_type("text/plain; charset=utf-8")
        .body(body.into())
}

fn validate_create_item(body: &CreateItemRequest) -> Result<(), HttpError> {
    let name_len = body.name.chars().count();
    if name_len == 0 || name_len > 200 {
        return Err((
            StatusCode::BAD_REQUEST,
            "name must be 1..=200 characters".to_string(),
        ));
    }

    if let Some(description) = &body.description {
        if description.chars().count() > 2000 {
            return Err((
                StatusCode::BAD_REQUEST,
                "description must be <= 2000 characters".to_string(),
            ));
        }
    }

    if !(0.0..1_000_000.0).contains(&body.price) {
        return Err((
            StatusCode::BAD_REQUEST,
            "price must be >= 0 and < 1000000".to_string(),
        ));
    }

    if let Some(stock) = body.stock {
        if !(0..=999_999).contains(&stock) {
            return Err((
                StatusCode::BAD_REQUEST,
                "stock must be 0..=999999".to_string(),
            ));
        }
    }

    if !validate_category(&body.category) {
        return Err((StatusCode::BAD_REQUEST, "invalid category".to_string()));
    }

    Ok(())
}

fn validate_update_item(body: &UpdateItemRequest) -> Result<(), HttpError> {
    if let Some(name) = &body.name {
        let name_len = name.chars().count();
        if name_len == 0 || name_len > 200 {
            return Err((
                StatusCode::BAD_REQUEST,
                "name must be 1..=200 characters".to_string(),
            ));
        }
    }

    if let Some(description) = &body.description {
        if description.chars().count() > 2000 {
            return Err((
                StatusCode::BAD_REQUEST,
                "description must be <= 2000 characters".to_string(),
            ));
        }
    }

    if let Some(price) = body.price {
        if price < 0.0 {
            return Err((StatusCode::BAD_REQUEST, "price must be >= 0".to_string()));
        }
    }

    if let Some(stock) = body.stock {
        if !(0..=999_999).contains(&stock) {
            return Err((
                StatusCode::BAD_REQUEST,
                "stock must be 0..=999999".to_string(),
            ));
        }
    }

    if let Some(category) = &body.category {
        if !validate_category(category) {
            return Err((StatusCode::BAD_REQUEST, "invalid category".to_string()));
        }
    }

    Ok(())
}

fn map_item_row(row: &tokio_postgres::Row) -> Result<ItemResponse, HttpError> {
    Ok(ItemResponse {
        id: row
            .try_get::<_, i64>("id")
            .map_err(|error| (StatusCode::INTERNAL_SERVER_ERROR, error.to_string()))?,
        name: row
            .try_get::<_, String>("name")
            .map_err(|error| (StatusCode::INTERNAL_SERVER_ERROR, error.to_string()))?,
        description: row
            .try_get::<_, Option<String>>("description")
            .map_err(|error| (StatusCode::INTERNAL_SERVER_ERROR, error.to_string()))?,
        price: row
            .try_get::<_, f64>("price")
            .map_err(|error| (StatusCode::INTERNAL_SERVER_ERROR, error.to_string()))?,
        stock: row
            .try_get::<_, Option<i64>>("stock")
            .map_err(|error| (StatusCode::INTERNAL_SERVER_ERROR, error.to_string()))?,
        category: row
            .try_get::<_, String>("category_name")
            .map_err(|error| (StatusCode::INTERNAL_SERVER_ERROR, error.to_string()))?,
    })
}

fn require_pool(state: &AppState) -> Result<&Pool, HttpError> {
    state.pool.as_ref().ok_or((
        StatusCode::SERVICE_UNAVAILABLE,
        "postgres is not configured for this target".to_string(),
    ))
}

async fn bootstrap_database(pool: &Pool) -> Result<(), String> {
    let mut client = pool.get().await.map_err(|error| error.to_string())?;
    client
        .batch_execute(CREATE_TABLE_SQL)
        .await
        .map_err(|error| error.to_string())?;
    client
        .batch_execute(CATEGORY_INDEX_SQL)
        .await
        .map_err(|error| error.to_string())?;

    if read_bool_env("KATANA_BENCHMARK_API_DISABLE_AUTOVACUUM", false) {
        client
            .batch_execute(DISABLE_AUTOVACUUM_SQL)
            .await
            .map_err(|error| error.to_string())?;
    } else {
        client
            .batch_execute(RESTORE_AUTOVACUUM_SQL)
            .await
            .map_err(|error| error.to_string())?;
    }

    if read_bool_env("KATANA_BENCHMARK_API_RESET", true) {
        client
            .batch_execute(RESET_ITEMS_SQL)
            .await
            .map_err(|error| error.to_string())?;
    }

    let seed_count = read_size_env("KATANA_BENCHMARK_API_SEED_COUNT", DEFAULT_SQL_SEED_COUNT);
    if seed_count > 0 {
        let tx = client.transaction().await.map_err(|error| error.to_string())?;
        tx.execute(SEED_ITEMS_SQL, &[&seed_count])
            .await
            .map_err(|error| error.to_string())?;
        tx.commit().await.map_err(|error| error.to_string())?;
        client
            .batch_execute(ANALYZE_ITEMS_SQL)
            .await
            .map_err(|error| error.to_string())?;
    }

    Ok(())
}

fn make_pool(dsn: &str, workers: usize) -> Result<Pool, String> {
    let config = tokio_postgres::Config::from_str(dsn).map_err(|error| error.to_string())?;
    let manager = Manager::from_config(
        config,
        NoTls,
        ManagerConfig {
            recycling_method: RecyclingMethod::Fast,
        },
    );
    Ok(Pool::builder(manager)
        .max_size((workers.saturating_mul(4)).clamp(4, 64))
        .build()
        .map_err(|error| error.to_string())?)
}

async fn hello() -> HttpResponse {
    HttpResponse::Ok()
        .content_type("text/plain; charset=utf-8")
        .body("Hello, World!")
}

async fn compute_sum(nums: web::types::Json<Vec<f64>>) -> HttpResponse {
    let values = nums.into_inner();
    if values.is_empty() || values.len() > MAX_ITEMS {
        return text_response(StatusCode::BAD_REQUEST, "payload must contain 1..=1024 numbers");
    }

    HttpResponse::Ok().json(&values.into_iter().sum::<f64>())
}

async fn health(state: web::types::State<AppState>) -> HttpResponse {
    HttpResponse::Ok().json(&HealthResponse {
        status: "ok",
        uptime_ms: state.started_at.elapsed().as_millis() as i64,
    })
}

async fn list_items(
    state: web::types::State<AppState>,
    query: web::types::Query<ListItemsQuery>,
) -> HttpResponse {
    let pool = match require_pool(&state) {
        Ok(pool) => pool,
        Err(error) => return text_response(error.0, error.1),
    };

    let query = query.into_inner();
    let limit = query.limit.unwrap_or(10);
    let offset = query.offset.unwrap_or(0);
    if !(1..=100).contains(&limit) {
        return text_response(StatusCode::BAD_REQUEST, "limit must be 1..=100");
    }
    if offset < 0 {
        return text_response(StatusCode::BAD_REQUEST, "offset must be >= 0");
    }
    if let Some(category) = &query.category {
        if !validate_category(category) {
            return text_response(StatusCode::BAD_REQUEST, "invalid category");
        }
    }

    let client = match pool.get().await {
        Ok(client) => client,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };
    let rows = if let Some(category) = query.category.as_deref() {
        client
            .query(LIST_ITEMS_BY_CATEGORY_SQL, &[&limit, &offset, &category])
            .await
    } else {
        client.query(LIST_ITEMS_ALL_SQL, &[&limit, &offset]).await
    };
    let rows = match rows {
        Ok(rows) => rows,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };

    let mut total = 0_i64;
    let mut items = Vec::new();
    for row in rows {
        if let Ok(Some(count)) = row.try_get::<_, Option<i64>>("total_count") {
            total = count;
        }
        let maybe_id = match row.try_get::<_, Option<i64>>("id") {
            Ok(value) => value,
            Err(error) => {
                return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string());
            }
        };
        if maybe_id.is_none() {
            continue;
        }
        match map_item_row(&row) {
            Ok(item) => items.push(item),
            Err(error) => return text_response(error.0, error.1),
        }
    }

    HttpResponse::Ok().json(&ItemListResponse {
        items,
        total,
        limit,
        offset,
    })
}

async fn get_item(
    state: web::types::State<AppState>,
    id: web::types::Path<i64>,
) -> HttpResponse {
    let pool = match require_pool(&state) {
        Ok(pool) => pool,
        Err(error) => return text_response(error.0, error.1),
    };
    let id = id.into_inner();
    let client = match pool.get().await {
        Ok(client) => client,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };
    let row = match client.query_opt(GET_ITEM_SQL, &[&id]).await {
        Ok(row) => row,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };
    match row {
        Some(row) => match map_item_row(&row) {
            Ok(item) => HttpResponse::Ok().json(&item),
            Err(error) => text_response(error.0, error.1),
        },
        None => text_response(StatusCode::NOT_FOUND, "item not found"),
    }
}

async fn create_item(
    state: web::types::State<AppState>,
    req: HttpRequest,
    body: web::types::Json<CreateItemRequest>,
) -> HttpResponse {
    let pool = match require_pool(&state) {
        Ok(pool) => pool,
        Err(error) => return text_response(error.0, error.1),
    };
    let body = body.into_inner();
    if let Err(error) = validate_create_item(&body) {
        return text_response(error.0, error.1);
    }

    let request_id = match req
        .headers()
        .get("x-request-id")
        .and_then(|value| value.to_str().ok())
    {
        Some(value) => value,
        None => return text_response(StatusCode::BAD_REQUEST, "X-Request-Id header is required"),
    };
    if Uuid::parse_str(request_id).is_err() {
        return text_response(StatusCode::BAD_REQUEST, "X-Request-Id must be a UUID");
    }
    let client = match pool.get().await {
        Ok(client) => client,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };
    let description = normalize_optional_text(body.description.as_deref().unwrap_or(""));
    let stock = body.stock.unwrap_or(0);
    let row = match client
        .query_one(
            CREATE_ITEM_SQL,
            &[
                &body.name,
                &description.is_some(),
                &description.clone().unwrap_or_default(),
                &body.price,
                &body.stock.is_some(),
                &stock,
                &body.category,
            ],
        )
        .await
    {
        Ok(row) => row,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };

    match map_item_row(&row) {
        Ok(item) => HttpResponse::Created().json(&item),
        Err(error) => text_response(error.0, error.1),
    }
}

async fn update_item(
    state: web::types::State<AppState>,
    id: web::types::Path<i64>,
    body: web::types::Json<UpdateItemRequest>,
) -> HttpResponse {
    let pool = match require_pool(&state) {
        Ok(pool) => pool,
        Err(error) => return text_response(error.0, error.1),
    };
    let body = body.into_inner();
    if let Err(error) = validate_update_item(&body) {
        return text_response(error.0, error.1);
    }

    let client = match pool.get().await {
        Ok(client) => client,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };
    let name = body.name.unwrap_or_default();
    let description = normalize_optional_text(body.description.as_deref().unwrap_or(""));
    let price = body.price.unwrap_or(0.0);
    let stock = body.stock.unwrap_or(0);
    let category = body.category.unwrap_or_default();
    let id = id.into_inner();

    let row = match client
        .query_opt(
            UPDATE_ITEM_SQL,
            &[
                &id,
                &(!name.is_empty()),
                &name,
                &description.is_some(),
                &description.clone().unwrap_or_default(),
                &body.price.is_some(),
                &price,
                &body.stock.is_some(),
                &stock,
                &(!category.is_empty()),
                &category,
            ],
        )
        .await
    {
        Ok(row) => row,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };

    match row {
        Some(row) => match map_item_row(&row) {
            Ok(item) => HttpResponse::Ok().json(&item),
            Err(error) => text_response(error.0, error.1),
        },
        None => text_response(StatusCode::NOT_FOUND, "item not found"),
    }
}

async fn delete_item(
    state: web::types::State<AppState>,
    id: web::types::Path<i64>,
) -> HttpResponse {
    let pool = match require_pool(&state) {
        Ok(pool) => pool,
        Err(error) => return text_response(error.0, error.1),
    };
    let id = id.into_inner();
    let client = match pool.get().await {
        Ok(client) => client,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };
    let affected = match client.execute(DELETE_ITEM_SQL, &[&id]).await {
        Ok(affected) => affected,
        Err(error) => return text_response(StatusCode::INTERNAL_SERVER_ERROR, error.to_string()),
    };
    if affected == 0 {
        return text_response(StatusCode::NOT_FOUND, "item not found");
    }
    HttpResponse::NoContent().finish()
}

#[ntex::main]
async fn main() -> io::Result<()> {
    let port = read_port();
    let workers = read_workers();
    let pool = read_postgres_dsn()
        .map(|dsn| make_pool(&dsn, workers))
        .transpose()
        .unwrap_or_else(|error| panic!("failed to configure postgres pool: {error}"));

    if read_bool_env("KATANA_BENCHMARK_API_BOOTSTRAP", true) {
        if let Some(pool) = &pool {
            bootstrap_database(pool)
                .await
                .unwrap_or_else(|error| panic!("failed to bootstrap postgres fixture: {error}"));
        }
    }

    let state = AppState {
        started_at: Instant::now(),
        pool,
    };

    println!("ntex comparison server listening on :{port} with {workers} workers");

    HttpServer::new(move || {
        App::new()
            .state(state.clone())
            .service(web::resource("/").route(web::get().to(hello)))
            .service(web::resource("/compute/sum").route(web::post().to(compute_sum)))
            .service(web::resource("/health").route(web::get().to(health)))
            .service(
                web::resource("/items")
                    .route(web::get().to(list_items))
                    .route(web::post().to(create_item)),
            )
            .service(
                web::resource("/items/{id}")
                    .route(web::get().to(get_item))
                    .route(web::put().to(update_item))
                    .route(web::delete().to(delete_item)),
            )
    })
    .workers(workers)
    .bind(("0.0.0.0", port))?
    .run()
    .await
}
