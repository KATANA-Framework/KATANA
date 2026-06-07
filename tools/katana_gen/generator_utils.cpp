#include "generator.hpp"

#include "katana/core/arena.hpp"

#include <cctype>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace katana_gen {

std::string inject_namespace(std::string content, const std::string& ns) {
    if (ns.empty()) {
        return content;
    }
    // Insertion point: after the leading run of comment / blank / #pragma / #include lines, so
    // includes and the pragma stay at global scope and everything else lands inside the namespace.
    size_t pos = 0;
    size_t insert_at = 0;
    while (pos < content.size()) {
        const size_t eol = content.find('\n', pos);
        const size_t line_end = (eol == std::string::npos) ? content.size() : eol + 1;
        std::string_view line(content.data() + pos, line_end - pos);
        std::string_view trimmed = line;
        while (!trimmed.empty() &&
               std::isspace(static_cast<unsigned char>(trimmed.front())) != 0) {
            trimmed.remove_prefix(1);
        }
        // Preamble = blank / comment / preprocessor directive (so #include and #pragma stay at
        // global scope) and top-level `using katana::x;` import declarations (which import a name,
        // no '='). A `using Foo = ...;` alias DOES contain '=' and is a real declaration that
        // must live inside the namespace, so it ends the preamble.
        const bool is_using_import =
            trimmed.starts_with("using ") && trimmed.find('=') == std::string_view::npos;
        const bool preamble = trimmed.empty() || trimmed.starts_with("//") ||
                              trimmed.starts_with("#") || is_using_import;
        if (!preamble) {
            break; // first real code line
        }
        insert_at = line_end;
        pos = line_end;
    }
    content.insert(insert_at, "\nnamespace " + ns + " {\n\n");
    content += "\n}  // namespace " + ns + "\n";
    return content;
}

namespace {

bool is_cpp_keyword(std::string_view name) {
    static const std::unordered_set<std::string_view> keywords = {
        "alignas",       "alignof",     "and",
        "and_eq",        "asm",         "auto",
        "bitand",        "bitor",       "bool",
        "break",         "case",        "catch",
        "char",          "char8_t",     "char16_t",
        "char32_t",      "class",       "compl",
        "concept",       "const",       "consteval",
        "constexpr",     "constinit",   "const_cast",
        "continue",      "co_await",    "co_return",
        "co_yield",      "decltype",    "default",
        "delete",        "do",          "double",
        "dynamic_cast",  "else",        "enum",
        "explicit",      "export",      "extern",
        "false",         "float",       "for",
        "friend",        "goto",        "if",
        "inline",        "int",         "long",
        "mutable",       "namespace",   "new",
        "noexcept",      "not",         "not_eq",
        "nullptr",       "operator",    "or",
        "or_eq",         "private",     "protected",
        "public",        "register",    "reinterpret_cast",
        "requires",      "return",      "short",
        "signed",        "sizeof",      "static",
        "static_assert", "static_cast", "struct",
        "switch",        "template",    "this",
        "thread_local",  "throw",       "true",
        "try",           "typedef",     "typeid",
        "typename",      "union",       "unsigned",
        "using",         "virtual",     "void",
        "volatile",      "wchar_t",     "while",
        "xor",           "xor_eq"};
    return keywords.contains(name);
}

} // namespace

std::string escape_json(std::string_view sv) {
    std::string out;
    out.reserve(sv.size() + 8);
    for (char c : sv) {
        switch (c) {
        case '\"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            out.push_back(c);
            break;
        }
    }
    return out;
}

std::string escape_cpp_string(std::string_view sv) {
    std::string out;
    out.reserve(sv.size() + 8);
    for (char c : sv) {
        switch (c) {
        case '\"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            out.push_back(c);
            break;
        }
    }
    return out;
}

