#include "katana/core/arena.hpp"
#include "katana/core/fd_watch.hpp"
#include "katana/core/http.hpp"
#include "katana/core/io_buffer.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"
#include "katana/core/shutdown.hpp"
#include "katana/core/tcp_listener.hpp"
#include "katana/core/tcp_socket.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace katana;
using namespace katana::http;
using namespace std::chrono;

// ============================================================================
// Logging Middleware
// ============================================================================

middleware_fn logging_middleware() {
    return middleware_fn([](const request& req, request_context& ctx, next_fn next) {
        (void)ctx;
        auto start = steady_clock::now();

        std::cout << "[REQUEST] " << method_to_string(req.http_method) << " " << req.uri << "\n";

        auto result = next();

        auto end = steady_clock::now();
        auto duration_ms = duration_cast<milliseconds>(end - start).count();

        if (result) {
            std::cout << "[RESPONSE] " << req.uri << " -> " << result->status << " (" << duration_ms
                      << "ms)\n";
        } else {
            std::cout << "[ERROR] " << req.uri << " -> error (" << duration_ms << "ms)\n";
        }

        return result;
    });
}

// ============================================================================
// Request ID Middleware
// ============================================================================

middleware_fn request_id_middleware() {
    static std::atomic<uint64_t> counter{0};

    return middleware_fn([](const request& req, request_context& ctx, next_fn next) {
        (void)ctx;
        (void)ctx;
        uint64_t request_id = counter.fetch_add(1, std::memory_order_relaxed);

        std::cout << "[REQ-" << request_id << "] Processing " << req.uri << "\n";

        auto result = next();

        if (result) {
            // Add X-Request-ID header to response
            result->set_header("X-Request-ID", std::to_string(request_id));
        }

        return result;
    });
}

// ============================================================================
// CORS Middleware
// ============================================================================

middleware_fn cors_middleware(std::string_view allowed_origin = "*") {
    return middleware_fn([allowed_origin](const request& req, request_context& ctx, next_fn next) {
        (void)ctx;
        // Handle preflight OPTIONS request
        if (req.http_method == method::options) {
            response resp;
            resp.status = 204;
            resp.reason = "No Content";
            resp.set_header("Access-Control-Allow-Origin", std::string(allowed_origin));
            resp.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            resp.set_header("Access-Control-Max-Age", "86400");
            return result<response>(std::move(resp));
        }

        // Process normal request
        auto result = next();

        if (result) {
            result->set_header("Access-Control-Allow-Origin", std::string(allowed_origin));
        }

        return result;
    });
}

// ============================================================================
// Authentication Middleware (Bearer Token)
// ============================================================================

middleware_fn auth_middleware(std::string_view valid_token = "secret-token-123") {
    return middleware_fn([valid_token](const request& req, request_context& ctx, next_fn next) {
        (void)ctx;
        auto auth_header = req.headers.get("Authorization");

        if (!auth_header) {
            auto problem = problem_details::unauthorized("Missing Authorization header");
            problem.detail = "Please provide a valid Bearer token";
            return result<response>(response::error(problem));
        }

        // Check if it starts with "Bearer "
        if (!auth_header->starts_with("Bearer ")) {
            auto problem = problem_details::unauthorized("Invalid Authorization format");
            problem.detail = "Expected: Bearer <token>";
            return result<response>(response::error(problem));
        }

        auto token = auth_header->substr(7); // Skip "Bearer "

        if (token != valid_token) {
            auto problem = problem_details::unauthorized("Invalid token");
            return result<response>(response::error(problem));
        }

        // Token is valid, proceed
        return next();
    });
}

// ============================================================================
// Rate Limiting Middleware (simple in-memory)
// ============================================================================

class simple_rate_limiter {
public:
    explicit simple_rate_limiter(size_t max_requests, milliseconds window)
        : max_requests_(max_requests), window_(window) {}

    bool allow_request() {
        auto now = steady_clock::now();

        // Clean old requests outside the window
        while (!requests_.empty() && (now - requests_.front()) > window_) {
            requests_.erase(requests_.begin());
        }

        if (requests_.size() >= max_requests_) {
            return false;
        }

        requests_.push_back(now);
        return true;
    }

private:
    size_t max_requests_;
    milliseconds window_;
    std::vector<steady_clock::time_point> requests_;
};

middleware_fn rate_limit_middleware(size_t max_requests = 100,
                                    milliseconds window = milliseconds(60000)) {
    // NOTE: In production, this should be per-IP or per-user
    // This is a global rate limiter for demonstration
    auto limiter = std::make_shared<simple_rate_limiter>(max_requests, window);

    return middleware_fn([limiter](const request&, request_context&, next_fn next) {
        if (!limiter->allow_request()) {
            auto problem = problem_details::service_unavailable(
                "Rate limit exceeded. Please try again later.");

            auto resp = response::error(problem);
            resp.set_header("Retry-After", "60");
            return result<response>(std::move(resp));
        }

        return next();
    });
}

