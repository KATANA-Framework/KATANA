-- name: get_note :one
SELECT id::bigint AS id, title::text AS title, body::text AS body,
       priority::text AS priority, metadata::text AS metadata,
       due_date::text AS due_date, created_at::text AS created_at
FROM demo_notes WHERE id = $1::bigint;