std::string schema_identifier(const document& doc, const katana::openapi::schema* s) {
    if (!s) {
        return "std::monostate";
    }
    if (!s->name.empty()) {
        return std::string(s->name);
    }

    // Context-aware naming: parent + field (e.g., Task.title → Task_Title_t)
    if (!s->parent_context.empty() && !s->field_context.empty()) {
        std::string parent(s->parent_context.begin(), s->parent_context.end());
        std::string field(s->field_context.begin(), s->field_context.end());

        // Capitalize first letter of field name
        if (!field.empty()) {
            field[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(field[0])));
        }

        std::string result = parent + "_" + field + "_t";
        // Sanitize: only alphanumeric and underscore
        for (auto& c : result) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
                c = '_';
            }
        }
        return result;
    }

    // Try to generate meaningful name from description or type
    std::string candidate;
    if (!s->description.empty()) {
        // Use first word of description
        auto desc = std::string_view(s->description);
        auto space_pos = desc.find(' ');
        if (space_pos != std::string_view::npos) {
            desc = desc.substr(0, space_pos);
        }
        candidate = std::string(desc) + "_t";
        // Sanitize: only alphanumeric and underscore
        for (auto& c : candidate) {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
                c = '_';
            }
        }
        if (!candidate.empty() && std::isalpha(static_cast<unsigned char>(candidate[0]))) {
            return candidate;
        }
    }

    // Fallback: use type + index for better readability
    std::string type_prefix;
    switch (s->kind) {
    case katana::openapi::schema_kind::string:
        type_prefix = "String";
        break;
    case katana::openapi::schema_kind::integer:
        type_prefix = "Integer";
        break;
    case katana::openapi::schema_kind::number:
        type_prefix = "Number";
        break;
    case katana::openapi::schema_kind::boolean:
        type_prefix = "Boolean";
        break;
    case katana::openapi::schema_kind::array:
        type_prefix = "Array";
        break;
    case katana::openapi::schema_kind::object:
        type_prefix = "Object";
        break;
    default:
        type_prefix = "Value";
        break;
    }

    for (size_t i = 0; i < doc.schemas.size(); ++i) {
        if (&doc.schemas[i] == s) {
            return type_prefix + "_" + std::to_string(i) + "_t";
        }
    }
    return "Unnamed_t";
}

bool is_free_form_object(const katana::openapi::schema* s) {
    // A `{type: object}` with no declared properties that still allows arbitrary content.
    // Such fields (e.g. a free-form `metadata`) carry real JSON, so they are stored as the
    // raw JSON text rather than the data-less std::monostate. An object explicitly closed
    // with `additionalProperties: false` is a genuinely empty object and stays monostate.
    return s != nullptr && s->kind == katana::openapi::schema_kind::object &&
           s->properties.empty() && s->enum_values.empty() && s->additional_properties_allowed;
}

std::string schema_banner(const document& doc, const katana::openapi::schema& s) {
    using katana::openapi::schema_kind;
    const char* kind = "object";
    switch (s.kind) {
    case schema_kind::object:
        kind = "object";
        break;
    case schema_kind::array:
        kind = "array";
        break;
    case schema_kind::string:
        kind = s.enum_values.empty() ? "string" : "enum";
        break;
    case schema_kind::integer:
        kind = "integer";
        break;
    case schema_kind::number:
        kind = "number";
        break;
    case schema_kind::boolean:
        kind = "boolean";
        break;
    case schema_kind::null_type:
        kind = "null";
        break;
    }
    std::string banner = schema_identifier(doc, &s);
    banner += " — ";
    banner += kind;
    if (!s.parent_context.empty() && !s.field_context.empty()) {
        banner += ", field ";
        banner.append(s.parent_context.begin(), s.parent_context.end());
        banner += '.';
        banner.append(s.field_context.begin(), s.field_context.end());
    } else if (s.kind == schema_kind::object && !s.properties.empty()) {
        banner += ", " + std::to_string(s.properties.size()) + " field(s)";
    }
    // Provenance: trace this generated type back to the spec line it came from.
    if (s.source_line != 0) {
        banner += "  ← ";
        if (!doc.source_file.empty()) {
            banner.append(doc.source_file.begin(), doc.source_file.end());
        } else {
            banner += "spec";
        }
        banner += ':';
        banner += std::to_string(s.source_line);
    }
    return banner;
}

