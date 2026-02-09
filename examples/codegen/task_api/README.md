# Task Management API

Comprehensive task management service demonstrating KATANA framework capabilities created for issue #5.

## Features

- Full CRUD operations for tasks
- Complex validation rules (string lengths, numeric ranges, date-time formats, email validation)
- Nested objects (User inside Task)
- Array handling with uniqueItems constraint
- Query parameter filtering with optional values
- Batch operations
- Complex search with multiple criteria
- Health check endpoint with metrics
- RFC 7807 Problem Details error responses

## API Endpoints

### Task Operations
- `GET /tasks` - List all tasks with optional filtering (status, priority, limit, offset)
- `POST /tasks` - Create a new task
- `GET /tasks/{id}` - Get a specific task by ID
- `PUT /tasks/{id}` - Update a task
- `DELETE /tasks/{id}` - Delete a task

### Batch & Search
- `POST /tasks/batch` - Create multiple tasks in a single request (up to 100)
- `POST /tasks/search` - Complex search with multiple criteria

### System
- `GET /health` - Health check with uptime and request count metrics

## Building

```bash
# From project root
cmake --preset examples
cmake --build --preset examples --target task_api
```

Binary will be at: `build/examples/examples/codegen/task_api/task_api`

## Running

```bash
# Default port 18081
./build/examples/examples/codegen/task_api/task_api

# Custom port
./build/examples/examples/codegen/task_api/task_api 8080
```

## Testing

### Create a task
```bash
curl -X POST http://localhost:18081/tasks \
  -H "Content-Type: application/json" \
  -d '{
    "title": "Implement benchmarks",
    "description": "Create comprehensive benchmark suite",
    "priority": 5,
    "tags": ["urgent", "performance"],
    "due_date": "2026-02-10T12:00:00Z"
  }'
```

### List tasks
```bash
curl http://localhost:18081/tasks
curl "http://localhost:18081/tasks?status=pending&priority=5&limit=10"
```

### Get a task
```bash
curl http://localhost:18081/tasks/1
```

### Update a task
```bash
curl -X PUT http://localhost:18081/tasks/1 \
  -H "Content-Type: application/json" \
  -d '{"status": "completed", "priority": 3}'
```

### Delete a task
```bash
curl -X DELETE http://localhost:18081/tasks/1
```

### Batch create
```bash
curl -X POST http://localhost:18081/tasks/batch \
  -H "Content-Type: application/json" \
  -d '{
    "tasks": [
      {"title": "Task 1", "priority": 1},
      {"title": "Task 2", "priority": 2},
      {"title": "Task 3", "priority": 3}
    ]
  }'
```

### Search tasks
```bash
curl -X POST http://localhost:18081/tasks/search \
  -H "Content-Type: application/json" \
  -d '{
    "statuses": ["pending", "in_progress"],
    "min_priority": 3,
    "max_priority": 5
  }'
```

### Health check
```bash
curl http://localhost:18081/health
```

## Benchmarking

This service is included in the automated benchmark suite. It tests:

- Simple CRUD operations (create, read, update, delete)
- Query parameter parsing and filtering
- Complex nested object validation
- Array validation with uniqueItems
- Batch operations with varying sizes (1, 10, 50, 100 items)
- Search with different criteria combinations
- Edge cases: max string lengths, boundary values, null handling

The benchmark results are automatically collected and added to `BENCHMARK_RESULTS.md`.

## Generated Code

The `generated/` directory contains auto-generated code from the OpenAPI spec:

- `generated_dtos.hpp` - C++ structures with arena allocators
- `generated_validators.hpp` - Validation logic for all constraints
- `generated_json.hpp` - JSON parsers and serializers (zero-copy where possible)
- `generated_routes.hpp` - Constexpr route table
- `generated_handlers.hpp` - Handler interfaces
- `generated_router_bindings.hpp` - Glue code for routing and parameter extraction

## Performance Characteristics

This service demonstrates:

- **Zero-copy parsing** - Using `std::string_view` where possible
- **Arena allocation** - Per-request memory management with single deallocation
- **Compile-time routing** - No runtime string comparisons
- **Type-safe handlers** - Compile-time errors for API contract violations
- **Efficient validation** - Early rejection of invalid requests

## Validation Edge Cases

The API spec includes various validation constraints tested by benchmarks:

- String lengths: minLength=1, maxLength=200 (titles), maxLength=2000 (descriptions)
- Numeric ranges: priority 1-5, limit 1-100, minimum IDs of 1
- Email format validation
- Date-time format validation (RFC 3339)
- Array constraints: maxItems=20 (tags), uniqueItems=true
- Enum validation: status (4 values), health status (3 values)
- Nested object validation: User inside Task
- Optional/nullable fields with proper handling
- Batch size limits: 1-100 tasks per batch request

All of these constraints are automatically enforced by the generated validators,
ensuring API contract compliance without manual boilerplate code.
