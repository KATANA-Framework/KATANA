# KATANA Router

Compile-time, zero-allocation HTTP router for the KATANA framework.

## Overview

The KATANA Router provides type-safe routing with compile-time path parsing and zero-allocation dispatch in the hot path. The router handles 404/405 errors automatically, generates RFC 7807 Problem Details, and supports middleware chains.

### Key features

- ✅ **Compile-time path parsing** — errors in `/users/{id}` show up at compile time
- ✅ **Zero-allocation hot path** — no heap allocations during dispatch
- ✅ **Automatic RFC 7807** — 404/405 are mapped to Problem Details automatically
- ✅ **Allow header** — generated automatically for 405 Method Not Allowed
- ✅ **Path parameters** — extracted automatically
- ✅ **Middleware chains** — middleware composition without virtual calls
- ✅ **Smart routing** — static segments take priority over parameters

---

## Quick Start

### Minimal router

```cpp
#include "katana/core/router.hpp"
#include "katana/core/http.hpp"

using namespace katana::http;

// Define routes
route_entry routes[] = {
    {method::get,
     path_pattern::from_literal<"/">(),
     handler_fn([](const request& req, request_context& ctx) {
         return response::ok("Hello, World!");
     })},

    {method::get,
     path_pattern::from_literal<"/users/{id}">(),
     handler_fn([](const request& req, request_context& ctx) {
         auto id = ctx.params.get("id").value_or("unknown");
         return response::ok(std::string("User ID: ") + std::string(id));
     })},
};

// Create the router
router r(routes);

// Dispatch request
monotonic_arena arena;
request_context ctx{arena};
request req;
req.http_method = method::get;
req.uri = "/users/42";

auto resp = dispatch_or_problem(r, req, ctx);
// resp.status == 200
// resp.body == "User ID: 42"
```

---

## Path Patterns

### Compile-time parsing

Path patterns are parsed at compile time via `from_literal<>()`:

```cpp
// ✅ Valid
auto pattern = path_pattern::from_literal<"/users/{id}">();
auto pattern = path_pattern::from_literal<"/orders/{orderId}/items/{itemId}">();
auto pattern = path_pattern::from_literal<"/static/about">();

// ❌ Compile errors
auto pattern = path_pattern::from_literal<"users">(); // must start with '/'
auto pattern = path_pattern::from_literal<"/users/{id">();  // unclosed brace
auto pattern = path_pattern::from_literal<"/users/{}">();   // empty parameter name
```

### Parameter syntax

```cpp
// Literal segments
"/users"              // exact match only

// Parameters (any value)
"/users/{id}"         // /users/123, /users/alice
"/users/{id}/posts"   // /users/42/posts

// Multiple parameters
"/orders/{orderId}/items/{itemId}"  // /orders/10/items/5
```

### Prioritization

Static segments have higher priority than parameters:

```cpp
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/me">(), handler_me},      // priority 1
    {method::get, path_pattern::from_literal<"/users/{id}">(), handler_id},    // priority 2
};

router r(routes);

// GET /users/me    → calls handler_me (exact match)
// GET /users/42    → calls handler_id (parameter)
```

**Prioritization algorithm:**
```cpp
score = literal_count * 16 + (MAX_ROUTE_SEGMENTS - param_count)
```

The more literal segments, the higher the priority.

---

## Path Parameters

### Extracting parameters

Parameters are extracted automatically into `request_context::params`:

```cpp
{method::get,
 path_pattern::from_literal<"/users/{id}">(),
 handler_fn([](const request& req, request_context& ctx) {
     // Get a parameter
     auto id = ctx.params.get("id");  // std::optional<std::string_view>

     if (!id) {
         return response::error(problem_details::bad_request("Missing id"));
     }

     // Safe fallback
     auto name = ctx.params.get("name").value_or("anonymous");

     return response::ok(std::string(*id));
 })}
```

### Multiple parameters

```cpp
{method::get,
 path_pattern::from_literal<"/orders/{orderId}/items/{itemId}">(),
 handler_fn([](const request& req, request_context& ctx) {
     auto order_id = ctx.params.get("orderId");
     auto item_id = ctx.params.get("itemId");

     // Both parameters are available
     return response::ok("OK");
 })}
```

### Parsing parameters

```cpp
#include <charconv>

// Convert to a number
auto id_str = ctx.params.get("id").value_or("");
int id = 0;
auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), id);
if (ec != std::errc{}) {
    return response::error(problem_details::bad_request("Invalid ID"));
}
```