// ============================================================================
// Content Type Validation Middleware
// ============================================================================

middleware_fn content_type_middleware(std::string_view required_type = "application/json") {
    return middleware_fn([required_type](const request& req, request_context& ctx, next_fn next) {
        (void)ctx;
        // Only check POST/PUT/PATCH requests
        if (req.http_method == method::post || req.http_method == method::put ||
            req.http_method == method::patch) {

            auto content_type = req.headers.get("Content-Type");

            if (!content_type) {
                auto problem = problem_details::bad_request("Missing Content-Type header");
                problem.detail = "Expected: " + std::string(required_type);
                return result<response>(response::error(problem));
            }

            // Simple check (not parsing charset, etc.)
            if (!content_type->starts_with(required_type)) {
                auto problem = problem_details::bad_request("Expected Content-Type: " +
                                                            std::string(required_type));
                return result<response>(response::error(problem));
            }
        }

        return next();
    });
}

// ============================================================================
// Error Recovery Middleware
// ============================================================================

middleware_fn error_recovery_middleware() {
    return middleware_fn([](const request& req, request_context& ctx, next_fn next) {
        (void)req;
        (void)ctx;
        try {
            return next();
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception in handler: " << e.what() << "\n";

            auto problem = problem_details::internal_server_error();
            problem.detail = "An unexpected error occurred";
            return result<response>(response::error(problem));
        } catch (...) {
            std::cerr << "[ERROR] Unknown exception in handler\n";

            auto problem = problem_details::internal_server_error();
            problem.detail = "An unknown error occurred";
            return result<response>(response::error(problem));
        }
    });
}

// ============================================================================
// Example Application
// ============================================================================

struct connection_state {
    tcp_socket socket;
    io_buffer read_buffer;
    io_buffer write_buffer;
    monotonic_arena arena;
    parser http_parser;
    std::unique_ptr<fd_watch> watch;

    explicit connection_state(tcp_socket sock)
        : socket(std::move(sock)), read_buffer(8192), write_buffer(8192), arena(8192),
          http_parser(&arena) {}
};

void handle_connection(connection_state& state, reactor& r, const router& api_router);
void accept_connection(reactor& r,
                       tcp_listener& listener,
                       std::vector<std::unique_ptr<connection_state>>& connections,
                       const router& api_router);

void handle_connection(connection_state& state,
                       [[maybe_unused]] reactor& r,
                       const router& api_router) {
    while (true) {
        auto buf = state.read_buffer.writable_span(4096);
        auto read_result = state.socket.read(buf);

        if (!read_result) {
            if (read_result.error().value() == EAGAIN ||
                read_result.error().value() == EWOULDBLOCK) {
                break;
            }
            state.watch.reset();
            return;
        }

        if (read_result->empty()) {
            state.watch.reset();
            return;
        }

        state.read_buffer.commit(read_result->size());
        auto readable = state.read_buffer.readable_span();
        auto parse_result = state.http_parser.parse(readable);

        if (!parse_result) {
            auto resp = response::error(problem_details::bad_request("Invalid HTTP request"));
            state.write_buffer.append(resp.serialize());
            state.watch.reset();
            return;
        }

        if (!state.http_parser.is_complete()) {
            continue;
        }

        const auto& req = state.http_parser.get_request();
        request_context ctx{state.arena};
        auto resp = dispatch_or_problem(api_router, req, ctx);

        state.write_buffer.append(resp.serialize());

        while (!state.write_buffer.empty()) {
            auto data = state.write_buffer.readable_span();
            auto write_result = state.socket.write(data);

            if (!write_result) {
                if (write_result.error().value() == EAGAIN ||
                    write_result.error().value() == EWOULDBLOCK) {
                    break;
                }
                state.watch.reset();
                return;
            }

            if (write_result.value() == 0) {
                break;
            }

            state.write_buffer.consume(write_result.value());
        }

        if (state.write_buffer.empty()) {
            state.watch.reset();
        }

        return;
    }
}

void accept_connection(reactor& r,
                       tcp_listener& listener,
                       std::vector<std::unique_ptr<connection_state>>& connections,
                       const router& api_router) {
    auto accept_result = listener.accept();
    if (!accept_result) {
        return;
    }

    auto state = std::make_unique<connection_state>(std::move(*accept_result));
    int32_t fd = state->socket.native_handle();

    auto* state_ptr = state.get();
    state->watch = std::make_unique<fd_watch>(
        r, fd, event_type::readable, [state_ptr, &r, &api_router](event_type) {
            handle_connection(*state_ptr, r, api_router);
        });

    connections.push_back(std::move(state));
}

