#include "generator.hpp"

#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace katana_gen {
namespace {

std::string
cpp_type_from_schema(const document& doc, const katana::openapi::schema* s, bool use_pmr) {
    if (!s) {
        return "std::monostate";
    }
    using katana::openapi::schema_kind;

    // Check if this is an enum
    if (s->kind == schema_kind::string && !s->enum_values.empty()) {
        return schema_identifier(doc, s) + "_enum";
    }

    bool allow_optional =
        false; // optional is intentionally disabled for now to keep arena ABI flat
    const bool nullable = s->nullable || allow_optional;

    auto wrap = [&](std::string base) {
        if (nullable || allow_optional) {
            return "std::optional<" + base + ">";
        }
        return base;
    };

    switch (s->kind) {
    case schema_kind::string:
        return wrap(use_pmr ? "arena_string<>" : "std::string");
    case schema_kind::integer:
        return wrap("int64_t");
    case schema_kind::number:
        return wrap("double");
    case schema_kind::boolean:
        return wrap("bool");
    case schema_kind::array:
        if (s->items) {
            return wrap((use_pmr ? "arena_vector<" : "std::vector<") +
                        cpp_type_from_schema(doc, s->items, use_pmr) + ">");
        }
        return wrap(use_pmr ? "arena_vector<std::string>" : "std::vector<std::string>");
    case schema_kind::object:
        return wrap(schema_identifier(doc, s));
    default:
        return wrap("std::monostate");
    }
}

void generate_dto_for_schema(std::ostream& out,
                             const document& doc,
                             const katana::openapi::schema& s,
                             bool use_pmr,
                             size_t indent = 0) {
    std::string ind(static_cast<size_t>(indent), ' ');
    auto struct_name = schema_identifier(doc, &s);

    if (s.properties.empty()) {
        auto alias = cpp_type_from_schema(doc, &s, use_pmr);
        // Avoid circular aliases like "using schema_10 = schema_10;"
        if (alias == struct_name) {
            // SKIP: Don't generate empty structs for circular aliases
            // These are artifacts from OpenAPI parsing (empty object schemas)
            // They serve no purpose and pollute the generated code
            return;
        }
        out << ind << "using " << struct_name << " = " << alias << ";\n\n";
        return;
    }

    out << ind << "struct " << struct_name << " {\n";

    // Generate compile-time metadata for validation constraints
    out << ind << "    // Compile-time metadata for validation\n";
    out << ind << "    struct metadata {\n";

    for (const auto& prop : s.properties) {
        if (!prop.type)
            continue;

        std::string prop_name_upper(prop.name.begin(), prop.name.end());
        // Convert to uppercase with underscores for constants
        for (auto& c : prop_name_upper) {
            if (c == '-' || c == ' ')
                c = '_';
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }

        // Required flag
        out << ind << "        static constexpr bool " << prop_name_upper
            << "_REQUIRED = " << (prop.required ? "true" : "false") << ";\n";

        // String constraints
        if (prop.type->kind == katana::openapi::schema_kind::string) {
            if (prop.type->min_length) {
                out << ind << "        static constexpr size_t " << prop_name_upper
                    << "_MIN_LENGTH = " << *prop.type->min_length << ";\n";
            }
            if (prop.type->max_length) {
                out << ind << "        static constexpr size_t " << prop_name_upper
                    << "_MAX_LENGTH = " << *prop.type->max_length << ";\n";
            }
            if (!prop.type->pattern.empty()) {
                out << ind << "        static constexpr std::string_view " << prop_name_upper
                    << "_PATTERN = \"" << prop.type->pattern << "\";\n";
            }
        }

        // Numeric constraints
        if (prop.type->kind == katana::openapi::schema_kind::integer ||
            prop.type->kind == katana::openapi::schema_kind::number) {
            if (prop.type->minimum) {
                out << ind << "        static constexpr double " << prop_name_upper
                    << "_MINIMUM = " << *prop.type->minimum << ";\n";
            }
            if (prop.type->maximum) {
                out << ind << "        static constexpr double " << prop_name_upper
                    << "_MAXIMUM = " << *prop.type->maximum << ";\n";
            }
            if (prop.type->exclusive_minimum) {
                out << ind << "        static constexpr double " << prop_name_upper
                    << "_EXCLUSIVE_MINIMUM = " << *prop.type->exclusive_minimum << ";\n";
            }
            if (prop.type->exclusive_maximum) {
                out << ind << "        static constexpr double " << prop_name_upper
                    << "_EXCLUSIVE_MAXIMUM = " << *prop.type->exclusive_maximum << ";\n";
            }
            if (prop.type->multiple_of) {
                out << ind << "        static constexpr double " << prop_name_upper
                    << "_MULTIPLE_OF = " << *prop.type->multiple_of << ";\n";
            }
        }

        // Array constraints
        if (prop.type->kind == katana::openapi::schema_kind::array) {
            if (prop.type->min_items) {
                out << ind << "        static constexpr size_t " << prop_name_upper
                    << "_MIN_ITEMS = " << *prop.type->min_items << ";\n";
            }
            if (prop.type->max_items) {
                out << ind << "        static constexpr size_t " << prop_name_upper
                    << "_MAX_ITEMS = " << *prop.type->max_items << ";\n";
            }
            if (prop.type->unique_items) {
                out << ind << "        static constexpr bool " << prop_name_upper
                    << "_UNIQUE_ITEMS = true;\n";
            }
        }
    }

    out << ind << "    };\n\n";

    // Generate compile-time static assertions for sanity checks
    for (const auto& prop : s.properties) {
        if (!prop.type)
            continue;

        std::string prop_name_upper(prop.name.begin(), prop.name.end());
        for (auto& c : prop_name_upper) {
            if (c == '-' || c == ' ')
                c = '_';
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }

        // String constraint assertions
        if (prop.type->kind == katana::openapi::schema_kind::string) {
            if (prop.type->min_length && prop.type->max_length) {
                out << ind << "    static_assert(metadata::" << prop_name_upper
                    << "_MIN_LENGTH <= metadata::" << prop_name_upper << "_MAX_LENGTH, \""
                    << prop.name << ": min_length must be <= max_length\");\n";
            }
        }

        // Numeric constraint assertions
        if (prop.type->kind == katana::openapi::schema_kind::integer ||
            prop.type->kind == katana::openapi::schema_kind::number) {
            if (prop.type->minimum && prop.type->maximum) {
                out << ind << "    static_assert(metadata::" << prop_name_upper
                    << "_MINIMUM <= metadata::" << prop_name_upper << "_MAXIMUM, \"" << prop.name
                    << ": minimum must be <= maximum\");\n";
            }
        }

        // Array constraint assertions
        if (prop.type->kind == katana::openapi::schema_kind::array) {
            if (prop.type->min_items && prop.type->max_items) {
                out << ind << "    static_assert(metadata::" << prop_name_upper
                    << "_MIN_ITEMS <= metadata::" << prop_name_upper << "_MAX_ITEMS, \""
                    << prop.name << ": min_items must be <= max_items\");\n";
            }
        }
    }

    out << "\n";

    if (use_pmr) {
        out << ind << "    explicit " << struct_name << "(monotonic_arena* arena = nullptr)\n";
        out << ind << "        : arena_(arena)";

        for (const auto& prop : s.properties) {
            auto cpp_type = cpp_type_from_schema(doc, prop.type, use_pmr);
            if (cpp_type.find("arena_vector") != std::string::npos ||
                cpp_type.find("arena_string") != std::string::npos) {
                out << ",\n"
                    << ind << "          " << prop.name << "(arena_allocator<char>(arena))";
            }
        }
        out << " {}\n\n";
        out << ind << "    monotonic_arena* arena_;\n";
    }

    for (const auto& prop : s.properties) {
        auto cpp_type = cpp_type_from_schema(doc, prop.type, use_pmr);
        out << ind << "    " << cpp_type << " " << prop.name;
        bool is_arena_type = use_pmr && (cpp_type.find("arena_string") != std::string::npos ||
                                         cpp_type.find("arena_vector") != std::string::npos);
        if (!prop.required && !is_arena_type) {
            out << " = {}";
        }
        out << ";\n";
    }

    out << ind << "};\n\n";
}

void generate_enum_for_schema(std::ostream& out,
                              const document& doc,
                              const katana::openapi::schema& s) {
    if (s.kind != katana::openapi::schema_kind::string || s.enum_values.empty()) {
        return;
    }

    auto enum_name = schema_identifier(doc, &s);
    out << "enum class " << enum_name << "_enum {\n";
    for (size_t i = 0; i < s.enum_values.size(); ++i) {
        const auto& val = s.enum_values[i];
        // Convert enum value to valid C++ identifier
        std::string identifier;
        identifier.reserve(val.size());
        for (char c : val) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                identifier.push_back(c);
            } else if (c == '-' || c == '_' || c == ' ') {
                identifier.push_back('_');
            }
        }
        if (identifier.empty() || std::isdigit(static_cast<unsigned char>(identifier[0]))) {
            identifier = "value_" + identifier;
        }
        out << "    " << identifier;
        if (i < s.enum_values.size() - 1) {
            out << ",";
        }
        out << "\n";
    }
    out << "};\n\n";

    // Add string conversion functions
    out << "inline std::string_view to_string(" << enum_name << "_enum e) {\n";
    out << "    switch (e) {\n";
    for (const auto& val : s.enum_values) {
        std::string identifier;
        identifier.reserve(val.size());
        for (char c : val) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                identifier.push_back(c);
            } else if (c == '-' || c == '_' || c == ' ') {
                identifier.push_back('_');
            }
        }
        if (identifier.empty() || std::isdigit(static_cast<unsigned char>(identifier[0]))) {
            identifier = "value_" + identifier;
        }
        out << "    case " << enum_name << "_enum::" << identifier << ": return \"" << val
            << "\";\n";
    }
    out << "    }\n";
    out << "    return \"\";\n";
    out << "}\n\n";

    // Add from_string function
    out << "inline std::optional<" << enum_name << "_enum> " << enum_name
        << "_enum_from_string(std::string_view s) {\n";
    for (const auto& val : s.enum_values) {
        std::string identifier;
        identifier.reserve(val.size());
        for (char c : val) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                identifier.push_back(c);
            } else if (c == '-' || c == '_' || c == ' ') {
                identifier.push_back('_');
            }
        }
        if (identifier.empty() || std::isdigit(static_cast<unsigned char>(identifier[0]))) {
            identifier = "value_" + identifier;
        }
        out << "    if (s == \"" << val << "\") return " << enum_name << "_enum::" << identifier
            << ";\n";
    }
    out << "    return std::nullopt;\n";
    out << "}\n\n";
}

} // namespace

std::string generate_dtos(const document& doc, bool use_pmr) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    if (use_pmr) {
        out << "#include \"katana/core/arena.hpp\"\n";
        out << "using katana::arena_allocator;\n";
        out << "using katana::arena_string;\n";
        out << "using katana::arena_vector;\n";
        out << "using katana::monotonic_arena;\n\n";
    } else {
        out << "#include <string>\n";
        out << "#include <vector>\n";
        out << "#include <variant>\n\n";
    }
    out << "#include <optional>\n";
    out << "#include <string_view>\n";
    out << "#include <cctype>\n\n";

    // Generate enums first
    for (const auto& schema : doc.schemas) {
        generate_enum_for_schema(out, doc, schema);
    }

    // Then generate DTOs
    for (const auto& schema : doc.schemas) {
        generate_dto_for_schema(out, doc, schema, use_pmr);
    }

    return out.str();
}

} // namespace katana_gen
