// layer: flat
// Auto-generated handler interfaces from OpenAPI specification
// 
// Zero-boilerplate design:
//   - Clean signatures: result<void> method(params, response& out)
//   - Automatic validation: schema constraints checked before handler call
//   - Auto parameter binding: path/query/header/body → typed arguments
//   - Context access: use katana::http::req(), ctx(), arena() for access
// 
// A worked implementation example is at the bottom of this file.
#pragma once

#include "katana/core/http.hpp"
#include "katana/core/problem.hpp"
#include "katana/core/router.hpp"
#include "generated_dtos.hpp"
#include <string_view>
#include <optional>
#include <variant>

using katana::http::request;
using katana::http::response;
using katana::http::request_context;

namespace generated {

// Base handler interface for all API operations
// Implement these methods to handle requests - validation is automatic!
struct api_handler {
    virtual ~api_handler() = default;

    // POST /text/uppercase
    // Convert text to uppercase
    virtual katana::result<void> text_uppercase(const text_uppercase_request& body, response& out) = 0;

    // POST /text/lowercase
    // Convert text to lowercase
    virtual katana::result<void> text_lowercase(const text_lowercase_request& body, response& out) = 0;

    // POST /text/reverse
    // Reverse text
    virtual katana::result<void> text_reverse(const text_reverse_request& body, response& out) = 0;

    // POST /text/stats
    // Calculate text statistics
    virtual katana::result<void> text_stats(const text_stats_request& body, response& out) = 0;

    // POST /text/transform
    // Apply multiple transformations
    virtual katana::result<void> text_transform(const text_transform_request& body, response& out) = 0;

};

// Optional async handler interface for generated routers.
// Implement only operations that should own deferred HTTP completion.
// Returning false falls back to the synchronous api_handler method.
struct async_api_handler {
    virtual ~async_api_handler() = default;

    virtual bool text_uppercase_async(const text_uppercase_request& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

    virtual bool text_lowercase_async(const text_lowercase_request& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

    virtual bool text_reverse_async(const text_reverse_request& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

    virtual bool text_stats_async(const text_stats_request& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

    virtual bool text_transform_async(const text_transform_request& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

};

// Convenience base for async-first services.
// Override *_async methods only; synchronous fallbacks return 501.
struct async_api_handler_base : api_handler, async_api_handler {
    virtual ~async_api_handler_base() = default;

    katana::result<void> text_uppercase(const text_uppercase_request& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("text_uppercase requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> text_lowercase(const text_lowercase_request& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("text_lowercase requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> text_reverse(const text_reverse_request& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("text_reverse requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> text_stats(const text_stats_request& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("text_stats requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> text_transform(const text_transform_request& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("text_transform requires an async override or sync implementation"));
        return {};
    }

};

// ============================================================================
// USAGE EXAMPLE
// ============================================================================
//
// class my_api : public generated::api_handler {
// public:
//     katana::result<void> text_uppercase(const text_uppercase_request& body, response& out) override {
//         auto input = body.text;
//         // Create the response on the request arena, fill it in
//         text_uppercase_response resp(&katana::http::arena());
//         // resp.result = ...your logic here...
//         respond::into(out).json(serialize_text_uppercase_response(resp));
//         return {};
//     }
// };
//
// Available response helpers:
//   - respond::into(out).text(...)
//   - respond::into(out).json(...)
//   - respond::into(out).created_json(...)
//   - respond::into(out).no_content()
//   - out.assign_error(katana::problem_details::bad_request(message))
//   - out.assign_error(katana::problem_details::unauthorized(message))
//   - out.assign_error(katana::problem_details::forbidden(message))
//   - out.assign_error(katana::problem_details::not_found(message))
//   - out.assign_error(katana::problem_details::internal_server_error(message))
//
// Context access functions (available in handler methods):
//   - katana::http::req()    - Get current request
//   - katana::http::ctx()    - Get request context
//   - katana::http::arena()  - Get arena allocator for zero-copy strings
//
} // namespace generated
