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

    // GET /shop/customers/revenue
    // Customers ranked by revenue within a country
    virtual katana::result<void> customer_revenue(std::string_view country, int64_t limit, int64_t offset, response& out) = 0;

    // GET /shop/products/top
    // Top products per category
    virtual katana::result<void> top_products(int64_t limit, response& out) = 0;

    // GET /shop/categories/stats
    // Aggregate statistics per category
    virtual katana::result<void> category_stats(response& out) = 0;

    // GET /shop/orders/{id}
    // Order detail with aggregated line totals
    virtual katana::result<void> order_detail(int64_t id, response& out) = 0;

    // POST /shop/orders
    // Create an order
    virtual katana::result<void> create_order(const CreateOrderRequest& body, response& out) = 0;

};

// Optional async handler interface for generated routers.
// Implement only operations that should own deferred HTTP completion.
// Returning false falls back to the synchronous api_handler method.
struct async_api_handler {
    virtual ~async_api_handler() = default;

    virtual bool customer_revenue_async(std::string_view country, int64_t limit, int64_t offset, katana::http::async_response_writer out) {
        (void)country;
        (void)limit;
        (void)offset;
        (void)out;
        return false;
    }

    virtual bool top_products_async(int64_t limit, katana::http::async_response_writer out) {
        (void)limit;
        (void)out;
        return false;
    }

    virtual bool category_stats_async(katana::http::async_response_writer out) {
        (void)out;
        return false;
    }

    virtual bool order_detail_async(int64_t id, katana::http::async_response_writer out) {
        (void)id;
        (void)out;
        return false;
    }

    virtual bool create_order_async(const CreateOrderRequest& body, katana::http::async_response_writer out) {
        (void)body;
        (void)out;
        return false;
    }

};

// Convenience base for async-first services.
// Override *_async methods only; synchronous fallbacks return 501.
struct async_api_handler_base : api_handler, async_api_handler {
    virtual ~async_api_handler_base() = default;

    katana::result<void> customer_revenue(std::string_view country, int64_t limit, int64_t offset, response& out) override {
        (void)country;
        (void)limit;
        (void)offset;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("customer_revenue requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> top_products(int64_t limit, response& out) override {
        (void)limit;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("top_products requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> category_stats(response& out) override {
        out = katana::http::response::error(
            katana::problem_details::not_implemented("category_stats requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> order_detail(int64_t id, response& out) override {
        (void)id;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("order_detail requires an async override or sync implementation"));
        return {};
    }

    katana::result<void> create_order(const CreateOrderRequest& body, response& out) override {
        (void)body;
        out = katana::http::response::error(
            katana::problem_details::not_implemented("create_order requires an async override or sync implementation"));
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
