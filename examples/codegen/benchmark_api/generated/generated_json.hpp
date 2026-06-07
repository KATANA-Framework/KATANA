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
[[nodiscard]] inline std::optional<SumResponse> parse_SumResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse> parse_StatsResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserRole> parse_UserRole(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse> parse_UserResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item> parse_Item(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList> parse_ItemList(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoResponse> parse_EchoResponse(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<health_check_response> parse_health_check_response(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<SumRequest> parse_SumRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<SumResponse> parse_SumResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsRequest> parse_StatsRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<StatsResponse> parse_StatsResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<RegisterRequest> parse_RegisterRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserRole> parse_UserRole(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UserResponse> parse_UserResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemCategory> parse_ItemCategory(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Item> parse_Item(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemMetadata> parse_ItemMetadata(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateItemRequest> parse_CreateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<UpdateItemRequest> parse_UpdateItemRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<ItemList> parse_ItemList(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoRequest> parse_EchoRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<EchoResponse> parse_EchoResponse(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<health_check_response> parse_health_check_response(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_SumRequest_into(const SumRequest& obj, std::string& out);
inline void serialize_SumResponse_into(const SumResponse& obj, std::string& out);
inline void serialize_StatsRequest_into(const StatsRequest& obj, std::string& out);
inline void serialize_StatsResponse_into(const StatsResponse& obj, std::string& out);
inline void serialize_RegisterRequest_into(const RegisterRequest& obj, std::string& out);
inline void serialize_UserRole_into(const UserRole& obj, std::string& out);
inline void serialize_UserResponse_into(const UserResponse& obj, std::string& out);
inline void serialize_ItemCategory_into(const ItemCategory& obj, std::string& out);
inline void serialize_Item_into(const Item& obj, std::string& out);
inline void serialize_ItemMetadata_into(const ItemMetadata& obj, std::string& out);
inline void serialize_CreateItemRequest_into(const CreateItemRequest& obj, std::string& out);
inline void serialize_UpdateItemRequest_into(const UpdateItemRequest& obj, std::string& out);
inline void serialize_ItemList_into(const ItemList& obj, std::string& out);
inline void serialize_EchoRequest_into(const EchoRequest& obj, std::string& out);
inline void serialize_EchoResponse_into(const EchoResponse& obj, std::string& out);
inline void serialize_health_check_response_into(const health_check_response& obj, std::string& out);

inline std::string serialize_SumRequest(const SumRequest& obj);
inline std::string serialize_SumResponse(const SumResponse& obj);
inline std::string serialize_StatsRequest(const StatsRequest& obj);
inline std::string serialize_StatsResponse(const StatsResponse& obj);
inline std::string serialize_RegisterRequest(const RegisterRequest& obj);
inline std::string serialize_UserRole(const UserRole& obj);
inline std::string serialize_UserResponse(const UserResponse& obj);
inline std::string serialize_ItemCategory(const ItemCategory& obj);
inline std::string serialize_Item(const Item& obj);
inline std::string serialize_ItemMetadata(const ItemMetadata& obj);
inline std::string serialize_CreateItemRequest(const CreateItemRequest& obj);
inline std::string serialize_UpdateItemRequest(const UpdateItemRequest& obj);
inline std::string serialize_ItemList(const ItemList& obj);
inline std::string serialize_EchoRequest(const EchoRequest& obj);
inline std::string serialize_EchoResponse(const EchoResponse& obj);
inline std::string serialize_health_check_response(const health_check_response& obj);

[[nodiscard]] inline std::optional<std::vector<SumRequest>> parse_SumRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumResponse>> parse_SumResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest>> parse_StatsRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse>> parse_StatsResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest>> parse_RegisterRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserRole>> parse_UserRole_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse>> parse_UserResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemCategory>> parse_ItemCategory_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item>> parse_Item_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata>> parse_ItemMetadata_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest>> parse_CreateItemRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest>> parse_UpdateItemRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList>> parse_ItemList_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest>> parse_EchoRequest_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoResponse>> parse_EchoResponse_array(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<health_check_response>> parse_health_check_response_array(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<std::vector<SumRequest>> parse_SumRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<SumResponse>> parse_SumResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsRequest>> parse_StatsRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<StatsResponse>> parse_StatsResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<RegisterRequest>> parse_RegisterRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserRole>> parse_UserRole_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UserResponse>> parse_UserResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemCategory>> parse_ItemCategory_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<Item>> parse_Item_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemMetadata>> parse_ItemMetadata_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest>> parse_CreateItemRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest>> parse_UpdateItemRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<ItemList>> parse_ItemList_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoRequest>> parse_EchoRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<EchoResponse>> parse_EchoResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<std::vector<health_check_response>> parse_health_check_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_SumRequest_array_into(const std::vector<SumRequest>& arr, std::string& out);
inline void serialize_SumRequest_array_into(const arena_vector<SumRequest>& arr, std::string& out);
inline void serialize_SumResponse_array_into(const std::vector<SumResponse>& arr, std::string& out);
inline void serialize_SumResponse_array_into(const arena_vector<SumResponse>& arr, std::string& out);
inline void serialize_StatsRequest_array_into(const std::vector<StatsRequest>& arr, std::string& out);
inline void serialize_StatsRequest_array_into(const arena_vector<StatsRequest>& arr, std::string& out);
inline void serialize_StatsResponse_array_into(const std::vector<StatsResponse>& arr, std::string& out);
inline void serialize_StatsResponse_array_into(const arena_vector<StatsResponse>& arr, std::string& out);
inline void serialize_RegisterRequest_array_into(const std::vector<RegisterRequest>& arr, std::string& out);
inline void serialize_RegisterRequest_array_into(const arena_vector<RegisterRequest>& arr, std::string& out);
inline void serialize_UserRole_array_into(const std::vector<UserRole>& arr, std::string& out);
inline void serialize_UserRole_array_into(const arena_vector<UserRole>& arr, std::string& out);
inline void serialize_UserResponse_array_into(const std::vector<UserResponse>& arr, std::string& out);
inline void serialize_UserResponse_array_into(const arena_vector<UserResponse>& arr, std::string& out);
inline void serialize_ItemCategory_array_into(const std::vector<ItemCategory>& arr, std::string& out);
inline void serialize_ItemCategory_array_into(const arena_vector<ItemCategory>& arr, std::string& out);
inline void serialize_Item_array_into(const std::vector<Item>& arr, std::string& out);
inline void serialize_Item_array_into(const arena_vector<Item>& arr, std::string& out);
inline void serialize_ItemMetadata_array_into(const std::vector<ItemMetadata>& arr, std::string& out);
inline void serialize_ItemMetadata_array_into(const arena_vector<ItemMetadata>& arr, std::string& out);
inline void serialize_CreateItemRequest_array_into(const std::vector<CreateItemRequest>& arr, std::string& out);
inline void serialize_CreateItemRequest_array_into(const arena_vector<CreateItemRequest>& arr, std::string& out);
inline void serialize_UpdateItemRequest_array_into(const std::vector<UpdateItemRequest>& arr, std::string& out);
inline void serialize_UpdateItemRequest_array_into(const arena_vector<UpdateItemRequest>& arr, std::string& out);
inline void serialize_ItemList_array_into(const std::vector<ItemList>& arr, std::string& out);
inline void serialize_ItemList_array_into(const arena_vector<ItemList>& arr, std::string& out);
inline void serialize_EchoRequest_array_into(const std::vector<EchoRequest>& arr, std::string& out);
inline void serialize_EchoRequest_array_into(const arena_vector<EchoRequest>& arr, std::string& out);
inline void serialize_EchoResponse_array_into(const std::vector<EchoResponse>& arr, std::string& out);
inline void serialize_EchoResponse_array_into(const arena_vector<EchoResponse>& arr, std::string& out);
inline void serialize_health_check_response_array_into(const std::vector<health_check_response>& arr, std::string& out);
inline void serialize_health_check_response_array_into(const arena_vector<health_check_response>& arr, std::string& out);

inline std::string serialize_SumRequest_array(const std::vector<SumRequest>& arr);
inline std::string serialize_SumRequest_array(const arena_vector<SumRequest>& arr);
inline std::string serialize_SumResponse_array(const std::vector<SumResponse>& arr);
inline std::string serialize_SumResponse_array(const arena_vector<SumResponse>& arr);
inline std::string serialize_StatsRequest_array(const std::vector<StatsRequest>& arr);
inline std::string serialize_StatsRequest_array(const arena_vector<StatsRequest>& arr);
inline std::string serialize_StatsResponse_array(const std::vector<StatsResponse>& arr);
inline std::string serialize_StatsResponse_array(const arena_vector<StatsResponse>& arr);
inline std::string serialize_RegisterRequest_array(const std::vector<RegisterRequest>& arr);
inline std::string serialize_RegisterRequest_array(const arena_vector<RegisterRequest>& arr);
inline std::string serialize_UserRole_array(const std::vector<UserRole>& arr);
inline std::string serialize_UserRole_array(const arena_vector<UserRole>& arr);
inline std::string serialize_UserResponse_array(const std::vector<UserResponse>& arr);
inline std::string serialize_UserResponse_array(const arena_vector<UserResponse>& arr);
inline std::string serialize_ItemCategory_array(const std::vector<ItemCategory>& arr);
inline std::string serialize_ItemCategory_array(const arena_vector<ItemCategory>& arr);
inline std::string serialize_Item_array(const std::vector<Item>& arr);
inline std::string serialize_Item_array(const arena_vector<Item>& arr);
inline std::string serialize_ItemMetadata_array(const std::vector<ItemMetadata>& arr);
inline std::string serialize_ItemMetadata_array(const arena_vector<ItemMetadata>& arr);
inline std::string serialize_CreateItemRequest_array(const std::vector<CreateItemRequest>& arr);
inline std::string serialize_CreateItemRequest_array(const arena_vector<CreateItemRequest>& arr);
inline std::string serialize_UpdateItemRequest_array(const std::vector<UpdateItemRequest>& arr);
inline std::string serialize_UpdateItemRequest_array(const arena_vector<UpdateItemRequest>& arr);
inline std::string serialize_ItemList_array(const std::vector<ItemList>& arr);
inline std::string serialize_ItemList_array(const arena_vector<ItemList>& arr);
inline std::string serialize_EchoRequest_array(const std::vector<EchoRequest>& arr);
inline std::string serialize_EchoRequest_array(const arena_vector<EchoRequest>& arr);
inline std::string serialize_EchoResponse_array(const std::vector<EchoResponse>& arr);
inline std::string serialize_EchoResponse_array(const arena_vector<EchoResponse>& arr);
inline std::string serialize_health_check_response_array(const std::vector<health_check_response>& arr);
inline std::string serialize_health_check_response_array(const arena_vector<health_check_response>& arr);

// ============================================================
// JSON Parse Functions
// ============================================================

// parse SumRequest — object, 1 field(s)  ← api.yaml:221
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

// parse SumResponse — object, 2 field(s)  ← api.yaml:233
[[nodiscard]] inline std::optional<SumResponse> parse_SumResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    SumResponse obj(arena);
    bool has_result = false;
    bool has_count = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "result") {
            if (auto v = katana::serde::parse_double(cur)) {
                obj.result = *v;
                has_result = true;
            } else { cur.skip_value(); }
        } else         if (*key == "count") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.count = *v;
                has_count = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_result) return std::nullopt;
    if (!has_count) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<SumResponse> parse_SumResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_SumResponse(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse StatsRequest — object, 2 field(s)  ← api.yaml:243
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

// parse StatsResponse — object, 6 field(s)  ← api.yaml:257
[[nodiscard]] inline std::optional<StatsResponse> parse_StatsResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    StatsResponse obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 3:
            if (*key == "min") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.min = *v;
                } else { cur.skip_value(); }
            } else if (*key == "max") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.max = *v;
                } else { cur.skip_value(); }
            } else if (*key == "sum") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.sum = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "mean") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.mean = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "count") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.count = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 6:
            if (*key == "median") {
                if (auto v = katana::serde::parse_double(cur)) {
                    obj.median = *v;
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

[[nodiscard]] inline std::optional<StatsResponse> parse_StatsResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_StatsResponse(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse RegisterRequest — object, 6 field(s)  ← api.yaml:279
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

// parse UserRole — enum  ← api.yaml:308
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

// parse UserResponse — object, 5 field(s)  ← api.yaml:312
[[nodiscard]] inline std::optional<UserResponse> parse_UserResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    UserResponse obj(arena);
    bool has_id = false;
    bool has_username = false;
    bool has_email = false;
    bool has_role = false;
    bool has_created_at = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 2:
            if (*key == "id") {
                if (auto v = cur.string()) {
                    obj.id = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_id = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "role") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = UserRole_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.role = *enum_val;
                    has_role = true;
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
            } else { cur.skip_value(); }
            break;
        case 10:
            if (*key == "created_at") {
                if (auto v = cur.string()) {
                    obj.created_at = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_created_at = true;
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
    if (!has_id) return std::nullopt;
    if (!has_username) return std::nullopt;
    if (!has_email) return std::nullopt;
    if (!has_role) return std::nullopt;
    if (!has_created_at) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<UserResponse> parse_UserResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_UserResponse(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse ItemCategory — enum  ← api.yaml:330
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

// parse Item — object, 8 field(s)  ← api.yaml:334
[[nodiscard]] inline std::optional<Item> parse_Item(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    Item obj(arena);
    bool has_id = false;
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
        case 2:
            if (*key == "id") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.id = *v;
                    has_id = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
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
    if (!has_id) return std::nullopt;
    if (!has_name) return std::nullopt;
    if (!has_price) return std::nullopt;
    if (!has_category) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<Item> parse_Item(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_Item(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse ItemMetadata — object, 4 field(s)  ← api.yaml:360
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

// parse CreateItemRequest — object, 7 field(s)  ← api.yaml:373
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

// parse UpdateItemRequest — object, 6 field(s)  ← api.yaml:405
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

// parse ItemList — object, 4 field(s)  ← api.yaml:431
[[nodiscard]] inline std::optional<ItemList> parse_ItemList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    ItemList obj(arena);
    bool has_items = false;
    bool has_total = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 5:
            if (*key == "items") {
                if (cur.try_array_start()) {
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_array_end()) break;
                        if (auto nested = parse_Item(cur, arena)) { obj.items.push_back(std::move(*nested)); }
                        else { cur.skip_value(); }
                        cur.try_comma();
                    }
                    has_items = true;
                } else { cur.skip_value(); }
            } else if (*key == "total") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.total = *v;
                    has_total = true;
                } else { cur.skip_value(); }
            } else if (*key == "limit") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.limit = *v;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 6:
            if (*key == "offset") {
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.offset = *v;
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
    if (!has_items) return std::nullopt;
    if (!has_total) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<ItemList> parse_ItemList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_ItemList(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse EchoRequest — object, 3 field(s)  ← api.yaml:448
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

// parse EchoResponse — object, 2 field(s)  ← api.yaml:462
[[nodiscard]] inline std::optional<EchoResponse> parse_EchoResponse(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    EchoResponse obj(arena);
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
        } else         if (*key == "length") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.length = *v;
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

[[nodiscard]] inline std::optional<EchoResponse> parse_EchoResponse(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_EchoResponse(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse health_check_response — object, 2 field(s)  ← api.yaml:209
[[nodiscard]] inline std::optional<health_check_response> parse_health_check_response(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    health_check_response obj(arena);

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "status") {
            if (auto v = cur.string()) {
                obj.status = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
            } else { cur.skip_value(); }
        } else         if (*key == "uptime_ms") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.uptime_ms = *v;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<health_check_response> parse_health_check_response(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_health_check_response(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

// serialize SumRequest — object, 1 field(s)  ← api.yaml:221
inline void serialize_SumRequest_into(const SumRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"values\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.values.size(); ++i) {
        if (i > 0) json.push_back(',');
        katana::serde::append_json_double(json, obj.values[i]);
    }
    json.push_back(']');
    json.push_back('}');
}

inline std::string serialize_SumRequest(const SumRequest& obj) {
    std::string json;
    size_t reserve_estimate = 76;
    reserve_estimate += obj.values.size() * 25;
    json.reserve(reserve_estimate);
    serialize_SumRequest_into(obj, json);
    return json;
}

// serialize SumResponse — object, 2 field(s)  ← api.yaml:233
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

// serialize StatsRequest — object, 2 field(s)  ← api.yaml:243
inline void serialize_StatsRequest_into(const StatsRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"values\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.values.size(); ++i) {
        if (i > 0) json.push_back(',');
        katana::serde::append_json_double(json, obj.values[i]);
    }
    json.push_back(']');
    if (obj.include_median) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"include_median\":");
    if (!obj.include_median) {
        json.append("null");
    } else {
        json.append(*obj.include_median ? "true" : "false");
    }
    }
    json.push_back('}');
}

inline std::string serialize_StatsRequest(const StatsRequest& obj) {
    std::string json;
    size_t reserve_estimate = 99;
    reserve_estimate += obj.values.size() * 25;
    json.reserve(reserve_estimate);
    serialize_StatsRequest_into(obj, json);
    return json;
}

// serialize StatsResponse — object, 6 field(s)  ← api.yaml:257
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

// serialize RegisterRequest — object, 6 field(s)  ← api.yaml:279
inline void serialize_RegisterRequest_into(const RegisterRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
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
    json.append("\"password\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.password, json);
    json.push_back('"');
    if (obj.age) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"age\":");
    {
        if (!obj.age) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.age);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.role) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"role\":");
    if (obj.role) {
        json.push_back('"');
        json.append(to_string(*obj.role));
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
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
    json.push_back('}');
}

inline std::string serialize_RegisterRequest(const RegisterRequest& obj) {
    std::string json;
    size_t reserve_estimate = 270;
    reserve_estimate += obj.username.size();
    reserve_estimate += obj.email.size();
    reserve_estimate += obj.password.size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    json.reserve(reserve_estimate);
    serialize_RegisterRequest_into(obj, json);
    return json;
}

// serialize UserRole — enum  ← api.yaml:308
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

// serialize UserResponse — object, 5 field(s)  ← api.yaml:312
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

// serialize ItemCategory — enum  ← api.yaml:330
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

// serialize Item — object, 8 field(s)  ← api.yaml:334
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

// serialize ItemMetadata — object, 4 field(s)  ← api.yaml:360
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

// serialize CreateItemRequest — object, 7 field(s)  ← api.yaml:373
inline void serialize_CreateItemRequest_into(const CreateItemRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
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

inline std::string serialize_CreateItemRequest(const CreateItemRequest& obj) {
    std::string json;
    size_t reserve_estimate = 408;
    reserve_estimate += obj.name.size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    json.reserve(reserve_estimate);
    serialize_CreateItemRequest_into(obj, json);
    return json;
}

// serialize UpdateItemRequest — object, 6 field(s)  ← api.yaml:405
inline void serialize_UpdateItemRequest_into(const UpdateItemRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (obj.name) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"name\":");
    if (obj.name) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.name, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
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
    if (obj.price) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"price\":");
    {
        if (!obj.price) {
            json.append("null");
        } else {
        katana::serde::append_json_double(json, *obj.price);
        }
    }
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
    if (obj.category) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"category\":");
    if (obj.category) {
        json.push_back('"');
        json.append(to_string(*obj.category));
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
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
    json.push_back('}');
}

inline std::string serialize_UpdateItemRequest(const UpdateItemRequest& obj) {
    std::string json;
    size_t reserve_estimate = 268;
    if (obj.name) reserve_estimate += obj.name->size();
    if (obj.description) reserve_estimate += obj.description->size();
    if (obj.tags) reserve_estimate += obj.tags->size() * 32;
    json.reserve(reserve_estimate);
    serialize_UpdateItemRequest_into(obj, json);
    return json;
}

// serialize ItemList — object, 4 field(s)  ← api.yaml:431
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

// serialize EchoRequest — object, 3 field(s)  ← api.yaml:448
inline void serialize_EchoRequest_into(const EchoRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"message\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.message, json);
    json.push_back('"');
    if (obj.repeat) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"repeat\":");
    {
        if (!obj.repeat) {
            json.append("null");
        } else {
            char buf[32];
            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj.repeat);
            json.append(buf, static_cast<size_t>(ptr - buf));
        }
    }
    }
    if (obj.uppercase) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"uppercase\":");
    if (!obj.uppercase) {
        json.append("null");
    } else {
        json.append(*obj.uppercase ? "true" : "false");
    }
    }
    json.push_back('}');
}

inline std::string serialize_EchoRequest(const EchoRequest& obj) {
    std::string json;
    size_t reserve_estimate = 93;
    reserve_estimate += obj.message.size();
    json.reserve(reserve_estimate);
    serialize_EchoRequest_into(obj, json);
    return json;
}

// serialize EchoResponse — object, 2 field(s)  ← api.yaml:462
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

// serialize health_check_response — object, 2 field(s)  ← api.yaml:209
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

// ============================================================
// Array Parse Functions
// ============================================================

// parse_array SumRequest — object, 1 field(s)  ← api.yaml:221
[[nodiscard]] inline std::optional<std::vector<SumRequest>> parse_SumRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<SumRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_SumRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SumRequest>> parse_SumRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumRequest_array(cur, arena);
}

// parse_array SumResponse — object, 2 field(s)  ← api.yaml:233
[[nodiscard]] inline std::optional<std::vector<SumResponse>> parse_SumResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<SumResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_SumResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<SumResponse>> parse_SumResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_SumResponse_array(cur, arena);
}

// parse_array StatsRequest — object, 2 field(s)  ← api.yaml:243
[[nodiscard]] inline std::optional<std::vector<StatsRequest>> parse_StatsRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsRequest>> parse_StatsRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsRequest_array(cur, arena);
}

// parse_array StatsResponse — object, 6 field(s)  ← api.yaml:257
[[nodiscard]] inline std::optional<std::vector<StatsResponse>> parse_StatsResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<StatsResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_StatsResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<StatsResponse>> parse_StatsResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_StatsResponse_array(cur, arena);
}

// parse_array RegisterRequest — object, 6 field(s)  ← api.yaml:279
[[nodiscard]] inline std::optional<std::vector<RegisterRequest>> parse_RegisterRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<RegisterRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_RegisterRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<RegisterRequest>> parse_RegisterRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_RegisterRequest_array(cur, arena);
}

// parse_array UserRole — enum  ← api.yaml:308
[[nodiscard]] inline std::optional<std::vector<UserRole>> parse_UserRole_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserRole> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserRole(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserRole>> parse_UserRole_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserRole_array(cur, arena);
}

// parse_array UserResponse — object, 5 field(s)  ← api.yaml:312
[[nodiscard]] inline std::optional<std::vector<UserResponse>> parse_UserResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UserResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UserResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UserResponse>> parse_UserResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UserResponse_array(cur, arena);
}

// parse_array ItemCategory — enum  ← api.yaml:330
[[nodiscard]] inline std::optional<std::vector<ItemCategory>> parse_ItemCategory_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemCategory> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemCategory(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemCategory>> parse_ItemCategory_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemCategory_array(cur, arena);
}

// parse_array Item — object, 8 field(s)  ← api.yaml:334
[[nodiscard]] inline std::optional<std::vector<Item>> parse_Item_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<Item> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_Item(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<Item>> parse_Item_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_Item_array(cur, arena);
}

// parse_array ItemMetadata — object, 4 field(s)  ← api.yaml:360
[[nodiscard]] inline std::optional<std::vector<ItemMetadata>> parse_ItemMetadata_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemMetadata> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemMetadata(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemMetadata>> parse_ItemMetadata_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemMetadata_array(cur, arena);
}

// parse_array CreateItemRequest — object, 7 field(s)  ← api.yaml:373
[[nodiscard]] inline std::optional<std::vector<CreateItemRequest>> parse_CreateItemRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<CreateItemRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_CreateItemRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<CreateItemRequest>> parse_CreateItemRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_CreateItemRequest_array(cur, arena);
}

