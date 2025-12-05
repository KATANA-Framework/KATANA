#pragma once

#include "katana/core/router.hpp"
#include <utility>

namespace katana::http {

// Thread-local storage for handler context
// Allows handlers to access request and request_context without explicit parameters
class handler_context {
public:
    // Get current request (must be called within handler scope)
    static const request& req() {
        if (!current_request_) {
            throw std::runtime_error("handler_context::req() called outside handler scope");
        }
        return *current_request_;
    }

    // Get current request_context (must be called within handler scope)
    static request_context& ctx() {
        if (!current_context_) {
            throw std::runtime_error("handler_context::ctx() called outside handler scope");
        }
        return *current_context_;
    }

    // RAII scope guard for setting context
    // Automatically sets and clears thread-local context
    class scope {
    public:
        scope(const request& req, request_context& ctx)
            : prev_request_(current_request_), prev_context_(current_context_) {
            current_request_ = &req;
            current_context_ = &ctx;
        }

        ~scope() {
            current_request_ = prev_request_;
            current_context_ = prev_context_;
        }

        // Non-copyable, non-movable
        scope(const scope&) = delete;
        scope& operator=(const scope&) = delete;
        scope(scope&&) = delete;
        scope& operator=(scope&&) = delete;

    private:
        const request* prev_request_;
        request_context* prev_context_;
    };

    // Helper functions for common operations
    static monotonic_arena& arena() { return ctx().arena; }

    static const headers_map& headers() { return req().headers; }

    static std::string_view uri() { return req().uri; }

    static std::string_view body() { return req().body; }

    static method http_method() { return req().http_method; }

    static const path_params& params() { return ctx().params; }

private:
    static thread_local const request* current_request_;
    static thread_local request_context* current_context_;
};

// Convenience aliases for cleaner code
inline const request& req() {
    return handler_context::req();
}
inline request_context& ctx() {
    return handler_context::ctx();
}
inline monotonic_arena& arena() {
    return handler_context::arena();
}

} // namespace katana::http
