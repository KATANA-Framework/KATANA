// layer: flat
// Auto-generated DTOs (Data Transfer Objects) from OpenAPI specification
//
// This file contains:
//   - Type definitions for request/response bodies
//   - Enum types with string conversion functions
//   - Compile-time metadata for validation constraints
//   - Zero-copy arena allocators for high performance
//
// All types include metadata structs with validation constraints:
//   - Required/optional flags
//   - String length constraints (min_length, max_length)
//   - Numeric constraints (minimum, maximum, exclusive bounds)
//   - Array constraints (min_items, max_items, uniqueness)
//
#pragma once

#include "katana/core/arena.hpp"
using katana::arena_allocator;
using katana::arena_string;
using katana::arena_vector;
using katana::monotonic_arena;

#include <optional>
#include <string_view>
#include <cctype>

#include <variant>

// ============================================================
// Enum Types
// ============================================================

// ============================================================
// Data Transfer Objects (DTOs)
// ============================================================

/// Data type with 6 fields
struct CustomerRevenue {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool CUSTOMER_ID_REQUIRED = true;
        static constexpr bool NAME_REQUIRED = true;
        static constexpr bool COUNTRY_REQUIRED = true;
        static constexpr bool REVENUE_REQUIRED = true;
        static constexpr bool ORDER_COUNT_REQUIRED = true;
        static constexpr bool REVENUE_RANK_REQUIRED = true;
    };


    explicit CustomerRevenue(monotonic_arena* arena = nullptr)
        : arena_(arena),
          name(arena_allocator<char>(arena)),
          country(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t customer_id;
    arena_string<> name;
    arena_string<> country;
    double revenue;
    int64_t order_count;
    int64_t revenue_rank;
};

using CustomerRevenue_Customer_id_t = int64_t;

using CustomerRevenue_Name_t = arena_string<>;

using CustomerRevenue_Country_t = arena_string<>;

using CustomerRevenue_Revenue_t = double;

using CustomerRevenue_Order_count_t = int64_t;

using CustomerRevenue_Revenue_rank_t = int64_t;

using CustomerRevenueList_Item_t = std::monostate;

/// Data type with 6 fields
struct TopProduct {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool PRODUCT_ID_REQUIRED = true;
        static constexpr bool NAME_REQUIRED = true;
        static constexpr bool CATEGORY_REQUIRED = true;
        static constexpr bool UNITS_SOLD_REQUIRED = true;
        static constexpr bool REVENUE_REQUIRED = true;
        static constexpr bool CATEGORY_RANK_REQUIRED = true;
    };


    explicit TopProduct(monotonic_arena* arena = nullptr)
        : arena_(arena),
          name(arena_allocator<char>(arena)),
          category(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t product_id;
    arena_string<> name;
    arena_string<> category;
    int64_t units_sold;
    double revenue;
    int64_t category_rank;
};

using TopProduct_Product_id_t = int64_t;

using TopProduct_Name_t = arena_string<>;

using TopProduct_Category_t = arena_string<>;

using TopProduct_Units_sold_t = int64_t;

using TopProduct_Revenue_t = double;

using TopProduct_Category_rank_t = int64_t;

using TopProductList_Item_t = std::monostate;

/// Data type with 6 fields
struct CategoryStat {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool CATEGORY_REQUIRED = true;
        static constexpr bool PRODUCT_COUNT_REQUIRED = true;
        static constexpr bool AVG_PRICE_REQUIRED = true;
        static constexpr bool TOTAL_STOCK_REQUIRED = true;
        static constexpr bool MIN_PRICE_REQUIRED = true;
        static constexpr bool MAX_PRICE_REQUIRED = true;
    };


    explicit CategoryStat(monotonic_arena* arena = nullptr)
        : arena_(arena),
          category(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> category;
    int64_t product_count;
    double avg_price;
    int64_t total_stock;
    double min_price;
    double max_price;
};

using CategoryStat_Category_t = arena_string<>;

using CategoryStat_Product_count_t = int64_t;

using CategoryStat_Avg_price_t = double;

