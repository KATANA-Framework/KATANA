// layer: flat
// Auto-generated JSON parsers and serializers from OpenAPI specification
//
// This file contains:
//   - parse_<Type>() functions: JSON string → C++ struct
//   - serialize_<Type>() functions: C++ struct → JSON string
//
// Features:
//   - Zero-copy parsing using arena allocators
//   - Streaming JSON generation without intermediate buffers
//   - Type-safe enum conversion
//   - Automatic null handling for optional fields
//
// All parse functions return std::optional<T>:
//   - std::nullopt on parse error (invalid JSON, wrong type, etc.)
//   - Parsed object on success
//
#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <optional>
#include <string>
#include <charconv>
#include <vector>

using katana::monotonic_arena;

// ============================================================
// Forward Declarations
// ============================================================

[[nodiscard]] inline std::optional<CustomerRevenue> parse_CustomerRevenue(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenueList> parse_CustomerRevenueList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct> parse_TopProduct(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProductList> parse_TopProductList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat> parse_CategoryStat(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStatList> parse_CategoryStatList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail> parse_OrderDetail(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderResponse> parse_CreateOrderResponse(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<CustomerRevenue> parse_CustomerRevenue(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CustomerRevenueList> parse_CustomerRevenueList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProduct> parse_TopProduct(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<TopProductList> parse_TopProductList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStat> parse_CategoryStat(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CategoryStatList> parse_CategoryStatList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<OrderDetail> parse_OrderDetail(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateOrderResponse> parse_CreateOrderResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_CustomerRevenue_into(const CustomerRevenue& obj, std::string& out);
inline void serialize_CustomerRevenueList_into(const CustomerRevenueList& obj, std::string& out);
inline void serialize_TopProduct_into(const TopProduct& obj, std::string& out);
inline void serialize_TopProductList_into(const TopProductList& obj, std::string& out);
inline void serialize_CategoryStat_into(const CategoryStat& obj, std::string& out);
inline void serialize_CategoryStatList_into(const CategoryStatList& obj, std::string& out);
inline void serialize_OrderDetail_into(const OrderDetail& obj, std::string& out);
inline void serialize_CreateOrderRequest_into(const CreateOrderRequest& obj, std::string& out);
inline void serialize_CreateOrderResponse_into(const CreateOrderResponse& obj, std::string& out);

inline std::string serialize_CustomerRevenue(const CustomerRevenue& obj);
inline std::string serialize_CustomerRevenueList(const CustomerRevenueList& obj);
inline std::string serialize_TopProduct(const TopProduct& obj);
inline std::string serialize_TopProductList(const TopProductList& obj);
inline std::string serialize_CategoryStat(const CategoryStat& obj);
inline std::string serialize_CategoryStatList(const CategoryStatList& obj);
inline std::string serialize_OrderDetail(const OrderDetail& obj);
inline std::string serialize_CreateOrderRequest(const CreateOrderRequest& obj);
inline std::string serialize_CreateOrderResponse(const CreateOrderResponse& obj);

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue>> parse_CustomerRevenue_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList>> parse_CustomerRevenueList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct>> parse_TopProduct_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProductList>> parse_TopProductList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat>> parse_CategoryStat_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStatList>> parse_CategoryStatList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail>> parse_OrderDetail_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest>> parse_CreateOrderRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse>> parse_CreateOrderResponse_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue>> parse_CustomerRevenue_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList>> parse_CustomerRevenueList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProduct>> parse_TopProduct_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<TopProductList>> parse_TopProductList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStat>> parse_CategoryStat_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CategoryStatList>> parse_CategoryStatList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<OrderDetail>> parse_OrderDetail_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest>> parse_CreateOrderRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse>> parse_CreateOrderResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_CustomerRevenue_array_into(const std::vector<CustomerRevenue>& arr, std::string& out);
inline void serialize_CustomerRevenue_array_into(const arena_vector<CustomerRevenue>& arr, std::string& out);
inline void serialize_CustomerRevenueList_array_into(const std::vector<CustomerRevenueList>& arr, std::string& out);
inline void serialize_CustomerRevenueList_array_into(const arena_vector<CustomerRevenueList>& arr, std::string& out);
inline void serialize_TopProduct_array_into(const std::vector<TopProduct>& arr, std::string& out);
inline void serialize_TopProduct_array_into(const arena_vector<TopProduct>& arr, std::string& out);
inline void serialize_TopProductList_array_into(const std::vector<TopProductList>& arr, std::string& out);
inline void serialize_TopProductList_array_into(const arena_vector<TopProductList>& arr, std::string& out);
inline void serialize_CategoryStat_array_into(const std::vector<CategoryStat>& arr, std::string& out);
inline void serialize_CategoryStat_array_into(const arena_vector<CategoryStat>& arr, std::string& out);
inline void serialize_CategoryStatList_array_into(const std::vector<CategoryStatList>& arr, std::string& out);
inline void serialize_CategoryStatList_array_into(const arena_vector<CategoryStatList>& arr, std::string& out);
inline void serialize_OrderDetail_array_into(const std::vector<OrderDetail>& arr, std::string& out);
inline void serialize_OrderDetail_array_into(const arena_vector<OrderDetail>& arr, std::string& out);
inline void serialize_CreateOrderRequest_array_into(const std::vector<CreateOrderRequest>& arr, std::string& out);
inline void serialize_CreateOrderRequest_array_into(const arena_vector<CreateOrderRequest>& arr, std::string& out);
inline void serialize_CreateOrderResponse_array_into(const std::vector<CreateOrderResponse>& arr, std::string& out);
inline void serialize_CreateOrderResponse_array_into(const arena_vector<CreateOrderResponse>& arr, std::string& out);

inline std::string serialize_CustomerRevenue_array(const std::vector<CustomerRevenue>& arr);
inline std::string serialize_CustomerRevenue_array(const arena_vector<CustomerRevenue>& arr);
inline std::string serialize_CustomerRevenueList_array(const std::vector<CustomerRevenueList>& arr);
inline std::string serialize_CustomerRevenueList_array(const arena_vector<CustomerRevenueList>& arr);
inline std::string serialize_TopProduct_array(const std::vector<TopProduct>& arr);
inline std::string serialize_TopProduct_array(const arena_vector<TopProduct>& arr);
inline std::string serialize_TopProductList_array(const std::vector<TopProductList>& arr);
inline std::string serialize_TopProductList_array(const arena_vector<TopProductList>& arr);
inline std::string serialize_CategoryStat_array(const std::vector<CategoryStat>& arr);
inline std::string serialize_CategoryStat_array(const arena_vector<CategoryStat>& arr);
inline std::string serialize_CategoryStatList_array(const std::vector<CategoryStatList>& arr);
inline std::string serialize_CategoryStatList_array(const arena_vector<CategoryStatList>& arr);
inline std::string serialize_OrderDetail_array(const std::vector<OrderDetail>& arr);
inline std::string serialize_OrderDetail_array(const arena_vector<OrderDetail>& arr);
inline std::string serialize_CreateOrderRequest_array(const std::vector<CreateOrderRequest>& arr);
inline std::string serialize_CreateOrderRequest_array(const arena_vector<CreateOrderRequest>& arr);
inline std::string serialize_CreateOrderResponse_array(const std::vector<CreateOrderResponse>& arr);
inline std::string serialize_CreateOrderResponse_array(const arena_vector<CreateOrderResponse>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

// parse CustomerRevenue — object, 6 field(s)  ← api.yaml:94
[[nodiscard]] inline std::optional<CustomerRevenue> parse_CustomerRevenue(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CustomerRevenue obj(arena);
    bool has_customer_id = false;
    bool has_name = false;
    bool has_country = false;
    bool has_revenue = false;
    bool has_order_count = false;
    bool has_revenue_rank = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "name") {
                if (auto v = cur.string()) {
                    obj.name = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_name = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 7:
            if (*key == "country") {
                if (auto v = cur.string()) {
                    obj.country = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_country = true;
                } else { cur.skip_value(); }
            } else if (*key == "revenue") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.revenue = *v;
                    has_revenue = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "customer_id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.customer_id = *v;
                    has_customer_id = true;
                } else { cur.skip_value(); }
            } else if (*key == "order_count") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.order_count = *v;
                    has_order_count = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 12:
            if (*key == "revenue_rank") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.revenue_rank = *v;
                    has_revenue_rank = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_customer_id) return std::nullopt;
    if (!has_name) return std::nullopt;
    if (!has_country) return std::nullopt;
    if (!has_revenue) return std::nullopt;
    if (!has_order_count) return std::nullopt;
    if (!has_revenue_rank) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CustomerRevenue> parse_CustomerRevenue(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CustomerRevenue(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CustomerRevenueList — object, 1 field(s)  ← api.yaml:104
[[nodiscard]] inline std::optional<CustomerRevenueList> parse_CustomerRevenueList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CustomerRevenueList obj(arena);
    bool has_items = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "items") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_CustomerRevenue(cur, arena)) { obj.items.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_items = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_items) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CustomerRevenueList> parse_CustomerRevenueList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CustomerRevenueList(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse TopProduct — object, 6 field(s)  ← api.yaml:109
[[nodiscard]] inline std::optional<TopProduct> parse_TopProduct(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    TopProduct obj(arena);
    bool has_product_id = false;
    bool has_name = false;
    bool has_category = false;
    bool has_units_sold = false;
    bool has_revenue = false;
    bool has_category_rank = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "name") {
                if (auto v = cur.string()) {
                    obj.name = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_name = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 7:
            if (*key == "revenue") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.revenue = *v;
                    has_revenue = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "category") {
                if (auto v = cur.string()) {
                    obj.category = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_category = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "product_id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.product_id = *v;
                    has_product_id = true;
                } else { cur.skip_value(); }
            } else if (*key == "units_sold") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.units_sold = *v;
                    has_units_sold = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 13:
            if (*key == "category_rank") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.category_rank = *v;
                    has_category_rank = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_product_id) return std::nullopt;
    if (!has_name) return std::nullopt;
    if (!has_category) return std::nullopt;
    if (!has_units_sold) return std::nullopt;
    if (!has_revenue) return std::nullopt;
    if (!has_category_rank) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<TopProduct> parse_TopProduct(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_TopProduct(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse TopProductList — object, 1 field(s)  ← api.yaml:119
[[nodiscard]] inline std::optional<TopProductList> parse_TopProductList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    TopProductList obj(arena);
    bool has_items = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "items") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_TopProduct(cur, arena)) { obj.items.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_items = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_items) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<TopProductList> parse_TopProductList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_TopProductList(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CategoryStat — object, 6 field(s)  ← api.yaml:124
[[nodiscard]] inline std::optional<CategoryStat> parse_CategoryStat(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CategoryStat obj(arena);
    bool has_category = false;
    bool has_product_count = false;
    bool has_avg_price = false;
    bool has_total_stock = false;
    bool has_min_price = false;
    bool has_max_price = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 8:
            if (*key == "category") {
                if (auto v = cur.string()) {
                    obj.category = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_category = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 9:
            if (*key == "avg_price") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.avg_price = *v;
                    has_avg_price = true;
                } else { cur.skip_value(); }
            } else if (*key == "min_price") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.min_price = *v;
                    has_min_price = true;
                } else { cur.skip_value(); }
            } else if (*key == "max_price") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.max_price = *v;
                    has_max_price = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "total_stock") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.total_stock = *v;
                    has_total_stock = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 13:
            if (*key == "product_count") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.product_count = *v;
                    has_product_count = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_category) return std::nullopt;
    if (!has_product_count) return std::nullopt;
    if (!has_avg_price) return std::nullopt;
    if (!has_total_stock) return std::nullopt;
    if (!has_min_price) return std::nullopt;
    if (!has_max_price) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CategoryStat> parse_CategoryStat(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CategoryStat(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CategoryStatList — object, 1 field(s)  ← api.yaml:134
[[nodiscard]] inline std::optional<CategoryStatList> parse_CategoryStatList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CategoryStatList obj(arena);
    bool has_items = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "items") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_CategoryStat(cur, arena)) { obj.items.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_items = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_items) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CategoryStatList> parse_CategoryStatList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CategoryStatList(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse OrderDetail — object, 7 field(s)  ← api.yaml:139
[[nodiscard]] inline std::optional<OrderDetail> parse_OrderDetail(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    OrderDetail obj(arena);
    bool has_order_id = false;
    bool has_status = false;
    bool has_customer_name = false;
    bool has_country = false;
    bool has_line_count = false;
    bool has_total_qty = false;
    bool has_total_amount = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 6:
            if (*key == "status") {
                if (auto v = cur.string()) {
                    obj.status = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_status = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 7:
            if (*key == "country") {
                if (auto v = cur.string()) {
                    obj.country = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_country = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "order_id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.order_id = *v;
                    has_order_id = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 9:
            if (*key == "total_qty") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.total_qty = *v;
                    has_total_qty = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "line_count") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.line_count = *v;
                    has_line_count = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 12:
            if (*key == "total_amount") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.total_amount = *v;
                    has_total_amount = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 13:
            if (*key == "customer_name") {
                if (auto v = cur.string()) {
                    obj.customer_name = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_customer_name = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        default:
            cur.skip_value();
            break;
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_order_id) return std::nullopt;
    if (!has_status) return std::nullopt;
    if (!has_customer_name) return std::nullopt;
    if (!has_country) return std::nullopt;
    if (!has_line_count) return std::nullopt;
    if (!has_total_qty) return std::nullopt;
    if (!has_total_amount) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<OrderDetail> parse_OrderDetail(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_OrderDetail(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CreateOrderRequest — object, 2 field(s)  ← api.yaml:150
[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CreateOrderRequest obj(arena);
    bool has_customer_id = false;
    bool has_status = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "customer_id") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.customer_id = *v;
                has_customer_id = true;
            } else { cur.skip_value(); }
        } else         if (*key == "status") {
            if (auto v = cur.string()) {
                obj.status = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_status = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_customer_id) return std::nullopt;
    if (!has_status) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CreateOrderRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CreateOrderResponse — object, 1 field(s)  ← api.yaml:156
[[nodiscard]] inline std::optional<CreateOrderResponse> parse_CreateOrderResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CreateOrderResponse obj(arena);
    bool has_id = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "id") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.id = *v;
                has_id = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_id) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateOrderResponse> parse_CreateOrderResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CreateOrderResponse(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

// serialize CustomerRevenue — object, 6 field(s)  ← api.yaml:94
inline void serialize_CustomerRevenue_into(const CustomerRevenue& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"customer_id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.customer_id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"country\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.country, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"revenue\":");
    {
        katana::serde::append_json_double(json, obj.revenue);
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"order_count\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.order_count);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"revenue_rank\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.revenue_rank);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_CustomerRevenue(const CustomerRevenue& obj) {
    std::string json;
    size_t reserve_estimate = 227;
    reserve_estimate += obj.name.size();
    reserve_estimate += obj.country.size();
    json.reserve(reserve_estimate);
    serialize_CustomerRevenue_into(obj, json);
    return json;
}

// serialize CustomerRevenueList — object, 1 field(s)  ← api.yaml:104
inline void serialize_CustomerRevenueList_into(const CustomerRevenueList& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"items\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.items.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_into(obj.items[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_CustomerRevenueList(const CustomerRevenueList& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.items.size() * 227;
    json.reserve(reserve_estimate);
    serialize_CustomerRevenueList_into(obj, json);
    return json;
}

// serialize TopProduct — object, 6 field(s)  ← api.yaml:109
inline void serialize_TopProduct_into(const TopProduct& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"product_id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.product_id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"category\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.category, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"units_sold\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.units_sold);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"revenue\":");
    {
        katana::serde::append_json_double(json, obj.revenue);
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"category_rank\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.category_rank);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_TopProduct(const TopProduct& obj) {
    std::string json;
    size_t reserve_estimate = 227;
    reserve_estimate += obj.name.size();
    reserve_estimate += obj.category.size();
    json.reserve(reserve_estimate);
    serialize_TopProduct_into(obj, json);
    return json;
}

// serialize TopProductList — object, 1 field(s)  ← api.yaml:119
inline void serialize_TopProductList_into(const TopProductList& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"items\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.items.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_into(obj.items[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_TopProductList(const TopProductList& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.items.size() * 227;
    json.reserve(reserve_estimate);
    serialize_TopProductList_into(obj, json);
    return json;
}

// serialize CategoryStat — object, 6 field(s)  ← api.yaml:124
inline void serialize_CategoryStat_into(const CategoryStat& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"category\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.category, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"product_count\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.product_count);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"avg_price\":");
    {
        katana::serde::append_json_double(json, obj.avg_price);
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"total_stock\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total_stock);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"min_price\":");
    {
        katana::serde::append_json_double(json, obj.min_price);
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"max_price\":");
    {
        katana::serde::append_json_double(json, obj.max_price);
    }
    json.push_back('}');
}

inline std::string serialize_CategoryStat(const CategoryStat& obj) {
    std::string json;
    size_t reserve_estimate = 232;
    reserve_estimate += obj.category.size();
    json.reserve(reserve_estimate);
    serialize_CategoryStat_into(obj, json);
    return json;
}

// serialize CategoryStatList — object, 1 field(s)  ← api.yaml:134
inline void serialize_CategoryStatList_into(const CategoryStatList& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"items\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.items.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_into(obj.items[i], json);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_CategoryStatList(const CategoryStatList& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.items.size() * 232;
    json.reserve(reserve_estimate);
    serialize_CategoryStatList_into(obj, json);
    return json;
}

// serialize OrderDetail — object, 7 field(s)  ← api.yaml:139
inline void serialize_OrderDetail_into(const OrderDetail& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"order_id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.order_id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"status\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.status, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"customer_name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.customer_name, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"country\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.country, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"line_count\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.line_count);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"total_qty\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total_qty);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"total_amount\":");
    {
        katana::serde::append_json_double(json, obj.total_amount);
    }
    json.push_back('}');
}

inline std::string serialize_OrderDetail(const OrderDetail& obj) {
    std::string json;
    size_t reserve_estimate = 276;
    reserve_estimate += obj.status.size();
    reserve_estimate += obj.customer_name.size();
    reserve_estimate += obj.country.size();
    json.reserve(reserve_estimate);
    serialize_OrderDetail_into(obj, json);
    return json;
}

// serialize CreateOrderRequest — object, 2 field(s)  ← api.yaml:150
inline void serialize_CreateOrderRequest_into(const CreateOrderRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"customer_id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.customer_id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"status\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.status, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_CreateOrderRequest(const CreateOrderRequest& obj) {
    std::string json;
    size_t reserve_estimate = 79;
    reserve_estimate += obj.status.size();
    json.reserve(reserve_estimate);
    serialize_CreateOrderRequest_into(obj, json);
    return json;
}

// serialize CreateOrderResponse — object, 1 field(s)  ← api.yaml:156
inline void serialize_CreateOrderResponse_into(const CreateOrderResponse& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"id\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.id);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_CreateOrderResponse(const CreateOrderResponse& obj) {
    std::string json;
    size_t reserve_estimate = 28;
    json.reserve(reserve_estimate);
    serialize_CreateOrderResponse_into(obj, json);
    return json;
}

// ============================================================
// Array Parse Functions
// ============================================================

// parse_array CustomerRevenue — object, 6 field(s)  ← api.yaml:94
[[nodiscard]] inline std::optional<std::vector<CustomerRevenue>> parse_CustomerRevenue_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenue> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenue(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenue>> parse_CustomerRevenue_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenue_array(cur, arena);
}

// parse_array CustomerRevenueList — object, 1 field(s)  ← api.yaml:104
[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList>> parse_CustomerRevenueList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CustomerRevenueList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CustomerRevenueList(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CustomerRevenueList>> parse_CustomerRevenueList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CustomerRevenueList_array(cur, arena);
}

// parse_array TopProduct — object, 6 field(s)  ← api.yaml:109
[[nodiscard]] inline std::optional<std::vector<TopProduct>> parse_TopProduct_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProduct> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProduct(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProduct>> parse_TopProduct_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProduct_array(cur, arena);
}

// parse_array TopProductList — object, 1 field(s)  ← api.yaml:119
[[nodiscard]] inline std::optional<std::vector<TopProductList>> parse_TopProductList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<TopProductList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_TopProductList(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<TopProductList>> parse_TopProductList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_TopProductList_array(cur, arena);
}

// parse_array CategoryStat — object, 6 field(s)  ← api.yaml:124
[[nodiscard]] inline std::optional<std::vector<CategoryStat>> parse_CategoryStat_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStat> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStat(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStat>> parse_CategoryStat_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStat_array(cur, arena);
}

// parse_array CategoryStatList — object, 1 field(s)  ← api.yaml:134
[[nodiscard]] inline std::optional<std::vector<CategoryStatList>> parse_CategoryStatList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CategoryStatList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CategoryStatList(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CategoryStatList>> parse_CategoryStatList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CategoryStatList_array(cur, arena);
}

// parse_array OrderDetail — object, 7 field(s)  ← api.yaml:139
[[nodiscard]] inline std::optional<std::vector<OrderDetail>> parse_OrderDetail_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<OrderDetail> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_OrderDetail(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<OrderDetail>> parse_OrderDetail_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_OrderDetail_array(cur, arena);
}

// parse_array CreateOrderRequest — object, 2 field(s)  ← api.yaml:150
[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest>> parse_CreateOrderRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateOrderRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateOrderRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderRequest>> parse_CreateOrderRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderRequest_array(cur, arena);
}

// parse_array CreateOrderResponse — object, 1 field(s)  ← api.yaml:156
[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse>> parse_CreateOrderResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateOrderResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateOrderResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateOrderResponse>> parse_CreateOrderResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateOrderResponse_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

// serialize_array CustomerRevenue — object, 6 field(s)  ← api.yaml:94
inline void serialize_CustomerRevenue_array_into(const std::vector<CustomerRevenue>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_array(const std::vector<CustomerRevenue>& arr) {
    std::string json;
    json.reserve(arr.size() * 227 + 2);
    serialize_CustomerRevenue_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenue_array_into(const arena_vector<CustomerRevenue>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenue_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenue_array(const arena_vector<CustomerRevenue>& arr) {
    std::string json;
    json.reserve(arr.size() * 227 + 2);
    serialize_CustomerRevenue_array_into(arr, json);
    return json;
}

// serialize_array CustomerRevenueList — object, 1 field(s)  ← api.yaml:104
inline void serialize_CustomerRevenueList_array_into(const std::vector<CustomerRevenueList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenueList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_array(const std::vector<CustomerRevenueList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_CustomerRevenueList_array_into(arr, json);
    return json;
}

inline void serialize_CustomerRevenueList_array_into(const arena_vector<CustomerRevenueList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CustomerRevenueList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CustomerRevenueList_array(const arena_vector<CustomerRevenueList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_CustomerRevenueList_array_into(arr, json);
    return json;
}

// serialize_array TopProduct — object, 6 field(s)  ← api.yaml:109
inline void serialize_TopProduct_array_into(const std::vector<TopProduct>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_array(const std::vector<TopProduct>& arr) {
    std::string json;
    json.reserve(arr.size() * 227 + 2);
    serialize_TopProduct_array_into(arr, json);
    return json;
}

inline void serialize_TopProduct_array_into(const arena_vector<TopProduct>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProduct_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProduct_array(const arena_vector<TopProduct>& arr) {
    std::string json;
    json.reserve(arr.size() * 227 + 2);
    serialize_TopProduct_array_into(arr, json);
    return json;
}

// serialize_array TopProductList — object, 1 field(s)  ← api.yaml:119
inline void serialize_TopProductList_array_into(const std::vector<TopProductList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProductList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_array(const std::vector<TopProductList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_TopProductList_array_into(arr, json);
    return json;
}

inline void serialize_TopProductList_array_into(const arena_vector<TopProductList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_TopProductList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_TopProductList_array(const arena_vector<TopProductList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_TopProductList_array_into(arr, json);
    return json;
}

// serialize_array CategoryStat — object, 6 field(s)  ← api.yaml:124
inline void serialize_CategoryStat_array_into(const std::vector<CategoryStat>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_array(const std::vector<CategoryStat>& arr) {
    std::string json;
    json.reserve(arr.size() * 232 + 2);
    serialize_CategoryStat_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStat_array_into(const arena_vector<CategoryStat>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStat_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStat_array(const arena_vector<CategoryStat>& arr) {
    std::string json;
    json.reserve(arr.size() * 232 + 2);
    serialize_CategoryStat_array_into(arr, json);
    return json;
}

// serialize_array CategoryStatList — object, 1 field(s)  ← api.yaml:134
inline void serialize_CategoryStatList_array_into(const std::vector<CategoryStatList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStatList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_array(const std::vector<CategoryStatList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_CategoryStatList_array_into(arr, json);
    return json;
}

inline void serialize_CategoryStatList_array_into(const arena_vector<CategoryStatList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CategoryStatList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CategoryStatList_array(const arena_vector<CategoryStatList>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_CategoryStatList_array_into(arr, json);
    return json;
}

// serialize_array OrderDetail — object, 7 field(s)  ← api.yaml:139
inline void serialize_OrderDetail_array_into(const std::vector<OrderDetail>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_array(const std::vector<OrderDetail>& arr) {
    std::string json;
    json.reserve(arr.size() * 276 + 2);
    serialize_OrderDetail_array_into(arr, json);
    return json;
}

inline void serialize_OrderDetail_array_into(const arena_vector<OrderDetail>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_OrderDetail_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_OrderDetail_array(const arena_vector<OrderDetail>& arr) {
    std::string json;
    json.reserve(arr.size() * 276 + 2);
    serialize_OrderDetail_array_into(arr, json);
    return json;
}

// serialize_array CreateOrderRequest — object, 2 field(s)  ← api.yaml:150
inline void serialize_CreateOrderRequest_array_into(const std::vector<CreateOrderRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderRequest_array(const std::vector<CreateOrderRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 79 + 2);
    serialize_CreateOrderRequest_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderRequest_array_into(const arena_vector<CreateOrderRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderRequest_array(const arena_vector<CreateOrderRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 79 + 2);
    serialize_CreateOrderRequest_array_into(arr, json);
    return json;
}

// serialize_array CreateOrderResponse — object, 1 field(s)  ← api.yaml:156
inline void serialize_CreateOrderResponse_array_into(const std::vector<CreateOrderResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderResponse_array(const std::vector<CreateOrderResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 28 + 2);
    serialize_CreateOrderResponse_array_into(arr, json);
    return json;
}

inline void serialize_CreateOrderResponse_array_into(const arena_vector<CreateOrderResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateOrderResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateOrderResponse_array(const arena_vector<CreateOrderResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 28 + 2);
    serialize_CreateOrderResponse_array_into(arr, json);
    return json;
}

