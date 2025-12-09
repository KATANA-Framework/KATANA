#!/bin/bash
# Long-running load test for products_api
# Tests: sustained load, memory stability, arena reuse, connection churn

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Configuration
SERVER_PORT="${SERVER_PORT:-8082}"
TEST_DURATION="${TEST_DURATION:-300}"  # 5 minutes default
CONCURRENCY="${CONCURRENCY:-64}"
RESULTS_DIR="${RESULTS_DIR:-$PROJECT_ROOT/profiling_results/load_test_$(date +%Y%m%dT%H%M%S)}"

echo "=== Products API Long-Running Load Test ==="
echo "Server Port: $SERVER_PORT"
echo "Duration: ${TEST_DURATION}s"
echo "Concurrency: $CONCURRENCY"
echo "Results Dir: $RESULTS_DIR"
echo ""

mkdir -p "$RESULTS_DIR"

# Check if products_api binary exists
PRODUCTS_API_BIN="$PROJECT_ROOT/build/examples/examples/codegen/products_api/products_api"
if [ ! -f "$PRODUCTS_API_BIN" ]; then
    echo "Error: products_api binary not found at $PRODUCTS_API_BIN"
    echo "Build it with: cmake --preset examples && cmake --build --preset examples --target products_api"
    exit 1
fi

# Start server in background
echo "Starting products_api server..."
"$PRODUCTS_API_BIN" > "$RESULTS_DIR/server.log" 2>&1 &
SERVER_PID=$!

# Give server time to start
sleep 2

# Check if server is running
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "Error: Server failed to start. Check $RESULTS_DIR/server.log"
    cat "$RESULTS_DIR/server.log"
    exit 1
fi

echo "Server started with PID $SERVER_PID"

# Cleanup function
cleanup() {
    echo ""
    echo "Stopping server..."
    kill $SERVER_PID 2>/dev/null || true
    wait $SERVER_PID 2>/dev/null || true
    echo "Server stopped"
}
trap cleanup EXIT

# Wait for server to be ready
for i in {1..10}; do
    if curl -s http://localhost:$SERVER_PORT/products > /dev/null 2>&1; then
        echo "Server is ready"
        break
    fi
    if [ $i -eq 10 ]; then
        echo "Error: Server did not become ready"
        exit 1
    fi
    sleep 1
done

# Phase 1: Fixed concurrency test (64 workers)
echo ""
echo "=== Phase 1: Fixed Concurrency ($CONCURRENCY workers) ==="
echo "Mixed workload: GET (70%), POST (15%), PUT (10%), DELETE (5%)"

cat > "$RESULTS_DIR/load_test.lua" << 'EOF'
-- Mixed CRUD workload for products_api
wrk.method = "GET"
wrk.headers["Content-Type"] = "application/json"

local counter = 0
local product_ids = {}

-- Initialize with some product IDs
for i = 1, 100 do
    table.insert(product_ids, i)
end

