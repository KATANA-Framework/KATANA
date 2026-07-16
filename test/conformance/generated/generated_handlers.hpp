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

    // GET /pets
    virtual katana::result<void> list_pets(std::optional<int64_t> limit, std::string_view X_Trace, std::optional<std::string_view> session, response& out) = 0;

    // POST /pets
    virtual katana::result<void> create_pet(const PetCreateRequest& body, response& out) = 0;

    // GET /pets/{petId}
    virtual katana::result<void> get_pet(int64_t petId, response& out) = 0;

    // DELETE /pets/{petId}
    virtual katana::result<void> delete_pet(int64_t petId, response& out) = 0;

};

// Optional async handler interface for generated routers.
// Implement only operations that should own deferred HTTP completion.
// Returning false falls back to the synchronous api_handler method.
struct async_api_handler {
    virtual ~async_api_handler() = default;

    virtual bool list_pets_async(std::optional<int64_t> limit, std::string_view X_Trace, std::optional<std::string_view> session, katana::http::async_response_writer out) {
        (void)limit;
        (void)X_Trace;
        (void)session;
        (void)out;
        return false;
    }

    virtual bool create_pet_async(const PetCreateRequest& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

    virtual bool get_pet_async(int64_t petId, katana::http::async_response_writer out) {
        (void)petId;
        (void)out;
        return false;
    }

    virtual bool delete_pet_async(int64_t petId, katana::http::async_response_writer out) {
        (void)petId;
        (void)out;
        return false;
    }

};

// Convenience base for async-first services.
// Override *_async methods only; synchronous fallbacks return 501.
struct async_api_handler_base : api_handler, async_api_handler {
    virtual ~async_api_handler_base() = default;

    katana::result<void> list_pets(std::optional<int64_t> limit, std::string_view X_Trace, std::optional<std::string_view> session, response& out) override {
        (void)limit;
        (void)X_Trace;
        (void)session;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("list_pets requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> create_pet(const PetCreateRequest& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("create_pet requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> get_pet(int64_t petId, response& out) override {
        (void)petId;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("get_pet requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> delete_pet(int64_t petId, response& out) override {
        (void)petId;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("delete_pet requires an async override or sync implementation"));
        return {};
    }

};

// ============================================================================
// USAGE NOTES
// ============================================================================
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
