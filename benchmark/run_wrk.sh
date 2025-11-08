#!/bin/bash

HOST="${1:-127.0.0.1}"
PORT="${2:-8080}"
DURATION="${3:-30s}"
THREADS="${4:-4}"
CONNECTIONS="${5:-100}"

echo "=== WRK Load Test ==="
echo "Target: http://${HOST}:${PORT}"
echo "Duration: ${DURATION}"
echo "Threads: ${THREADS}"
echo "Connections: ${CONNECTIONS}"
echo ""

if ! command -v wrk &> /dev/null; then
    echo "wrk not found. Install with: sudo apt-get install wrk"
    exit 1
fi

wrk -t${THREADS} -c${CONNECTIONS} -d${DURATION} \
    --latency \
    http://${HOST}:${PORT}/
