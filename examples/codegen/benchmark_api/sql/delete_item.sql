-- name: delete_item :exec
DELETE FROM katana_stage4_items
WHERE id = $1::bigint;
