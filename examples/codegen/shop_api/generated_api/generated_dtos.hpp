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

// CustomerRevenue — object, 6 field(s)  ← api.yaml:94
/// Data type with 6 fields
struct CustomerRevenue {
    // Compile-time metadata for validation
    struct field_constraints {
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

// CustomerRevenue_Customer_id_t — integer, field CustomerRevenue.customer_id  ← api.yaml:98
using CustomerRevenue_Customer_id_t = int64_t;

// CustomerRevenue_Name_t — string, field CustomerRevenue.name  ← api.yaml:99
using CustomerRevenue_Name_t = arena_string<>;

// CustomerRevenue_Country_t — string, field CustomerRevenue.country  ← api.yaml:100
using CustomerRevenue_Country_t = arena_string<>;

// CustomerRevenue_Revenue_t — number, field CustomerRevenue.revenue  ← api.yaml:101
using CustomerRevenue_Revenue_t = double;

// CustomerRevenue_Order_count_t — integer, field CustomerRevenue.order_count  ← api.yaml:102
using CustomerRevenue_Order_count_t = int64_t;

// CustomerRevenue_Revenue_rank_t — integer, field CustomerRevenue.revenue_rank  ← api.yaml:103
using CustomerRevenue_Revenue_rank_t = int64_t;

// TopProduct — object, 6 field(s)  ← api.yaml:109
/// Data type with 6 fields
struct TopProduct {
    // Compile-time metadata for validation
    struct field_constraints {
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

// TopProduct_Product_id_t — integer, field TopProduct.product_id  ← api.yaml:113
using TopProduct_Product_id_t = int64_t;

// TopProduct_Name_t — string, field TopProduct.name  ← api.yaml:114
using TopProduct_Name_t = arena_string<>;

// TopProduct_Category_t — string, field TopProduct.category  ← api.yaml:115
using TopProduct_Category_t = arena_string<>;

// TopProduct_Units_sold_t — integer, field TopProduct.units_sold  ← api.yaml:116
using TopProduct_Units_sold_t = int64_t;

// TopProduct_Revenue_t — number, field TopProduct.revenue  ← api.yaml:117
using TopProduct_Revenue_t = double;

// TopProduct_Category_rank_t — integer, field TopProduct.category_rank  ← api.yaml:118
using TopProduct_Category_rank_t = int64_t;

// CategoryStat — object, 6 field(s)  ← api.yaml:124
/// Data type with 6 fields
struct CategoryStat {
    // Compile-time metadata for validation
    struct field_constraints {
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

// CategoryStat_Category_t — string, field CategoryStat.category  ← api.yaml:128
using CategoryStat_Category_t = arena_string<>;

// CategoryStat_Product_count_t — integer, field CategoryStat.product_count  ← api.yaml:129
using CategoryStat_Product_count_t = int64_t;

// CategoryStat_Avg_price_t — number, field CategoryStat.avg_price  ← api.yaml:130
using CategoryStat_Avg_price_t = double;

// CategoryStat_Total_stock_t — integer, field CategoryStat.total_stock  ← api.yaml:131
using CategoryStat_Total_stock_t = int64_t;

// CategoryStat_Min_price_t — number, field CategoryStat.min_price  ← api.yaml:132
using CategoryStat_Min_price_t = double;

// CategoryStat_Max_price_t — number, field CategoryStat.max_price  ← api.yaml:133
using CategoryStat_Max_price_t = double;

// OrderDetail — object, 7 field(s)  ← api.yaml:139
/// Data type with 7 fields
struct OrderDetail {
    // Compile-time metadata for validation
    struct field_constraints {
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

// OrderDetail_Order_id_t — integer, field OrderDetail.order_id  ← api.yaml:143
using OrderDetail_Order_id_t = int64_t;

// OrderDetail_Status_t — string, field OrderDetail.status  ← api.yaml:144
using OrderDetail_Status_t = arena_string<>;

// OrderDetail_Customer_name_t — string, field OrderDetail.customer_name  ← api.yaml:145
using OrderDetail_Customer_name_t = arena_string<>;

// OrderDetail_Country_t — string, field OrderDetail.country  ← api.yaml:146
using OrderDetail_Country_t = arena_string<>;

// OrderDetail_Line_count_t — integer, field OrderDetail.line_count  ← api.yaml:147
using OrderDetail_Line_count_t = int64_t;

// OrderDetail_Total_qty_t — integer, field OrderDetail.total_qty  ← api.yaml:148
using OrderDetail_Total_qty_t = int64_t;

// OrderDetail_Total_amount_t — number, field OrderDetail.total_amount  ← api.yaml:149
using OrderDetail_Total_amount_t = double;

// CreateOrderRequest — object, 2 field(s)  ← api.yaml:150
/// Data type with 2 fields
struct CreateOrderRequest {
    // Compile-time metadata for validation
    struct field_constraints {
        static constexpr bool CUSTOMER_ID_REQUIRED = true;
        static constexpr double CUSTOMER_ID_MINIMUM = 1;
        static constexpr bool STATUS_REQUIRED = true;
        static constexpr size_t STATUS_MIN_LENGTH = 1;
        static constexpr size_t STATUS_MAX_LENGTH = 32;
    };

