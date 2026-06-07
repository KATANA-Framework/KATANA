-- name: top_products_by_category :many
SELECT
  p.id::bigint AS product_id,
  p.name::text AS name,
  p.category::text AS category,
  COALESCE(SUM(oi.qty), 0)::bigint AS units_sold,
  COALESCE(SUM(oi.qty * oi.unit_price), 0)::float8 AS revenue,
  RANK() OVER (PARTITION BY p.category ORDER BY COALESCE(SUM(oi.qty), 0) DESC)::bigint AS category_rank
FROM shop_products p
LEFT JOIN shop_order_items oi ON oi.product_id = p.id
GROUP BY p.id, p.name, p.category
ORDER BY p.category, units_sold DESC
LIMIT $1::bigint;
