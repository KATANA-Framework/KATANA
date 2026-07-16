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

[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserRole> parse_UserRole(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserRole> parse_UserRole(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_SumResponse_into(const SumResponse& obj, std::string& out);
inline void serialize_StatsResponse_into(const StatsResponse& obj, std::string& out);
inline void serialize_UserRole_into(const UserRole& obj, std::string& out);
inline void serialize_UserResponse_into(const UserResponse& obj, std::string& out);
inline void serialize_ItemCategory_into(const ItemCategory& obj, std::string& out);
inline void serialize_Item_into(const Item& obj, std::string& out);
inline void serialize_ItemMetadata_into(const ItemMetadata& obj, std::string& out);
inline void serialize_ItemList_into(const ItemList& obj, std::string& out);
inline void serialize_JsonMessage_into(const JsonMessage& obj, std::string& out);
inline void serialize_EchoResponse_into(const EchoResponse& obj, std::string& out);
inline void serialize_health_check_response_into(const health_check_response& obj, std::string& out);

inline std::string serialize_SumResponse(const SumResponse& obj);
inline std::string serialize_StatsResponse(const StatsResponse& obj);
inline std::string serialize_UserRole(const UserRole& obj);
inline std::string serialize_UserResponse(const UserResponse& obj);
inline std::string serialize_ItemCategory(const ItemCategory& obj);
inline std::string serialize_Item(const Item& obj);
inline std::string serialize_ItemMetadata(const ItemMetadata& obj);
inline std::string serialize_ItemList(const ItemList& obj);
inline std::string serialize_JsonMessage(const JsonMessage& obj);
inline std::string serialize_EchoResponse(const EchoResponse& obj);
inline std::string serialize_health_check_response(const health_check_response& obj);

// ============================================================
// JSON Parse Functions
// ============================================================

// parse SumRequest — object, 1 field(s)  ← api.yaml:234
[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    SumRequest obj(arena);
    bool has_values = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "values") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto v = katana::serde::parse_double(cur)) {
                        obj.values.push_back(*v);
                    } else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_values = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_values) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_SumRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse StatsRequest — object, 2 field(s)  ← api.yaml:256
[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    StatsRequest obj(arena);
    bool has_values = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "values") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto v = katana::serde::parse_double(cur)) {
                        obj.values.push_back(*v);
                    } else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_values = true;
            } else { cur.skip_value(); }
        } else         if (*key == "include_median") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.include_median = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_values) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_StatsRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse RegisterRequest — object, 6 field(s)  ← api.yaml:292
[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    RegisterRequest obj(arena);
    bool has_username = false;
    bool has_email = false;
    bool has_password = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 3:
            if (*key == "age") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.age = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "role") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = UserRole_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.role = *enum_val;
                    }
                } else { cur.skip_value(); }
            } else if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).push_back(katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena)));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "email") {
                if (auto v = cur.string()) {
                    obj.email = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_email = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "username") {
                if (auto v = cur.string()) {
                    obj.username = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_username = true;
                } else { cur.skip_value(); }
            } else if (*key == "password") {
                if (auto v = cur.string()) {
                    obj.password = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_password = true;
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
    if (!has_username) return std::nullopt;
    if (!has_email) return std::nullopt;
    if (!has_password) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_RegisterRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse UserRole — enum  ← api.yaml:321
[[nodiscard]] inline std::optional<UserRole> parse_UserRole(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return UserRole_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<UserRole> parse_UserRole(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_UserRole(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse ItemCategory — enum  ← api.yaml:343
[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return ItemCategory_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_ItemCategory(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse ItemMetadata — object, 4 field(s)  ← api.yaml:373
[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    ItemMetadata obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 3:
            if (*key == "sku") {
                if (auto v = cur.string()) {
                    obj.sku = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 9:
            if (*key == "weight_kg") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.weight_kg = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "dimensions") {
                if (auto v = cur.string()) {
                    obj.dimensions = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 12:
            if (*key == "manufacturer") {
                if (auto v = cur.string()) {
                    obj.manufacturer = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
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
    return obj;
}

[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_ItemMetadata(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CreateItemRequest — object, 7 field(s)  ← api.yaml:386
[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CreateItemRequest obj(arena);
    bool has_name = false;
    bool has_price = false;
    bool has_category = false;

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
            } else if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).push_back(katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena)));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "price") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.price = *v;
                    has_price = true;
                } else { cur.skip_value(); }
            } else if (*key == "stock") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.stock = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "category") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = ItemCategory_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.category = *enum_val;
                    has_category = true;
                    }
                } else { cur.skip_value(); }
            } else if (*key == "metadata") {
                if (auto nested = parse_ItemMetadata(cur, arena)) {
                    obj.metadata = std::move(*nested);
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
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
    if (!has_name) return std::nullopt;
    if (!has_price) return std::nullopt;
    if (!has_category) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CreateItemRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse UpdateItemRequest — object, 6 field(s)  ← api.yaml:418
[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    UpdateItemRequest obj(arena);

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
                } else { cur.skip_value(); }
            } else if (*key == "tags") {
                if (cur.try_array_start()) {
                    obj.tags.emplace(arena);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto v = cur.string()) {
                            (*obj.tags).push_back(katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena)));
                        } else { cur.skip_value(); }
                        cur.try_comma();
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "price") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.price = *v;
                } else { cur.skip_value(); }
            } else if (*key == "stock") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.stock = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "category") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = ItemCategory_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.category = *enum_val;
                    }
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 11:
            if (*key == "description") {
                if (auto v = cur.string()) {
                    obj.description = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
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
    return obj;
}

[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_UpdateItemRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse EchoRequest — object, 3 field(s)  ← api.yaml:469
[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    EchoRequest obj(arena);
    bool has_message = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "message") {
            if (auto v = cur.string()) {
                obj.message = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_message = true;
            } else { cur.skip_value(); }
        } else         if (*key == "repeat") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.repeat = *v;
            } else { cur.skip_value(); }
        } else         if (*key == "uppercase") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.uppercase = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_message) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_EchoRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

