-- name: bulk_upsert_users :many
INSERT INTO katana_stage4_users (
  id,
  name,
  active
)
SELECT
  batch.id,
  batch.name,
  batch.active
FROM UNNEST(
  $1::bigint[],
  $2::text[],
  $3::bool[]
) AS batch(id, name, active)
ON CONFLICT (id) DO UPDATE
SET
  name = EXCLUDED.name,
  active = EXCLUDED.active
RETURNING
  id::bigint AS id,
  name::text AS name,
  active::bool AS active;