// parse_array UpdateItemRequest — object, 6 field(s)  ← api.yaml:405
[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest>> parse_UpdateItemRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<UpdateItemRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_UpdateItemRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<UpdateItemRequest>> parse_UpdateItemRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_UpdateItemRequest_array(cur, arena);
}

// parse_array ItemList — object, 4 field(s)  ← api.yaml:431
[[nodiscard]] inline std::optional<std::vector<ItemList>> parse_ItemList_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<ItemList> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_ItemList(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<ItemList>> parse_ItemList_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_ItemList_array(cur, arena);
}

// parse_array EchoRequest — object, 3 field(s)  ← api.yaml:448
[[nodiscard]] inline std::optional<std::vector<EchoRequest>> parse_EchoRequest_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoRequest> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoRequest(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoRequest>> parse_EchoRequest_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoRequest_array(cur, arena);
}

// parse_array EchoResponse — object, 2 field(s)  ← api.yaml:462
[[nodiscard]] inline std::optional<std::vector<EchoResponse>> parse_EchoResponse_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<EchoResponse> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_EchoResponse(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<EchoResponse>> parse_EchoResponse_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_EchoResponse_array(cur, arena);
}

// parse_array health_check_response — object, 2 field(s)  ← api.yaml:209
[[nodiscard]] inline std::optional<std::vector<health_check_response>> parse_health_check_response_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_array_start()) return std::nullopt;

    std::vector<health_check_response> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) break;
        
        // Parse object at current cursor position
        auto obj = parse_health_check_response(cur, arena);
        if (!obj) return std::nullopt;
        result.push_back(std::move(*obj));
        
        cur.try_comma();
    }
    return result;
}

