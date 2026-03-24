-- name: create_item :one
INSERT INTO katana_stage4_items (
  name,
  description,
  price,
  stock,
  category
)
VALUES (
  $1::text,
  CASE WHEN $2::bool THEN $3::text ELSE NULL END,
  $4::float8,
  CASE WHEN $5::bool THEN $6::bigint ELSE NULL END,
  $7::text
)
RETURNING
  id::bigint AS id,
  name::text AS name,
  description::text AS description,
  price::float8 AS price,
  stock::bigint AS stock,
  category::text AS category_name;
