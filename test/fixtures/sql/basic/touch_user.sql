-- name: touch_user :exec
UPDATE katana_stage4_users
SET last_seen_at = NOW()
WHERE id = $1::bigint;
