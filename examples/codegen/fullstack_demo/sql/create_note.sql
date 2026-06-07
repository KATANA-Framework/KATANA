-- name: create_note :one
INSERT INTO demo_notes (title, body, priority, metadata, due_date)
VALUES ($1::text, $2::text, $3::text, $4::jsonb, $5::timestamptz)
RETURNING id::bigint AS id, created_at::text AS created_at;