using CategoryStat_Total_stock_t = int64_t;

using CategoryStat_Min_price_t = double;

using CategoryStat_Max_price_t = double;

using CategoryStatList_Item_t = std::monostate;

/// Data type with 7 fields
struct OrderDetail {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ORDER_ID_REQUIRED = true;
        static constexpr bool STATUS_REQUIRED = true;
        static constexpr bool CUSTOMER_NAME_REQUIRED = true;
        static constexpr bool COUNTRY_REQUIRED = true;
        static constexpr bool LINE_COUNT_REQUIRED = true;
        static constexpr bool TOTAL_QTY_REQUIRED = true;
        static constexpr bool TOTAL_AMOUNT_REQUIRED = true;
    };


    explicit OrderDetail(monotonic_arena* arena = nullptr)
        : arena_(arena),
          status(arena_allocator<char>(arena)),
          customer_name(arena_allocator<char>(arena)),
          country(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t order_id;
    arena_string<> status;
    arena_string<> customer_name;
    arena_string<> country;
    int64_t line_count;
    int64_t total_qty;
    double total_amount;
};

using OrderDetail_Order_id_t = int64_t;

using OrderDetail_Status_t = arena_string<>;

using OrderDetail_Customer_name_t = arena_string<>;

using OrderDetail_Country_t = arena_string<>;

using OrderDetail_Line_count_t = int64_t;

using OrderDetail_Total_qty_t = int64_t;

using OrderDetail_Total_amount_t = double;

/// Data type with 2 fields
struct CreateOrderRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool CUSTOMER_ID_REQUIRED = true;
        static constexpr double CUSTOMER_ID_MINIMUM = 1;
        static constexpr bool STATUS_REQUIRED = true;
        static constexpr size_t STATUS_MIN_LENGTH = 1;
        static constexpr size_t STATUS_MAX_LENGTH = 32;
    };

    static_assert(metadata::STATUS_MIN_LENGTH <= metadata::STATUS_MAX_LENGTH, "status: min_length must be <= max_length");

    explicit CreateOrderRequest(monotonic_arena* arena = nullptr)
        : arena_(arena),
          status(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t customer_id;
    arena_string<> status;
};

using CreateOrderRequest_Customer_id_t = int64_t;

using CreateOrderRequest_Status_t = arena_string<>;

/// Data type with 1 fields
struct CreateOrderResponse {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ID_REQUIRED = true;
    };


    explicit CreateOrderResponse(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    int64_t id;
};

using CreateOrderResponse_Id_t = int64_t;

using customer_revenue_param_country = arena_string<>;

using customer_revenue_param_limit = int64_t;

using customer_revenue_param_offset = int64_t;

using schema = std::monostate;

using top_products_param_limit = int64_t;

using schema_1 = std::monostate;

using schema_2 = std::monostate;

using order_detail_param_id = int64_t;

using schema_3 = std::monostate;

using schema_4 = std::monostate;

using schema_5 = std::monostate;

/// Data type with 1 fields
struct CustomerRevenueList {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ITEMS_REQUIRED = true;
    };


    explicit CustomerRevenueList(monotonic_arena* arena = nullptr)
        : arena_(arena),
          items(arena_allocator<CustomerRevenue>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<CustomerRevenue> items;
};

using CustomerRevenueList_Items_t = arena_vector<CustomerRevenue>;

/// Data type with 1 fields
struct TopProductList {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ITEMS_REQUIRED = true;
    };


    explicit TopProductList(monotonic_arena* arena = nullptr)
        : arena_(arena),
          items(arena_allocator<TopProduct>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<TopProduct> items;
};

using TopProductList_Items_t = arena_vector<TopProduct>;

/// Data type with 1 fields
struct CategoryStatList {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool ITEMS_REQUIRED = true;
    };


    explicit CategoryStatList(monotonic_arena* arena = nullptr)
        : arena_(arena),
          items(arena_allocator<CategoryStat>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<CategoryStat> items;
};

using CategoryStatList_Items_t = arena_vector<CategoryStat>;