---

## Request Handlers

### Handler signature

```cpp
using handler_fn = inplace_function<
    result<response>(const request&, request_context&),
    160
>;
```

Handlers take:
- `const request&` — the HTTP request
- `request_context&` — the context with the arena and path parameters

They return:
- `result<response>` — either a response or an error_code

### Lambda handlers

```cpp
handler_fn([](const request& req, request_context& ctx) {
    return response::ok("Hello!");
})
```

### Capturing state

```cpp
struct user_repository { /* ... */ };

user_repository repo;

handler_fn([&repo](const request& req, request_context& ctx) {
    auto users = repo.find_all();
    // ...
    return response::ok("OK");
})
```

### Error handling

```cpp
handler_fn([](const request& req, request_context& ctx) {
    // Return an error_code
    if (something_wrong) {
        return std::unexpected(make_error_code(error_code::bad_request));
    }

    // Return Problem Details
    if (not_found) {
        return response::error(problem_details::not_found("Resource not found"));
    }

    // Success
    return response::ok("Success");
})
```

---

## HTTP Methods

Supported methods:

```cpp
method::get      // GET
method::head     // HEAD
method::post     // POST
method::put      // PUT
method::del      // DELETE
method::patch    // PATCH
method::options  // OPTIONS
```

### Multiple methods for one path

```cpp
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/{id}">(), get_user_handler},
    {method::put, path_pattern::from_literal<"/users/{id}">(), update_user_handler},
    {method::del, path_pattern::from_literal<"/users/{id}">(), delete_user_handler},
};
```

---

## Automatic Error Handling

### 404 Not Found

Returned automatically when the path is not found:

```cpp
// No route for /missing
request req;
req.http_method = method::get;
req.uri = "/missing";

auto resp = dispatch_or_problem(r, req, ctx);
// resp.status == 404
// resp contains RFC 7807 Problem Details
```

**Response:**
```json
{
  "type": "about:blank",
  "title": "Not Found",
  "status": 404,
  "detail": "The requested resource was not found"
}
```

### 405 Method Not Allowed

Returned automatically when the method is not supported for the path:

```cpp
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/{id}">(), handler},
};

router r(routes);

request req;
req.http_method = method::post;  // POST is not supported
req.uri = "/users/1";

auto resp = dispatch_or_problem(r, req, ctx);
// resp.status == 405
// resp.headers["Allow"] == "GET"
```

**Response:**
```http
HTTP/1.1 405 Method Not Allowed
Allow: GET
Content-Type: application/problem+json

{
  "type": "about:blank",
  "title": "Method Not Allowed",
  "status": 405
}
```

### Allow header

On a 405, an `Allow` header is generated automatically with all allowed methods:

```cpp
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/items/{id}">(), get_handler},
    {method::post, path_pattern::from_literal<"/items/{id}">(), post_handler},
    {method::del, path_pattern::from_literal<"/items/{id}">(), delete_handler},
};

// PUT /items/1 → 405 + "Allow: GET, POST, DELETE"
```

---

## Middleware

### Middleware signature

```cpp
using middleware_fn = inplace_function<
    result<response>(const request&, request_context&, next_fn),
    160
>;
```

Middleware takes:
- `const request&` — the request
- `request_context&` — the context
- `next_fn` — a function that calls the next middleware/handler

### Logging middleware

```cpp
middleware_fn logging_middleware([](const request& req, request_context& ctx, next_fn next) {
    std::cout << "[REQUEST] " << method_to_string(req.http_method) << " " << req.uri << "\n";

    auto result = next();  // call the next middleware/handler

    if (result) {
        std::cout << "[RESPONSE] " << result->status << "\n";
    }

    return result;
});
```

### Authentication middleware

```cpp
middleware_fn auth_middleware([](const request& req, request_context& ctx, next_fn next) {
    auto token = req.headers.get("Authorization");

    if (!token || !validate_token(*token)) {
        return result<response>(
            response::error(problem_details::unauthorized("Invalid token"))
        );
    }

    // Token valid, proceed
    return next();
});
```

### CORS middleware

```cpp
middleware_fn cors_middleware([](const request& req, request_context& ctx, next_fn next) {
    // Handle preflight
    if (req.http_method == method::options) {
        response resp;
        resp.status = 204;
        resp.set_header("Access-Control-Allow-Origin", "*");
        resp.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        return result<response>(std::move(resp));
    }

    // Normal request
    auto result = next();
    if (result) {
        result->set_header("Access-Control-Allow-Origin", "*");
    }
    return result;
});
```

