#include "generator.hpp"

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

void generate_json_parser_for_schema(std::ostream& out,
                                     const document& doc,
                                     const katana::openapi::schema& s,
                                     bool use_pmr) {
    auto struct_name = schema_identifier(doc, &s);
    out << "inline std::optional<" << struct_name << "> parse_" << struct_name
        << "(std::string_view json, monotonic_arena* arena) {\n";
    out << "    using katana::serde::json_cursor;\n";
    out << "    json_cursor cur{json.data(), json.data() + json.size()};\n";
    if (!use_pmr) {
        out << "    (void)arena;\n";
    }

    // Scalars and arrays
    if (s.properties.empty()) {
        using katana::openapi::schema_kind;
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
            out << "    if (auto v = katana::serde::parse_size(cur)) return " << struct_name
                << "{static_cast<int64_t>(*v)};\n";
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
                out << "        if (auto v = katana::serde::parse_size(cur)) {\n";
                out << "            result.push_back(static_cast<int64_t>(*v));\n";
                out << "        } else { cur.skip_value(); }\n";
                break;
            case schema_kind::boolean:
                out << "        if (auto v = katana::serde::parse_bool(cur)) {\n";
                out << "            result.push_back(*v);\n";
                out << "        } else { cur.skip_value(); }\n";
                break;
            case schema_kind::string:
                out << "        if (auto v = cur.string()) {\n";
                if (use_pmr) {
                    out << "            result.emplace_back(v->begin(), v->end(), "
                           "arena_allocator<char>(arena));\n";
                } else {
                    out << "            result.emplace_back(v->begin(), v->end());\n";
                }
                out << "        } else { cur.skip_value(); }\n";
                break;
            default:
                // For complex types (objects, nested arrays), use string_view approach
                out << "        auto start = cur.ptr;\n";
                out << "        cur.skip_value();\n";
                out << "        std::string_view elem(start, static_cast<size_t>(cur.ptr - "
                       "start));\n";
                out << "        if (auto parsed = parse_" << schema_identifier(doc, s.items)
                    << "(elem, arena)) result.push_back(*parsed);\n";
                break;
            }

            out << "        cur.try_comma();\n";
            out << "    }\n";
            out << "    return result;\n";
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

    for (const auto& prop : s.properties) {
        out << "        if (*key == \"" << prop.name << "\") {\n";
        if (prop.required) {
            out << "            has_" << prop.name << " = true;\n";
        }
        if (prop.type) {
            using katana::openapi::schema_kind;
            bool is_enum =
                prop.type->kind == schema_kind::string && !prop.type->enum_values.empty();

            auto nested_name = schema_identifier(doc, prop.type);
            if (is_enum && !nested_name.empty()) {
                out << "            if (auto v = cur.string()) {\n";
                out << "                auto enum_val = " << nested_name
                    << "_enum_from_string(std::string_view(v->begin(), v->end()));\n";
                out << "                if (enum_val) obj." << prop.name << " = *enum_val;\n";
                out << "            } else { cur.skip_value(); }\n";
            } else {
                switch (prop.type->kind) {
                case schema_kind::string:
                    out << "            if (auto v = cur.string()) {\n";
                    if (use_pmr) {
                        out << "                obj." << prop.name
                            << " = arena_string<>(v->begin(), v->end(), "
                               "arena_allocator<char>(arena));\n";
                    } else {
                        out << "                obj." << prop.name
                            << " = std::string(v->begin(), v->end());\n";
                    }
                    out << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::integer:
                    out << "            if (auto v = katana::serde::parse_size(cur)) {\n";
                    out << "                obj." << prop.name << " = static_cast<int64_t>(*v);\n";
                    out << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::number:
                    out << "            if (auto v = katana::serde::parse_double(cur)) {\n";
                    out << "                obj." << prop.name << " = *v;\n";
                    out << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::boolean:
                    out << "            if (auto v = katana::serde::parse_bool(cur)) {\n";
                    out << "                obj." << prop.name << " = *v;\n";
                    out << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::array:
                    out << "            if (cur.try_array_start()) {\n";
                    out << "                while (!cur.eof()) {\n";
                    out << "                    cur.skip_ws();\n";
                    out << "                    if (cur.try_array_end()) break;\n";
                    if (prop.type->items) {
                        auto* item = prop.type->items;
                        switch (item->kind) {
                        case schema_kind::string:
                            out << "                    if (auto v = cur.string()) {\n";
                            if (use_pmr) {
                                out << "                        obj." << prop.name
                                    << ".emplace_back(v->begin(), v->end(), "
                                       "arena_allocator<char>(arena));\n";
                            } else {
                                out << "                        obj." << prop.name
                                    << ".emplace_back(v->begin(), v->end());\n";
                            }
                            out << "                    } else { cur.skip_value(); }\n";
                            break;
                        case schema_kind::integer:
                            out << "                    if (auto v = "
                                   "katana::serde::parse_size(cur)) {\n";
                            out << "                        obj." << prop.name
                                << ".push_back(static_cast<int64_t>(*v));\n";
                            out << "                    } else { cur.skip_value(); }\n";
                            break;
                        case schema_kind::number:
                            out << "                    if (auto v = "
                                   "katana::serde::parse_double(cur)) {\n";
                            out << "                        obj." << prop.name
                                << ".push_back(*v);\n";
                            out << "                    } else { cur.skip_value(); }\n";
                            break;
                        case schema_kind::boolean:
                            out << "                    if (auto v = "
                                   "katana::serde::parse_bool(cur)) {\n";
                            out << "                        obj." << prop.name
                                << ".push_back(*v);\n";
                            out << "                    } else { cur.skip_value(); }\n";
                            break;
                        case schema_kind::object: {
                            auto nested_array_name = schema_identifier(doc, item);
                            if (!nested_array_name.empty()) {
                                out << "                    {\n";
                                out << "                        const char* value_start = "
                                       "cur.ptr;\n";
                                out << "                        cur.skip_value();\n";
                                out << "                        std::string_view sv(value_start, "
                                       "static_cast<size_t>(cur.ptr - value_start));\n";
                                out << "                        if (auto nested = parse_"
                                    << nested_array_name << "(sv, arena)) { obj." << prop.name
                                    << ".push_back(*nested); }\n";
                                out << "                    }\n";
                            } else {
                                out << "                    cur.skip_value();\n";
                            }
                            break;
                        }
                        case schema_kind::array:
                        case schema_kind::null_type:
                            out << "                    cur.skip_value();\n";
                            break;
                        default:
                            out << "                    cur.skip_value();\n";
                            break;
                        }
                    } else {
                        out << "                    cur.skip_value();\n";
                    }
                    out << "                    cur.try_comma();\n";
                    out << "                }\n";
                    out << "            } else { cur.skip_value(); }\n";
                    break;
                case schema_kind::object: {
                    auto nested_obj_name = schema_identifier(doc, prop.type);
                    if (!nested_obj_name.empty()) {
                        out << "            {\n";
                        out << "                const char* value_start = cur.ptr;\n";
                        out << "                cur.skip_value();\n";
                        out << "                std::string_view sv(value_start, "
                               "static_cast<size_t>(cur.ptr - value_start));\n";
                        out << "                auto nested = parse_" << nested_obj_name
                            << "(sv, arena);\n";
                        out << "                if (nested) obj." << prop.name << " = *nested;\n";
                        out << "            }\n";
                    } else {
                        out << "            cur.skip_value();\n";
                    }
                    break;
                }
                case schema_kind::null_type:
                    out << "            cur.skip_value();\n";
                    break;
                default:
                    out << "            cur.skip_value();\n";
                    break;
                }
            }
        } else {
            out << "            cur.skip_value();\n";
        }
        out << "        } else ";
    }
    out << "{\n";
    out << "            cur.skip_value();\n";
    out << "        }\n";
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

void generate_json_serializer_for_schema(std::ostream& out,
                                         const document& doc,
                                         const katana::openapi::schema& s) {
    auto struct_name = schema_identifier(doc, &s);
    out << "inline std::string serialize_" << struct_name << "(const " << struct_name
        << "& obj) {\n";
    if (s.properties.empty()) {
        using katana::openapi::schema_kind;
        switch (s.kind) {
        case schema_kind::string:
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
                out << "    std::string json;\n";
                out << "    json.reserve(obj->size() + 16);\n";
                out << "    json.push_back('\"');\n";
                out << "    json.append(katana::serde::escape_json_string(*obj));\n";
                out << "    json.push_back('\"');\n";
                out << "    return json;\n";
            } else {
                out << "    std::string json;\n";
                out << "    json.reserve(obj.size() + 16);\n";
                out << "    json.push_back('\"');\n";
                out << "    json.append(katana::serde::escape_json_string(obj));\n";
                out << "    json.push_back('\"');\n";
                out << "    return json;\n";
            }
            out << "}\n\n";
            return;
        case schema_kind::integer:
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
                out << "    char buf[32];\n";
                out << "    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), *obj);\n";
                out << "    return std::string(buf, static_cast<size_t>(ptr - buf));\n";
            } else {
                out << "    char buf[32];\n";
                out << "    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);\n";
                out << "    return std::string(buf, static_cast<size_t>(ptr - buf));\n";
            }
            out << "}\n\n";
            return;
        case schema_kind::number:
            if (s.nullable) {
                out << "    if (!obj) return std::string(\"null\");\n";
                out << "    char buf[64];\n";
                out << "    auto res = std::to_chars(buf, buf + sizeof(buf), *obj);\n";
                out << "    if (res.ec == std::errc()) return std::string(buf, "
                       "static_cast<size_t>(res.ptr - buf));\n";
                out << "    return {};\n";
            } else {
                out << "    char buf[64];\n";
                out << "    auto res = std::to_chars(buf, buf + sizeof(buf), obj);\n";
                out << "    if (res.ec == std::errc()) return std::string(buf, "
                       "static_cast<size_t>(res.ptr - buf));\n";
                out << "    return {};\n";
            }
            out << "}\n\n";
            return;
        case schema_kind::boolean:
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
                out << "    if (!obj) return std::string(\"null\");\n";
            }
            out << "    const auto& arr = " << (s.nullable ? "*obj" : "obj") << ";\n";
            out << "    std::string json;\n";
            out << "    json.reserve(arr.size() * 16 + 2);\n";
            out << "    json.push_back('[');\n";
            out << "    for (size_t i = 0; i < arr.size(); ++i) {\n";
            out << "        if (i > 0) json.push_back(',');\n";
            out << "        json.append(serialize_" << schema_identifier(doc, s.items)
                << "(arr[i]));\n";
            out << "    }\n";
            out << "    json.push_back(']');\n";
            out << "    return json;\n";
            out << "}\n\n";
            return;
        default:
            out << "    (void)obj;\n";
            out << "    return {};\n";
            out << "}\n\n";
            return;
        }
    }
    out << "    std::string json;\n";
    out << "    json.reserve(256);\n";
    out << "    json.push_back('{');\n";
    out << "    bool first = true;\n\n";

    for (const auto& prop : s.properties) {
        out << "    if (!first) json.push_back(',');\n";
        out << "    first = false;\n";
        out << "    json.append(\"\\\"" << prop.name << "\\\":\");\n";

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
                        out << "        json.append(katana::serde::escape_json_string(*obj."
                            << prop.name << "));\n";
                        out << "        json.push_back('\"');\n";
                        out << "    } else {\n";
                        out << "        json.append(\"null\");\n";
                        out << "    }\n";
                    } else {
                        out << "    json.push_back('\"');\n";
                        out << "    json.append(katana::serde::escape_json_string(obj." << prop.name
                            << "));\n";
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
                            out << "        json.push_back('\"');\n";
                            out << "        json.append(katana::serde::escape_json_string("
                                << (is_optional ? "(*obj." + prop.name + ")[i]"
                                                : "obj." + prop.name + "[i]")
                                << "));\n";
                            out << "        json.push_back('\"');\n";
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
                            out << "        json.append(serialize_" << nested_array_name << "("
                                << (is_optional ? "(*obj." + prop.name + ")[i]"
                                                : "obj." + prop.name + "[i]")
                                << "));\n";
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
                    out << "    json.append(serialize_" << nested_name << "(obj." << prop.name
                        << "));\n";
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
    out << "    return json;\n";
    out << "}\n\n";
}