std::string to_snake_case(std::string_view id) {
    std::string method_name;
    method_name.reserve(id.size() + 4);
    for (char c : id) {
        if (std::isupper(static_cast<unsigned char>(c)) && !method_name.empty()) {
            method_name += '_';
            method_name += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        } else {
            method_name += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }
    return method_name;
}

std::string sanitize_identifier(std::string_view name) {
    std::string id;
    id.reserve(name.size() + 2);
    for (char c : name) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            id.push_back(c);
        } else if (c == '-') {
            id.push_back('_');
        } else {
            id.push_back('_');
        }
    }
    if (id.empty() || std::isdigit(static_cast<unsigned char>(id.front()))) {
        id.insert(id.begin(), '_');
    }
    if (is_cpp_keyword(id)) {
        id.push_back('_');
    }
    return id;
}

std::string property_member_identifier(std::string_view name) {
    return sanitize_identifier(name);
}

std::string metadata_constant_identifier(std::string_view name) {
    std::string id = sanitize_identifier(name);
    for (auto& c : id) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return id;
}

bool is_optional_property(const katana::openapi::property& prop) {
    return !prop.required || (prop.type && prop.type->nullable);
}

std::string method_enum_literal(katana::http::method m) {
    using katana::http::method;
    switch (m) {
    case method::get:
        return "get";
    case method::post:
        return "post";
    case method::put:
        return "put";
    case method::del:
        return "del";
    case method::patch:
        return "patch";
    case method::head:
        return "head";
    case method::options:
        return "options";
    default:
        return "unknown";
    }
}

namespace {

// Propagate naming context (parent schema name + field) into inline child schemas —
// array items, object properties, additionalProperties — so they end up named
// `<Parent>_<Field>_t` instead of an opaque `schema_N`. The OpenAPI loader already
// sets field_context for object properties, but NOT for array items, which is why a
// body like `array<number>` produced a bare `using schema = double;`. Cycle-safe via
// the visited set; never clobbers a name or context that already exists.
void propagate_inline_naming_context(document& doc,
                                     const katana::openapi::schema* s,
                                     std::string_view parent,
                                     std::string_view field,
                                     std::unordered_set<const katana::openapi::schema*>& visited) {
    if (s == nullptr || !visited.insert(s).second) {
        return;
    }
    // Leave enums alone: they have a dedicated, prettier naming pass (parent minus
    // _request/_response + field, e.g. text_transform_operation) that only fires when the
    // enum still has a generic name. Filling its context here would pre-empt that and
    // rename it to the verbose <Parent>_<Field>_t form, churning stable public names.
    if (!s->enum_values.empty()) {
        return;
    }
    auto* writable = const_cast<katana::openapi::schema*>(s);
    // Fill in whichever half of the context is missing. The loader already sets
    // field_context="item" on array items but leaves parent_context empty (an inline
    // array has no name at parse time), which left schema_identifier one piece short and
    // sent the item to the `schema_N` fallback. Set each piece independently so a
    // half-contexted node is completed rather than skipped.
    if (s->name.empty()) {
        if (s->parent_context.empty() && !parent.empty()) {
            writable->parent_context = katana::arena_string<>(
                parent.begin(), parent.end(), katana::arena_allocator<char>(doc.arena_));
        }
        if (s->field_context.empty() && !field.empty()) {
            writable->field_context = katana::arena_string<>(
                field.begin(), field.end(), katana::arena_allocator<char>(doc.arena_));
        }
    }

    // The name children should treat as their parent for context purposes.
    std::string self;
    if (!s->name.empty()) {
        self.assign(s->name.begin(), s->name.end());
    } else if (!s->parent_context.empty() && !s->field_context.empty()) {
        self.append(s->parent_context.begin(), s->parent_context.end());
        self.push_back('_');
        self.append(s->field_context.begin(), s->field_context.end());
    }
    if (self.empty()) {
        return; // no usable parent name; leave any children to the fallback
    }

    if (s->items != nullptr) {
        propagate_inline_naming_context(doc, s->items, self, "item", visited);
    }
    for (const auto& prop : s->properties) {
        if (prop.type != nullptr) {
            propagate_inline_naming_context(
                doc, prop.type, self,
                std::string_view(prop.name.data(), prop.name.size()), visited);
        }
    }
    if (s->additional_properties != nullptr) {
        propagate_inline_naming_context(doc, s->additional_properties, self, "value", visited);
    }
}

} // namespace