    static_assert(field_constraints::STATUS_MIN_LENGTH <= field_constraints::STATUS_MAX_LENGTH, "status: min_length must be <= max_length");

    explicit CreateOrderRequest(monotonic_arena* arena = nullptr)
        : arena_(arena),
          status(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t customer_id;
    arena_string<> status;
};

// CreateOrderRequest_Customer_id_t — integer, field CreateOrderRequest.customer_id  ← api.yaml:154
using CreateOrderRequest_Customer_id_t = int64_t;

// CreateOrderRequest_Status_t — string, field CreateOrderRequest.status  ← api.yaml:155
using CreateOrderRequest_Status_t = arena_string<>;

// CreateOrderResponse — object, 1 field(s)  ← api.yaml:156
/// Data type with 1 fields
struct CreateOrderResponse {
    // Compile-time metadata for validation
    struct field_constraints {
        static constexpr bool ID_REQUIRED = true;
    };


    explicit CreateOrderResponse(monotonic_arena* arena = nullptr)
        : arena_(arena) {}

    monotonic_arena* arena_;
    int64_t id;
};

// CreateOrderResponse_Id_t — integer, field CreateOrderResponse.id  ← api.yaml:160
using CreateOrderResponse_Id_t = int64_t;

// customer_revenue_param_country — string  ← api.yaml:17
using customer_revenue_param_country = arena_string<>;

// customer_revenue_param_limit — integer  ← api.yaml:21
using customer_revenue_param_limit = int64_t;

// customer_revenue_param_offset — integer  ← api.yaml:25
using customer_revenue_param_offset = int64_t;

// top_products_param_limit — integer  ← api.yaml:41
using top_products_param_limit = int64_t;

// order_detail_param_id — integer  ← api.yaml:68
using order_detail_param_id = int64_t;

// CustomerRevenueList — object, 1 field(s)  ← api.yaml:104
/// Data type with 1 fields
struct CustomerRevenueList {
    // Compile-time metadata for validation
    struct field_constraints {
        static constexpr bool ITEMS_REQUIRED = true;
    };


    explicit CustomerRevenueList(monotonic_arena* arena = nullptr)
        : arena_(arena),
          items(arena_allocator<CustomerRevenue>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<CustomerRevenue> items;
};

// CustomerRevenueList_Items_t — array, field CustomerRevenueList.items  ← api.yaml:108
using CustomerRevenueList_Items_t = arena_vector<CustomerRevenue>;

// TopProductList — object, 1 field(s)  ← api.yaml:119
/// Data type with 1 fields
struct TopProductList {
    // Compile-time metadata for validation
    struct field_constraints {
        static constexpr bool ITEMS_REQUIRED = true;
    };


    explicit TopProductList(monotonic_arena* arena = nullptr)
        : arena_(arena),
          items(arena_allocator<TopProduct>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<TopProduct> items;
};

// TopProductList_Items_t — array, field TopProductList.items  ← api.yaml:123
using TopProductList_Items_t = arena_vector<TopProduct>;

// CategoryStatList — object, 1 field(s)  ← api.yaml:134
/// Data type with 1 fields
struct CategoryStatList {
    // Compile-time metadata for validation
    struct field_constraints {
        static constexpr bool ITEMS_REQUIRED = true;
    };


    explicit CategoryStatList(monotonic_arena* arena = nullptr)
        : arena_(arena),
          items(arena_allocator<CategoryStat>(arena)) {}

    monotonic_arena* arena_;
    arena_vector<CategoryStat> items;
};

// CategoryStatList_Items_t — array, field CategoryStatList.items  ← api.yaml:138
using CategoryStatList_Items_t = arena_vector<CategoryStat>;

