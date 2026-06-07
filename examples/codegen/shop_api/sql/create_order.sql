-- name: create_order :one
INSERT INTO shop_orders (customer_id, status)
VALUES ($1::bigint, $2::text)
RETURNING id::bigint AS id;