### Middleware chains

```cpp
std::array<middleware_fn, 3> global_middleware = {
    logging_middleware(),
    cors_middleware(),
    auth_middleware(),
};

auto chain = make_middleware_chain(global_middleware);

route_entry routes[] = {
    {method::get,
     path_pattern::from_literal<"/protected">(),
     handler,
     chain},  // apply the middleware chain
};
```

**Execution order:**
```
Request → logging → cors → auth → handler → auth → cors → logging → Response
          ↓         ↓      ↓        ↓          ↑      ↑      ↑
          before    before before  execute    after  after  after
```

### Per-route middleware

Different routes can have different middleware:

```cpp
auto public_middleware = make_middleware_chain(std::array{logging_middleware()});
auto protected_middleware = make_middleware_chain(std::array{
    logging_middleware(),
    auth_middleware()
});

route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/public">(),
     public_handler, public_middleware},

    {method::get, path_pattern::from_literal<"/protected">(),
     protected_handler, protected_middleware},
};
```

---

## Query String Handling

Query strings are stripped automatically during matching:

```cpp
// GET /users/42?page=1&limit=10
// Matches /users/{id}, params = {id: "42"}

{method::get,
 path_pattern::from_literal<"/users/{id}">(),
 handler_fn([](const request& req, request_context& ctx) {
     auto id = ctx.params.get("id");  // "42"

     // The query string is available via req.uri
     // Query parameter parsing is manual (for now)
     auto query_start = req.uri.find('?');
     if (query_start != std::string_view::npos) {
         auto query = req.uri.substr(query_start + 1);
         // parse query...
     }

     return response::ok("OK");
 })}
```

---

## Request Context

```cpp
struct request_context {
    monotonic_arena& arena;     // arena for allocations
    path_params params;         // extracted path parameters
};
```

### Arena allocations

```cpp
handler_fn([](const request& req, request_context& ctx) {
    // Allocate from the arena
    auto* buffer = ctx.arena.allocate(1024);

    // Strings from the arena
    arena_string<> str(ctx.arena);
    str.append("Hello");

    return response::ok("OK");
    // The arena is reset automatically after the request completes
})
```

---

## Dispatch API

### `dispatch()`

Plain dispatch without automatic error handling:

```cpp
router r(routes);
monotonic_arena arena;
request_context ctx{arena};

result<response> res = r.dispatch(req, ctx);

if (res) {
    // Success
    send_response(*res);
} else {
    // Error (404, 405, etc.)
    auto error_code = res.error();
    handle_error(error_code);
}
```

### `dispatch_with_info()`

Dispatch with extra information:

```cpp
dispatch_result res = r.dispatch_with_info(req, ctx);

if (res.path_matched) {
    // Path found, but the method is wrong (405)
    std::string allow = allow_header_from_mask(res.allowed_methods_mask);
    // allow == "GET, POST"
} else {
    // Path not found (404)
}
```

### `dispatch_or_problem()`

**The recommended approach** — maps errors to RFC 7807 automatically:

```cpp
response resp = dispatch_or_problem(r, req, ctx);
// Always returns a response (200, 404, 405, 500)
send_response(resp);
```

---

## Integration with HTTP Server

### Using `router_handler`

Simple integration with an existing HTTP server:

```cpp
router r(routes);
http::router_handler handler(r);

// In your HTTP server loop:
monotonic_arena arena;
const request& req = parse_request(...);

response resp = handler(req, arena);
send_response(resp);
```

---

## Performance Characteristics

### Compile-time guarantees

- ✅ Path patterns are validated at compile time
- ✅ No runtime path parsing
- ✅ All data structures are constexpr-safe

### Zero-allocation hot path

```cpp
// Dispatch — zero heap allocations
route_entry routes[] = { /* ... */ };
router r(routes);  // routes are stored as a std::span

monotonic_arena arena;
request_context ctx{arena};  // path_params on the stack

auto resp = r.dispatch(req, ctx);  // no heap alloc
```

**What is NOT allocated:**
- Route table (passed as a span)
- Path parameters (fixed-size array on the stack)
- Middleware chain (passed as pointer + size)

**What is allocated:**
- Response body (if dynamic)
- Headers (if added in the handler)

### Routing complexity

- **Time:** O(N) where N is the number of routes
- **Space:** O(1) stack space
- **Optimization:** Linear scan with early exit on the first match

