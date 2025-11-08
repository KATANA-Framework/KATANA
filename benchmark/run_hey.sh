#!/bin/bash

HOST="${1:-127.0.0.1}"
PORT="${2:-8080}"
REQUESTS="${3:-100000}"
CONCURRENCY="${4:-100}"

echo "=== Hey Load Test ==="
echo "Target: http://${HOST}:${PORT}"
echo "Total requests: ${REQUESTS}"
echo "Concurrency: ${CONCURRENCY}"
echo ""

if ! command -v hey &> /dev/null; then
    echo "hey not found."
    echo "Install with: go install github.com/rakyll/hey@latest"
    exit 1
fi

hey -n ${REQUESTS} -c ${CONCURRENCY} \
    -m GET \
    http://${HOST}:${PORT}/
