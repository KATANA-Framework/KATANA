-- name: ingest_event :one
INSERT INTO pulse_events (kind, product_id, props)
VALUES ($1::text, $2::bigint, $3::jsonb)
RETURNING id::bigint AS id, created_at::text AS created_at;