void generate_json_array_parser(std::ostream& out,
                                const document& doc,
                                const katana::openapi::schema& s,
                                [[maybe_unused]] bool use_pmr) {
    auto struct_name = schema_identifier(doc, &s);
    out << "inline std::optional<std::vector<" << struct_name << ">> parse_" << struct_name
        << "_array(std::string_view json, monotonic_arena* arena) {\n";
    out << "    using katana::serde::json_cursor;\n";
    out << "    json_cursor cur{json.data(), json.data() + json.size()};\n";
    out << "    if (!cur.try_array_start()) return std::nullopt;\n\n";
    out << "    std::vector<" << struct_name << "> result;\n";
    out << "    while (!cur.eof()) {\n";
    out << "        cur.skip_ws();\n";
    out << "        if (cur.try_array_end()) break;\n";
    out << "        \n";
    out << "        // Parse object at current position\n";
    out << "        size_t obj_start = cur.pos();\n";
    out << "        cur.skip_value();\n";
    out << "        size_t obj_end = cur.pos();\n";
    out << "        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);\n";
    out << "        \n";
    out << "        auto obj = parse_" << struct_name << "(obj_json, arena);\n";
    out << "        if (!obj) return std::nullopt;\n";
    out << "        result.push_back(std::move(*obj));\n";
    out << "        \n";
    out << "        cur.try_comma();\n";
    out << "    }\n";
    out << "    return result;\n";
    out << "}\n\n";
}

