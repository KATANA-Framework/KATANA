# HTTP Server Abstraction

High-level HTTP server abstraction that eliminates boilerplate code for request handling, connection management, and reactor pool setup.

## Overview

The `katana::http::server` class provides a fluent interface for building HTTP servers with minimal code. It handles all the low-level details:

- Reactor pool management
- TCP listener setup
- Connection state management
- Request parsing and response serialization
- Graceful shutdown
- Request/response lifecycle hooks

## Quick Start

```cpp
#include "katana/core/http_server.hpp"
#include "katana/core/router.hpp"

using namespace katana::http;

int main() {
    // Define routes
    route_entry routes[] = {
        {method::get,
         path_pattern::from_literal<"/">(),
         handler_fn([](const request& req, request_context& ctx) {
             return response::ok("Hello, World!");
         })},
    };

    router api_router(routes);

    // That's it! Server handles everything else
    return server(api_router)
        .listen(8080)
        .workers(4)
        .run();
}
```

## API Reference

### Constructor

```cpp
explicit server(const router& rt);
```

Creates a server instance with the given router. The router defines all HTTP endpoints and their handlers.

### Configuration Methods

All configuration methods return `server&`, enabling fluent method chaining.

#### `server& listen(uint16_t port)`

Set the port to listen on. Uses `0.0.0.0` as the default host (all interfaces).

```cpp
server(router)
    .listen(8080)
    .run();
```

#### `server& bind(const std::string& host, uint16_t port)`

Set both the host and port to bind to.

```cpp
server(router)
    .bind("127.0.0.1", 8080)  // localhost only
    .run();
```

#### `server& workers(size_t count)`

Set the number of worker threads (reactors). Defaults to the number of CPU cores.

```cpp
server(router)
    .listen(8080)
    .workers(4)  // 4 worker threads
    .run();
```

**Recommendation**: Use one worker per CPU core for optimal performance.

#### `server& backlog(int32_t size)`

Set the TCP listen backlog size. Defaults to 128.

```cpp
server(router)
    .listen(8080)
    .backlog(512)  // Larger backlog for high connection rates
    .run();
```

#### `server& reuseport(bool enable = true)`

Enable or disable `SO_REUSEPORT` socket option. Defaults to false.

```cpp
server(router)
    .listen(8080)
    .reuseport(true)  // Enable SO_REUSEPORT
    .run();
```

**Note**: `SO_REUSEPORT` allows multiple sockets to bind to the same port, improving load distribution across cores.

#### `server& graceful_shutdown(std::chrono::milliseconds timeout)`

Set the graceful shutdown timeout. Defaults to 5 seconds.

```cpp
server(router)
    .listen(8080)
    .graceful_shutdown(std::chrono::seconds(10))
    .run();
```

### Lifecycle Hooks

#### `server& on_start(std::function<void()> callback)`

Set a callback to be called after the server starts listening.

```cpp
server(router)
    .listen(8080)
    .on_start([]() {
        std::cout << "Server started!\n";
        std::cout << "Visit http://localhost:8080\n";
    })
    .run();
```

**Note**: If no `on_start` callback is set, the server prints default startup information.

#### `server& on_stop(std::function<void()> callback)`

Set a callback to be called when the server receives a shutdown signal.

```cpp
server(router)
    .listen(8080)
    .on_stop([]() {
        std::cout << "Server shutting down...\n";
        // Cleanup resources
    })
    .run();
```

#### `server& on_request(std::function<void(const request&, const response&)> callback)`

Set a callback to be called for each completed request. Useful for logging, metrics, etc.

```cpp
server(router)
    .listen(8080)
    .on_request([](const request& req, const response& resp) {
        std::cout << method_to_string(req.http_method) << " "
                  << req.uri << " -> " << resp.status << "\n";
    })
    .run();
```

**Performance Note**: This callback is called on the hot path. Keep it lightweight to avoid impacting request latency.

### Starting the Server

#### `int run()`

Starts the server and blocks until shutdown signal is received (SIGINT, SIGTERM).

Returns `0` on success, `1` on error.

```cpp
int exit_code = server(router)
    .listen(8080)
    .run();

return exit_code;
```

## Complete Example

Here's a complete example with all features:

```cpp
#include "katana/core/http_server.hpp"
#include "katana/core/router.hpp"
#include <iostream>

using namespace katana::http;

int main() {
    // Define API routes
    route_entry routes[] = {
        {method::get,
         path_pattern::from_literal<"/">(),
         handler_fn([](const request& req, request_context& ctx) {
             return response::ok("Hello, KATANA!");
         })},

        {method::get,
         path_pattern::from_literal<"/users/{id}">(),
         handler_fn([](const request& req, request_context& ctx) {
             auto id = ctx.params.get("id").value_or("unknown");
             return response::json(R"({"id":")" + std::string(id) + R"("})");
         })},
    };

    router api_router(routes);

    // Configure and run server
    return server(api_router)
        .bind("0.0.0.0", 8080)
        .workers(4)
        .backlog(512)
        .reuseport(true)
        .graceful_shutdown(std::chrono::seconds(10))
        .on_start([]() {
            std::cout << "✨ Server started!\n";
            std::cout << "\nTry:\n";
            std::cout << "  curl http://localhost:8080/\n";
            std::cout << "  curl http://localhost:8080/users/42\n\n";
        })
        .on_stop([]() {
            std::cout << "\n👋 Server stopping...\n";
        })
        .on_request([](const request& req, const response& resp) {
            std::cout << method_to_string(req.http_method) << " "
                      << req.uri << " -> " << resp.status << "\n";
        })
        .run();
}
```

