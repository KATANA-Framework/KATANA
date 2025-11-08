#!/bin/bash

HOST="${1:-127.0.0.1}"
PORT="${2:-8080}"
DURATION="${3:-30s}"
CONNECTIONS="${4:-100}"

echo "=== Bombardier Load Test ==="
echo "Target: http://${HOST}:${PORT}"
echo "Duration: ${DURATION}"
echo "Connections: ${CONNECTIONS}"
echo ""

if ! command -v bombardier &> /dev/null; then
    echo "bombardier not found."
    echo "Install with: go install github.com/codesenberg/bombardier@latest"
    exit 1
fi

bombardier -c ${CONNECTIONS} -d ${DURATION} \
    --latencies \
    --print r,p,l \
    http://${HOST}:${PORT}/
