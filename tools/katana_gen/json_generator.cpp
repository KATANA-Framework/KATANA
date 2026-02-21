#include "generator.hpp"

#include <map>
#include <sstream>
#include <string>

namespace katana_gen {
namespace {

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Unified Code Generation Framework
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

enum class parse_context {
    top_level,       // Return value directly: return Type{...};
    object_property, // Assign to property: obj.field = ...;
    array_item       // Append to array: obj.field.push_back(...);
};

enum class serialize_context {
    top_level,       // Return string: return "...";
    object_property, // Append to json: json.append(...);
    array_item       // Append to json inside array loop
};

struct parse_gen_context {
    parse_context ctx;
    std::string target_var; // "obj", "result", etc.
    std::string field_name; // Property or array field name
    bool use_pmr;
    int indent; // Indentation level for generated code
};

struct serialize_gen_context {
    serialize_context ctx;
    std::string source_expr; // "obj", "obj.field", "obj.field[i]", etc.
    int indent;              // Indentation level
};

// FNV-1a hash for compile-time key dispatch
constexpr uint64_t fnv1a_hash(std::string_view str) noexcept {
    uint64_t hash = 14695981039346656037ull;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ull;
    }
    return hash;
}

// Generate the field parsing body for a single property (reusable across strategies)
void generate_field_parse_body(std::ostream& out,
                               const document& doc,
                               const katana::openapi::property& prop,
                               bool use_pmr,
                               const std::string& indent) {
    if (prop.required) {
        out << indent << "    has_" << prop.name << " = true;\n";
    }
    if (!prop.type) {
        out << indent << "    cur.skip_value();\n";
        return;
    }
    using katana::openapi::schema_kind;
    bool is_enum = prop.type->kind == schema_kind::string && !prop.type->enum_values.empty();
    auto nested_name = schema_identifier(doc, prop.type);
    if (is_enum && !nested_name.empty()) {
        out << indent << "    if (auto v = cur.string()) {\n";
        out << indent << "        auto enum_val = " << nested_name
            << "_enum_from_string(std::string_view(v->begin(), v->end()));\n";
        out << indent << "        if (enum_val) obj." << prop.name << " = *enum_val;\n";
        out << indent << "    } else { cur.skip_value(); }\n";
    } else {
        switch (prop.type->kind) {
        case schema_kind::string:
            out << indent << "    if (auto v = cur.string()) {\n";
            if (use_pmr) {
                out << indent << "        obj." << prop.name
                    << " = arena_string<>(v->begin(), v->end(), "
                       "arena_allocator<char>(arena));\n";
            } else {
                out << indent << "        obj." << prop.name
                    << " = std::string(v->begin(), v->end());\n";
            }
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::integer:
            out << indent << "    if (auto v = katana::serde::parse_int64(cur)) {\n";
            out << indent << "        obj." << prop.name << " = *v;\n";
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::number:
            out << indent << "    if (auto v = katana::serde::parse_double(cur)) {\n";
            out << indent << "        obj." << prop.name << " = *v;\n";
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::boolean:
            out << indent << "    if (auto v = katana::serde::parse_bool(cur)) {\n";
            out << indent << "        obj." << prop.name << " = *v;\n";
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::array:
            out << indent << "    if (cur.try_array_start()) {\n";
            out << indent << "        while (!cur.eof()) {\n";
            out << indent << "            cur.skip_ws();\n";
            out << indent << "            if (cur.try_array_end()) break;\n";
            if (prop.type->items) {
                auto* item = prop.type->items;
                switch (item->kind) {
                case schema_kind::string:
                    if (!item->enum_values.empty()) {
                        auto enum_item_name = schema_identifier(doc, item);
                        out << indent << "            if (auto v = cur.string()) {\n";
                        out << indent << "                auto enum_val = " << enum_item_name
                            << "_enum_from_string(std::string_view(v->begin(), v->end()));\n";
                        out << indent
                            << "                if (enum_val) obj." << prop.name
                            << ".push_back(*enum_val);\n";
                        out << indent << "            } else { cur.skip_value(); }\n";
                    } else {
                        out << indent << "            if (auto v = cur.string()) {\n";
                        if (use_pmr) {
                            out << indent << "                obj." << prop.name
                                << ".emplace_back(v->begin(), v->end(), "
                                   "arena_allocator<char>(arena));\n";
                        } else {
                            out << indent << "                obj." << prop.name
                                << ".emplace_back(v->begin(), v->end());\n";
                        }
                        out << indent << "            } else { cur.skip_value(); }\n";
                    }
                    break;
                case schema_kind::integer:
                    out << indent
                        << "            if (auto v = "
                           "katana::serde::parse_int64(cur)) {\n";
                    out << indent << "                obj." << prop.name << ".push_back(*v);\n";
                    out << indent << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::number:
                    out << indent
                        << "            if (auto v = "
                           "katana::serde::parse_double(cur)) {\n";
                    out << indent << "                obj." << prop.name << ".push_back(*v);\n";
                    out << indent << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::boolean:
                    out << indent
                        << "            if (auto v = "
                           "katana::serde::parse_bool(cur)) {\n";
                    out << indent << "                obj." << prop.name << ".push_back(*v);\n";
                    out << indent << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::object: {
                    auto nested_array_name = schema_identifier(doc, item);
                    if (!nested_array_name.empty()) {
                        out << indent << "            if (auto nested = parse_" << nested_array_name
                            << "(cur, arena)) { obj." << prop.name << ".push_back(*nested); }\n";
                        out << indent << "            else { cur.skip_value(); }\n";
                    } else {
                        out << indent << "            cur.skip_value();\n";
                    }
                    break;
                }
                default:
                    out << indent << "            cur.skip_value();\n";
                    break;
                }
            } else {
                out << indent << "            cur.skip_value();\n";
            }
            out << indent << "            cur.try_comma();\n";
            out << indent << "        }\n";
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::object: {
            auto nested_obj_name = schema_identifier(doc, prop.type);
            if (!nested_obj_name.empty()) {
                out << indent << "    if (auto nested = parse_" << nested_obj_name
                    << "(cur, arena)) {\n";
                out << indent << "        obj." << prop.name << " = *nested;\n";
                out << indent << "    } else { cur.skip_value(); }\n";
            } else {
                out << indent << "    cur.skip_value();\n";
            }
            break;
        }
        default:
            out << indent << "    cur.skip_value();\n";
            break;
        }
    }
}

// Compute a reserve estimate for the serializer based on the schema's fields.
// type_estimate: bool=5, int=20, double=25, string=32, array=64, object=128
size_t compute_reserve_estimate(const document& /*doc*/, const katana::openapi::schema& s) {
    using katana::openapi::schema_kind;
    size_t estimated = 2; // {}
    for (const auto& prop : s.properties) {
        size_t type_est = 32; // default for unknown
        if (prop.type) {
            switch (prop.type->kind) {
            case schema_kind::boolean:
                type_est = 5;
                break;
            case schema_kind::integer:
                type_est = 20;
                break;
            case schema_kind::number:
                type_est = 25;
                break;
            case schema_kind::string:
                type_est = 32;
                break;
            case schema_kind::array:
                type_est = 64;
                break;
            case schema_kind::object:
                type_est = 128;
                break;
            default:
                type_est = 32;
                break;
            }
        }
        estimated += prop.name.length() + 4 + type_est; // key + quotes + colon + comma + value
    }
    return estimated;
}

// ────────────────────────────────────────────────────────────────────────
// Parser: cursor-based overload (primary implementation)
// ────────────────────────────────────────────────────────────────────────

void generate_json_parser_for_schema_cursor(std::ostream& out,
                                            const document& doc,
                                            const katana::openapi::schema& s,
                                            bool use_pmr) {
    auto struct_name = schema_identifier(doc, &s);
    out << "[[nodiscard]] inline std::optional<" << struct_name << "> parse_" << struct_name
        << "(katana::serde::json_cursor& cur, monotonic_arena* arena) {\n";
    if (!use_pmr) {
        out << "    (void)arena;\n";
    }

    // Scalars and arrays
    if (s.properties.empty()) {
        using katana::openapi::schema_kind;

        // Check if this is an enum type (string with enum_values)
        bool is_enum = s.kind == schema_kind::string && !s.enum_values.empty();

        if (is_enum) {
            // Generate enum parser
            out << "    (void)arena;\n";
            out << "    if (auto v = cur.string()) {\n";
            out << "        return " << struct_name
                << "_enum_from_string(std::string_view(v->begin(), v->end()));\n";
            out << "    }\n";
            out << "    return std::nullopt;\n";
            out << "}\n\n";
            return;
        }

        switch (s.kind) {
        case schema_kind::string:
            out << "    if (auto v = cur.string()) {\n";
            if (use_pmr) {
                out << "        return " << struct_name
                    << "{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};\n";
            } else {
                out << "        return " << struct_name << "{std::string(v->begin(), v->end())};\n";
            }
            out << "    }\n";
            out << "    return std::nullopt;\n";
            out << "}\n\n";
            return;
        case schema_kind::integer:
            out << "    (void)arena;\n";
            out << "    if (auto v = katana::serde::parse_int64(cur)) return " << struct_name
                << "{*v};\n";
            out << "    return std::nullopt;\n";
            out << "}\n\n";
            return;
        case schema_kind::number:
            out << "    (void)arena;\n";
            out << "    if (auto v = katana::serde::parse_double(cur)) return " << struct_name
                << "{*v};\n";
            out << "    return std::nullopt;\n";
            out << "}\n\n";
            return;
        case schema_kind::boolean:
            out << "    (void)arena;\n";
            out << "    if (auto v = katana::serde::parse_bool(cur)) return " << struct_name
                << "{*v};\n";
            out << "    return std::nullopt;\n";
            out << "}\n\n";
            return;
        case schema_kind::array:
            if (!s.items) {
                out << "    cur.skip_value();\n    return std::nullopt;\n}\n\n";
                return;
            }
            out << "    if (!cur.try_array_start()) return std::nullopt;\n";
            if (use_pmr) {
                // For PMR allocators, construct with arena allocator for the item type
                // Use brace initialization to avoid most vexing parse
                auto item_type_name = schema_identifier(doc, s.items);
                out << "    " << struct_name << " result{arena_allocator<" << item_type_name
                    << ">(arena)};\n";
            } else {
                out << "    " << struct_name << " result;\n";
            }
            out << "    while (!cur.eof()) {\n";
            out << "        cur.skip_ws();\n";
            out << "        if (cur.try_array_end()) break;\n";

            // Optimized: direct parsing for primitives (no intermediate string_view)
            switch (s.items->kind) {
            case schema_kind::number:
                out << "        if (auto v = katana::serde::parse_double(cur)) {\n";
                out << "            result.push_back(*v);\n";
                out << "        } else { cur.skip_value(); }\n";
                break;
            case schema_kind::integer:
                out << "        if (auto v = katana::serde::parse_int64(cur)) {\n";
                out << "            result.push_back(*v);\n";
                out << "        } else { cur.skip_value(); }\n";
                break;
            case schema_kind::boolean:
                out << "        if (auto v = katana::serde::parse_bool(cur)) {\n";
                out << "            result.push_back(*v);\n";
                out << "        } else { cur.skip_value(); }\n";
                break;
            case schema_kind::string:
                if (!s.items->enum_values.empty()) {
                    auto enum_item_name = schema_identifier(doc, s.items);
                    out << "        if (auto v = cur.string()) {\n";
                    out << "            auto enum_val = " << enum_item_name
                        << "_enum_from_string(std::string_view(v->begin(), v->end()));\n";
                    out << "            if (enum_val) result.push_back(*enum_val);\n";
                    out << "        } else { cur.skip_value(); }\n";
                } else {
                    out << "        if (auto v = cur.string()) {\n";
                    if (use_pmr) {
                        out << "            result.emplace_back(v->begin(), v->end(), "
                               "arena_allocator<char>(arena));\n";
                    } else {
                        out << "            result.emplace_back(v->begin(), v->end());\n";
                    }
                    out << "        } else { cur.skip_value(); }\n";
                }
                break;
            default:
                // For complex types (objects, nested arrays), pass cursor directly
                out << "        if (auto parsed = parse_" << schema_identifier(doc, s.items)
                    << "(cur, arena)) result.push_back(*parsed);\n"
                    << "        else cur.skip_value();\n";
                break;
            }

            out << "        cur.try_comma();\n";
            out << "    }\n";
            out << "    return result;\n";
            out << "}\n\n";
            return;
        case schema_kind::object:
            out << "    (void)arena;\n";
            out << "    if (!cur.try_object_start()) {\n";
            out << "        cur.skip_value();\n";
            out << "        return std::nullopt;\n";
            out << "    }\n";
            out << "    while (!cur.eof()) {\n";
            out << "        cur.skip_ws();\n";
            out << "        if (cur.try_object_end()) break;\n";
            out << "        auto key = cur.string();\n";
            out << "        if (!key || !cur.consume(':')) {\n";
            out << "            return std::nullopt;\n";
            out << "        }\n";
            out << "        cur.skip_value();\n";
            out << "        cur.try_comma();\n";
            out << "    }\n";
            out << "    return " << struct_name << "{};\n";
            out << "}\n\n";
            return;
        default:
            out << "    (void)arena;\n";
            out << "    cur.skip_value();\n    return std::nullopt;\n}\n\n";
            return;
        }
    }

    // For empty objects (structures created to break circular aliases)
    out << "    if (!cur.try_object_start()) return std::nullopt;\n\n";
    out << "    " << struct_name << " obj(arena);\n";

    // track required properties
    for (const auto& prop : s.properties) {
        if (prop.required) {
            out << "    bool has_" << prop.name << " = false;\n";
        }
    }
    out << "\n";

    out << "    while (!cur.eof()) {\n";
    out << "        cur.skip_ws();\n";
    out << "        if (cur.try_object_end()) break;\n";
    out << "        auto key = cur.string();\n";
    out << "        if (!key || !cur.consume(':')) break;\n\n";

    const size_t field_count = s.properties.size();

    if (field_count <= 3) {
        // Strategy 1: Linear chain (1-3 fields) — branch predictor handles well
        for (const auto& prop : s.properties) {
            out << "        if (*key == \"" << prop.name << "\") {\n";
            generate_field_parse_body(out, doc, prop, use_pmr, "        ");
            out << "        } else ";
        }
        out << "{\n";
        out << "            cur.skip_value();\n";
        out << "        }\n";
    } else if (field_count <= 15) {
        // Strategy 2: Switch on key size + first char (4-15 fields) — O(1) for most keys
        // Group properties by key length
        std::map<size_t, std::vector<const katana::openapi::property*>> by_length;
        for (const auto& prop : s.properties) {
            by_length[prop.name.size()].push_back(&prop);
        }

        out << "        switch (key->size()) {\n";
        for (const auto& [len, props] : by_length) {
            out << "        case " << len << ":\n";
            bool first = true;
            for (const auto* prop : props) {
                if (first) {
                    out << "            if (*key == \"" << prop->name << "\") {\n";
                    first = false;
                } else {
                    out << "            } else if (*key == \"" << prop->name << "\") {\n";
                }
                generate_field_parse_body(out, doc, *prop, use_pmr, "            ");
            }
            out << "            } else { cur.skip_value(); }\n";
            out << "            break;\n";
        }
        out << "        default:\n";
        out << "            cur.skip_value();\n";
        out << "            break;\n";
        out << "        }\n";
    } else {
        // Strategy 3: FNV-1a hash switch (16+ fields) — O(1) with collision check
        out << "        {\n";
        out << "            constexpr auto fnv1a = [](std::string_view s) noexcept -> uint64_t {\n";
        out << "                uint64_t h = 14695981039346656037ull;\n";
        out << "                for (char c : s) { h ^= static_cast<uint64_t>(c); h *= "
               "1099511628211ull; }\n";
        out << "                return h;\n";
        out << "            };\n";
        out << "            switch (fnv1a(*key)) {\n";
        for (const auto& prop : s.properties) {
            uint64_t hash = fnv1a_hash(prop.name);
            out << "            case " << hash << "ull: // \"" << prop.name << "\"\n";
            out << "                if (*key == \"" << prop.name << "\") {\n";
            generate_field_parse_body(out, doc, prop, use_pmr, "                ");
            out << "                } else { cur.skip_value(); }\n";
            out << "                break;\n";
        }
        out << "            default:\n";
        out << "                cur.skip_value();\n";
        out << "                break;\n";
        out << "            }\n";
        out << "        }\n";
    }
    out << "        cur.try_comma();\n";
    out << "    }\n";

    // required check
    for (const auto& prop : s.properties) {
        if (prop.required) {
            out << "    if (!has_" << prop.name << ") return std::nullopt;\n";
        }
    }

    out << "    return obj;\n";
    out << "}\n\n";
}

// ────────────────────────────────────────────────────────────────────────
// Parser: string_view overload (thin wrapper that creates cursor)
// ────────────────────────────────────────────────────────────────────────

void generate_json_parser_for_schema(std::ostream& out,
                                     const document& doc,
                                     const katana::openapi::schema& s,
                                     bool use_pmr) {
    auto struct_name = schema_identifier(doc, &s);

    // Generate cursor-based overload first (primary implementation)
    generate_json_parser_for_schema_cursor(out, doc, s, use_pmr);

    // Generate string_view overload as thin wrapper
    out << "[[nodiscard]] inline std::optional<" << struct_name << "> parse_" << struct_name
        << "(std::string_view json, monotonic_arena* arena) {\n";
    out << "    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};\n";
    out << "    return parse_" << struct_name << "(cur, arena);\n";
    out << "}\n\n";
}

// ────────────────────────────────────────────────────────────────────────
// Serializer: serialize_into (primary) + serialize (thin wrapper)
// ────────────────────────────────────────────────────────────────────────

void generate_json_serializer_for_schema(std::ostream& out,
                                         const document& doc,
                                         const katana::openapi::schema& s) {
    auto struct_name = schema_identifier(doc, &s);

    // ── serialize_into: appends to existing string ──
    out << "inline void serialize_" << struct_name << "_into(const " << struct_name
        << "& obj, std::string& json) {\n";
    if (s.properties.empty()) {
        using katana::openapi::schema_kind;

        // Check if this is an enum type
        bool is_enum = s.kind == schema_kind::string && !s.enum_values.empty();

        if (is_enum) {
            out << "    auto str = to_string(obj);\n";
            out << "    json.push_back('\"');\n";
            out << "    json.append(str);\n";
            out << "    json.push_back('\"');\n";
            out << "}\n\n";
            // thin wrapper
            out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                << "& obj) {\n";
            out << "    std::string json;\n";
            out << "    auto str = to_string(obj);\n";
            out << "    json.reserve(str.size() + 2);\n";
            out << "    serialize_" << struct_name << "_into(obj, json);\n";
            out << "    return json;\n";
            out << "}\n\n";
            return;
        }

        switch (s.kind) {
        case schema_kind::string:
            if (s.nullable) {
                out << "    if (!obj) { json.append(\"null\"); return; }\n";
                out << "    json.push_back('\"');\n";
                out << "    katana::serde::escape_json_string_into(*obj, json);\n";
                out << "    json.push_back('\"');\n";
            } else {
                out << "    json.push_back('\"');\n";
                out << "    katana::serde::escape_json_string_into(obj, json);\n";
                out << "    json.push_back('\"');\n";
            }
            out << "}\n\n";
            // thin wrapper
            out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                << "& obj) {\n";
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
                out << "    std::string json;\n";
                out << "    json.reserve(obj->size() + 16);\n";
            } else {
                out << "    std::string json;\n";
                out << "    json.reserve(obj.size() + 16);\n";
            }
            out << "    serialize_" << struct_name << "_into(obj, json);\n";
            out << "    return json;\n";
            out << "}\n\n";
            return;
        case schema_kind::integer:
            if (s.nullable) {
                out << "    if (!obj) { json.append(\"null\"); return; }\n";
                out << "    char buf[32];\n";
                out << "    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj);\n";
                out << "    json.append(buf, static_cast<size_t>(ptr - buf));\n";
            } else {
                out << "    char buf[32];\n";
                out << "    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);\n";
                out << "    json.append(buf, static_cast<size_t>(ptr - buf));\n";
            }
            out << "}\n\n";
            // thin wrapper
            out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                << "& obj) {\n";
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
            }
            out << "    std::string json;\n";
            out << "    serialize_" << struct_name << "_into(obj, json);\n";
            out << "    return json;\n";
            out << "}\n\n";
            return;
        case schema_kind::number:
            if (s.nullable) {
                out << "    if (!obj) { json.append(\"null\"); return; }\n";
                out << "    char buf[64];\n";
                out << "    auto res = std::to_chars(buf, buf + sizeof(buf), *obj);\n";
                out << "    if (res.ec == std::errc()) json.append(buf, "
                       "static_cast<size_t>(res.ptr - buf));\n";
            } else {
                out << "    char buf[64];\n";
                out << "    auto res = std::to_chars(buf, buf + sizeof(buf), obj);\n";
                out << "    if (res.ec == std::errc()) json.append(buf, "
                       "static_cast<size_t>(res.ptr - buf));\n";
            }
            out << "}\n\n";
            // thin wrapper
            out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                << "& obj) {\n";
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
            }
            out << "    std::string json;\n";
            out << "    serialize_" << struct_name << "_into(obj, json);\n";
            out << "    return json;\n";
            out << "}\n\n";
            return;
        case schema_kind::boolean:
            if (s.nullable) {
                out << "    if (!obj) { json.append(\"null\"); return; }\n";
                out << "    json.append(*obj ? \"true\" : \"false\");\n";
            } else {
                out << "    json.append(obj ? \"true\" : \"false\");\n";
            }
            out << "}\n\n";
            // thin wrapper
            out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                << "& obj) {\n";
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
                out << "    return *obj ? \"true\" : \"false\";\n";
            } else {
                out << "    return obj ? \"true\" : \"false\";\n";
            }
            out << "}\n\n";
            return;
        case schema_kind::array:
            if (s.nullable) {
                out << "    if (!obj) { json.append(\"null\"); return; }\n";
            }
            out << "    const auto& arr = " << (s.nullable ? "*obj" : "obj") << ";\n";
            out << "    json.push_back('[');\n";
            out << "    for (size_t i = 0; i < arr.size(); ++i) {\n";
            out << "        if (i > 0) json.push_back(',');\n";
            out << "        serialize_" << schema_identifier(doc, s.items)
                << "_into(arr[i], json);\n";
            out << "    }\n";
            out << "    json.push_back(']');\n";
            out << "}\n\n";
            // thin wrapper
            out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                << "& obj) {\n";
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
            }
            out << "    std::string json;\n";
            out << "    json.reserve(" << (s.nullable ? "*obj" : "obj") << ".size() * 16 + 2);\n";
            out << "    serialize_" << struct_name << "_into(obj, json);\n";
            out << "    return json;\n";
            out << "}\n\n";
            return;
        case schema_kind::object:
            out << "    (void)obj;\n";
            if (s.nullable) {
                out << "    if (!obj) { json.append(\"null\"); return; }\n";
            }
            out << "    json.append(\"{}\");\n";
            out << "}\n\n";
            // thin wrapper
            out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                << "& obj) {\n";
            out << "    (void)obj;\n";
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
            }
            out << "    return std::string(\"{}\");\n";
            out << "}\n\n";
            return;
        default:
            out << "    (void)obj;\n";
            out << "}\n\n";
            // thin wrapper
            out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                << "& obj) {\n";
            out << "    (void)obj;\n";
            out << "    return {};\n";
            out << "}\n\n";
            return;
        }
    }

    // Object with properties - serialize_into
    size_t reserve_est = compute_reserve_estimate(doc, s);
    out << "    json.push_back('{');\n";

    bool first_field = true;
    for (const auto& prop : s.properties) {
        if (first_field) {
            out << "    json.append(\"\\\"" << prop.name << "\\\":\");\n";
            first_field = false;
        } else {
            out << "    json.append(\",\\\"" << prop.name << "\\\":\");\n";
        }

        if (prop.type) {
            using katana::openapi::schema_kind;
            bool is_enum =
                prop.type->kind == schema_kind::string && !prop.type->enum_values.empty();
            auto nested_name = schema_identifier(doc, prop.type);
            bool is_optional = prop.type->nullable;

            if (is_enum && !nested_name.empty()) {
                out << "    json.push_back('\"');\n";
                out << "    json.append(to_string(obj." << prop.name << "));\n";
                out << "    json.push_back('\"');\n";
            } else {
                switch (prop.type->kind) {
                case schema_kind::string:
                    if (is_optional) {
                        out << "    if (obj." << prop.name << ") {\n";
                        out << "        json.push_back('\"');\n";
                        out << "        katana::serde::escape_json_string_into(*obj." << prop.name
                            << ", json);\n";
                        out << "        json.push_back('\"');\n";
                        out << "    } else {\n";
                        out << "        json.append(\"null\");\n";
                        out << "    }\n";
                    } else {
                        out << "    json.push_back('\"');\n";
                        out << "    katana::serde::escape_json_string_into(obj." << prop.name
                            << ", json);\n";
                        out << "    json.push_back('\"');\n";
                    }
                    break;
                case schema_kind::integer:
                    if (is_optional) {
                        out << "    {\n";
                        out << "        if (!obj." << prop.name << ") {\n";
                        out << "            json.append(\"null\");\n";
                        out << "        } else {\n";
                        out << "            char buf[32];\n";
                        out << "            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), "
                               "*obj."
                            << prop.name << ");\n";
                        out << "            json.append(buf, static_cast<size_t>(ptr - buf));\n";
                        out << "        }\n";
                        out << "    }\n";
                    } else {
                        out << "    {\n";
                        out << "        char buf[32];\n";
                        out << "        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj."
                            << prop.name << ");\n";
                        out << "        json.append(buf, static_cast<size_t>(ptr - buf));\n";
                        out << "    }\n";
                    }
                    break;
                case schema_kind::number:
                    out << "    {\n";
                    if (is_optional) {
                        out << "        if (!obj." << prop.name << ") {\n";
                        out << "            json.append(\"null\");\n";
                        out << "        } else {\n";
                    }
                    out << "        char buf[64];\n";
                    out << "        auto res = std::to_chars(buf, buf + sizeof(buf), "
                        << (is_optional ? "*obj." + prop.name : "obj." + prop.name) << ");\n";
                    out << "        if (res.ec == std::errc()) json.append(buf, "
                           "static_cast<size_t>(res.ptr - buf));\n";
                    if (is_optional) {
                        out << "        }\n";
                    }
                    out << "    }\n";
                    break;
                case schema_kind::boolean:
                    if (is_optional) {
                        out << "    if (!obj." << prop.name << ") {\n";
                        out << "        json.append(\"null\");\n";
                        out << "    } else {\n";
                        out << "        json.append(*obj." << prop.name
                            << " ? \"true\" : \"false\");\n";
                        out << "    }\n";
                    } else {
                        out << "    json.append(obj." << prop.name << " ? \"true\" : \"false\");\n";
                    }
                    break;
                case schema_kind::array:
                    if (is_optional) {
                        out << "    if (!obj." << prop.name << ") {\n";
                        out << "        json.append(\"null\");\n";
                        out << "        break;\n";
                        out << "    }\n";
                    }
                    out << "    json.push_back('[');\n";
                    out << "    for (size_t i = 0; i < "
                        << (is_optional ? "obj." + prop.name + "->size()"
                                        : "obj." + prop.name + ".size()")
                        << "; ++i) {\n";
                    out << "        if (i > 0) json.push_back(',');\n";
                    if (prop.type->items) {
                        switch (prop.type->items->kind) {
                        case schema_kind::string:
                            if (!prop.type->items->enum_values.empty()) {
                                out << "        json.push_back('\"');\n";
                                out << "        json.append(to_string("
                                    << (is_optional ? "(*obj." + prop.name + ")[i]"
                                                    : "obj." + prop.name + "[i]")
                                    << "));\n";
                                out << "        json.push_back('\"');\n";
                            } else {
                                out << "        json.push_back('\"');\n";
                                out << "        katana::serde::escape_json_string_into("
                                    << (is_optional ? "(*obj." + prop.name + ")[i]"
                                                    : "obj." + prop.name + "[i]")
                                    << ", json);\n";
                                out << "        json.push_back('\"');\n";
                            }
                            break;
                        case schema_kind::integer:
                            out << "        {\n";
                            out << "            char buf[32];\n";
                            out << "            auto [ptr, ec] = std::to_chars(buf, buf + "
                                   "sizeof(buf), "
                                << (is_optional ? "(*obj." + prop.name + ")[i]"
                                                : "obj." + prop.name + "[i]")
                                << ");\n";
                            out << "            json.append(buf, static_cast<size_t>(ptr - "
                                   "buf));\n";
                            out << "        }\n";
                            break;
                        case schema_kind::number:
                            out << "        {\n";
                            out << "            char buf[64];\n";
                            out << "            auto res = std::to_chars(buf, buf + sizeof(buf), "
                                << (is_optional ? "(*obj." + prop.name + ")[i]"
                                                : "obj." + prop.name + "[i]")
                                << ");\n";
                            out << "            if (res.ec == std::errc()) json.append(buf, "
                                   "static_cast<size_t>(res.ptr - buf));\n";
                            out << "        }\n";
                            break;
                        case schema_kind::boolean:
                            out << "        json.append("
                                << (is_optional ? "(*obj." + prop.name + ")[i]"
                                                : "obj." + prop.name + "[i]")
                                << " ? \"true\" : \"false\");\n";
                            break;
                        case schema_kind::object: {
                            auto nested_array_name = schema_identifier(doc, prop.type->items);
                            out << "        serialize_" << nested_array_name << "_into("
                                << (is_optional ? "(*obj." + prop.name + ")[i]"
                                                : "obj." + prop.name + "[i]")
                                << ", json);\n";
                            break;
                        }
                        case schema_kind::array:
                        case schema_kind::null_type:
                            out << "        json.append(\"null\");\n";
                            break;
                        default:
                            out << "        json.append(\"null\");\n";
                            break;
                        }
                    } else {
                        out << "        json.append(\"null\");\n";
                    }
                    out << "    }\n";
                    out << "    json.push_back(']');\n";
                    break;
                case schema_kind::object:
                    out << "    serialize_" << nested_name << "_into(obj." << prop.name
                        << ", json);\n";
                    break;
                case schema_kind::null_type:
                    out << "    json.append(\"null\");\n";
                    break;
                default:
                    out << "    json.append(\"null\");\n";
                    break;
                }
            }
        }
    }

    out << "    json.push_back('}');\n";
    out << "}\n\n";

    // ── serialize: thin wrapper ──
    out << "inline std::string serialize_" << struct_name << "(const " << struct_name
        << "& obj) {\n";
    out << "    std::string json;\n";
    out << "    json.reserve(" << reserve_est << ");\n";
    out << "    serialize_" << struct_name << "_into(obj, json);\n";
    out << "    return json;\n";
    out << "}\n\n";
}