request = function()
    counter = counter + 1
    local op = counter % 100

    if op < 70 then
        -- GET list (40%) or GET by ID (30%)
        if op < 40 then
            return wrk.format("GET", "/products?limit=10&offset=0")
        else
            local id = product_ids[math.random(#product_ids)]
            return wrk.format("GET", "/products/" .. id)
        end
    elseif op < 85 then
        -- POST create (15%)
        local body = string.format([[{"sku":"TEST-%d","name":"Product %d","price":99.99,"stock":100,"category":"electronics"}]], counter, counter)
        return wrk.format("POST", "/products", nil, body)
    elseif op < 95 then
        -- PUT update (10%)
        local id = product_ids[math.random(#product_ids)]
        local body = '{"price":79.99,"stock":80}'
        return wrk.format("PUT", "/products/" .. id, nil, body)
    else
        -- DELETE (5%)
        local id = product_ids[math.random(#product_ids)]
        return wrk.format("DELETE", "/products/" .. id)
    end
end

response = function(status, headers, body)
    -- Track created product IDs for future operations
    if status == 201 then
        local id = body:match('"id":(%d+)')
        if id then
            table.insert(product_ids, tonumber(id))
        end
    end
end
EOF

if command -v wrk &> /dev/null; then
    wrk -t $CONCURRENCY -c $CONCURRENCY -d ${TEST_DURATION}s --latency \
        -s "$RESULTS_DIR/load_test.lua" \
        http://localhost:$SERVER_PORT/ > "$RESULTS_DIR/phase1_results.txt" 2>&1

    echo "Phase 1 completed. Results in $RESULTS_DIR/phase1_results.txt"
    cat "$RESULTS_DIR/phase1_results.txt"
else
    echo "Warning: wrk not installed, using curl-based load test"

    # Fallback: simple curl-based test
    echo "Running $CONCURRENCY concurrent requests for ${TEST_DURATION}s..."
    END_TIME=$((SECONDS + TEST_DURATION))
    SUCCESS=0
    ERRORS=0

    while [ $SECONDS -lt $END_TIME ]; do
        for i in $(seq 1 $CONCURRENCY); do
            (
                RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:$SERVER_PORT/products 2>&1)
                if [ "$RESPONSE" = "200" ]; then
                    echo "success" >> "$RESULTS_DIR/curl_results.tmp"
                else
                    echo "error" >> "$RESULTS_DIR/curl_results.tmp"
                fi
            ) &
        done
        wait
    done

    SUCCESS=$(grep -c "success" "$RESULTS_DIR/curl_results.tmp" 2>/dev/null || echo 0)
    ERRORS=$(grep -c "error" "$RESULTS_DIR/curl_results.tmp" 2>/dev/null || echo 0)

    echo "Results: $SUCCESS successful, $ERRORS errors" | tee "$RESULTS_DIR/phase1_results.txt"
fi

# Phase 2: Variable concurrency ramp test
echo ""
echo "=== Phase 2: Variable Concurrency Ramp Test ==="
echo "Testing with 64, 128, 256 concurrent connections"

for CONC in 64 128 256; do
    echo ""
    echo "Testing with $CONC connections..."

    if command -v wrk &> /dev/null; then
        wrk -t $CONC -c $CONC -d 30s --latency \
            -s "$RESULTS_DIR/load_test.lua" \
            http://localhost:$SERVER_PORT/ > "$RESULTS_DIR/phase2_conc${CONC}.txt" 2>&1

        echo "Concurrency $CONC completed"
        grep -E "(Requests/sec|Latency|requests in)" "$RESULTS_DIR/phase2_conc${CONC}.txt" || true
    else
        echo "wrk not available, skipping phase 2"
        break
    fi

    # Cool down between tests
    sleep 5
done

# Phase 3: Connection churn test
echo ""
echo "=== Phase 3: Connection Churn Test ==="
echo "Frequent connection open/close to test cleanup"

CHURN_REQUESTS=10000
echo "Making $CHURN_REQUESTS requests with new connections..."

START_TIME=$(date +%s)
CHURN_SUCCESS=0
CHURN_ERRORS=0

for i in $(seq 1 $CHURN_REQUESTS); do
    # Force new connection each time (no keep-alive)
    RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -H "Connection: close" \
        http://localhost:$SERVER_PORT/products 2>&1)

    if [ "$RESPONSE" = "200" ]; then
        ((CHURN_SUCCESS++))
    else
        ((CHURN_ERRORS++))
    fi

    # Progress indicator every 1000 requests
    if [ $((i % 1000)) -eq 0 ]; then
        echo "  Progress: $i/$CHURN_REQUESTS"
    fi
done

END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

echo ""
echo "Churn test results:" | tee "$RESULTS_DIR/phase3_churn.txt"
echo "  Total requests: $CHURN_REQUESTS" | tee -a "$RESULTS_DIR/phase3_churn.txt"
echo "  Successful: $CHURN_SUCCESS" | tee -a "$RESULTS_DIR/phase3_churn.txt"
echo "  Errors: $CHURN_ERRORS" | tee -a "$RESULTS_DIR/phase3_churn.txt"
echo "  Duration: ${DURATION}s" | tee -a "$RESULTS_DIR/phase3_churn.txt"
echo "  Rate: $((CHURN_REQUESTS / DURATION)) req/s" | tee -a "$RESULTS_DIR/phase3_churn.txt"

# Memory check (if server logs include memory info)
echo ""
echo "=== Memory Stability Check ==="
echo "Checking server logs for memory issues..."

if grep -qi "leak\|oom\|bad_alloc" "$RESULTS_DIR/server.log"; then
    echo "WARNING: Potential memory issues detected in server logs"
    grep -i "leak\|oom\|bad_alloc" "$RESULTS_DIR/server.log"
else
    echo "No memory issues detected in server logs"
fi

# Summary
echo ""
echo "=== Load Test Summary ==="
echo "All test results saved to: $RESULTS_DIR"
echo ""
echo "Files created:"
ls -lh "$RESULTS_DIR"
echo ""
echo "✓ Load test completed"
