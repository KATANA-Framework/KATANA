#include "generator.hpp"

#include <map>
#include <sstream>
#include <string>
#include <unordered_set>

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

// A schema synthesized from a parent property (e.g. an inline nested object or
// array field). Such schemas are only ever parsed *in place* from a cursor by the
// parent's parser — user code never feeds them a raw JSON string — so they do not
// need the string_view entry-point overload. Top-level component schemas and
// operation request/response/parameter bodies have no field context and keep it.
bool is_field_wrapper_schema(const katana::openapi::schema& s) {
    return !s.parent_context.empty() && !s.field_context.empty();
}

constexpr size_t DEFAULT_INLINE_ARENA_ARRAY_CAPACITY = 8;

bool supports_inline_arena_array(const katana::openapi::schema* s) {
    if (!s || s->kind != katana::openapi::schema_kind::array || !s->items) {
        return false;
    }

    using katana::openapi::schema_kind;
    switch (s->items->kind) {
    case schema_kind::number:
    case schema_kind::integer:
    case schema_kind::boolean:
        return true;
    default:
        return false;
    }
}

size_t inline_arena_array_capacity(const katana::openapi::schema* s) {
    if (!supports_inline_arena_array(s)) {
        return 0;
    }
    if (s->max_items) {
        return std::min(static_cast<size_t>(*s->max_items), DEFAULT_INLINE_ARENA_ARRAY_CAPACITY);
    }
    return DEFAULT_INLINE_ARENA_ARRAY_CAPACITY;
}

std::string member_expr(std::string_view object_expr, std::string_view property_name) {
    return std::string(object_expr) + "." + property_member_identifier(property_name);
}

