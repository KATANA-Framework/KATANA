-- name: create_product :one
INSERT INTO pulse_products (sku, name, price_cents)
VALUES ($1::text, $2::text, $3::bigint)
RETURNING id::bigint AS id, created_at::text AS created_at;