// ────────────────────────────────────────────────────────────────────────
// Array parser: cursor-based overload + string_view wrapper
// ────────────────────────────────────────────────────────────────────────

void generate_json_array_parser(std::ostream& out,
                                const document& doc,
                                const katana::openapi::schema& s,
                                [[maybe_unused]] bool use_pmr) {
    auto struct_name = schema_identifier(doc, &s);

    // Cursor-based overload (primary)
    out << "[[nodiscard]] inline std::optional<std::vector<" << struct_name << ">> parse_"
        << struct_name << "_array(katana::serde::json_cursor& cur, monotonic_arena* arena) {\n";
    out << "    if (!cur.try_array_start()) return std::nullopt;\n\n";
    out << "    std::vector<" << struct_name << "> result;\n";
    out << "    while (!cur.eof()) {\n";
    out << "        cur.skip_ws();\n";
    out << "        if (cur.try_array_end()) break;\n";
    out << "        \n";
    out << "        // Parse object at current cursor position\n";
    out << "        auto obj = parse_" << struct_name << "(cur, arena);\n";
    out << "        if (!obj) return std::nullopt;\n";
    out << "        result.push_back(std::move(*obj));\n";
    out << "        \n";
    out << "        cur.try_comma();\n";
    out << "    }\n";
    out << "    return result;\n";
    out << "}\n\n";

    // String_view overload (thin wrapper)
    out << "[[nodiscard]] inline std::optional<std::vector<" << struct_name << ">> parse_"
        << struct_name << "_array(std::string_view json, monotonic_arena* arena) {\n";
    out << "    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};\n";
    out << "    return parse_" << struct_name << "_array(cur, arena);\n";
    out << "}\n\n";
}

