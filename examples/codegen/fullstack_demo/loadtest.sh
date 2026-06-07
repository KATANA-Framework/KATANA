#!/usr/bin/env bash
# Load-test harness for the fullstack_demo Notes API.
# Drives wrk against the cached-read, SQL-read, and SQL-write paths and prints a summary
# plus the server's own /metrics counters. Requires: wrk, a running PostgreSQL, and the
# built service binary.
#
#   PG_DSN=postgresql://katana@127.0.0.1:5433/katana \
#   NOTES_BIN=/tmp/notes DURATION=10 THREADS=4 CONNS=64 ./loadtest.sh
set -u

PORT="${PORT:-8099}"
BASE="http://127.0.0.1:${PORT}"
DURATION="${DURATION:-8}"
THREADS="${THREADS:-4}"
CONNS="${CONNS:-64}"
NOTES_BIN="${NOTES_BIN:-/tmp/notes}"
HERE="$(cd "$(dirname "$0")" && pwd)"

command -v wrk >/dev/null || { echo "wrk not found"; exit 1; }
[ -x "$NOTES_BIN" ] || { echo "service binary $NOTES_BIN not found/executable"; exit 1; }

echo "Starting service on :$PORT ..."
PORT="$PORT" WORKERS="${WORKERS:-4}" "$NOTES_BIN" >/tmp/notes_load.out 2>&1 &
SRV=$!
trap 'kill $SRV 2>/dev/null; wait $SRV 2>/dev/null' EXIT
sleep 1.5

# Seed one note so the SQL-read path has a target.
SEED=$(curl -s -X POST "$BASE/notes" -H 'Content-Type: application/json' \
  --data '{"title":"seed","body":"seed","priority":"low","metadata":{},"due_date":"2026-06-10T09:00:00Z"}')
ID=$(echo "$SEED" | python3 -c "import json,sys;print(json.load(sys.stdin).get('id',1))" 2>/dev/null || echo 1)
echo "Seeded note id=$ID"

run() {
  local name="$1"; shift
  echo
  echo "=== $name (t=$THREADS c=$CONNS d=${DURATION}s) ==="
  wrk -t"$THREADS" -c"$CONNS" -d"${DURATION}s" --latency "$@" 2>&1 \
    | grep -E 'Requests/sec|Latency|50%|90%|99%|Non-2xx|Socket errors'
}

run "GET /healthz   (built-in, no DB)"   "$BASE/healthz"
run "GET /notes     (cached list)"        "$BASE/notes?limit=20"
run "GET /notes/{id}(SQL read)"           "$BASE/notes/$ID"
run "POST /notes    (SQL write)"          -s "$HERE/wrk_create.lua" "$BASE/notes"

echo
echo "=== server /metrics after load ==="
curl -s "$BASE/metrics" | grep -E 'requests_total|in_flight'