// Generate the field parsing body for a single property (reusable across strategies)
void generate_field_parse_body(std::ostream& out,
                               const document& doc,
                               const katana::openapi::property& prop,
                               bool use_pmr,
                               const std::string& indent) {
    const auto member_name = property_member_identifier(prop.name);
    const bool is_optional = is_optional_property(prop);
    using katana::openapi::schema_kind;
    // Mark the required field as present only AFTER a value of the right type is
    // successfully parsed — so null / wrong-typed values don't satisfy the presence check.
    // For typeless / free-form fields (no value to type-check) mere key presence counts.
    const std::string mark_present =
        prop.required ? (indent + "        has_" + member_name + " = true;\n") : std::string();
    if (!prop.type) {
        out << indent << "    cur.skip_value();\n";
        if (prop.required) {
            out << indent << "    has_" << member_name << " = true;\n";
        }
        return;
    }
    bool is_enum = prop.type->kind == schema_kind::string && !prop.type->enum_values.empty();
    auto nested_name = schema_identifier(doc, prop.type);
    if (is_enum && !nested_name.empty()) {
        out << indent << "    if (auto v = cur.string()) {\n";
        out << indent << "        std::string ev_scratch_;\n";
        out << indent << "        auto enum_val = " << nested_name
            << "_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));\n";
        out << indent << "        if (enum_val) { obj." << member_name << " = *enum_val;\n";
        out << mark_present;
        out << indent << "        }\n";
        out << indent << "    } else { cur.skip_value(); }\n";
    } else {
        switch (prop.type->kind) {
        case schema_kind::string:
            out << indent << "    if (auto v = cur.string()) {\n";
            if (use_pmr) {
                out << indent << "        obj." << member_name
                    << " = katana::serde::decode_json_string<arena_string<>>(*v, "
                       "arena_allocator<char>(arena));\n";
            } else {
                out << indent << "        obj." << member_name
                    << " = katana::serde::decode_json_string(*v);\n";
            }
            out << mark_present;
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::integer:
            out << indent << "    if (auto v = katana::serde::parse_int64(cur)) {\n";
            out << indent << "        obj." << member_name << " = *v;\n";
            out << mark_present;
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::number:
            out << indent << "    if (auto v = katana::serde::parse_double(cur)) {\n";
            out << indent << "        obj." << member_name << " = *v;\n";
            out << mark_present;
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::boolean:
            out << indent << "    if (auto v = katana::serde::parse_bool(cur)) {\n";
            out << indent << "        obj." << member_name << " = *v;\n";
            out << mark_present;
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::array:
            out << indent << "    if (cur.try_array_start()) {\n";
            if (is_optional) {
                if (use_pmr) {
                    out << indent << "        obj." << member_name << ".emplace(arena);\n";
                } else {
                    out << indent << "        obj." << member_name << ".emplace();\n";
                }
            }
            out << indent << "        while (!cur.eof()) {\n";
            out << indent << "            cur.skip_ws();\n";
            out << indent << "            if (cur.try_array_end()) break;\n";
            if (prop.type->items) {
                auto* item = prop.type->items;
                const std::string array_expr =
                    is_optional ? "(*obj." + member_name + ")" : "obj." + member_name;
                switch (item->kind) {
                case schema_kind::string:
                    if (!item->enum_values.empty()) {
                        auto enum_item_name = schema_identifier(doc, item);
                        out << indent << "            if (auto v = cur.string()) {\n";
                        out << indent << "                std::string ev_scratch_;\n";
                        out << indent << "                auto enum_val = " << enum_item_name
                            << "_enum_from_string(katana::serde::decode_json_view(*v, "
                               "ev_scratch_));\n";
                        out << indent << "                if (enum_val) " << array_expr
                            << ".push_back(*enum_val);\n";
                        out << indent << "            } else { cur.skip_value(); }\n";
                    } else {
                        out << indent << "            if (auto v = cur.string()) {\n";
                        if (use_pmr) {
                            out << indent << "                " << array_expr
                                << ".push_back(katana::serde::decode_json_string<arena_string<>>(*v, "
                                   "arena_allocator<char>(arena)));\n";
                        } else {
                            out << indent << "                " << array_expr
                                << ".push_back(katana::serde::decode_json_string(*v));\n";
                        }
                        out << indent << "            } else { cur.skip_value(); }\n";
                    }
                    break;
                case schema_kind::integer:
                    out << indent
                        << "            if (auto v = "
                           "katana::serde::parse_int64(cur)) {\n";
                    out << indent << "                " << array_expr << ".push_back(*v);\n";
                    out << indent << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::number:
                    out << indent
                        << "            if (auto v = "
                           "katana::serde::parse_double(cur)) {\n";
                    out << indent << "                " << array_expr << ".push_back(*v);\n";
                    out << indent << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::boolean:
                    out << indent
                        << "            if (auto v = "
                           "katana::serde::parse_bool(cur)) {\n";
                    out << indent << "                " << array_expr << ".push_back(*v);\n";
                    out << indent << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::object: {
                    auto nested_array_name = schema_identifier(doc, item);
                    if (!nested_array_name.empty()) {
                        out << indent << "            if (auto nested = parse_" << nested_array_name
                            << "(cur, arena)) { " << array_expr
                            << ".push_back(std::move(*nested)); }\n";
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
            out << mark_present;
            out << indent << "    } else { cur.skip_value(); }\n";
            break;
        case schema_kind::object: {
            auto nested_obj_name = schema_identifier(doc, prop.type);
            if (!nested_obj_name.empty()) {
                out << indent << "    if (auto nested = parse_" << nested_obj_name
                    << "(cur, arena)) {\n";
                out << indent << "        obj." << member_name << " = std::move(*nested);\n";
                out << mark_present;
                out << indent << "    } else { cur.skip_value(); }\n";
            } else {
                out << indent << "    cur.skip_value();\n";
                out << mark_present;
            }
            break;
        }
        default:
            out << indent << "    cur.skip_value();\n";
            out << mark_present;
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

size_t
compute_value_estimate(const document& doc, const katana::openapi::schema* s, int depth = 0) {
    using katana::openapi::schema_kind;
    if (!s) {
        return 32;
    }
    if (depth > 2) {
        return 64;
    }

    switch (s->kind) {
    case schema_kind::boolean:
        return 5;
    case schema_kind::integer:
        return 20;
    case schema_kind::number:
        return 25;
    case schema_kind::string:
        return 32;
    case schema_kind::array:
        return 2 + compute_value_estimate(doc, s->items, depth + 1) * 4;
    case schema_kind::object:
        return compute_reserve_estimate(doc, *s);
    case schema_kind::null_type:
        return 4;
    default:
        return 32;
    }
}

void emit_runtime_reserve_adjustment(std::ostream& out,
                                     const document& doc,
                                     const katana::openapi::property& prop,
                                     std::string_view object_expr,
                                     std::string_view reserve_var) {
    using katana::openapi::schema_kind;
    if (!prop.type) {
        return;
    }

    const std::string field_expr = member_expr(object_expr, prop.name);
    const bool is_optional = is_optional_property(prop);

    auto emit_optional = [&](const std::string& add_expr) {
        if (is_optional) {
            out << "    if (" << field_expr << ") " << reserve_var << " += " << add_expr << ";\n";
        } else {
            out << "    " << reserve_var << " += " << add_expr << ";\n";
        }
    };

    switch (prop.type->kind) {
    case schema_kind::string:
        if (!prop.type->enum_values.empty()) {
            break;
        }
        emit_optional(is_optional ? field_expr + "->size()" : field_expr + ".size()");
        break;
    case schema_kind::array: {
        const size_t item_estimate = compute_value_estimate(doc, prop.type->items, 1);
        emit_optional((is_optional ? field_expr + "->size()" : field_expr + ".size()") + " * " +
                      std::to_string(item_estimate));
        break;
    }
    case schema_kind::object:
        if (!is_optional) {
            out << "    " << reserve_var << " += " << compute_reserve_estimate(doc, *prop.type)
                << ";\n";
        }
        break;
    default:
        break;
    }
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
            out << "        std::string ev_scratch_;\n";
            out << "        return " << struct_name
                << "_enum_from_string(katana::serde::decode_json_view(*v, ev_scratch_));\n";
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
                    << "{katana::serde::decode_json_string<arena_string<>>(*v, "
                       "arena_allocator<char>(arena))};\n";
            } else {
                out << "        return " << struct_name
                    << "{katana::serde::decode_json_string(*v)};\n";
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
            if (!(use_pmr && inline_arena_array_capacity(&s) > 0)) {
                out << "    size_t reserve_hint = 0;\n";
                out << "    for (const char* p = cur.ptr; p < cur.end; ++p) {\n";
                out << "        if (*p == ',') ++reserve_hint;\n";
                out << "    }\n";
                out << "    if (cur.ptr < cur.end && *cur.ptr != ']') ++reserve_hint;\n";
                out << "    result.reserve(reserve_hint);\n";
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
                        out << "            result.push_back(katana::serde::decode_json_string<"
                               "arena_string<>>(*v, arena_allocator<char>(arena)));\n";
                    } else {
                        out << "            result.push_back(katana::serde::decode_json_string(*v));\n";
                    }
                    out << "        } else { cur.skip_value(); }\n";
                }
                break;
            default:
                // For complex types (objects, nested arrays), pass cursor directly
                out << "        if (auto parsed = parse_" << schema_identifier(doc, s.items)
                    << "(cur, arena)) result.push_back(std::move(*parsed));\n"
                    << "        else cur.skip_value();\n";
                break;
            }

            out << "        cur.try_comma();\n";
            out << "    }\n";
            out << "    return result;\n";
            out << "}\n\n";
            return;
        case schema_kind::object:
            if (is_free_form_object(&s)) {
                // Free-form object: capture the whole value verbatim as raw JSON text.
                out << "    cur.skip_ws();\n";
                out << "    const char* ff_start_ = cur.ptr;\n";
                out << "    cur.skip_value();\n";
                if (use_pmr) {
                    out << "    return " << struct_name
                        << "(ff_start_, cur.ptr, arena_allocator<char>(arena));\n";
                } else {
                    out << "    (void)arena;\n";
                    out << "    return " << struct_name << "(ff_start_, cur.ptr);\n";
                }
                out << "}\n\n";
                return;
            }
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
            out << "    bool has_" << property_member_identifier(prop.name) << " = false;\n";
        }
    }
    out << "\n";

    out << "    bool obj_closed_ = false;\n";
    out << "    while (!cur.eof()) {\n";
    out << "        cur.skip_ws();\n";
    out << "        if (cur.try_object_end()) { obj_closed_ = true; break; }\n";
    out << "        auto key = cur.string();\n";
    out << "        if (!key || !cur.consume(':')) break;\n\n";

    const size_t field_count = s.properties.size();

    if (field_count <= 3) {
        // Strategy 1: Linear chain (1-3 fields) — branch predictor handles well
        for (const auto& prop : s.properties) {
            out << "        if (*key == \"" << escape_cpp_string(prop.name) << "\") {\n";
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
                    out << "            if (*key == \"" << escape_cpp_string(prop->name)
                        << "\") {\n";
                    first = false;
                } else {
                    out << "            } else if (*key == \"" << escape_cpp_string(prop->name)
                        << "\") {\n";
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
            out << "            case " << hash << "ull: // \"" << escape_cpp_string(prop.name)
                << "\"\n";
            out << "                if (*key == \"" << escape_cpp_string(prop.name) << "\") {\n";
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
    // Reject an object that ran off the end without its closing '}' (unterminated).
    out << "    if (!obj_closed_) return std::nullopt;\n";

    // required check
    for (const auto& prop : s.properties) {
        if (prop.required) {
            out << "    if (!has_" << property_member_identifier(prop.name)
                << ") return std::nullopt;\n";
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

    out << "// parse " << schema_banner(doc, s) << "\n";

    // Generate cursor-based overload first (primary implementation)
    generate_json_parser_for_schema_cursor(out, doc, s, use_pmr);

    // Inline field wrappers are only parsed in place via the cursor overload above;
    // they never receive a raw JSON string, so the string_view entry point is dead.
    if (is_field_wrapper_schema(s)) {
        return;
    }

    if (s.kind == katana::openapi::schema_kind::array) {
        out << "[[nodiscard]] inline std::optional<" << struct_name << "> parse_" << struct_name
            << "(std::string_view json, monotonic_arena* arena) {\n";
        out << "    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};\n";
        out << "    if (!cur.try_array_start()) return std::nullopt;\n";
        if (use_pmr) {
            auto item_type_name = schema_identifier(doc, s.items);
            out << "    " << struct_name << " result{arena_allocator<" << item_type_name
                << ">(arena)};\n";
        } else {
            out << "    " << struct_name << " result;\n";
        }
        if (!(use_pmr && inline_arena_array_capacity(&s) > 0)) {
            out << "    size_t reserve_hint = 0;\n";
            out << "    for (char ch : json) {\n";
            out << "        if (ch == ',') ++reserve_hint;\n";
            out << "    }\n";
            out << "    if (!json.empty() && json != \"[]\") ++reserve_hint;\n";
            out << "    result.reserve(reserve_hint);\n";
        }
        out << "    while (!cur.eof()) {\n";
        out << "        cur.skip_ws();\n";
        out << "        if (cur.try_array_end()) break;\n";

        auto item_kind = s.items ? s.items->kind : katana::openapi::schema_kind::object;
        switch (item_kind) {
        case katana::openapi::schema_kind::number:
            out << "        if (auto v = katana::serde::parse_double(cur)) {\n";
            out << "            result.push_back(*v);\n";
            out << "        } else { cur.skip_value(); }\n";
            break;
        case katana::openapi::schema_kind::integer:
            out << "        if (auto v = katana::serde::parse_int64(cur)) {\n";
            out << "            result.push_back(*v);\n";
            out << "        } else { cur.skip_value(); }\n";
            break;
        case katana::openapi::schema_kind::boolean:
            out << "        if (auto v = katana::serde::parse_bool(cur)) {\n";
            out << "            result.push_back(*v);\n";
            out << "        } else { cur.skip_value(); }\n";
            break;
        case katana::openapi::schema_kind::string:
            if (s.items && !s.items->enum_values.empty()) {
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
            out << "        if (auto parsed = parse_" << schema_identifier(doc, s.items)
                << "(cur, arena)) result.push_back(std::move(*parsed));\n"
                << "        else cur.skip_value();\n";
            break;
        }

        out << "        cur.try_comma();\n";
        out << "    }\n";
        out << "    cur.skip_ws();\n";
        out << "    if (!cur.eof()) return std::nullopt;\n";
        out << "    return result;\n";
        out << "}\n\n";
    } else {
        // Generate string_view overload as thin wrapper. Rejects trailing garbage /
        // unterminated input: after the value, only whitespace may remain before EOF.
        out << "[[nodiscard]] inline std::optional<" << struct_name << "> parse_" << struct_name
            << "(std::string_view json, monotonic_arena* arena) {\n";
        out << "    katana::serde::json_cursor cur{json.data(), json.data() + json.size()};\n";
        out << "    auto result = parse_" << struct_name << "(cur, arena);\n";
        out << "    if (!result) return std::nullopt;\n";
        out << "    cur.skip_ws();\n";
        out << "    if (!cur.eof()) return std::nullopt;\n";
        out << "    return result;\n";
        out << "}\n\n";
    }
}

// ────────────────────────────────────────────────────────────────────────
// Serializer: serialize_into (primary) + serialize (thin wrapper)
// ────────────────────────────────────────────────────────────────────────

void generate_json_serializer_for_schema(std::ostream& out,
                                         const document& doc,
                                         const katana::openapi::schema& s) {
    auto struct_name = schema_identifier(doc, &s);

    out << "// serialize " << schema_banner(doc, s) << "\n";

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
                out << "    katana::serde::append_json_double(json, *obj);\n";
            } else {
                out << "    katana::serde::append_json_double(json, obj);\n";
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
            if (is_free_form_object(&s)) {
                // Free-form object: the value already holds raw JSON; emit it verbatim.
                out << "    if (obj.empty()) { json.append(\"null\"); }\n";
                out << "    else { json.append(obj.data(), obj.size()); }\n";
                out << "}\n\n";
                out << "inline std::string serialize_" << struct_name << "(const " << struct_name
                    << "& obj) {\n";
                out << "    std::string json;\n";
                out << "    json.reserve(obj.size() + 4);\n";
                out << "    serialize_" << struct_name << "_into(obj, json);\n";
                out << "    return json;\n";
                out << "}\n\n";
                return;
            }
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

    out << "    bool first_field_ = true;\n";
    for (const auto& prop : s.properties) {
        const auto member_name = property_member_identifier(prop.name);
        const auto prop_key = escape_cpp_string(prop.name);
        // Optional (non-required) fields are OMITTED entirely when absent — `null` is only a
        // valid value for a nullable field, so a missing non-required field must not appear.
        // Required fields are always emitted (a required+nullable one emits null when empty).
        // Runtime comma tracking is needed because any field may be skipped.
        const bool omittable = !prop.required;
        if (omittable) {
            out << "    if (obj." << member_name << ") {\n";
        }
        out << "    if (!first_field_) json.push_back(',');\n";
        out << "    first_field_ = false;\n";
        out << "    json.append(\"\\\"" << prop_key << "\\\":\");\n";

        if (prop.type) {
            using katana::openapi::schema_kind;
            bool is_enum =
                prop.type->kind == schema_kind::string && !prop.type->enum_values.empty();
            auto nested_name = schema_identifier(doc, prop.type);
            bool is_optional = is_optional_property(prop);

            if (is_enum && !nested_name.empty()) {
                if (is_optional) {
                    out << "    if (obj." << member_name << ") {\n";
                    out << "        json.push_back('\"');\n";
                    out << "        json.append(to_string(*obj." << member_name << "));\n";
                    out << "        json.push_back('\"');\n";
                    out << "    } else {\n";
                    out << "        json.append(\"null\");\n";
                    out << "    }\n";
                } else {
                    out << "    json.push_back('\"');\n";
                    out << "    json.append(to_string(obj." << member_name << "));\n";
                    out << "    json.push_back('\"');\n";
                }
            } else {
                switch (prop.type->kind) {
                case schema_kind::string:
                    if (is_optional) {
                        out << "    if (obj." << member_name << ") {\n";
                        out << "        json.push_back('\"');\n";
                        out << "        katana::serde::escape_json_string_into(*obj." << member_name
                            << ", json);\n";
                        out << "        json.push_back('\"');\n";
                        out << "    } else {\n";
                        out << "        json.append(\"null\");\n";
                        out << "    }\n";
                    } else {
                        out << "    json.push_back('\"');\n";
                        out << "    katana::serde::escape_json_string_into(obj." << member_name
                            << ", json);\n";
                        out << "    json.push_back('\"');\n";
                    }
                    break;
                case schema_kind::integer:
                    if (is_optional) {
                        out << "    {\n";
                        out << "        if (!obj." << member_name << ") {\n";
                        out << "            json.append(\"null\");\n";
                        out << "        } else {\n";
                        out << "            char buf[32];\n";
                        out << "            auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), "
                               "*obj."
                            << member_name << ");\n";
                        out << "            json.append(buf, static_cast<size_t>(ptr - buf));\n";
                        out << "        }\n";
                        out << "    }\n";
                    } else {
                        out << "    {\n";
                        out << "        char buf[32];\n";
                        out << "        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj."
                            << member_name << ");\n";
                        out << "        json.append(buf, static_cast<size_t>(ptr - buf));\n";
                        out << "    }\n";
                    }
                    break;
                case schema_kind::number:
                    out << "    {\n";
                    if (is_optional) {
                        out << "        if (!obj." << member_name << ") {\n";
                        out << "            json.append(\"null\");\n";
                        out << "        } else {\n";
                    }
                    out << "        katana::serde::append_json_double(json, "
                        << (is_optional ? "*obj." + member_name : "obj." + member_name) << ");\n";
                    if (is_optional) {
                        out << "        }\n";
                    }
                    out << "    }\n";
                    break;
                case schema_kind::boolean:
                    if (is_optional) {
                        out << "    if (!obj." << member_name << ") {\n";
                        out << "        json.append(\"null\");\n";
                        out << "    } else {\n";
                        out << "        json.append(*obj." << member_name
                            << " ? \"true\" : \"false\");\n";
                        out << "    }\n";
                    } else {
                        out << "    json.append(obj." << member_name
                            << " ? \"true\" : \"false\");\n";
                    }
                    break;
                case schema_kind::array:
                    if (is_optional) {
                        out << "    if (!obj." << member_name << ") {\n";
                        out << "        json.append(\"null\");\n";
                        out << "    } else {\n";
                    }
                    out << "    json.push_back('[');\n";
                    out << "    for (size_t i = 0; i < "
                        << (is_optional ? "obj." + member_name + "->size()"
                                        : "obj." + member_name + ".size()")
                        << "; ++i) {\n";
                    out << "        if (i > 0) json.push_back(',');\n";
                    if (prop.type->items) {
                        switch (prop.type->items->kind) {
                        case schema_kind::string:
                            if (!prop.type->items->enum_values.empty()) {
                                out << "        json.push_back('\"');\n";
                                out << "        json.append(to_string("
                                    << (is_optional ? "(*obj." + member_name + ")[i]"
                                                    : "obj." + member_name + "[i]")
                                    << "));\n";
                                out << "        json.push_back('\"');\n";
                            } else {
                                out << "        json.push_back('\"');\n";
                                out << "        katana::serde::escape_json_string_into("
                                    << (is_optional ? "(*obj." + member_name + ")[i]"
                                                    : "obj." + member_name + "[i]")
                                    << ", json);\n";
                                out << "        json.push_back('\"');\n";
                            }
                            break;
                        case schema_kind::integer:
                            out << "        {\n";
                            out << "            char buf[32];\n";
                            out << "            auto [ptr, ec] = std::to_chars(buf, buf + "
                                   "sizeof(buf), "
                                << (is_optional ? "(*obj." + member_name + ")[i]"
                                                : "obj." + member_name + "[i]")
                                << ");\n";
                            out << "            json.append(buf, static_cast<size_t>(ptr - "
                                   "buf));\n";
                            out << "        }\n";
                            break;
                        case schema_kind::number:
                            out << "        katana::serde::append_json_double(json, "
                                << (is_optional ? "(*obj." + member_name + ")[i]"
                                                : "obj." + member_name + "[i]")
                                << ");\n";
                            break;
                        case schema_kind::boolean:
                            out << "        json.append("
                                << (is_optional ? "(*obj." + member_name + ")[i]"
                                                : "obj." + member_name + "[i]")
                                << " ? \"true\" : \"false\");\n";
                            break;
                        case schema_kind::object: {
                            auto nested_array_name = schema_identifier(doc, prop.type->items);
                            out << "        serialize_" << nested_array_name << "_into("
                                << (is_optional ? "(*obj." + member_name + ")[i]"
                                                : "obj." + member_name + "[i]")
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
                    if (is_optional) {
                        out << "    }\n";
                    }
                    break;
                case schema_kind::object:
                    if (is_optional) {
                        out << "    if (obj." << member_name << ") {\n";
                        out << "        serialize_" << nested_name << "_into(*obj." << member_name
                            << ", json);\n";
                        out << "    } else {\n";
                        out << "        json.append(\"null\");\n";
                        out << "    }\n";
                    } else {
                        out << "    serialize_" << nested_name << "_into(obj." << member_name
                            << ", json);\n";
                    }
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
        if (omittable) {
            out << "    }\n";
        }
    }

    out << "    json.push_back('}');\n";
    out << "}\n\n";

    // ── serialize: thin wrapper ──
    out << "inline std::string serialize_" << struct_name << "(const " << struct_name
        << "& obj) {\n";
    out << "    std::string json;\n";
    out << "    size_t reserve_estimate = " << reserve_est << ";\n";
    for (const auto& prop : s.properties) {
        emit_runtime_reserve_adjustment(out, doc, prop, "obj", "reserve_estimate");
    }
    out << "    json.reserve(reserve_estimate);\n";
    out << "    serialize_" << struct_name << "_into(obj, json);\n";
    out << "    return json;\n";
    out << "}\n\n";
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

namespace {

// A per-field scalar or array wrapper schema (e.g. `using SumResponse_Result_t = double;`
// or `using SumRequest_Values_t = arena_vector<...>`). Synthesized from a property, they
// carry parent_context + field_context, and struct parsers/serializers handle scalar AND
// array *fields* inline — they never call parse_<wrapper>/serialize_<wrapper>. So the whole
// emitted function family is dead weight (the dominant bloat in generated_json.hpp) UNLESS
// the wrapper is the element type of a top-level array, whose serializer DOES call
// serialize_<item>_into. That exception is handled by the caller via an items reference set.
// Object field wrappers are excluded (kept): parent parsers call them for object fields and
// object array items. Enums are excluded too (their conversion helpers are referenced).
bool is_value_field_wrapper(const katana::openapi::schema& s) {
    using katana::openapi::schema_kind;
    if (s.parent_context.empty() || s.field_context.empty() || !s.enum_values.empty()) {
        return false;
    }
    switch (s.kind) {
    case schema_kind::string:
    case schema_kind::integer:
    case schema_kind::number:
    case schema_kind::boolean:
    case schema_kind::array:
        return true;
    default:
        return false;
    }
}

// True for a named, empty object schema that lowers to `std::monostate`
// (content-less 204 responses, trivial parameter schemas, empty component
// schemas). These carry no data; their parse/serialize helpers are only worth
// emitting if some other generated type actually calls them — otherwise they are
// the `schema_N` aliases that clutter the output with dead, undebuggable noise.
bool is_empty_object_alias(const katana::openapi::schema& s) {
    using katana::openapi::schema_kind;
    return s.kind == schema_kind::object && s.properties.empty() && !s.name.empty() &&
           s.enum_values.empty() && s.one_of.empty() && s.any_of.empty() && s.all_of.empty() &&
           s.additional_properties == nullptr;
}

// Collect the identifiers of every schema reachable as a parse_<X>/serialize_<X>
// call target: object-typed properties, array items, composition members, and
// operation request/response/parameter bodies. A named empty-object schema absent
// from this set is never called and can be dropped from JSON generation entirely.
void collect_json_referenced_names(const document& doc, std::unordered_set<std::string>& out) {
    auto note = [&](const katana::openapi::schema* s) {
        if (s) {
            out.insert(schema_identifier(doc, s));
        }
    };
    for (const auto& s : doc.schemas) {
        for (const auto& prop : s.properties) {
            note(prop.type);
            if (prop.type) {
                note(prop.type->items);
            }
        }
        note(s.items);
        for (const auto* m : s.one_of) {
            note(m);
        }
        for (const auto* m : s.any_of) {
            note(m);
        }
        for (const auto* m : s.all_of) {
            note(m);
        }
        note(s.additional_properties);
    }
    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            if (op.body) {
                for (const auto& media : op.body->content) {
                    note(media.type);
                }
            }
            for (const auto& param : op.parameters) {
                note(param.type);
            }
            for (const auto& resp : op.responses) {
                for (const auto& media : resp.content) {
                    note(media.type);
                }
            }
        }
    }
}

} // namespace

std::string generate_json_parsers(const document& doc, bool use_pmr, const std::string& ns,
                                  std::string_view serdes_mode) {
    std::ostringstream out;

    // Directional pruning: a server never parses its own responses nor serializes request
    // bodies, so each direction is emitted only for the schemas its side can actually reach.
    const serdes_reachability reach = collect_serdes_reachability(doc, serdes_mode);
    const auto emit_parse = [&](const katana::openapi::schema& s) {
        return reach.parse_set.contains(&s);
    };
    const auto emit_serialize = [&](const katana::openapi::schema& s) {
        return reach.serialize_set.contains(&s);
    };

    // Schemas reachable as a parse_/serialize_ call target. Used to drop dead,
    // never-called `std::monostate` aliases (the `schema_N` clutter) without
    // removing any helper another generated type actually invokes.
    std::unordered_set<std::string> json_referenced;
    collect_json_referenced_names(doc, json_referenced);

    // Element types of top-level (non-field-wrapper) arrays. A top-level array body's
    // serializer emits `serialize_<item>_into(...)`, so its element wrapper must NOT be
    // dropped as a dead value-field-wrapper even though, as a plain field, it would be.
    std::unordered_set<std::string> array_item_refs;
    for (const auto& s : doc.schemas) {
        if (s.kind == katana::openapi::schema_kind::array && s.items != nullptr &&
            !is_field_wrapper_schema(s)) {
            array_item_refs.insert(schema_identifier(doc, s.items));
        }
    }

    // JSON entry points: schemas parsed/serialized from a raw string by the router —
    // request bodies and response bodies. Operation PARAMETERS are NOT here: they are
    // parsed from the query/path string via from_chars, never via parse_<X>(json). So a
    // top-level scalar/array schema that is only a parameter has a fully dead JSON family.
    std::unordered_set<std::string> entrypoint_refs;
    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            if (op.body != nullptr) {
                for (const auto& media : op.body->content) {
                    if (media.type != nullptr) {
                        entrypoint_refs.insert(schema_identifier(doc, media.type));
                    }
                }
            }
            for (const auto& resp : op.responses) {
                for (const auto& media : resp.content) {
                    if (media.type != nullptr) {
                        entrypoint_refs.insert(schema_identifier(doc, media.type));
                    }
                }
            }
        }
    }

    // A named, top-level scalar/array schema (not a field wrapper, not an enum/object)
    // whose JSON helpers are never invoked: not a body/response entry point and not the
    // element of a kept array. These are the per-parameter wrappers (listTasks_param_limit
    // and friends) that bloat the output with dead parse/serialize families.
    const auto is_dead_param_wrapper = [&](const katana::openapi::schema& s) {
        using katana::openapi::schema_kind;
        if (s.name.empty() || is_field_wrapper_schema(s) || !s.enum_values.empty()) {
            return false;
        }
        switch (s.kind) {
        case schema_kind::string:
        case schema_kind::integer:
        case schema_kind::number:
        case schema_kind::boolean:
        case schema_kind::array:
            break;
        default:
            return false;
        }
        const auto name = schema_identifier(doc, &s);
        return !entrypoint_refs.contains(name) && !array_item_refs.contains(name);
    };

    const auto skip_for_json = [&](const katana::openapi::schema& s) {
        if (should_skip_schema(s)) {
            return true;
        }
        // Dead scalar/array field wrappers — unless referenced as a top-level array element.
        if (is_value_field_wrapper(s) && !array_item_refs.contains(schema_identifier(doc, &s))) {
            return true;
        }
        if (is_dead_param_wrapper(s)) {
            return true;
        }
        if (is_empty_object_alias(s) && !json_referenced.contains(schema_identifier(doc, &s))) {
            return true;
        }
        return false;
    };

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

    // Forward declarations: parse (string_view overload) — skipped for inline field
    // wrappers, which are only ever parsed in place via the cursor overload.
    for (const auto& schema : doc.schemas) {
        if (!skip_for_json(schema) && emit_parse(schema) && !is_field_wrapper_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "[[nodiscard]] inline std::optional<" << name << "> parse_" << name
                << "(std::string_view json, monotonic_arena* arena);\n";
        }
    }
    out << "\n";
    // Forward declarations: parse (cursor overload)
    for (const auto& schema : doc.schemas) {
        if (!skip_for_json(schema) && emit_parse(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "[[nodiscard]] inline std::optional<" << name << "> parse_" << name
                << "(katana::serde::json_cursor& cur, monotonic_arena* arena);\n";
        }
    }
    out << "\n";
    // Forward declarations: serialize_into
    for (const auto& schema : doc.schemas) {
        if (!skip_for_json(schema) && emit_serialize(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline void serialize_" << name << "_into(const " << name
                << "& obj, std::string& out);\n";
        }
    }
    out << "\n";
    // Forward declarations: serialize
    for (const auto& schema : doc.schemas) {
        if (!skip_for_json(schema) && emit_serialize(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline std::string serialize_" << name << "(const " << name << "& obj);\n";
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
        if (!skip_for_json(schema) && emit_parse(schema)) {
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
        if (!skip_for_json(schema) && emit_serialize(schema)) {
            generate_json_serializer_for_schema(out, doc, schema);
        }
    }

    // NB: the per-element-type `parse_<X>_array` / `serialize_<X>_array` family used to be
    // emitted here but was never called — struct parsers/serializers inline their array
    // fields, and a top-level array body uses its own array schema's parser/serializer. The
    // dead family (up to ~6 functions per type) is no longer generated.

    return inject_namespace(out.str(), ns);
}

} // namespace katana_gen
