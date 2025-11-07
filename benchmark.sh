#!/bin/bash

set -e

if ! command -v wrk &> /dev/null; then
    echo "wrk is not installed. Please install it first:"
    echo "  Ubuntu/Debian: sudo apt-get install wrk"
    echo "  Or build from source: https://github.com/wg/wrk"
    exit 1
fi

DURATION=${1:-10s}
THREADS=${2:-4}
CONNECTIONS=${3:-100}
PORT=8080

echo "Starting hello-world server..."
./build/hello_world_server &
SERVER_PID=$!

sleep 2

if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "Failed to start server"
    exit 1
fi

echo "Running benchmark with:"
echo "  Duration: $DURATION"
echo "  Threads: $THREADS"
echo "  Connections: $CONNECTIONS"
echo ""

wrk -t$THREADS -c$CONNECTIONS -d$DURATION http://localhost:$PORT/

kill -SIGINT $SERVER_PID
wait $SERVER_PID 2>/dev/null || true

echo ""
echo "Benchmark complete!"