int main() {
    // Create middleware chain
    std::array<middleware_fn, 5> global_middleware = {
        error_recovery_middleware(),
        logging_middleware(),
        request_id_middleware(),
        cors_middleware("*"),
        content_type_middleware("application/json"),
    };

    auto middleware_chain = make_middleware_chain(global_middleware);

    // Create protected middleware (requires auth)
    std::array<middleware_fn, 1> protected_middleware = {
        auth_middleware("secret-token-123"),
    };

    auto protected_chain = make_middleware_chain(protected_middleware);

    // Define routes
    route_entry routes[] = {
        // Public endpoint
        {method::get,
         path_pattern::from_literal<"/api/health">(),
         handler_fn([](const request&, request_context&) {
             return response::json("{\"status\":\"healthy\"}");
         }),
         middleware_chain},

        // Public endpoint with rate limiting
        {method::get,
         path_pattern::from_literal<"/api/public">(),
         handler_fn([](const request&, request_context&) {
             return response::json("{\"message\":\"This is a public endpoint\"}");
         }),
         middleware_chain},

        // Protected endpoint (requires auth token)
        {method::get,
         path_pattern::from_literal<"/api/protected">(),
         handler_fn([](const request&, request_context&) {
             return response::json("{\"message\":\"This is a protected resource\"}");
         }),
         protected_chain},

        // Echo endpoint (with all middleware)
        {method::post,
         path_pattern::from_literal<"/api/echo">(),
         handler_fn([](const request& req, request_context&) {
             std::string body = "{\"echo\":\"";
             body.append(req.body.data(), req.body.size());
             body.push_back('"');
             body.push_back('}');
             return response::json(std::move(body));
         }),
         middleware_chain},
    };

    router api_router(routes);

    // Setup TCP listener
    tcp_listener listener(8080);
    if (!listener) {
        std::cerr << "Failed to create listener on port 8080\n";
        return 1;
    }

    listener.set_reuseport(true).set_backlog(1024);

    reactor_pool_config config;
    config.reactor_count = 1;
    reactor_pool pool(config);

    std::vector<std::unique_ptr<connection_state>> connections;
    std::unique_ptr<fd_watch> accept_watch;

    auto& reactor = pool.get_reactor(0);
    accept_watch = std::make_unique<fd_watch>(
        reactor,
        listener.native_handle(),
        event_type::readable,
        [&reactor, &listener, &connections, &api_router](event_type) {
            accept_connection(reactor, listener, connections, api_router);
        });

    std::cout << "Middleware Examples Server listening on http://localhost:8080\n\n";

    std::cout << "Available Endpoints:\n";
    std::cout << "  GET  /api/health     - Health check (public)\n";
    std::cout << "  GET  /api/public     - Public endpoint\n";
    std::cout << "  GET  /api/protected  - Protected endpoint (requires auth)\n";
    std::cout << "  POST /api/echo       - Echo JSON body\n\n";

    std::cout << "Active Middleware:\n";
    std::cout << "  - Error Recovery     : Catches exceptions and returns 500\n";
    std::cout << "  - Logging            : Logs all requests and responses\n";
    std::cout << "  - Request ID         : Adds X-Request-ID header\n";
    std::cout << "  - CORS               : Adds CORS headers for all origins\n";
    std::cout << "  - Content-Type Check : Validates application/json for POST/PUT\n";
    std::cout << "  - Authentication     : Bearer token validation (protected routes)\n\n";

    std::cout << "Example Commands:\n";
    std::cout << "  # Public endpoint\n";
    std::cout << "  curl http://localhost:8080/api/public\n\n";

    std::cout << "  # Protected endpoint (will fail without token)\n";
    std::cout << "  curl http://localhost:8080/api/protected\n\n";

    std::cout << "  # Protected endpoint with valid token\n";
    std::cout << "  curl -H \"Authorization: Bearer secret-token-123\" "
                 "http://localhost:8080/api/protected\n\n";

    std::cout << "  # Echo with JSON\n";
    std::cout << "  curl -X POST -H \"Content-Type: application/json\" \\\n";
    std::cout << "       -d '{\"test\":\"data\"}' http://localhost:8080/api/echo\n\n";

    std::cout << "  # CORS preflight\n";
    std::cout << "  curl -X OPTIONS http://localhost:8080/api/public\n\n";

    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback(
        [&pool]() { pool.graceful_stop(std::chrono::milliseconds(5000)); });

    pool.start();
    pool.wait();

    std::cout << "Server stopped\n";
    return 0;
}
