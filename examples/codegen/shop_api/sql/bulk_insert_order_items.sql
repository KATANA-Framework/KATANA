-- name: bulk_insert_order_items :exec
INSERT INTO shop_order_items (order_id, product_id, qty, unit_price)
SELECT b.order_id, b.product_id, b.qty, b.unit_price
FROM UNNEST($1::bigint[], $2::bigint[], $3::bigint[], $4::float8[])
  AS b(order_id, product_id, qty, unit_price);
