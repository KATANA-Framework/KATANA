# KATANA Examples

## Building Examples

```bash
cmake -B build -DENABLE_EXAMPLES=ON
cmake --build build
```

## Basic Examples

### basic_reactor.cpp
Minimal reactor example showing event loop basics.

```bash
./build/basic_reactor_example
```

### hello_world_server.cpp
Simple HTTP server responding to all requests.

```bash
./build/hello_world_server 8080
curl http://localhost:8080/
```

### raii_echo_server.cpp
TCP echo server using RAII patterns and modern C++ idioms.

```bash
./build/raii_echo_server 8080
echo "Hello" | nc localhost 8080
```

### raii_http_server.cpp
HTTP server with monadic error handling.

```bash
./build/raii_http_server 8080
curl http://localhost:8080/
```

## REST API Example

### rest_api_server.cpp
Complete REST API with routing and JSON support.

```bash
./build/rest_api_server 8080
```

**Endpoints:**

- `GET /users` - List all users
- `POST /users` - Create user (JSON body: `{"name":"...","email":"..."}`)
- `GET /users/:id` - Get user by ID
- `PUT /users/:id` - Update user (JSON body: `{"name":"...","email":"..."}`)
- `DELETE /users/:id` - Delete user

**Examples:**

```bash
# List users
curl http://localhost:8080/users

# Get user by ID
curl http://localhost:8080/users/1

# Create user
curl -X POST http://localhost:8080/users \
  -H "Content-Type: application/json" \
  -d '{"name":"Alice","email":"alice@example.com"}'

# Update user
curl -X PUT http://localhost:8080/users/1 \
  -H "Content-Type: application/json" \
  -d '{"name":"Alice Updated","email":"alice@updated.com"}'

# Delete user
curl -X DELETE http://localhost:8080/users/1
```