For a large number of routes (> 100), group by prefix or use the upcoming compile-time routing table generated from OpenAPI.

---

## Best Practices

### 1. Use `dispatch_or_problem()`

```cpp
// ✅ Good
response resp = dispatch_or_problem(r, req, ctx);

// ❌ Avoid (requires manual error handling)
result<response> res = r.dispatch(req, ctx);
if (!res) { /* handle 404/405 manually */ }
```

### 2. Static routes before parameters

```cpp
// ✅ Good (automatic prioritization)
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/me">(), ...},
    {method::get, path_pattern::from_literal<"/users/{id}">(), ...},
};

// Order doesn't matter — static segments always take priority
```

### 3. Validate parameters

```cpp
handler_fn([](const request& req, request_context& ctx) {
    auto id_str = ctx.params.get("id");
    if (!id_str) {
        return response::error(problem_details::bad_request("Missing id"));
    }

    int id;
    auto [ptr, ec] = std::from_chars(id_str->data(), id_str->data() + id_str->size(), id);
    if (ec != std::errc{}) {
        return response::error(problem_details::bad_request("Invalid id format"));
    }

    if (id <= 0) {
        return response::error(problem_details::bad_request("Id must be positive"));
    }

    // id valid
    return response::ok("OK");
})
```

### 4. Middleware order matters

```cpp
// ✅ Good (error recovery on the outside)
std::array<middleware_fn, 3> middleware = {
    error_recovery_middleware(),  // 1. Catch exceptions
    logging_middleware(),          // 2. Log request/response
    auth_middleware(),             // 3. Validate auth
};

// ❌ Bad (auth exceptions won't be caught)
std::array<middleware_fn, 3> middleware = {
    auth_middleware(),
    logging_middleware(),
    error_recovery_middleware(),
};
```

### 5. Group routes by prefix

```cpp
// For better readability
route_entry routes[] = {
    // User routes
    {method::get, path_pattern::from_literal<"/api/users">(), ...},
    {method::get, path_pattern::from_literal<"/api/users/{id}">(), ...},
    {method::post, path_pattern::from_literal<"/api/users">(), ...},

    // Order routes
    {method::get, path_pattern::from_literal<"/api/orders">(), ...},
    {method::get, path_pattern::from_literal<"/api/orders/{id}">(), ...},
};
```

---

## Examples

Full working examples:

- **`examples/router_rest_api.cpp`** — REST API with CRUD operations
- **`examples/middleware_examples.cpp`** — Examples of every middleware type
- **`examples/hello_world_server.cpp`** — Minimal HTTP server with a router

---

## Roadmap

### Planned Features

- [ ] **OpenAPI codegen** — generate routes automatically from an OpenAPI spec
- [ ] **Query parameter parsing** — built-in query string parsing
- [ ] **Compile-time routing table** — O(1) lookup for large route sets
- [ ] **Regex path parameters** — `/users/{id:\d+}` with compile-time validation
- [ ] **Path prefixes** — grouping routes by prefix
- [ ] **Route metadata** — tags and descriptions for documentation

### Future Middleware

- [ ] Rate limiting (per-IP, per-user)
- [ ] Caching (etag, conditional requests)
- [ ] Compression (gzip, brotli)
- [ ] Request ID propagation
- [ ] OpenTelemetry tracing

---

## Troubleshooting

### Compile error: "route path cannot be empty"

```cpp
// ❌ Error
auto pattern = path_pattern::from_literal<"">();

// ✅ Fix
auto pattern = path_pattern::from_literal<"/">();
```

### Compile error: "route path must start with '/'"

```cpp
// ❌ Error
auto pattern = path_pattern::from_literal<"users">();

// ✅ Fix
auto pattern = path_pattern::from_literal<"/users">();
```

### Path parameter not found

```cpp
// Check whether the parameter is present
auto id = ctx.params.get("id");
if (!id) {
    // Parameter is missing
    return response::error(problem_details::bad_request("Missing id parameter"));
}
```

### Middleware chain not executing

```cpp
// Make sure the chain is passed into route_entry
route_entry routes[] = {
    {method::get,
     path_pattern::from_literal<"/path">(),
     handler,
     middleware_chain},  // ← don't forget this!
};
```

---

## See Also

- [OPENAPI.md](OPENAPI.md) — OpenAPI loader and AST
- [ARCHITECTURE.md](../ARCHITECTURE.md) — Overall framework architecture
- [TESTING.md](TESTING.md) — Testing routers