[[nodiscard]] inline std::optional<std::vector<health_check_response>> parse_health_check_response_array(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    return parse_health_check_response_array(cur, arena);
}

// ============================================================
// Array Serialize Functions
// ============================================================

// serialize_array SumRequest — object, 1 field(s)  ← api.yaml:221
inline void serialize_SumRequest_array_into(const std::vector<SumRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_array(const std::vector<SumRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 76 + 2);
    serialize_SumRequest_array_into(arr, json);
    return json;
}

inline void serialize_SumRequest_array_into(const arena_vector<SumRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumRequest_array(const arena_vector<SumRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 76 + 2);
    serialize_SumRequest_array_into(arr, json);
    return json;
}

// serialize_array SumResponse — object, 2 field(s)  ← api.yaml:233
inline void serialize_SumResponse_array_into(const std::vector<SumResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumResponse_array(const std::vector<SumResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 66 + 2);
    serialize_SumResponse_array_into(arr, json);
    return json;
}

inline void serialize_SumResponse_array_into(const arena_vector<SumResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_SumResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_SumResponse_array(const arena_vector<SumResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 66 + 2);
    serialize_SumResponse_array_into(arr, json);
    return json;
}

// serialize_array StatsRequest — object, 2 field(s)  ← api.yaml:243
inline void serialize_StatsRequest_array_into(const std::vector<StatsRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_array(const std::vector<StatsRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 99 + 2);
    serialize_StatsRequest_array_into(arr, json);
    return json;
}