// ────────────────────────────────────────────────────────────────────────
// Array serializer: serialize_into + thin wrapper
// ────────────────────────────────────────────────────────────────────────

void generate_json_array_serializer(std::ostream& out,
                                    const document& doc,
                                    const katana::openapi::schema& s,
                                    bool use_pmr) {
    auto struct_name = schema_identifier(doc, &s);

    // serialize_into for std::vector
    out << "inline void serialize_" << struct_name << "_array_into(const std::vector<"
        << struct_name << ">& arr, std::string& json) {\n";
    out << "    json.push_back('[');\n";
    out << "    for (size_t i = 0; i < arr.size(); ++i) {\n";
    out << "        if (i > 0) json.push_back(',');\n";
    out << "        serialize_" << struct_name << "_into(arr[i], json);\n";
    out << "    }\n";
    out << "    json.push_back(']');\n";
    out << "}\n\n";

    // thin wrapper for std::vector
    out << "inline std::string serialize_" << struct_name << "_array(const std::vector<"
        << struct_name << ">& arr) {\n";
    out << "    std::string json;\n";
    out << "    json.reserve(arr.size() * 32 + 2);\n";
    out << "    serialize_" << struct_name << "_array_into(arr, json);\n";
    out << "    return json;\n";
    out << "}\n\n";

    if (use_pmr) {
        // serialize_into for arena_vector
        out << "inline void serialize_" << struct_name << "_array_into(const arena_vector<"
            << struct_name << ">& arr, std::string& json) {\n";
        out << "    json.push_back('[');\n";
        out << "    for (size_t i = 0; i < arr.size(); ++i) {\n";
        out << "        if (i > 0) json.push_back(',');\n";
        out << "        serialize_" << struct_name << "_into(arr[i], json);\n";
        out << "    }\n";
        out << "    json.push_back(']');\n";
        out << "}\n\n";

        // thin wrapper for arena_vector
        out << "inline std::string serialize_" << struct_name << "_array(const arena_vector<"
            << struct_name << ">& arr) {\n";
        out << "    std::string json;\n";
        out << "    json.reserve(arr.size() * 32 + 2);\n";
        out << "    serialize_" << struct_name << "_array_into(arr, json);\n";
        out << "    return json;\n";
        out << "}\n\n";
    }
}

} // namespace