void generate_json_array_serializer(std::ostream& out,
                                    const document& doc,
                                    const katana::openapi::schema& s,
                                    bool use_pmr) {
    auto struct_name = schema_identifier(doc, &s);
    out << "inline std::string serialize_" << struct_name << "_array(const std::vector<"
        << struct_name << ">& arr) {\n";
    out << "    std::string json;\n";
    out << "    json.reserve(arr.size() * 32 + 2);\n";
    out << "    json.push_back('[');\n";
    out << "    for (size_t i = 0; i < arr.size(); ++i) {\n";
    out << "        json.append(serialize_" << struct_name << "(arr[i]));\n";
    out << "        if (i < arr.size() - 1) json.push_back(',');\n";
    out << "    }\n";
    out << "    json.push_back(']');\n";
    out << "    return json;\n";
    out << "}\n\n";

    if (use_pmr) {
        out << "inline std::string serialize_" << struct_name << "_array(const arena_vector<"
            << struct_name << ">& arr) {\n";
        out << "    std::string json;\n";
        out << "    json.reserve(arr.size() * 32 + 2);\n";
        out << "    json.push_back('[');\n";
        out << "    for (size_t i = 0; i < arr.size(); ++i) {\n";
        out << "        json.append(serialize_" << struct_name << "(arr[i]));\n";
        out << "        if (i < arr.size() - 1) json.push_back(',');\n";
        out << "    }\n";
        out << "    json.push_back(']');\n";
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

    // Skip empty object artifacts (circular alias placeholders from OpenAPI parsing)
    if (s.kind == schema_kind::object && s.properties.empty()) {
        return true;
    }

    return false;
}

std::string generate_json_parsers(const document& doc, bool use_pmr) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include \"katana/core/arena.hpp\"\n";
    out << "#include \"katana/core/serde.hpp\"\n";
    out << "#include <optional>\n";
    out << "#include <string>\n";
    out << "#include <charconv>\n";
    out << "#include <vector>\n\n";
    out << "using katana::monotonic_arena;\n\n";

    // Forward declarations to allow cross-references between schemas
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline std::optional<" << name << "> parse_" << name
                << "(std::string_view json, monotonic_arena* arena);\n";
        }
    }
    out << "\n";
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline std::string serialize_" << name << "(const " << name << "& obj);\n";
        }
    }
    out << "\n";
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            auto name = schema_identifier(doc, &schema);
            out << "inline std::optional<std::vector<" << name << ">> parse_" << name
                << "_array(std::string_view json, monotonic_arena* arena);\n";
        }
    }
    out << "\n";
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

    // Only generate parsers for non-trivial schemas (objects with properties or arrays)
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            generate_json_parser_for_schema(out, doc, schema, use_pmr);
        }
    }

    // Only generate serializers for non-trivial schemas
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            generate_json_serializer_for_schema(out, doc, schema);
        }
    }

    // Generate array parsers only for non-trivial schemas
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            generate_json_array_parser(out, doc, schema, use_pmr);
        }
    }

    // Generate array serializers only for non-trivial schemas
    for (const auto& schema : doc.schemas) {
        if (!should_skip_schema(schema)) {
            generate_json_array_serializer(out, doc, schema, use_pmr);
        }
    }

    return out.str();
}

} // namespace katana_gen
