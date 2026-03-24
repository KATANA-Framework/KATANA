-- name: get_item :one
SELECT
  id::bigint AS id,
  name::text AS name,
  description::text AS description,
  price::float8 AS price,
  stock::bigint AS stock,
  category::text AS category_name
FROM katana_stage4_items
WHERE id = $1::bigint;
