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

[[nodiscard]] inline std::optional<PetCreateRequest> parse_PetCreateRequest(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<PetCreateRequest> parse_PetCreateRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_PetResponse_into(const PetResponse& obj, std::string& out);
inline void serialize_ListPetsResponse_into(const ListPetsResponse& obj, std::string& out);

inline std::string serialize_PetResponse(const PetResponse& obj);
inline std::string serialize_ListPetsResponse(const ListPetsResponse& obj);

// ============================================================
// JSON Parse Functions
// ============================================================

// parse PetCreateRequest — object, 3 field(s)
[[nodiscard]] inline std::optional<PetCreateRequest> parse_PetCreateRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    PetCreateRequest obj(arena);
    bool has_name = false;
    bool has_age = false;
    bool has_ownerEmail = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "name") {
            if (auto v = cur.string()) {
                obj.name = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_name = true;
            } else { cur.skip_value(); }
        } else         if (*key == "age") {
            if (auto v = katana::serde::parse_int64(cur)) {
                obj.age = *v;
                has_age = true;
            } else { cur.skip_value(); }
        } else         if (*key == "ownerEmail") {
            if (auto v = cur.string()) {
                obj.ownerEmail = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                has_ownerEmail = true;
            } else { cur.skip_value(); }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!obj_closed_) return std::nullopt;
    if (!has_name) return std::nullopt;
    if (!has_age) return std::nullopt;
    if (!has_ownerEmail) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<PetCreateRequest> parse_PetCreateRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_PetCreateRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

// serialize PetResponse — object, 4 field(s)
inline void serialize_PetResponse_into(const PetResponse& obj, std::string& json) {
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
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"age\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.age);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"ownerEmail\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.ownerEmail, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_PetResponse(const PetResponse& obj) {
    std::string json;
    size_t reserve_estimate = 141;
    reserve_estimate += obj.name.size();
    reserve_estimate += obj.ownerEmail.size();
    json.reserve(reserve_estimate);
    serialize_PetResponse_into(obj, json);
    return json;
}

// serialize ListPetsResponse — object, 3 field(s)
inline void serialize_ListPetsResponse_into(const ListPetsResponse& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"trace\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.trace, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"limitApplied\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.limitApplied);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (obj.session) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"session\":");
    if (obj.session) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.session, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    json.push_back('}');
}

inline std::string serialize_ListPetsResponse(const ListPetsResponse& obj) {
    std::string json;
    size_t reserve_estimate = 122;
    reserve_estimate += obj.trace.size();
    if (obj.session) reserve_estimate += obj.session->size();
    json.reserve(reserve_estimate);
    serialize_ListPetsResponse_into(obj, json);
    return json;
}