// Check if schema should be skipped (simple type alias or empty object artifact)
bool should_skip_schema(const katana::openapi::schema& s) {
    using katana::openapi::schema_kind;

    if (!s.properties.empty()) {
        return false; // Has properties, it's a real object - don't skip
    }

    // Skip only truly unnamed empty object artifacts.
    // Named empty objects are emitted as monostate aliases and still need
    // parse/serialize helpers when referenced from other generated types.
    if (s.kind == schema_kind::object && s.properties.empty() && s.name.empty()) {
        return true;
    }

    return false;
}

std::string generate_json_parsers(const document& doc, bool use_pmr) {
    std::ostringstream out;
    out << "// Auto-generated JSON parsers and serializers from OpenAPI specification\n";
    out << "//\n";
    out << "// This file contains:\n";
    out << "//   - parse_<Type>() functions: JSON string → C++ struct\n";
    out << "//   - serialize_<Type>() functions: C++ struct → JSON string\n";
    out << "//\n";
    out << "// Features:\n";
    out << "//   - Zero-copy parsing using arena allocators\n";
    out << "//   - Streaming JSON generation without intermediate buffers\n";
    out << "//   - Type-safe enum conversion\n";
    out << "//   - Automatic null handling for optional fields\n";
    out << "//\n";
    out << "// All parse functions return std::optional<T>:\n";
    out << "//   - std::nullopt on parse error (invalid JSON, wrong type, etc.)\n";
    out << "//   - Parsed object on success\n";
    out << "//\n";
    out << "#pragma once\n\n";
    out << "#include \"katana/core/arena.hpp\"\n";
    out << "#include \"katana/core/serde.hpp\"\n";
    out << "#include <optional>\n";
    out << "#include <string>\n";
    out << "#include <charconv>\n";
    out << "#include <vector>\n\n";
    out << "using katana::monotonic_arena;\n\n";

    // ============================================================
    // Forward Declarations
    // ============================================================
    out << "// ============================================================\n";
    out << "// Forward Declarations\n";
    out << "// ============================================================\n\n";

    // Forward declarations: parse (string_view overload)
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "[[nodiscard]] inline std::optional<" << name << "> parse_" << name
                << "(std::string_view json, monotonic_arena* arena);\n";
        }
    }
    out << "\n";
    // Forward declarations: parse (cursor overload)
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "[[nodiscard]] inline std::optional<" << name << "> parse_" << name
                << "(katana::serde::json_cursor& cur, monotonic_arena* arena);\n";
        }
    }
    out << "\n";
    // Forward declarations: serialize_into
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline void serialize_" << name << "_into(const " << name
                << "& obj, std::string& out);\n";
        }
    }
    out << "\n";
    // Forward declarations: serialize
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline std::string serialize_" << name << "(const " << name << "& obj);\n";
        }
    }
    out << "\n";
    // Forward declarations: parse_array (string_view overload)
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "[[nodiscard]] inline std::optional<std::vector<" << name << ">> parse_" << name
                << "_array(std::string_view json, monotonic_arena* arena);\n";
        }
    }
    out << "\n";
    // Forward declarations: parse_array (cursor overload)
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "[[nodiscard]] inline std::optional<std::vector<" << name << ">> parse_" << name
                << "_array(katana::serde::json_cursor& cur, monotonic_arena* arena);\n";
        }
    }
    out << "\n";
    // Forward declarations: serialize_array_into
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline void serialize_" << name << "_array_into(const std::vector<" << name
                << ">& arr, std::string& out);\n";
            if (use_pmr) {
                out << "inline void serialize_" << name << "_array_into(const arena_vector<" << name
                    << ">& arr, std::string& out);\n";
            }
        }
    }
    out << "\n";
    // Forward declarations: serialize_array
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline std::string serialize_" << name << "_array(const std::vector<" << name
                << ">& arr);\n";
            if (use_pmr) {
                out << "inline std::string serialize_" << name << "_array(const arena_vector<"
                    << name << ">& arr);\n";
            }
        }
    }
    out << "\n";

    // ============================================================
    // JSON Parse Functions
    // ============================================================
    out << "// ============================================================\n";
    out << "// JSON Parse Functions\n";
    out << "// ============================================================\n\n";

    // Only generate parsers for non-trivial schemas (objects with properties or arrays)
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            generate_json_parser_for_schema(out, doc, schema, use_pmr);
        }
    }

    // ============================================================
    // JSON Serialize Functions
    // ============================================================
    out << "// ============================================================\n";
    out << "// JSON Serialize Functions\n";
    out << "// ============================================================\n\n";

    // Only generate serializers for non-trivial schemas
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            generate_json_serializer_for_schema(out, doc, schema);
        }
    }

    // ============================================================
    // Array Parse Functions
    // ============================================================
    out << "// ============================================================\n";
    out << "// Array Parse Functions\n";
    out << "// ============================================================\n\n";

    // Generate array parsers only for non-trivial schemas
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            generate_json_array_parser(out, doc, schema, use_pmr);
        }
    }

    // ============================================================
    // Array Serialize Functions
    // ============================================================
    out << "// ============================================================\n";
    out << "// Array Serialize Functions\n";
    out << "// ============================================================\n\n";

    // Generate array serializers only for non-trivial schemas
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            generate_json_array_serializer(out, doc, schema, use_pmr);
        }
    }

    return out.str();
}

} // namespace katana_gen