inline void serialize_StatsRequest_array_into(const arena_vector<StatsRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsRequest_array(const arena_vector<StatsRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 99 + 2);
    serialize_StatsRequest_array_into(arr, json);
    return json;
}

// serialize_array StatsResponse — object, 6 field(s)  ← api.yaml:257
inline void serialize_StatsResponse_array_into(const std::vector<StatsResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_array(const std::vector<StatsResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 195 + 2);
    serialize_StatsResponse_array_into(arr, json);
    return json;
}

inline void serialize_StatsResponse_array_into(const arena_vector<StatsResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_StatsResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_StatsResponse_array(const arena_vector<StatsResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 195 + 2);
    serialize_StatsResponse_array_into(arr, json);
    return json;
}

// serialize_array RegisterRequest — object, 6 field(s)  ← api.yaml:279
inline void serialize_RegisterRequest_array_into(const std::vector<RegisterRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_array(const std::vector<RegisterRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 270 + 2);
    serialize_RegisterRequest_array_into(arr, json);
    return json;
}

inline void serialize_RegisterRequest_array_into(const arena_vector<RegisterRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_RegisterRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_RegisterRequest_array(const arena_vector<RegisterRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 270 + 2);
    serialize_RegisterRequest_array_into(arr, json);
    return json;
}

