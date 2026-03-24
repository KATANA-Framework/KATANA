-- name: get_user :one
SELECT
  id::bigint AS id,
  name::text AS name,
  active::bool AS active
FROM katana_stage4_users
WHERE id = $1::bigint;
