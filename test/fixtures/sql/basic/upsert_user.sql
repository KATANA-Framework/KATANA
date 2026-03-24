-- name: upsert_user :one
INSERT INTO katana_stage4_users (
  id,
  name,
  active
)
VALUES (
  $1::bigint,
  $2::text,
  $3::bool
)
ON CONFLICT (id) DO UPDATE
SET
  name = EXCLUDED.name,
  active = EXCLUDED.active
RETURNING
  id::bigint AS id,
  name::text AS name,
  active::bool AS active;
