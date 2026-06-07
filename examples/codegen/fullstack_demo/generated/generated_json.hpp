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

[[nodiscard]] inline std::optional<Priority> parse_Priority(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateNoteRequest> parse_CreateNoteRequest(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Note> parse_Note(std::string_view json, monotonic_arena* arena);
[[nodiscard]] inline std::optional<NoteList> parse_NoteList(std::string_view json, monotonic_arena* arena);

[[nodiscard]] inline std::optional<Priority> parse_Priority(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateNoteRequest> parse_CreateNoteRequest(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<CreateNoteRequest_Metadata_t> parse_CreateNoteRequest_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Note> parse_Note(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<Note_Metadata_t> parse_Note_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena);
[[nodiscard]] inline std::optional<NoteList> parse_NoteList(katana::serde::json_cursor& cur, monotonic_arena* arena);

inline void serialize_Priority_into(const Priority& obj, std::string& out);
inline void serialize_CreateNoteRequest_into(const CreateNoteRequest& obj, std::string& out);
inline void serialize_CreateNoteRequest_Metadata_t_into(const CreateNoteRequest_Metadata_t& obj, std::string& out);
inline void serialize_Note_into(const Note& obj, std::string& out);
inline void serialize_Note_Metadata_t_into(const Note_Metadata_t& obj, std::string& out);
inline void serialize_NoteList_into(const NoteList& obj, std::string& out);

inline std::string serialize_Priority(const Priority& obj);
inline std::string serialize_CreateNoteRequest(const CreateNoteRequest& obj);
inline std::string serialize_CreateNoteRequest_Metadata_t(const CreateNoteRequest_Metadata_t& obj);
inline std::string serialize_Note(const Note& obj);
inline std::string serialize_Note_Metadata_t(const Note_Metadata_t& obj);
inline std::string serialize_NoteList(const NoteList& obj);

// ============================================================
// JSON Parse Functions
// ============================================================

// parse Priority — enum  ← api.yaml:43
[[nodiscard]] inline std::optional<Priority> parse_Priority(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    (void)arena;
    if (auto v = cur.string()) {
        std::string ev_scratch_;
        return Priority_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
    }
    return std::nullopt;
}

[[nodiscard]] inline std::optional<Priority> parse_Priority(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_Priority(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CreateNoteRequest — object, 5 field(s)  ← api.yaml:46
[[nodiscard]] inline std::optional<CreateNoteRequest> parse_CreateNoteRequest(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    CreateNoteRequest obj(arena);
    bool has_title = false;
    bool has_body = false;
    bool has_priority = false;
    bool has_due_date = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        switch (key->size()) {
        case 4:
            if (*key == "body") {
                if (auto v = cur.string()) {
                    obj.body = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_body = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "title") {
                if (auto v = cur.string()) {
                    obj.title = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_title = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "priority") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = Priority_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.priority = *enum_val;
                    has_priority = true;
                    }
                } else { cur.skip_value(); }
            } else if (*key == "metadata") {
                if (auto nested = parse_CreateNoteRequest_Metadata_t(cur, arena)) {
                    obj.metadata = std::move(*nested);
                } else { cur.skip_value(); }
            } else if (*key == "due_date") {
                if (auto v = cur.string()) {
                    obj.due_date = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_due_date = true;
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
    if (!has_title) return std::nullopt;
    if (!has_body) return std::nullopt;
    if (!has_priority) return std::nullopt;
    if (!has_due_date) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<CreateNoteRequest> parse_CreateNoteRequest(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_CreateNoteRequest(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse CreateNoteRequest_Metadata_t — object, field CreateNoteRequest.metadata  ← api.yaml:53
[[nodiscard]] inline std::optional<CreateNoteRequest_Metadata_t> parse_CreateNoteRequest_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    cur.skip_ws();
    const char* ff_start_ = cur.ptr;
    cur.skip_value();
    return CreateNoteRequest_Metadata_t(ff_start_, cur.ptr, arena_allocator<char>(arena));
}

// parse Note — object, 7 field(s)  ← api.yaml:55
[[nodiscard]] inline std::optional<Note> parse_Note(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    Note obj(arena);
    bool has_id = false;
    bool has_title = false;
    bool has_body = false;
    bool has_priority = false;
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
                if (auto v = katana::serde::parse_int64(cur)) {
                    obj.id = *v;
                    has_id = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 4:
            if (*key == "body") {
                if (auto v = cur.string()) {
                    obj.body = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_body = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 5:
            if (*key == "title") {
                if (auto v = cur.string()) {
                    obj.title = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
                    has_title = true;
                } else { cur.skip_value(); }
            } else { cur.skip_value(); }
            break;
        case 8:
            if (*key == "priority") {
                if (auto v = cur.string()) {
                    std::string ev_scratch_;
                    auto enum_val = Priority_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));
                    if (enum_val) { obj.priority = *enum_val;
                    has_priority = true;
                    }
                } else { cur.skip_value(); }
            } else if (*key == "metadata") {
                if (auto nested = parse_Note_Metadata_t(cur, arena)) {
                    obj.metadata = std::move(*nested);
                } else { cur.skip_value(); }
            } else if (*key == "due_date") {
                if (auto v = cur.string()) {
                    obj.due_date = katana::serde::decode_json_string<arena_string<>>(*v, arena_allocator<char>(arena));
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
    if (!has_title) return std::nullopt;
    if (!has_body) return std::nullopt;
    if (!has_priority) return std::nullopt;
    if (!has_created_at) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<Note> parse_Note(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_Note(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// parse Note_Metadata_t — object, field Note.metadata  ← api.yaml:63
[[nodiscard]] inline std::optional<Note_Metadata_t> parse_Note_Metadata_t(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    cur.skip_ws();
    const char* ff_start_ = cur.ptr;
    cur.skip_value();
    return Note_Metadata_t(ff_start_, cur.ptr, arena_allocator<char>(arena));
}

// parse NoteList — object, 2 field(s)  ← api.yaml:66
[[nodiscard]] inline std::optional<NoteList> parse_NoteList(katana::serde::json_cursor& cur, monotonic_arena* arena) {
    if (!cur.try_object_start()) return std::nullopt;

    NoteList obj(arena);
    bool has_notes = false;
    bool has_count = false;

    bool obj_closed_ = false;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) { obj_closed_ = true; break; }
        auto key = cur.string();
        if (!key || !cur.consume(':')) break;

        if (*key == "notes") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) break;
                    if (auto nested = parse_Note(cur, arena)) { obj.notes.push_back(std::move(*nested)); }
                    else { cur.skip_value(); }
                    cur.try_comma();
                }
                has_notes = true;
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
    if (!has_notes) return std::nullopt;
    if (!has_count) return std::nullopt;
    return obj;
}

[[nodiscard]] inline std::optional<NoteList> parse_NoteList(std::string_view json, monotonic_arena* arena) {
    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};
    auto result = parse_NoteList(cur, arena);
    if (!result) return std::nullopt;
    cur.skip_ws();
    if (!cur.eof()) return std::nullopt;
    return result;
}

// ============================================================
// JSON Serialize Functions
// ============================================================

// serialize Priority — enum  ← api.yaml:43
inline void serialize_Priority_into(const Priority& obj, std::string& json) {
    auto str = to_string(obj);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
}

inline std::string serialize_Priority(const Priority& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    serialize_Priority_into(obj, json);
    return json;
}

// serialize CreateNoteRequest — object, 5 field(s)  ← api.yaml:46
inline void serialize_CreateNoteRequest_into(const CreateNoteRequest& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"body\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.body, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"priority\":");
    json.push_back('"');
    json.append(to_string(obj.priority));
    json.push_back('"');
    if (obj.metadata) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"metadata\":");
    if (obj.metadata) {
        serialize_CreateNoteRequest_Metadata_t_into(*obj.metadata, json);
    } else {
        json.append("null");
    }
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"due_date\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.due_date, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_CreateNoteRequest(const CreateNoteRequest& obj) {
    std::string json;
    size_t reserve_estimate = 311;
    reserve_estimate += obj.title.size();
    reserve_estimate += obj.body.size();
    reserve_estimate += obj.due_date.size();
    json.reserve(reserve_estimate);
    serialize_CreateNoteRequest_into(obj, json);
    return json;
}

// serialize CreateNoteRequest_Metadata_t — object, field CreateNoteRequest.metadata  ← api.yaml:53
inline void serialize_CreateNoteRequest_Metadata_t_into(const CreateNoteRequest_Metadata_t& obj, std::string& json) {
    if (obj.empty()) { json.append("null"); }
    else { json.append(obj.data(), obj.size()); }
}

inline std::string serialize_CreateNoteRequest_Metadata_t(const CreateNoteRequest_Metadata_t& obj) {
    std::string json;
    json.reserve(obj.size() + 4);
    serialize_CreateNoteRequest_Metadata_t_into(obj, json);
    return json;
}

// serialize Note — object, 7 field(s)  ← api.yaml:55
inline void serialize_Note_into(const Note& obj, std::string& json) {
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
    json.append("\"title\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.title, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"body\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.body, json);
    json.push_back('"');
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"priority\":");
    json.push_back('"');
    json.append(to_string(obj.priority));
    json.push_back('"');
    if (obj.metadata) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"metadata\":");
    if (obj.metadata) {
        serialize_Note_Metadata_t_into(*obj.metadata, json);
    } else {
        json.append("null");
    }
    }
    if (obj.due_date) {
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"due_date\":");
    if (obj.due_date) {
        json.push_back('"');
        katana::serde::escape_json_string_into(*obj.due_date, json);
        json.push_back('"');
    } else {
        json.append("null");
    }
    }
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"created_at\":");
    json.push_back('"');
    katana::serde::escape_json_string_into(obj.created_at, json);
    json.push_back('"');
    json.push_back('}');
}

inline std::string serialize_Note(const Note& obj) {
    std::string json;
    size_t reserve_estimate = 383;
    reserve_estimate += obj.title.size();
    reserve_estimate += obj.body.size();
    if (obj.due_date) reserve_estimate += obj.due_date->size();
    reserve_estimate += obj.created_at.size();
    json.reserve(reserve_estimate);
    serialize_Note_into(obj, json);
    return json;
}

// serialize Note_Metadata_t — object, field Note.metadata  ← api.yaml:63
inline void serialize_Note_Metadata_t_into(const Note_Metadata_t& obj, std::string& json) {
    if (obj.empty()) { json.append("null"); }
    else { json.append(obj.data(), obj.size()); }
}

inline std::string serialize_Note_Metadata_t(const Note_Metadata_t& obj) {
    std::string json;
    json.reserve(obj.size() + 4);
    serialize_Note_Metadata_t_into(obj, json);
    return json;
}

// serialize NoteList — object, 2 field(s)  ← api.yaml:66
inline void serialize_NoteList_into(const NoteList& obj, std::string& json) {
    json.push_back('{');
    bool first_field_ = true;
    if (!first_field_) json.push_back(',');
    first_field_ = false;
    json.append("\"notes\":");
    json.push_back('[');
    for (size_t i = 0; i < obj.notes.size(); ++i) {
        if (i > 0) json.push_back(',');
        serialize_Note_into(obj.notes[i], json);
    }
    json.push_back(']');
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

inline std::string serialize_NoteList(const NoteList& obj) {
    std::string json;
    size_t reserve_estimate = 104;
    reserve_estimate += obj.notes.size() * 383;
    json.reserve(reserve_estimate);
    serialize_NoteList_into(obj, json);
    return json;
}

