-- name: list_notes :many
SELECT id::bigint AS id, title::text AS title, body::text AS body,
       priority::text AS priority, metadata::text AS metadata,
       due_date::text AS due_date, created_at::text AS created_at
FROM demo_notes
WHERE ($1::text IS NULL OR priority = $1::text)
ORDER BY id DESC
LIMIT $2::bigint OFFSET $3::bigint;
