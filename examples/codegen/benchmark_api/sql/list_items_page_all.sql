-- name: list_items_page_all :many
SELECT
  page.id::bigint AS id,
  page.name::text AS name,
  page.description::text AS description,
  page.price::float8 AS price,
  page.stock::bigint AS stock,
  page.category::text AS category_name,
  counts.total_count::bigint AS total_count
FROM (
  SELECT COUNT(*)::bigint AS total_count
  FROM katana_stage4_items
) AS counts
LEFT JOIN LATERAL (
  SELECT
    id,
    name,
    description,
    price,
    stock,
    category
  FROM katana_stage4_items
  ORDER BY id
  LIMIT $1::bigint OFFSET $2::bigint
) AS page ON TRUE;
