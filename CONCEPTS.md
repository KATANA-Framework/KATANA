# KATANA Framework Concepts

Core design philosophy and usage patterns of KATANA Framework.

---

## Design Philosophy

### STL-Style API

KATANA follows modern C++ standard library conventions:

- **Value semantics by default**: move-enabled types, no raw pointers
- **RAII everywhere**: automatic resource management, no manual cleanup
- **Generic programming**: templates, concepts, compile-time polymorphism
- **Standard vocabulary types**: `std::expected`, `std::string_view`, `std::span`, `std::pmr`
- **Composable abstractions**: small, focused components with clear contracts

Example from standard library analogy:
```cpp
// std::vector style
std::vector<int> vec;
vec.push_back(42);

// KATANA reactor style
katana::reactor r;
r.schedule([](){ /* task */ });
r.run();
```

### Reactor-per-Core Isolation

Each CPU core operates an **independent event loop** with:

- **No shared mutable state** between reactors
- **No locks** in the critical path
- **No inter-thread communication** during request processing
- **Per-core resources**: connection pools, caches, arena allocators

```
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│  Core 0      │  │  Core 1      │  │  Core N      │
│ ┌──────────┐ │  │ ┌──────────┐ │  │ ┌──────────┐ │
│ │ Reactor  │ │  │ │ Reactor  │ │  │ │ Reactor  │ │
│ │  epoll   │ │  │ │  epoll   │ │  │ │  epoll   │ │
│ └──────────┘ │  │ └──────────┘ │  │ └──────────┘ │
│ ┌──────────┐ │  │ ┌──────────┐ │  │ ┌──────────┐ │
│ │ DB Pool  │ │  │ │ DB Pool  │ │  │ │ DB Pool  │ │
│ └──────────┘ │  │ └──────────┘ │  │ └──────────┘ │
│ ┌──────────┐ │  │ ┌──────────┐ │  │ ┌──────────┐ │
│ │  Cache   │ │  │ │  Cache   │ │  │ │  Cache   │ │
│ └──────────┘ │  │ └──────────┘ │  │ └──────────┘ │
└──────────────┘  └──────────────┘  └──────────────┘
```

**Benefits of isolation**:
- No race conditions on request state
- No lock contention
- Predictable latencies (p99/p999)
- CPU cache locality (L1/L2 stay hot)

### Thread Pinning: Optional Optimization

Thread pinning (`sched_setaffinity`) is **NOT a correctness requirement**:

**Correctness comes from isolation, not pinning**:
- Each reactor has completely independent state
- No shared data structures between threads
- No mutex/locks to protect

**Thread pinning is a performance optimization**:
- Reduces L1/L2 cache invalidation on thread migration
- Improves NUMA locality on multi-socket systems
- Stabilizes tail latencies (p99/p999)

**When to use pinning**:
- Production deployments with strict latency SLAs
- Multi-socket NUMA systems
- High-throughput scenarios where cache locality matters

**When to skip pinning**:
- Development environments
- Platforms without `sched_setaffinity` (macOS, Windows require different APIs)
- Containerized environments where CPU affinity is managed externally
- Systems running multiple workloads (let OS scheduler decide)

```cpp
// Optional: enable pinning in production config
reactor_pool pool(std::thread::hardware_concurrency(),
                  reactor_pool::options{.enable_pinning = true});

// Development: skip pinning
reactor_pool pool(std::thread::hardware_concurrency(),
                  reactor_pool::options{.enable_pinning = false});
```

---

## Memory Model

### Arena-per-Request

All request-scoped allocations use a **monotonic arena allocator**:

```cpp
void handle_request(http::request req, arena& request_arena) {
    // All allocations come from arena
    std::pmr::polymorphic_allocator<char> alloc(&request_arena);
    std::pmr::vector<user> users(alloc);

    // ... process request ...

    // Arena automatically reset after response sent (RAII)
}
```

**Properties**:
- **O(1) allocation**: bump-pointer allocator
- **O(1) deallocation**: single reset operation after request completes
- **No fragmentation**: monotonic growth, then reset
- **Cache-friendly**: sequential allocation improves locality

### Zero-Copy String Views

Parsed data uses `std::string_view` to avoid unnecessary copies:

```cpp
struct http_request {
    std::string_view method;   // points into receive buffer
    std::string_view uri;      // no allocation needed
    std::span<const header> headers;
};
```

**Safety**: Views are only valid during request lifetime (arena manages backing memory).

### Allocation Modes

```cpp
// 1. Arena (default, production)
arena request_arena(64_KB);
std::pmr::vector<int> data(&request_arena);

// 2. Standard allocator (development/debugging)
std::vector<int> data;  // uses global new/delete

// 3. Custom pmr (advanced use cases)
std::pmr::synchronized_pool_resource pool;
std::pmr::vector<int> data(&pool);
```

---

## Error Handling

### std::expected for Results

No exceptions in hot-path, use `std::expected` for fallible operations:

