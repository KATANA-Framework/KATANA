-- name: get_product :one
SELECT id::bigint AS id, sku::text AS sku, name::text AS name,
       price_cents::bigint AS price_cents, created_at::text AS created_at
FROM pulse_products WHERE id = $1::bigint;
