#include "generator.hpp"

#include <algorithm>
#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace katana_gen {
namespace {

// Returns the alignment rank for a C++ type string.
// Higher rank = larger alignment = should come first in struct layout.
// 8-byte types: string, vector, object, int64_t, double
// 1-byte types: bool
int alignment_rank(const std::string& cpp_type) {
    if (cpp_type == "bool") {
        return 1;
    }
    // Everything else (int64_t, double, std::string, arena_string, std::vector,
    // arena_vector, object types) is 8-byte aligned on 64-bit
    return 8;
}

constexpr size_t DEFAULT_INLINE_ARENA_ARRAY_CAPACITY = 8;

// Extracts the inner type T from "arena_vector<T>" or "arena_vector<T, N>".
// Returns empty string if the type is not an arena_vector.
std::string extract_arena_vector_inner_type(const std::string& cpp_type) {
    const std::string prefix = "arena_vector<";
    auto pos = cpp_type.find(prefix);
    if (pos == std::string::npos) {
        return {};
    }
    auto start = pos + prefix.size();
    // Find the matching closing '>'
    int depth = 1;
    auto end = start;
    while (end < cpp_type.size() && depth > 0) {
        if (cpp_type[end] == '<')
            ++depth;
        if (cpp_type[end] == '>')
            --depth;
        if (depth > 0)
            ++end;
    }
    const auto payload = cpp_type.substr(start, end - start);

    int nested_depth = 0;
    for (size_t i = 0; i < payload.size(); ++i) {
        if (payload[i] == '<') {
            ++nested_depth;
        } else if (payload[i] == '>') {
            --nested_depth;
        } else if (payload[i] == ',' && nested_depth == 0) {
            return payload.substr(0, i);
        }
    }

    return payload;
}

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

std::string cpp_type_from_schema(const document& doc,
                                 const katana::openapi::schema* s,
                                 bool use_pmr,
                                 bool inline_top_level_small_array = false,
                                 bool wrap_optional = true) {
    if (!s) {
        return "std::monostate";
    }
    using katana::openapi::schema_kind;

    // Check if this is an enum
    if (s->kind == schema_kind::string && !s->enum_values.empty()) {
        return schema_identifier(doc, s) + "_enum";
    }

    auto wrap = [&](std::string base) {
        if (wrap_optional && s->nullable) {
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
            const auto item_type = cpp_type_from_schema(doc, s->items, use_pmr);
            if (use_pmr && inline_top_level_small_array) {
                const auto inline_capacity = inline_arena_array_capacity(s);
                if (inline_capacity > 0) {
                    return wrap("arena_vector<" + item_type + ", " +
                                std::to_string(inline_capacity) + ">");
                }
            }
            return wrap((use_pmr ? "arena_vector<" : "std::vector<") + item_type + ">");
        }
        return wrap(use_pmr ? "arena_vector<std::string>" : "std::vector<std::string>");
    case schema_kind::object:
        // Empty objects (including free-form additionalProperties) are represented as monostate
        // to avoid self-referential aliases like "using X = X;".
        if (s->properties.empty()) {
            return wrap("std::monostate");
        }
        return wrap(schema_identifier(doc, s));
    default:
        return wrap("std::monostate");
    }
}

std::string
cpp_type_for_property(const document& doc, const katana::openapi::property& prop, bool use_pmr) {
    auto base = cpp_type_from_schema(doc, prop.type, use_pmr, false, false);
    if (is_optional_property(prop)) {
        return "std::optional<" + base + ">";
    }
    return base;
}

std::vector<std::string> enum_value_identifiers(const katana::openapi::schema& s) {
    std::vector<std::string> identifiers;
    identifiers.reserve(s.enum_values.size());
    std::unordered_set<std::string> used;
    used.reserve(s.enum_values.size());

    for (const auto& val : s.enum_values) {
        std::string identifier = sanitize_identifier(val);
        if (identifier.empty()) {
            identifier = "value";
        }
        auto candidate = identifier;
        size_t suffix = 1;
        while (!used.insert(candidate).second) {
            candidate = identifier + "_" + std::to_string(++suffix);
        }
        identifiers.push_back(std::move(candidate));
    }

    return identifiers;
}

void generate_dto_for_schema(std::ostream& out,
                             const document& doc,
                             const katana::openapi::schema& s,
                             bool use_pmr,
                             size_t indent = 0) {
    std::string ind(static_cast<size_t>(indent), ' ');
    auto struct_name = schema_identifier(doc, &s);

    if (s.properties.empty()) {
        auto alias = cpp_type_from_schema(doc, &s, use_pmr, true);
        // Avoid circular aliases like "using schema_10 = schema_10;"
        if (alias == struct_name) {
            // SKIP: Don't generate empty structs for circular aliases
            // These are artifacts from OpenAPI parsing (empty object schemas)
            // They serve no purpose and pollute the generated code
            return;
        }
        // Add doc comment for type aliases
        if (!s.description.empty()) {
            out << ind << "/// " << s.description << "\n";
        }
        out << ind << "using " << struct_name << " = " << alias << ";\n\n";
        return;
    }

    // Add documentation comment for struct
    if (!s.description.empty()) {
        out << ind << "/// " << s.description << "\n";
    } else {
        // Generate helpful comment based on struct name
        std::string name_str(s.name.begin(), s.name.end());
        if (name_str.find("_request") != std::string::npos) {
            out << ind << "/// Request body type with " << s.properties.size() << " fields\n";
        } else if (name_str.find("_response") != std::string::npos) {
            out << ind << "/// Response body type with " << s.properties.size() << " fields\n";
        } else {
            out << ind << "/// Data type with " << s.properties.size() << " fields\n";
        }
    }

    out << ind << "struct " << struct_name << " {\n";

    // Generate compile-time metadata for validation constraints
    out << ind << "    // Compile-time metadata for validation\n";
    out << ind << "    struct metadata {\n";

    for (const auto& prop : s.properties) {
        if (!prop.type)
            continue;

        const auto prop_name_upper = metadata_constant_identifier(prop.name);

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
                    << "_PATTERN = \"" << escape_cpp_string(prop.type->pattern) << "\";\n";
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

        const auto prop_name_upper = metadata_constant_identifier(prop.name);

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

    // Collect properties with their resolved C++ types for sorting
    struct prop_entry {
        const katana::openapi::property* prop;
        std::string cpp_type;
        int align;
        size_t source_index;
    };
    std::vector<prop_entry> sorted_props;
    sorted_props.reserve(s.properties.size());
    for (size_t index = 0; index < s.properties.size(); ++index) {
        const auto& prop = s.properties[index];
        auto cpp_type = cpp_type_for_property(doc, prop, use_pmr);
        sorted_props.push_back({&prop, cpp_type, alignment_rank(cpp_type), index});
    }
    // Keep deterministic source order within each alignment bucket without relying on stable_sort,
    // which pulls in deprecated libstdc++ temporary-buffer helpers under some clang CI toolchains.
    std::sort(
        sorted_props.begin(), sorted_props.end(), [](const prop_entry& a, const prop_entry& b) {
            if (a.align != b.align) {
                return a.align > b.align;
            }
            return a.source_index < b.source_index;
        });

    if (use_pmr) {
        out << ind << "    explicit " << struct_name << "(monotonic_arena* arena = nullptr)\n";
        out << ind << "        : arena_(arena)";

        for (const auto& entry : sorted_props) {
            const auto& cpp_type = entry.cpp_type;
            const auto member_name = property_member_identifier(entry.prop->name);
            if (cpp_type.find("std::optional<") != std::string::npos) {
                continue;
            }
            if (cpp_type.find("arena_vector") != std::string::npos) {
                // Use semantic allocator: arena_allocator<T> for arena_vector<T>
                auto inner = extract_arena_vector_inner_type(cpp_type);
                out << ",\n"
                    << ind << "          " << member_name << "(arena_allocator<" << inner
                    << ">(arena))";
            } else if (cpp_type.find("arena_string") != std::string::npos) {
                out << ",\n"
                    << ind << "          " << member_name << "(arena_allocator<char>(arena))";
            }
        }
        out << " {}\n\n";
        out << ind << "    monotonic_arena* arena_;\n";
    }

    // Fields ordered by alignment for optimal packing
    for (const auto& entry : sorted_props) {
        const auto& cpp_type = entry.cpp_type;
        const auto* prop = entry.prop;
        const auto member_name = property_member_identifier(prop->name);

        // Add doc comment for property if type has description
        if (prop->type && !prop->type->description.empty()) {
            out << ind << "    /// " << prop->type->description << "\n";
        } else if (is_optional_property(*prop)) {
            out << ind << "    /// Optional field\n";
        }

        out << ind << "    " << cpp_type << " " << member_name;
        bool is_arena_type = use_pmr && (cpp_type.find("arena_string") != std::string::npos ||
                                         cpp_type.find("arena_vector") != std::string::npos);
        // Don't use = {} for types with explicit arena constructors
        // (object types with properties have explicit ctor when use_pmr is true)
        bool is_arena_object = use_pmr && prop->type &&
                               prop->type->kind == katana::openapi::schema_kind::object &&
                               !prop->type->properties.empty();
        bool is_optional_member = cpp_type.find("std::optional<") != std::string::npos;
        if (!is_optional_member && !prop->required && !is_arena_type && !is_arena_object) {
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

    // Add documentation comment if description is available
    if (!s.description.empty()) {
        out << "/// " << s.description << "\n";
    } else {
        out << "/// Enum with " << s.enum_values.size() << " possible values\n";
    }

    const auto identifiers = enum_value_identifiers(s);

    out << "enum class " << enum_name << "_enum {\n";
    for (size_t i = 0; i < s.enum_values.size(); ++i) {
        out << "    " << identifiers[i];
        if (i < s.enum_values.size() - 1) {
            out << ",";
        }
        out << "\n";
    }
    out << "};\n\n";

    // Add string conversion functions
    out << "inline std::string_view to_string(" << enum_name << "_enum e) {\n";
    out << "    switch (e) {\n";
    for (size_t i = 0; i < s.enum_values.size(); ++i) {
        out << "    case " << enum_name << "_enum::" << identifiers[i] << ": return \""
            << escape_cpp_string(s.enum_values[i]) << "\";\n";
    }
    out << "    }\n";
    out << "    return \"\";\n";
    out << "}\n\n";

    // Add from_string function
    out << "inline std::optional<" << enum_name << "_enum> " << enum_name
        << "_enum_from_string(std::string_view s) {\n";
    for (size_t i = 0; i < s.enum_values.size(); ++i) {
        out << "    if (s == \"" << escape_cpp_string(s.enum_values[i]) << "\") return "
            << enum_name << "_enum::" << identifiers[i] << ";\n";
    }
    out << "    return std::nullopt;\n";
    out << "}\n\n";
}

// Helper: unwrap arrays and return the innermost element type schema pointer.
const katana::openapi::schema* unwrap_array(const katana::openapi::schema* t) {
    while (t && t->kind == katana::openapi::schema_kind::array && t->items) {
        t = t->items;
    }
    return t;
}

// Collects schema dependencies for a given schema.
// Returns pointers to schemas that must be defined before 's'.
// Handles both struct schemas (with properties) and type alias schemas (without properties).
void collect_schema_deps(const document& /*doc*/,
                         const katana::openapi::schema& s,
                         bool /*use_pmr*/,
                         std::vector<const katana::openapi::schema*>& deps) {
    if (!s.properties.empty()) {
        // Struct schema: collect deps from properties
        for (const auto& prop : s.properties) {
            if (!prop.type)
                continue;

            const auto* t = unwrap_array(prop.type);
            // Object types with properties generate structs that need ordering
            if (t && t->kind == katana::openapi::schema_kind::object && !t->properties.empty()) {
                deps.push_back(t);
            }
        }
    } else {
        // Type alias schema (e.g., "using X = arena_vector<SomeStruct>;")
        // Check if this alias references an object type through arrays
        const auto* t = unwrap_array(&s);
        if (t && t != &s && t->kind == katana::openapi::schema_kind::object &&
            !t->properties.empty()) {
            deps.push_back(t);
        }
        // Direct object reference without array wrapping
        if (s.kind == katana::openapi::schema_kind::object && s.items) {
            const auto* inner = unwrap_array(s.items);
            if (inner && inner->kind == katana::openapi::schema_kind::object &&
                !inner->properties.empty()) {
                deps.push_back(inner);
            }
        }
    }
}

// Topologically sorts schemas so that dependencies are emitted before dependents.
// Schemas without dependencies keep their original relative order.
std::vector<size_t> topological_sort_schemas(const document& doc, bool use_pmr) {
    const size_t n = doc.schemas.size();

    // Map schema pointer -> index in doc.schemas
    std::unordered_map<const katana::openapi::schema*, size_t> schema_to_idx;
    schema_to_idx.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        schema_to_idx[&doc.schemas[i]] = i;
    }

    // Build adjacency list: adj[i] = set of indices that schema i depends on
    std::vector<std::unordered_set<size_t>> deps(n);
    for (size_t i = 0; i < n; ++i) {
        std::vector<const katana::openapi::schema*> dep_schemas;
        collect_schema_deps(doc, doc.schemas[i], use_pmr, dep_schemas);
        for (const auto* dep : dep_schemas) {
            auto it = schema_to_idx.find(dep);
            if (it != schema_to_idx.end() && it->second != i) {
                deps[i].insert(it->second);
            }
        }
    }

    // Kahn's algorithm for topological sort
    std::vector<size_t> in_degree(n, 0);
    std::vector<std::vector<size_t>> reverse_adj(n); // reverse_adj[j] = schemas that depend on j
    for (size_t i = 0; i < n; ++i) {
        for (size_t j : deps[i]) {
            reverse_adj[j].push_back(i);
            ++in_degree[i];
        }
    }

    // Use a queue that preserves original order for schemas with same priority
    // (stable topological sort using original index as tie-breaker)
    std::vector<size_t> queue;
    for (size_t i = 0; i < n; ++i) {
        if (in_degree[i] == 0) {
            queue.push_back(i);
        }
    }
    // Sort initial queue by original index to maintain stability
    std::sort(queue.begin(), queue.end());

    std::vector<size_t> result;
    result.reserve(n);
    size_t front = 0;

    while (front < queue.size()) {
        size_t idx = queue[front++];
        result.push_back(idx);

        // Collect newly freed nodes
        std::vector<size_t> newly_freed;
        for (size_t dependent : reverse_adj[idx]) {
            if (--in_degree[dependent] == 0) {
                newly_freed.push_back(dependent);
            }
        }
        // Sort to maintain stable order
        std::sort(newly_freed.begin(), newly_freed.end());
        for (size_t nf : newly_freed) {
            queue.push_back(nf);
        }
    }

    // If there are cycles, append remaining schemas in original order
    if (result.size() < n) {
        std::unordered_set<size_t> emitted(result.begin(), result.end());
        for (size_t i = 0; i < n; ++i) {
            if (!emitted.contains(i)) {
                result.push_back(i);
            }
        }
    }

    return result;
}

} // namespace

std::string generate_dtos(const document& doc, bool use_pmr) {
    std::ostringstream out;
    out << "// Auto-generated DTOs (Data Transfer Objects) from OpenAPI specification\n";
    out << "//\n";
    out << "// This file contains:\n";
    out << "//   - Type definitions for request/response bodies\n";
    out << "//   - Enum types with string conversion functions\n";
    out << "//   - Compile-time metadata for validation constraints\n";
    out << "//   - Zero-copy arena allocators for high performance\n";
    out << "//\n";
    out << "// All types include metadata structs with validation constraints:\n";
    out << "//   - Required/optional flags\n";
    out << "//   - String length constraints (min_length, max_length)\n";
    out << "//   - Numeric constraints (minimum, maximum, exclusive bounds)\n";
    out << "//   - Array constraints (min_items, max_items, uniqueness)\n";
    out << "//\n";
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
    out << "#include <variant>\n\n";

    // Generate enums first
    out << "// ============================================================\n";
    out << "// Enum Types\n";
    out << "// ============================================================\n\n";
    for (const auto& schema : doc.schemas) {
        generate_enum_for_schema(out, doc, schema);
    }

    // Topologically sort schemas so dependencies are defined before dependents
    auto sorted_indices = topological_sort_schemas(doc, use_pmr);

    // Then generate DTOs in dependency order
    out << "// ============================================================\n";
    out << "// Data Transfer Objects (DTOs)\n";
    out << "// ============================================================\n\n";
    for (size_t idx : sorted_indices) {
        generate_dto_for_schema(out, doc, doc.schemas[idx], use_pmr);
    }

    return out.str();
}

} // namespace katana_gen
