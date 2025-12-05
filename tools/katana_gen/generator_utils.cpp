#include "generator.hpp"

#include "katana/core/arena.hpp"

#include <cctype>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace katana_gen {

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
    return id;
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
                        return op_base + "_resp_" + status + "_" + std::to_string(current_media);
                    });
                }
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
}

} // namespace katana_gen
