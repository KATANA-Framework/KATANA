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
