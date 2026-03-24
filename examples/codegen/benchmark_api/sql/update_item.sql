-- name: update_item :one
UPDATE katana_stage4_items
SET
  name = CASE WHEN $2::bool THEN $3::text ELSE name END,
  description = CASE WHEN $4::bool THEN $5::text ELSE description END,
  price = CASE WHEN $6::bool THEN $7::float8 ELSE price END,
  stock = CASE WHEN $8::bool THEN $9::bigint ELSE stock END,
  category = CASE WHEN $10::bool THEN $11::text ELSE category END,
  updated_at = NOW()
WHERE id = $1::bigint
RETURNING
  id::bigint AS id,
  name::text AS name,
  description::text AS description,
  price::float8 AS price,
  stock::bigint AS stock,
  category::text AS category_name;
