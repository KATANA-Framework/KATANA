#!/bin/bash

HOST="${1:-127.0.0.1}"
PORT="${2:-8080}"
BASE_URL="http://${HOST}:${PORT}"

echo "=== REST API Benchmark ==="
echo "Target: ${BASE_URL}"
echo ""

if ! command -v bombardier &> /dev/null; then
    echo "bombardier not found."
    echo "Install with: go install github.com/codesenberg/bombardier@latest"
    echo "Using curl for basic test instead..."

    echo "Testing GET /users"
    curl -s "${BASE_URL}/users" | jq .

    echo -e "\nTesting POST /users"
    curl -s -X POST "${BASE_URL}/users" \
        -H "Content-Type: application/json" \
        -d '{"name":"Test User","email":"test@example.com"}' | jq .

    echo -e "\nTesting GET /users/1"
    curl -s "${BASE_URL}/users/1" | jq .

    exit 0
fi

echo "Testing GET /users endpoint..."
bombardier -c 50 -d 10s --latencies --print l \
    ${BASE_URL}/users

echo -e "\nTesting GET /users/1 endpoint..."
bombardier -c 50 -d 10s --latencies --print l \
    ${BASE_URL}/users/1

echo -e "\nTesting POST /users endpoint..."
bombardier -c 10 -d 5s --latencies --print l \
    -m POST \
    -H "Content-Type: application/json" \
    -b '{"name":"Benchmark User","email":"bench@example.com"}' \
    ${BASE_URL}/users