// serialize SumResponse — object, 2 field(s)  ← api.yaml:246
inline void serialize_SumResponse_into(const SumResponse& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"result\":");
    {
        katana::serde::append_json_double(json, obj.result);
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"count\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.count);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
}

inline std::string serialize_SumResponse(const SumResponse& obj) {
    std::string json;
    size_t reserve_estimate = 66;
    json.reserve(reserve_estimate);
    serialize_SumResponse_into(obj, json);
    return json;
}

// serialize StatsResponse — object, 6 field(s)  ← api.yaml:270
inline void serialize_StatsResponse_into(const StatsResponse& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.min) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"min\":");
    {
        if (!obj.min) {
            json.append("null");
        } else {
        katana::serde::append_json_double(json, *obj.min);
        }
    }
    }
    if (obj.max) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"max\":");
    {
        if (!obj.max) {
            json.append("null");
        } else {
        katana::serde::append_json_double(json, *obj.max);
        }
    }
    }
    if (obj.mean) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"mean\":");
    {
        if (!obj.mean) {
            json.append("null");
        } else {
        katana::serde::append_json_double(json, *obj.mean);
        }
    }
    }
    if (obj.sum) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"sum\":");
    {
        if (!obj.sum) {
            json.append("null");
        } else {
        katana::serde::append_json_double(json, *obj.sum);
        }
    }
    }
    if (obj.count) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"count\":");
    {
        if (!obj.count) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.count);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.median) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"median\":");
    {
        if (!obj.median) {
            json.append("null");
        } else {
        katana::serde::append_json_double(json, *obj.median);
        }
    }
    }
    json.push_back('}');
}

inline std::string serialize_StatsResponse(const StatsResponse& obj) {
    std::string json;
    size_t reserve_estimate = 195;
    json.reserve(reserve_estimate);
    serialize_StatsResponse_into(obj, json);
    return json;
}

// serialize UserRole — enum  ← api.yaml:321
inline void serialize_UserRole_into(const UserRole& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_UserRole(const UserRole& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_UserRole_into(obj, json);
    return json;
}

// serialize UserResponse — object, 5 field(s)  ← api.yaml:325
inline void serialize_UserResponse_into(const UserResponse& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"id\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.id, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"username\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.username, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"email\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.email, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"role\":");
    json.push_back('"');
    json.append(to_string(obj.role));
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"created_at\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.created_at, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_UserResponse(const UserResponse& obj) {
    std::string json;
    size_t reserve_estimate = 211;
    reserve_estimate += obj.id.size();
    reserve_estimate += obj.username.size();
    reserve_estimate += obj.email.size();
    reserve_estimate += obj.created_at.size();
    json.reserve(reserve_estimate);
    serialize_UserResponse_into(obj, json);
    return json;
}

