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

[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<CreateOrderRequest> parse_CreateOrderRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_CustomerRevenue_into(const CustomerRevenue& obj, std::string& out);
inline void serialize_CustomerRevenueList_into(const CustomerRevenueList& obj, std::string& out);
inline void serialize_TopProduct_into(const TopProduct& obj, std::string& out);
inline void serialize_TopProductList_into(const TopProductList& obj, std::string& out);
inline void serialize_CategoryStat_into(const CategoryStat& obj, std::string& out);
inline void serialize_CategoryStatList_into(const CategoryStatList& obj, std::string& out);
inline void serialize_OrderDetail_into(const OrderDetail& obj, std::string& out);
inline void serialize_CreateOrderResponse_into(const CreateOrderResponse& obj, std::string& out);

inline std::string serialize_CustomerRevenue(const CustomerRevenue& obj);
inline std::string serialize_CustomerRevenueList(const CustomerRevenueList& obj);
inline std::string serialize_TopProduct(const TopProduct& obj);
inline std::string serialize_TopProductList(const TopProductList& obj);
inline std::string serialize_CategoryStat(const CategoryStat& obj);
inline std::string serialize_CategoryStatList(const CategoryStatList& obj);
inline std::string serialize_OrderDetail(const OrderDetail& obj);
inline std::string serialize_CreateOrderResponse(const CreateOrderResponse& obj);

// ============================================================
// JSON Parse Functions
// ============================================================

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

