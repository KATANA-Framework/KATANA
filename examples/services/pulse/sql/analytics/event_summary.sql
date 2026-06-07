-- name: event_summary :many
SELECT kind::text AS kind, count(*)::bigint AS count
FROM pulse_events
GROUP BY kind
ORDER BY count DESC;