```cpp
auto parse_request(std::span<const char> data)
    -> std::expected<http_request, parse_error>;

auto handle_request(http_request req)
    -> std::expected<http_response, error_info>;

// Usage with monadic operations
auto result = parse_request(buffer)
    .and_then([](auto req) { return validate(req); })
    .and_then([](auto req) { return process(req); })
    .transform([](auto data) { return serialize(data); })
    .or_else([](auto err) { return error_response(err); });
```

### Problem Details (RFC 7807)

HTTP errors use standardized JSON format:

```cpp
if (!user) {
    return problem::not_found()
        .type("https://api.example.com/problems/user-not-found")
        .title("User Not Found")
        .detail("User with ID 42 does not exist")
        .instance("/users/42");
}
```

Serializes to:
```json
{
    "type": "https://api.example.com/problems/user-not-found",
    "title": "User Not Found",
    "status": 404,
    "detail": "User with ID 42 does not exist",
    "instance": "/users/42"
}
```

---

## Event Loop Integration

### Reactor Lifecycle

```cpp
// 1. Create reactor
reactor r;

// 2. Register handlers
r.on_readable(socket_fd, [](int fd) {
    // Handle read event
});

r.on_writable(socket_fd, [](int fd) {
    // Handle write event
});

// 3. Schedule tasks
r.schedule([]() {
    // Execute on next event loop iteration
});

// 4. Run event loop (blocking)
auto result = r.run();

if (!result) {
    std::cerr << "Reactor error: " << result.error().message() << "\n";
}
```

### Graceful Shutdown

```cpp
// Signal handler
std::signal(SIGINT, [](int) {
    reactor::stop_all();  // request graceful stop
});

reactor r;
r.run();  // returns when stop requested

// Cleanup happens automatically via RAII
```

---

## I/O Patterns

### Vectored I/O (scatter/gather)

Efficient multi-buffer operations:

```cpp
io_buffer buf;
buf.append("HTTP/1.1 200 OK\r\n");
buf.append("Content-Length: 13\r\n\r\n");
buf.append("Hello, World!");

// Single writev() syscall for all buffers
auto written = buf.write_to(socket_fd);
```

### Edge-Triggered epoll

```cpp
// EPOLLET: only notified on state transitions
reactor.register_fd(fd, EPOLLIN | EPOLLET);

// Handler must read until EAGAIN
void on_readable(int fd) {
    while (true) {
        auto result = read(fd, buffer, size);
        if (result == -1 && errno == EAGAIN) break;  // all data read
        // ... process data ...
    }
}
```

### Non-Blocking Sockets

All I/O operations are non-blocking:

```cpp
int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
// Returns immediately with EAGAIN if would block
```

---

## HTTP Processing

### Request Lifecycle

```cpp
// 1. Parse HTTP request (incremental, zero-copy)
http::parser p;
auto req = p.parse(receive_buffer);

// 2. Validate and route
if (!req) {
    return serialize(req.error());
}

// 3. Execute handler
auto response = handle_user_request(*req);

// 4. Serialize response
http::serializer s;
auto output = s.serialize(response);

// 5. Send via vectored I/O
output.write_to(socket_fd);
```

### Keep-Alive Connections

```cpp
bool should_keep_alive(const http_request& req) {
    auto conn = req.header("Connection");
    if (req.version == http_version::http_1_1) {
        return conn != "close";  // keep-alive by default in HTTP/1.1
    } else {
        return conn == "keep-alive";  // opt-in for HTTP/1.0
    }
}
```

---

## Resource Limits

### Built-in Safety Limits

```cpp
constexpr size_t MAX_HEADER_SIZE   = 8_KB;
constexpr size_t MAX_BODY_SIZE     = 10_MB;
constexpr size_t MAX_URI_LENGTH    = 2_KB;
constexpr size_t MAX_HEADER_COUNT  = 100;
```

### Timeouts

```cpp
constexpr auto READ_TIMEOUT  = 30s;
constexpr auto WRITE_TIMEOUT = 30s;
constexpr auto IDLE_TIMEOUT  = 60s;

reactor.schedule_timeout(READ_TIMEOUT, [fd]() {
    close_connection(fd);  // timeout expired
});
```

### File Descriptor Limits

```cpp
system_limits::raise_fd_limit(65536);  // request higher ulimit
```

---

## Wheel Timer

Fast constant-time timer operations:

```cpp
wheel_timer timer(512 /* slots */, 100ms /* tick */);

// O(1) add timeout
auto id = timer.add(5s, [](){ /* callback */ });

// O(1) cancel
timer.cancel(id);

// O(slots) tick processing
timer.tick();  // call every 100ms
```

**Use cases**:
- Connection idle timeouts
- Request processing deadlines
- Keep-alive expiration
- Rate limiting windows

---

## Metrics and Observability

### Built-in Metrics

```cpp
struct reactor_metrics {
    uint64_t tasks_executed;
    uint64_t events_processed;
    uint64_t timers_fired;
    uint64_t exceptions_caught;
};

auto metrics = reactor.get_metrics();
std::cout << "Tasks: " << metrics.tasks_executed << "\n";
```

### Integration Points (Stage 5+)

