#include "katana/core/http_server.hpp"
#include "katana/core/router.hpp"
#include "katana/core/system_limits.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

using namespace katana;
using namespace katana::http;

namespace {

constexpr uint16_t DEFAULT_PORT = 18080;

result<void> handle_root(const request&, request_context&, response& out) {
    out.assign_text("Hello, World!");
    return result<void>{};
}

result<void> handle_named_hello(const request&, request_context& ctx, response& out) {
    const std::string_view name = ctx.params.get("name").value_or("world");
    std::string body;
    body.reserve(7 + name.size());
    body.append("Hello ");
    body.append(name);
    body.push_back('!');
    out.assign_text(std::move(body));
    return result<void>{};
}

uint16_t read_port(const char* env_name, uint16_t fallback) {
    if (const char* value = std::getenv(env_name)) {
        int parsed = std::atoi(value);
        if (parsed > 0 && parsed < 65536) {
            return static_cast<uint16_t>(parsed);
        }
    }
    return fallback;
}

uint16_t server_port() {
    return read_port("PORT", read_port("HELLO_PORT", DEFAULT_PORT));
}

uint16_t worker_count() {
    if (const char* value = std::getenv("KATANA_WORKERS")) {
        int parsed = std::atoi(value);
        if (parsed > 0 && parsed < 65536) {
            return static_cast<uint16_t>(parsed);
        }
    }

    const uint32_t hw = std::max(1u, std::thread::hardware_concurrency());
    const uint32_t capped = std::min<uint32_t>(hw, 64);
    return static_cast<uint16_t>(capped);
}

const router& hello_router() {
    static const route_entry routes[] = {
        {method::get, path_pattern::from_literal<"/">(), handler_fn(handle_root)},
        {method::get,
         path_pattern::from_literal<"/hello/{name}">(),
         handler_fn(handle_named_hello)},
    };

    static const router r(routes);
    return r;
}

class hello_fast_router {
public:
    explicit hello_fast_router(const router& fallback) : fallback_(fallback) {}

    result<void> dispatch_to(const request& req, request_context& ctx, response& out) const {
        std::string_view path = req.uri;
        if (const size_t query_pos = path.find('?'); query_pos != std::string_view::npos) {
            path = path.substr(0, query_pos);
        }

        if (req.http_method == method::get && path == "/") {
            return handle_root(req, ctx, out);
        }

        return fallback_.dispatch(req, ctx, out);
    }

private:
    const router& fallback_;
};

const hello_fast_router& hello_dispatcher() {
    static const hello_fast_router dispatcher(hello_router());
    return dispatcher;
}

} // namespace

int main() {
    auto limits_result = system_limits::set_max_fds(65536);
    if (!limits_result) {
        std::cerr << "Failed to set max FDs: " << limits_result.error().message() << '\n';
    }

    const uint16_t port = server_port();
    const uint16_t workers = worker_count();

    return server(hello_dispatcher())
        .listen(port)
        .workers(workers)
        .on_start([&]() {
            std::cout << "Hello world server running on :" << port << " with " << workers
                      << " worker threads" << std::endl;
        })
        .run();
}