// serialize_array UserRole — enum  ← api.yaml:308
inline void serialize_UserRole_array_into(const std::vector<UserRole>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserRole_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserRole_array(const std::vector<UserRole>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserRole_array_into(arr, json);
    return json;
}

inline void serialize_UserRole_array_into(const arena_vector<UserRole>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserRole_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserRole_array(const arena_vector<UserRole>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_UserRole_array_into(arr, json);
    return json;
}

// serialize_array UserResponse — object, 5 field(s)  ← api.yaml:312
inline void serialize_UserResponse_array_into(const std::vector<UserResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_array(const std::vector<UserResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 211 + 2);
    serialize_UserResponse_array_into(arr, json);
    return json;
}

inline void serialize_UserResponse_array_into(const arena_vector<UserResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UserResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UserResponse_array(const arena_vector<UserResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 211 + 2);
    serialize_UserResponse_array_into(arr, json);
    return json;
}

// serialize_array ItemCategory — enum  ← api.yaml:330
inline void serialize_ItemCategory_array_into(const std::vector<ItemCategory>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemCategory_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemCategory_array(const std::vector<ItemCategory>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemCategory_array_into(arr, json);
    return json;
}

inline void serialize_ItemCategory_array_into(const arena_vector<ItemCategory>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemCategory_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemCategory_array(const arena_vector<ItemCategory>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    serialize_ItemCategory_array_into(arr, json);
    return json;
}

// serialize_array Item — object, 8 field(s)  ← api.yaml:334
inline void serialize_Item_array_into(const std::vector<Item>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_array(const std::vector<Item>& arr) {
    std::string json;
    json.reserve(arr.size() * 434 + 2);
    serialize_Item_array_into(arr, json);
    return json;
}

inline void serialize_Item_array_into(const arena_vector<Item>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Item_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_Item_array(const arena_vector<Item>& arr) {
    std::string json;
    json.reserve(arr.size() * 434 + 2);
    serialize_Item_array_into(arr, json);
    return json;
}

// serialize_array ItemMetadata — object, 4 field(s)  ← api.yaml:360
inline void serialize_ItemMetadata_array_into(const std::vector<ItemMetadata>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_array(const std::vector<ItemMetadata>& arr) {
    std::string json;
    json.reserve(arr.size() * 173 + 2);
    serialize_ItemMetadata_array_into(arr, json);
    return json;
}

inline void serialize_ItemMetadata_array_into(const arena_vector<ItemMetadata>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemMetadata_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemMetadata_array(const arena_vector<ItemMetadata>& arr) {
    std::string json;
    json.reserve(arr.size() * 173 + 2);
    serialize_ItemMetadata_array_into(arr, json);
    return json;
}

// serialize_array CreateItemRequest — object, 7 field(s)  ← api.yaml:373
inline void serialize_CreateItemRequest_array_into(const std::vector<CreateItemRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_array(const std::vector<CreateItemRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 408 + 2);
    serialize_CreateItemRequest_array_into(arr, json);
    return json;
}

inline void serialize_CreateItemRequest_array_into(const arena_vector<CreateItemRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_CreateItemRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_CreateItemRequest_array(const arena_vector<CreateItemRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 408 + 2);
    serialize_CreateItemRequest_array_into(arr, json);
    return json;
}

// serialize_array UpdateItemRequest — object, 6 field(s)  ← api.yaml:405
inline void serialize_UpdateItemRequest_array_into(const std::vector<UpdateItemRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_array(const std::vector<UpdateItemRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 268 + 2);
    serialize_UpdateItemRequest_array_into(arr, json);
    return json;
}

inline void serialize_UpdateItemRequest_array_into(const arena_vector<UpdateItemRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_UpdateItemRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_UpdateItemRequest_array(const arena_vector<UpdateItemRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 268 + 2);
    serialize_UpdateItemRequest_array_into(arr, json);
    return json;
}

// serialize_array ItemList — object, 4 field(s)  ← api.yaml:431
inline void serialize_ItemList_array_into(const std::vector<ItemList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_array(const std::vector<ItemList>& arr) {
    std::string json;
    json.reserve(arr.size() * 163 + 2);
    serialize_ItemList_array_into(arr, json);
    return json;
}

inline void serialize_ItemList_array_into(const arena_vector<ItemList>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_ItemList_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_ItemList_array(const arena_vector<ItemList>& arr) {
    std::string json;
    json.reserve(arr.size() * 163 + 2);
    serialize_ItemList_array_into(arr, json);
    return json;
}

// serialize_array EchoRequest — object, 3 field(s)  ← api.yaml:448
inline void serialize_EchoRequest_array_into(const std::vector<EchoRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_array(const std::vector<EchoRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 93 + 2);
    serialize_EchoRequest_array_into(arr, json);
    return json;
}

inline void serialize_EchoRequest_array_into(const arena_vector<EchoRequest>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoRequest_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoRequest_array(const arena_vector<EchoRequest>& arr) {
    std::string json;
    json.reserve(arr.size() * 93 + 2);
    serialize_EchoRequest_array_into(arr, json);
    return json;
}

// serialize_array EchoResponse — object, 2 field(s)  ← api.yaml:462
inline void serialize_EchoResponse_array_into(const std::vector<EchoResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoResponse_array(const std::vector<EchoResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_EchoResponse_array_into(arr, json);
    return json;
}

inline void serialize_EchoResponse_array_into(const arena_vector<EchoResponse>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_EchoResponse_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_EchoResponse_array(const arena_vector<EchoResponse>& arr) {
    std::string json;
    json.reserve(arr.size() * 75 + 2);
    serialize_EchoResponse_array_into(arr, json);
    return json;
}

// serialize_array health_check_response — object, 2 field(s)  ← api.yaml:209
inline void serialize_health_check_response_array_into(const std::vector<health_check_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_health_check_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_health_check_response_array(const std::vector<health_check_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 77 + 2);
    serialize_health_check_response_array_into(arr, json);
    return json;
}

inline void serialize_health_check_response_array_into(const arena_vector<health_check_response>& arr, std::string& json) {
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_health_check_response_into(arr[i], json);
    }
    json.push_back(']');
}

inline std::string serialize_health_check_response_array(const arena_vector<health_check_response>& arr) {
    std::string json;
    json.reserve(arr.size() * 77 + 2);
    serialize_health_check_response_array_into(arr, json);
    return json;
}

