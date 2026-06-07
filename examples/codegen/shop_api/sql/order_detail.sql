-- name: order_detail :one
SELECT
  o.id::bigint AS order_id,
  o.status::text AS status,
  c.name::text AS customer_name,
  c.country::text AS country,
  COUNT(oi.id)::bigint AS line_count,
  COALESCE(SUM(oi.qty), 0)::bigint AS total_qty,
  COALESCE(SUM(oi.qty * oi.unit_price), 0)::float8 AS total_amount
FROM shop_orders o
JOIN shop_customers c ON c.id = o.customer_id
LEFT JOIN shop_order_items oi ON oi.order_id = o.id
WHERE o.id = $1::bigint
GROUP BY o.id, o.status, c.name, c.country;