void ensure_inline_schema_names(document& doc, std::string_view naming_style) {
    std::unordered_set<std::string> used;
    used.reserve(doc.schemas.size() * 2);
    for (const auto& s : doc.schemas) {
        if (!s.name.empty()) {
            used.insert(std::string(s.name));
        }
    }

    const bool flat_naming =
        naming_style == "flat" || naming_style == "short" || naming_style == "sequential";
    size_t inline_counter = 0;

    auto unique_name = [&](std::string base) {
        base = sanitize_identifier(base);
        if (base.empty()) {
            base = "schema";
        }
        std::string candidate = base;
        int idx = 0;
        while (used.contains(candidate)) {
            candidate = base + "_" + std::to_string(++idx);
        }
        used.insert(candidate);
        return candidate;
    };

    auto next_flat_name = [&]() {
        return std::string("InlineSchema") + std::to_string(++inline_counter);
    };

    auto assign_if_empty = [&](const katana::openapi::schema* s, auto&& base_fn) {
        if (!s || !s->name.empty()) {
            return;
        }

        // Context-aware naming: if parent_context and field_context are set, use them
        if (!s->parent_context.empty() && !s->field_context.empty()) {
            std::string parent(s->parent_context.begin(), s->parent_context.end());
            std::string field(s->field_context.begin(), s->field_context.end());
            // Capitalize first letter of field name
            if (!field.empty()) {
                field[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(field[0])));
            }
            std::string label = unique_name(parent + "_" + field + "_t");
            auto* writable = const_cast<katana::openapi::schema*>(s);
            writable->name = katana::arena_string<>(
                label.begin(), label.end(), katana::arena_allocator<char>(doc.arena_));
            return;
        }

        std::string base = std::forward<decltype(base_fn)>(base_fn)();
        auto label = unique_name(base);
        auto* writable = const_cast<katana::openapi::schema*>(s);
        writable->name = katana::arena_string<>(
            label.begin(), label.end(), katana::arena_allocator<char>(doc.arena_));
    };

    for (auto& path : doc.paths) {
        for (auto& op : path.operations) {
            std::string op_base =
                !op.operation_id.empty()
                    ? sanitize_identifier(op.operation_id)
                    : sanitize_identifier(std::string(katana::http::method_to_string(op.method)) +
                                          std::string("_") + std::string(path.path));

            if (op.body) {
                int media_idx = 0;
                for (auto& media : op.body->content) {
                    int current_media = media_idx++;
                    assign_if_empty(media.type, [&]() {
                        if (flat_naming) {
                            return next_flat_name();
                        }
                        // Better naming: use "_request" instead of "_body_0"
                        // Only add media index if there are multiple media types
                        if (op.body->content.size() == 1) {
                            return op_base + "_request";
                        }
                        return op_base + "_body_" + std::to_string(current_media);
                    });
                }
            }

            for (auto& param : op.parameters) {
                assign_if_empty(param.type, [&]() {
                    if (flat_naming) {
                        return next_flat_name();
                    }
                    return op_base + "_param_" + sanitize_identifier(param.name);
                });
            }

            for (auto& resp : op.responses) {
                int media_idx = 0;
                for (auto& media : resp.content) {
                    std::string status = resp.is_default ? "default" : std::to_string(resp.status);
                    int current_media = media_idx++;
                    assign_if_empty(media.type, [&]() {
                        if (flat_naming) {
                            return next_flat_name();
                        }
                        // Better naming: use "_response" for 200 OK, or "_response_<code>" for
                        // other codes Only add media index if there are multiple media types
                        std::string suffix;
                        if (resp.status == 200 && !resp.is_default) {
                            suffix = "_response"; // Common case: 200 OK
                        } else {
                            suffix = "_response_" + status; // Other codes: 404, 500, etc.
                        }

                        if (resp.content.size() > 1) {
                            suffix += "_" + std::to_string(current_media);
                        }

                        return op_base + suffix;
                    });
                }
            }
        }
    }

    // Propagate naming context into inline children (array items especially) before the
    // fallback, so nested schemas get `<Parent>_<Field>_t` names rather than `schema_N`.
    {
        std::unordered_set<const katana::openapi::schema*> visited;
        for (auto& s : doc.schemas) {
            if (!s.name.empty()) {
                propagate_inline_naming_context(doc, &s, {}, {}, visited);
            }
        }
    }

    // Fallback for any remaining unnamed schemas
    for (auto& s : doc.schemas) {
        assign_if_empty(&s, [&]() {
            if (flat_naming) {
                return next_flat_name();
            }
            return std::string("schema");
        });
    }

    // Second pass: Set parent_context for property schemas now that parent schemas have names
    // Walk through all schemas with properties and set parent_context on property type schemas
    for (auto& parent_schema : doc.schemas) {
        if (parent_schema.name.empty() || parent_schema.properties.empty()) {
            continue;
        }

        std::string parent_name(parent_schema.name.begin(), parent_schema.name.end());

        for (auto& prop : parent_schema.properties) {
            if (!prop.type || prop.type->name.empty()) {
                continue;
            }

            // Set parent_context for this property's schema
            auto* writable = const_cast<katana::openapi::schema*>(prop.type);
            if (writable->parent_context.empty()) {
                writable->parent_context =
                    katana::arena_string<>(parent_name.begin(),
                                           parent_name.end(),
                                           katana::arena_allocator<char>(doc.arena_));
            }
        }
    }

    // Third pass: improve names for schemas with context (property schemas, enum fields, etc.)
    for (auto& s : doc.schemas) {
        if (s.name.empty()) {
            continue;
        }

        // Check if this schema has context information but a generic "schema_N" name
        std::string current_name(s.name.begin(), s.name.end());
        bool is_generic_schema_name = (current_name.find("schema_") == 0) ||
                                      (current_name.find("schema") == 0) ||
                                      (current_name.find("InlineSchema") == 0);

        if (is_generic_schema_name && !s.parent_context.empty() && !s.field_context.empty()) {
            // This schema has context - generate a better name
            std::string parent(s.parent_context.begin(), s.parent_context.end());
            std::string field(s.field_context.begin(), s.field_context.end());

            // For enum schemas, use a clean name without "_t" suffix
            // e.g., "text_transform_request" + "operation" = "text_transform_operation"
            std::string new_base;
            if (s.kind == katana::openapi::schema_kind::string && !s.enum_values.empty()) {
                // Enum: use parent (without _request suffix if present) + field
                // text_transform_request + operation → text_transform_operation
                if (parent.size() >= 8 && parent.substr(parent.length() - 8) == "_request") {
                    parent = parent.substr(0, parent.length() - 8); // Remove "_request"
                } else if (parent.size() >= 9 &&
                           parent.substr(parent.length() - 9) == "_response") {
                    parent = parent.substr(0, parent.length() - 9); // Remove "_response"
                }
                new_base = parent + "_" + field;
            } else {
                // Other types: keep full parent name
                // Capitalize first letter of field
                if (!field.empty()) {
                    field[0] =
                        static_cast<char>(std::toupper(static_cast<unsigned char>(field[0])));
                }
                new_base = parent + "_" + field;
            }

            auto new_name = unique_name(new_base);
            auto* writable = const_cast<katana::openapi::schema*>(&s);
            writable->name = katana::arena_string<>(
                new_name.begin(), new_name.end(), katana::arena_allocator<char>(doc.arena_));
        }
    }
}

} // namespace katana_gen
