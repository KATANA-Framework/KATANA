#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"

#include <functional>
#include <stdexcept>
#include <string>

namespace katana::test_support {

// Lightweight harness to run HTTP handlers without spinning up reactor_pool.
class HttpHandlerHarness {
public:
    using Handler = std::function<http::response(const http::request&, monotonic_arena&)>;

    explicit HttpHandlerHarness(Handler handler) : handler_(std::move(handler)) {}

    // Parse raw HTTP request text, run handler, and return response.
    http::response run_raw(std::string raw_request) const {
        monotonic_arena arena;
        http::parser parser(&arena);
        auto result = parser.parse(http::as_bytes(raw_request));
        if (!result.has_value() || *result != http::parser::state::complete) {
            throw std::runtime_error("Failed to parse HTTP request in harness");
        }
        return handler_(parser.get_request(), arena);
    }

    // Run handler against an existing request (deep-copied into arena for safety).
    http::response run(const http::request& req) const {
        monotonic_arena arena;
        http::request copy = duplicate_request(req, arena);
        return handler_(copy, arena);
    }

private:
    static http::request duplicate_request(const http::request& req, monotonic_arena& arena) {
        http::request copy;
        copy.http_method = req.http_method;

        if (!req.uri.empty()) {
            char* uri = arena.allocate_string(req.uri);
            copy.uri = std::string_view(uri, req.uri.size());
        }

        copy.headers = http::headers_map(&arena);
        for (const auto& [name, value] : req.headers) {
            copy.headers.set_view(name, value);
        }

        if (!req.body.empty()) {
            char* body = arena.allocate_string(req.body);
            copy.body = std::string_view(body, req.body.size());
        }

        return copy;
    }

    Handler handler_;
};

} // namespace katana::test_support