## Code Reduction Comparison

### Before: Manual Setup (200+ lines)

```cpp
int main() {
    // Create TCP listener
    tcp_listener listener(8080);
    if (!listener) {
        std::cerr << "Failed to create listener\n";
        return 1;
    }
    listener.set_reuseport(false).set_backlog(128);

    // Setup reactor pool
    reactor_pool_config config;
    config.reactor_count = 4;
    reactor_pool pool(config);

    // Connection state management
    std::vector<std::unique_ptr<connection_state>> connections;
    std::unique_ptr<fd_watch> accept_watch;

    auto& r = pool.get_reactor(0);
    accept_watch = std::make_unique<fd_watch>(
        r, listener.native_handle(), event_type::readable,
        [&](event_type) {
            // Accept connection logic
            auto accept_result = listener.accept();
            if (!accept_result) return;

            auto state = std::make_unique<connection_state>(std::move(*accept_result));
            int32_t fd = state->socket.native_handle();

            auto* state_ptr = state.get();
            state->watch = std::make_unique<fd_watch>(
                r, fd, event_type::readable,
                [state_ptr, &r](event_type) {
                    // Request handling logic
                    // ... 100+ more lines ...
                });

            connections.push_back(std::move(state));
        });

    // Signal handlers
    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback([&pool]() {
        pool.graceful_stop(std::chrono::seconds(5));
    });

    std::cout << "Server listening on http://0.0.0.0:8080\n";
    std::cout << "Workers: 4\n";

    pool.start();
    pool.wait();

    return 0;
}
```

### After: Server Abstraction (~10 lines)

```cpp
int main() {
    router api_router(routes);

    return server(api_router)
        .listen(8080)
        .workers(4)
        .on_start([]() {
            std::cout << "Server listening on http://0.0.0.0:8080\n";
            std::cout << "Workers: 4\n";
        })
        .run();
}
```

**Result**: **95% code reduction** for common use cases!

## Integration with OpenAPI Codegen

The server abstraction works seamlessly with generated code:

```cpp
#include "katana/core/http_server.hpp"
#include "generated/generated_router_bindings.hpp"
#include "generated/generated_handlers.hpp"

int main() {
    // Your handler implementation
    MyApiHandler handler;

    // Generated router binding
    const auto& api_router = generated::make_router(handler);

    // Server handles everything else
    return http::server(api_router)
        .listen(8080)
        .workers(4)
        .run();
}
```

See the [Compute API codegen example](../examples/codegen/compute_api/) for a complete integration example.

## Performance Considerations

### Zero Overhead

The server abstraction is a **zero-overhead wrapper**:
- No virtual function calls in the hot path
- All configuration happens during initialization
- Callbacks are `std::function` but stored outside the hot path
- Request handling uses the same optimized code path as manual setup

### Request Processing Flow

1. **Accept**: New connection arrives, `tcp_listener::accept()` called
2. **Parse**: HTTP request parsed using arena-backed parser
3. **Route**: Router dispatches to handler (zero-allocation lookup)
4. **Handle**: Handler executes, returns response
5. **Serialize**: Response serialized to connection buffer
6. **Write**: Response written to socket
7. **Cleanup**: Arena reset, connection reused or closed

The server abstraction doesn't add any overhead to this flow.

### Scalability

- **Reactor-per-core**: Each worker thread runs its own reactor
- **No shared state**: Connections are handled entirely within one reactor
- **Lock-free hot path**: No mutex or atomic operations during request processing
- **Arena allocation**: Per-request memory allocated from monotonic arena, freed in one operation

## Error Handling

The server handles common error scenarios gracefully:

### Listener Creation Failure

```cpp
int exit_code = server(router)
    .listen(8080)
    .run();

// Returns 1 if listener creation fails (port in use, permission denied, etc.)
```

### Request Parsing Errors

Invalid HTTP requests automatically receive `400 Bad Request` responses with RFC 7807 Problem Details.

### Connection Failures

- Read errors: Connection closed gracefully
- Write errors: Connection closed, partial data discarded
- Timeout: Handled by reactor wheel timer (future feature)

### Shutdown

- **Graceful**: Server waits for in-flight requests to complete (up to timeout)
- **Signal handling**: SIGINT and SIGTERM trigger graceful shutdown
- **Cleanup**: All resources (connections, watchers, reactors) properly cleaned up

## Advanced Usage

### Custom Connection State

For advanced use cases requiring custom per-connection state, you can still use the lower-level APIs directly. The server abstraction is designed for 95% of use cases.

### Multiple Servers

You can run multiple servers on different ports (requires separate threads):

```cpp
std::thread t1([&]() {
    server(api_router).listen(8080).run();
});

std::thread t2([&]() {
    server(admin_router).listen(9090).run();
});

t1.join();
t2.join();
```

### Integration with Other Components

The server integrates seamlessly with other KATANA components:

- **Router**: Pass any router instance to the server
- **Middleware**: Use router middleware for auth, logging, etc.
- **Problem Details**: Automatic RFC 7807 error responses
- **Arena Allocators**: Per-request arena allocation built-in

## See Also

- [Router Documentation](ROUTER.md) - Define HTTP endpoints
- [OpenAPI Codegen](OPENAPI.md) - Generate routes from OpenAPI spec
- [Architecture](../ARCHITECTURE.md) - Framework design principles
- [Examples](../examples/) - Complete working examples
