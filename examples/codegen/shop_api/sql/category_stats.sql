-- name: category_stats :many
SELECT
  category::text AS category,
  COUNT(*)::bigint AS product_count,
  AVG(price)::float8 AS avg_price,
  SUM(stock)::bigint AS total_stock,
  MIN(price)::float8 AS min_price,
  MAX(price)::float8 AS max_price
FROM shop_products
GROUP BY category
ORDER BY product_count DESC;
