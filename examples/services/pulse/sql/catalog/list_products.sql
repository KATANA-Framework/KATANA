-- name: list_products :many
SELECT id::bigint AS id, sku::text AS sku, name::text AS name,
       price_cents::bigint AS price_cents, created_at::text AS created_at
FROM pulse_products
ORDER BY id DESC
LIMIT $1::bigint OFFSET $2::bigint;