```cpp
// Prometheus metrics endpoint
GET /metrics
# TYPE http_requests_total counter
http_requests_total{method="GET",status="200"} 1234

// OpenTelemetry spans
auto span = tracer.start_span("handle_request");
span.set_attribute("http.method", "GET");
// ... process ...
span.end();
```

---

## Platform Abstraction

### Cross-Platform I/O Backend

```cpp
#ifdef __linux__
    using io_backend = epoll_reactor;
#elif defined(__APPLE__)
    using io_backend = kqueue_reactor;
#elif defined(_WIN32)
    using io_backend = iocp_reactor;
#endif

reactor<io_backend> r;  // platform-specific implementation
```

### CPU Topology Detection

```cpp
auto cores = cpu_info::physical_cores();  // detect physical cores
auto threads = cpu_info::logical_cores(); // detect hyperthreads

// NUMA-aware (Linux only)
if (cpu_info::has_numa()) {
    auto nodes = cpu_info::numa_nodes();
}
```

---

## Usage Patterns

### Pattern 1: Simple HTTP Server

```cpp
#include <katana/core/reactor.hpp>
#include <katana/core/http.hpp>

int main() {
    reactor r;

    int server_fd = create_server_socket(8080);
    r.on_readable(server_fd, [&](int fd) {
        int client = accept(fd);
        r.on_readable(client, [client, &r](int) {
            char buf[4096];
            ssize_t n = read(client, buf, sizeof(buf));

            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 13\r\n\r\n"
                "Hello, World!";
            write(client, response.data(), response.size());
            close(client);
        });
    });

    return r.run() ? 0 : 1;
}
```

### Pattern 2: Reactor Pool (Multi-Core)

```cpp
#include <katana/core/reactor_pool.hpp>

int main() {
    auto cores = std::thread::hardware_concurrency();
    reactor_pool pool(cores);

    pool.listen(8080, [](int client_fd, reactor& r) {
        // Each connection handled on reactor that accepted it
        handle_connection(client_fd, r);
    });

    pool.run();  // blocks until shutdown
    return 0;
}
```

### Pattern 3: Arena-Managed Request

```cpp
void process_request(http_request req, arena& arena) {
    std::pmr::polymorphic_allocator<char> alloc(&arena);

    // All allocations from arena
    std::pmr::vector<std::pmr::string> tokens(alloc);
    parse_query(req.uri, tokens);

    auto response = generate_response(tokens);
    send_response(response);

    // Arena automatically reset after function returns
}
```

---

## Performance Considerations

### Cache Locality

Reactor-per-core ensures:
- **L1 cache**: hot code paths stay in 32-64KB L1
- **L2 cache**: working set fits in 256KB-1MB L2
- **No false sharing**: each reactor on separate cache lines

### System Call Minimization

- **Vectored I/O**: `writev()` instead of multiple `write()` calls
- **Edge-triggered epoll**: notification only on state change
- **Batch operations**: process multiple events per `epoll_wait()`

### Allocation Efficiency

- **Arena allocator**: ~10 CPU cycles per allocation (vs ~100 for malloc)
- **No deallocation overhead**: single reset operation
- **No fragmentation**: linear allocation pattern

---

## Anti-Patterns to Avoid

### ❌ Sharing State Between Reactors

```cpp
// BAD: shared mutex across reactors
std::mutex global_lock;
std::unordered_map<int, user> users;  // contention!

// GOOD: per-reactor state
struct reactor_context {
    std::unordered_map<int, user> users;  // no lock needed
};
```

### ❌ Blocking Operations in Event Loop

```cpp
// BAD: blocks event loop
r.schedule([]() {
    sleep(1);  // entire reactor stalls!
});

// GOOD: use timer
r.schedule_timeout(1s, []() {
    // executes after 1s without blocking
});
```

### ❌ Long-Lived Arena Allocations

```cpp
// BAD: arena allocated but never reset
arena global_arena;  // leaks memory!

// GOOD: arena per request (RAII)
void handle(http_request req) {
    arena request_arena;
    // ... use arena ...
}  // arena destroyed, memory released
```

---

## Inspiration and References

KATANA draws inspiration from:

- **C++ Standard Library**: value semantics, RAII, generic programming
- **Seastar Framework**: reactor-per-core, share-nothing architecture
- **Nginx**: event-driven I/O, worker processes isolation
- **Qt's QSockets**: STL-like API design, clear ownership semantics
- **Rust's tokio**: explicit async boundaries, zero-cost abstractions

Key differences from other frameworks:
- **No coroutines in Stage 1**: simple synchronous handlers, complexity added later
- **Zero external dependencies**: only C++23 stdlib and Linux syscalls
- **Contract-first**: OpenAPI/SQL generate code, not annotations
- **Built-in observability**: metrics and tracing from day one

---

## Next Steps

- [README.md](README.md) - Framework overview and quick start
- [ARCHITECTURE.md](ARCHITECTURE.md) - Detailed architecture design
- [Stage 1 Roadmap](README.md#этап-1--базовый-runtime) - Current development stage
- `examples/hello_world_server.cpp` - Working example implementation