// serialize ItemCategory — enum  ← api.yaml:343
inline void serialize_ItemCategory_into(const ItemCategory& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_ItemCategory(const ItemCategory& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_ItemCategory_into(obj, json);
    return json;
}

// serialize Item — object, 8 field(s)  ← api.yaml:347
inline void serialize_Item_into(const Item& obj, std::string& json) {
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
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"name\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.name, json);
    json.push_back('"');
    if (obj.description) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"description\":");
    if (obj.description) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.description, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"price\":");
    {
        katana::serde::append_json_double(json, obj.price);
    }
    if (obj.stock) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"stock\":");
    {
        if (!obj.stock) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.stock);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"category\":");
    json.push_back('"');
    json.append(to_string(obj.category));
    json.push_back('"');
    if (obj.tags) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"tags\":");
    if (!obj.tags) {
        json.append("null");
    } else {
    json.push_back('[');
    for (size_t i = 0; i < obj.tags->size(); ++i) {
        if (i > 0) json.push_back(',');
        json.push_back('"');
        katana::serde::escape_json_string_into((*obj.tags)[i], json);
        json.push_back('"');
    }
    json.push_back(']');
    }
    }
    if (obj.metadata) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"metadata\":");
    if (obj.metadata) {
        serialize_ItemMetadata_into(*obj.metadata, json);
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_Item(const Item& obj) {
    std::string json;
    size_t reserve_estimate = 434;
    reserve_estimate += obj.name.size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    json.reserve(reserve_estimate);
    serialize_Item_into(obj, json);
    return json;
}

// serialize ItemMetadata — object, 4 field(s)  ← api.yaml:373
inline void serialize_ItemMetadata_into(const ItemMetadata& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.weight_kg) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"weight_kg\":");
    {
        if (!obj.weight_kg) {
            json.append("null");
        } else {
        katana::serde::append_json_double(json, *obj.weight_kg);
        }
    }
    }
    if (obj.dimensions) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"dimensions\":");
    if (obj.dimensions) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.dimensions, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.manufacturer) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"manufacturer\":");
    if (obj.manufacturer) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.manufacturer, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.sku) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"sku\":");
    if (obj.sku) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.sku, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_ItemMetadata(const ItemMetadata& obj) {
    std::string json;
    size_t reserve_estimate = 173;
    if (obj.dimensions) reserve_estimate += obj.dimensions->size();
    if (obj.manufacturer) reserve_estimate += obj.manufacturer->size();
    if (obj.sku) reserve_estimate += obj.sku->size();
    json.reserve(reserve_estimate);
    serialize_ItemMetadata_into(obj, json);
    return json;
}

// serialize ItemList — object, 4 field(s)  ← api.yaml:444
inline void serialize_ItemList_into(const ItemList& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"items\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.items.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_into(obj.items[i], json);
    }
    json.push_back(']');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"total\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.total);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (obj.limit) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"limit\":");
    {
        if (!obj.limit) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.limit);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.offset) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"offset\":");
    {
        if (!obj.offset) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.offset);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    json.push_back('}');
}

inline std::string serialize_ItemList(const ItemList& obj) {
    std::string json;
    size_t reserve_estimate = 163;
    reserve_estimate += obj.items.size() * 434;
    json.reserve(reserve_estimate);
    serialize_ItemList_into(obj, json);
    return json;
}

// serialize JsonMessage — object, 1 field(s)  ← api.yaml:461
inline void serialize_JsonMessage_into(const JsonMessage& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"message\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.message, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_JsonMessage(const JsonMessage& obj) {
    std::string json;
    size_t reserve_estimate = 45;
    reserve_estimate += obj.message.size();
    json.reserve(reserve_estimate);
    serialize_JsonMessage_into(obj, json);
    return json;
}

// serialize EchoResponse — object, 2 field(s)  ← api.yaml:483
inline void serialize_EchoResponse_into(const EchoResponse& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"message\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.message, json);
    json.push_back('"');
    if (obj.length) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"length\":");
    {
        if (!obj.length) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.length);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    json.push_back('}');
}

inline std::string serialize_EchoResponse(const EchoResponse& obj) {
    std::string json;
    size_t reserve_estimate = 75;
    reserve_estimate += obj.message.size();
    json.reserve(reserve_estimate);
    serialize_EchoResponse_into(obj, json);
    return json;
}

// serialize health_check_response — object, 2 field(s)  ← api.yaml:222
inline void serialize_health_check_response_into(const health_check_response& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.status) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"status\":");
    if (obj.status) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.status, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (obj.uptime_ms) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"uptime_ms\":");
    {
        if (!obj.uptime_ms) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.uptime_ms);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    json.push_back('}');
}

inline std::string serialize_health_check_response(const health_check_response& obj) {
    std::string json;
    size_t reserve_estimate = 77;
    if (obj.status) reserve_estimate += obj.status->size();
    json.reserve(reserve_estimate);
    serialize_health_check_response_into(obj, json);
    return json;
}

