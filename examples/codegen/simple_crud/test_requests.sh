#!/bin/bash

# Simple CRUD API Test Requests
# Make sure the server is running before executing this script

BASE_URL="http://localhost:8080"

echo "=========================================="
echo "Testing Simple CRUD API"
echo "=========================================="
echo ""

# Test 1: Create first task
echo "1. Creating task: Buy milk"
curl -X POST "$BASE_URL/tasks" \
  -H "Content-Type: application/json" \
  -d '{"title":"Buy milk","completed":false}' \
  -w "\n" -s
echo ""

# Test 2: Create second task
echo "2. Creating task: Write code"
curl -X POST "$BASE_URL/tasks" \
  -H "Content-Type: application/json" \
  -d '{"title":"Write code","description":"Implement feature X","completed":false}' \
  -w "\n" -s
echo ""

# Test 3: Create third task
echo "3. Creating task: Review PR"
curl -X POST "$BASE_URL/tasks" \
  -H "Content-Type: application/json" \
  -d '{"title":"Review PR","completed":true}' \
  -w "\n" -s
echo ""

# Test 4: List all tasks
echo "4. Listing all tasks"
curl -X GET "$BASE_URL/tasks" \
  -H "Content-Type: application/json" \
  -w "\n" -s | jq .
echo ""

# Test 5: Get specific task
echo "5. Getting task with id=1"
curl -X GET "$BASE_URL/tasks/1" \
  -H "Content-Type: application/json" \
  -w "\n" -s | jq .
echo ""

# Test 6: Update task
echo "6. Updating task 1 (marking as completed)"
curl -X PUT "$BASE_URL/tasks/1" \
  -H "Content-Type: application/json" \
  -d '{"completed":true}' \
  -w "\n" -s | jq .
echo ""

# Test 7: Get updated task
echo "7. Getting updated task 1"
curl -X GET "$BASE_URL/tasks/1" \
  -H "Content-Type: application/json" \
  -w "\n" -s | jq .
echo ""

# Test 8: Delete task
echo "8. Deleting task 2"
curl -X DELETE "$BASE_URL/tasks/2" \
  -w "\n" -s
echo "Status code should be 204"
echo ""

# Test 9: List all tasks after deletion
echo "9. Listing all tasks after deletion"
curl -X GET "$BASE_URL/tasks" \
  -H "Content-Type: application/json" \
  -w "\n" -s | jq .
echo ""

# Test 10: Try to get deleted task (should return 404)
echo "10. Trying to get deleted task 2 (should fail with 404)"
curl -X GET "$BASE_URL/tasks/2" \
  -H "Content-Type: application/json" \
  -w "\nHTTP Status: %{http_code}\n" -s | jq .
echo ""

# Test 11: Try to create task with empty title (should fail with 400)
echo "11. Creating task with empty title (should fail with 400)"
curl -X POST "$BASE_URL/tasks" \
  -H "Content-Type: application/json" \
  -d '{"title":""}' \
  -w "\nHTTP Status: %{http_code}\n" -s | jq .
echo ""

echo "=========================================="
echo "Tests completed!"
echo "=========================================="
