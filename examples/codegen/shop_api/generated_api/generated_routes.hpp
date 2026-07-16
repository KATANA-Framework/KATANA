// layer: flat
#pragma once

#include "katana/core/http.hpp"
#include "katana/core/http_utils.hpp"
#include "katana/core/router.hpp"
#include <array>
#include <span>
#include <string_view>

namespace generated {

using katana::http_utils::content_type_info;

struct route_entry {
    std::string_view path;
    katana::http::method method;
    std::string_view operation_id;
    std::span<const content_type_info> consumes;
    std::span<const content_type_info> produces;
};

inline constexpr content_type_info content_types_0[] = {
    {"application/json"},
};

inline constexpr route_entry routes[] = {
    {"/shop/customers/revenue", katana::http::method::get, "customer_revenue", {}, content_types_0},
    {"/shop/products/top", katana::http::method::get, "top_products", {}, content_types_0},
    {"/shop/categories/stats", katana::http::method::get, "category_stats", {}, content_types_0},
    {"/shop/orders/{id}", katana::http::method::get, "order_detail", {}, content_types_0},
    {"/shop/orders", katana::http::method::post, "create_order", content_types_0, content_types_0},
};

inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);

// Compile-time route metadata for type safety
namespace route_metadata {
    // customer_revenue: GET /shop/customers/revenue
    struct customer_revenue_metadata {
        static constexpr std::string_view path = "/shop/customers/revenue";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "customer_revenue";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = false;
    };

    // top_products: GET /shop/products/top
    struct top_products_metadata {
        static constexpr std::string_view path = "/shop/products/top";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "top_products";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = false;
    };

    // category_stats: GET /shop/categories/stats
    struct category_stats_metadata {
        static constexpr std::string_view path = "/shop/categories/stats";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "category_stats";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = false;
    };

    // order_detail: GET /shop/orders/{id}
    struct order_detail_metadata {
        static constexpr std::string_view path = "/shop/orders/{id}";
        static constexpr katana::http::method method = katana::http::method::get;
        static constexpr std::string_view operation_id = "order_detail";
        static constexpr size_t path_param_count = 1;
        static constexpr bool has_request_body = false;
    };

    // create_order: POST /shop/orders
    struct create_order_metadata {
        static constexpr std::string_view path = "/shop/orders";
        static constexpr katana::http::method method = katana::http::method::post;
        static constexpr std::string_view operation_id = "create_order";
        static constexpr size_t path_param_count = 0;
        static constexpr bool has_request_body = true;
    };

} // namespace route_metadata

// Compile-time validations
static_assert(route_count > 0, "At least one route must be defined");
} // namespace generated
