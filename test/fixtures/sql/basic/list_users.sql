-- name: list_users :many
SELECT
  id::bigint AS id,
  name::text AS name
FROM katana_stage4_users
WHERE active = $1::bool
ORDER BY id;
