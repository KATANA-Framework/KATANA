#include "generator.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace katana_gen {

std::string generate_router_table(const document& doc) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include \"katana/core/http.hpp\"\n";
    out << "#include \"katana/core/router.hpp\"\n";
    out << "#include <array>\n";
    out << "#include <span>\n";
    out << "#include <string_view>\n\n";
    out << "namespace generated {\n\n";
    out << "struct content_type_info {\n";
    out << "    std::string_view mime_type;\n";
    out << "};\n\n";
    out << "struct route_entry {\n";
    out << "    std::string_view path;\n";
    out << "    katana::http::method method;\n";
    out << "    std::string_view operation_id;\n";
    out << "    std::span<const content_type_info> consumes;\n";
    out << "    std::span<const content_type_info> produces;\n";
    out << "};\n\n";

    // Generate content type arrays for each operation
    size_t route_idx = 0;
    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            // Request content types (consumes)
            if (op.body && !op.body->content.empty()) {
                out << "inline constexpr content_type_info route_" << route_idx
                    << "_consumes[] = {\n";
                for (const auto& media : op.body->content) {
                    out << "    {\"" << media.content_type << "\"},\n";
                }
                out << "};\n\n";
            }

            // Response content types (produces)
            bool has_response_content = false;
            for (const auto& resp : op.responses) {
                if (!resp.content.empty()) {
                    has_response_content = true;
                    break;
                }
            }

            if (has_response_content) {
                out << "inline constexpr content_type_info route_" << route_idx
                    << "_produces[] = {\n";
                // Collect unique content types from all responses
                std::vector<std::string_view> unique_types;
                for (const auto& resp : op.responses) {
                    for (const auto& media : resp.content) {
                        bool found = false;
                        std::string_view media_type_sv(media.content_type.data(),
                                                       media.content_type.size());
                        for (const auto& existing : unique_types) {
                            if (existing == media_type_sv) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            unique_types.push_back(media_type_sv);
                        }
                    }
                }
                for (const auto& type : unique_types) {
                    out << "    {\"" << type << "\"},\n";
                }
                out << "};\n\n";
            }
            ++route_idx;
        }
    }

    out << "inline constexpr route_entry routes[] = {\n";
    route_idx = 0;
    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            out << "    {\"" << path.path << "\", ";
            out << "katana::http::method::" << method_enum_literal(op.method);
            out << ", \"" << op.operation_id << "\", ";

            // Consumes
            if (op.body && !op.body->content.empty()) {
                out << "route_" << route_idx << "_consumes, ";
            } else {
                out << "{}, ";
            }

            // Produces
            bool has_response_content = false;
            for (const auto& resp : op.responses) {
                if (!resp.content.empty()) {
                    has_response_content = true;
                    break;
                }
            }
            if (has_response_content) {
                out << "route_" << route_idx << "_produces";
            } else {
                out << "{}";
            }

            out << "},\n";
            ++route_idx;
        }
    }

    out << "};\n\n";
    out << "inline constexpr size_t route_count = sizeof(routes) / sizeof(routes[0]);\n\n";

    // Generate compile-time metadata for type checking
    out << "// Compile-time route metadata for type safety\n";
    out << "namespace route_metadata {\n";

    route_idx = 0;
    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            if (!op.operation_id.empty()) {
                out << "    // " << op.operation_id << ": "
                    << katana::http::method_to_string(op.method) << " " << path.path << "\n";
                out << "    struct " << op.operation_id << "_metadata {\n";
                out << "        static constexpr std::string_view path = \"" << path.path
                    << "\";\n";
                out << "        static constexpr katana::http::method method = "
                       "katana::http::method::"
                    << method_enum_literal(op.method) << ";\n";
                out << "        static constexpr std::string_view operation_id = \""
                    << op.operation_id << "\";\n";

                // Add parameter count
                size_t path_param_count = 0;
                for (const auto& param : op.parameters) {
                    if (param.in == katana::openapi::param_location::path) {
                        path_param_count++;
                    }
                }
                out << "        static constexpr size_t path_param_count = " << path_param_count
                    << ";\n";
                out << "        static constexpr bool has_request_body = "
                    << (op.body ? "true" : "false") << ";\n";
                out << "    };\n\n";
            }
            ++route_idx;
        }
    }

    out << "} // namespace route_metadata\n\n";

    // Add compile-time validation
    out << "// Compile-time validations\n";
    out << "static_assert(route_count > 0, \"At least one route must be defined\");\n";

    out << "} // namespace generated\n";
    return out.str();
}

std::string generate_router_bindings(const document& doc) {
    std::ostringstream out;
    out << "// Auto-generated router bindings from OpenAPI specification\n";
    out << "// \n";
    out << "// Performance characteristics:\n";
    out << "//   - Compile-time route parsing (constexpr path_pattern)\n";
    out << "//   - Zero-copy parameter extraction (string_view)\n";
    out << "//   - Fast paths for common Accept headers (3 levels)\n";
    out << "//   - Single allocation for validation errors with reserve\n";
    out << "//   - Arena-based JSON parsing (request-scoped memory)\n";
    out << "//   - Thread-local handler context (reactor-per-core compatible)\n";
    out << "//   - std::from_chars for fastest integer parsing\n";
    out << "//   - Inplace functions (160 bytes SBO, no heap allocation)\n";
    out << "// \n";
    out << "// Hot path optimizations:\n";
    out << "//   1. Content negotiation: O(1) for */*, single type, or exact match\n";
    out << "//   2. Validation: Only on error path, single allocation\n";
    out << "//   3. Parameter parsing: Zero-copy with std::from_chars\n";
    out << "//   4. Handler context: RAII scope guard (zero-cost abstraction)\n";
    out << "#pragma once\n\n";
    out << "#include \"katana/core/router.hpp\"\n";
    out << "#include \"katana/core/problem.hpp\"\n";
    out << "#include \"katana/core/serde.hpp\"\n";
    out << "#include \"katana/core/handler_context.hpp\"\n";
    out << "#include \"katana/core/http_server.hpp\"\n";
    out << "#include \"generated_routes.hpp\"\n";
    out << "#include \"generated_handlers.hpp\"\n";
    out << "#include \"generated_json.hpp\"\n";
    out << "#include \"generated_validators.hpp\"\n";
    out << "#include <array>\n";
    out << "#include <charconv>\n";
    out << "#include <optional>\n";
    out << "#include <variant>\n";
    out << "#include <span>\n";
    out << "#include <string_view>\n";
    out << "\n";
    out << "namespace generated {\n\n";

    out << "inline std::optional<std::string_view> query_param(std::string_view uri, "
           "std::string_view key) {\n";
    out << "    auto qpos = uri.find('?');\n";
    out << "    if (qpos == std::string_view::npos) return std::nullopt;\n";
    out << "    auto query = uri.substr(qpos + 1);\n";
    out << "    while (!query.empty()) {\n";
    out << "        auto amp = query.find('&');\n";
    out << "        auto part = query.substr(0, amp);\n";
    out << "        auto eq = part.find('=');\n";
    out << "        auto name = part.substr(0, eq);\n";
    out << "        if (name == key) {\n";
    out << "            if (eq == std::string_view::npos) return std::string_view{};\n";
    out << "            return part.substr(eq + 1);\n";
    out << "        }\n";
    out << "        if (amp == std::string_view::npos) break;\n";
    out << "        query.remove_prefix(amp + 1);\n";
    out << "    }\n";
    out << "    return std::nullopt;\n";
    out << "}\n\n";

    out << "inline std::optional<std::string_view> cookie_param(const katana::http::request& req, "
           "std::string_view key) {\n";
    out << "    auto cookie = req.headers.get(\"Cookie\");\n";
    out << "    if (!cookie) return std::nullopt;\n";
    out << "    std::string_view rest = *cookie;\n";
    out << "    while (!rest.empty()) {\n";
    out << "        auto sep = rest.find(';');\n";
    out << "        auto token = rest.substr(0, sep);\n";
    out << "        if (sep != std::string_view::npos) rest.remove_prefix(sep + 1);\n";
    out << "        auto eq = token.find('=');\n";
    out << "        if (eq == std::string_view::npos) continue;\n";
    out << "        auto name = katana::serde::trim_view(token.substr(0, eq));\n";
    out << "        auto val = katana::serde::trim_view(token.substr(eq + 1));\n";
    out << "        if (name == key) return val;\n";
    out << "        if (sep == std::string_view::npos) break;\n";
    out << "    }\n";
    out << "    return std::nullopt;\n";
    out << "}\n\n";

    out << "inline std::optional<size_t> find_content_type(std::optional<std::string_view> "
           "header,\n"
           "                                               std::span<const content_type_info> "
           "allowed) {\n";
    out << "    if (allowed.empty()) return std::nullopt;\n";
    out << "    if (!header) return std::nullopt;\n";
    out << "    for (size_t i = 0; i < allowed.size(); ++i) {\n";
    out << "        auto& ct = allowed[i];\n";
    out << "        if (header->substr(0, ct.mime_type.size()) == ct.mime_type) return i;\n";
    out << "    }\n";
    out << "    return std::nullopt;\n";
    out << "}\n\n";

    out << "inline std::optional<std::string_view> negotiate_response_type(\n"
           "    const katana::http::request& req, std::span<const content_type_info> produces) {\n";
    out << "    if (produces.empty()) return std::nullopt;\n";
    out << "    auto accept = req.headers.get(\"Accept\");\n";
    out << "    // Fast path: no Accept header or */*, return first\n";
    out << "    if (!accept || accept->empty() || *accept == \"*/*\") {\n";
    out << "        return produces.front().mime_type;\n";
    out << "    }\n";
    out << "    // Fast path: exact match with first content type (common case)\n";
    out << "    if (produces.size() == 1 && *accept == produces.front().mime_type) {\n";
    out << "        return produces.front().mime_type;\n";
    out << "    }\n";
    out << "    // Fast path: common exact matches without quality values\n";
    out << "    if (accept->find(',') == std::string_view::npos && accept->find(';') == "
           "std::string_view::npos) {\n";
    out << "        // Single value without q-factor\n";
    out << "        for (auto& ct : produces) {\n";
    out << "            if (ct.mime_type == *accept) return ct.mime_type;\n";
    out << "        }\n";
    out << "    }\n";
    out << "    // Slow path: full parsing with quality values and wildcards\n";
    out << "    std::string_view remaining = *accept;\n";
    out << "    while (!remaining.empty()) {\n";
    out << "        auto comma = remaining.find(',');\n";
    out << "        auto token = comma == std::string_view::npos ? remaining : remaining.substr(0, "
           "comma);\n";
    out << "        if (comma == std::string_view::npos) remaining = {};\n";
    out << "        else remaining = remaining.substr(comma + 1);\n";
    out << "        token = katana::serde::trim_view(token);\n";
    out << "        if (token.empty()) continue;\n";
    out << "        auto semicolon = token.find(';');\n";
    out << "        if (semicolon != std::string_view::npos) token = "
           "katana::serde::trim_view(token.substr(0, semicolon));\n";
    out << "        if (token == \"*/*\") return produces.front().mime_type;\n";
    out << "        if (token.size() > 2 && token.substr(token.size() - 2) == \"/*\") {\n";
    out << "            auto prefix = token.substr(0, token.size() - 1); // keep trailing '/'\n";
    out << "            for (auto& ct : produces) {\n";
    out << "                if (ct.mime_type.starts_with(prefix)) {\n";
    out << "                    return ct.mime_type;\n";
    out << "                }\n";
    out << "            }\n";
    out << "        } else {\n";
    out << "            for (auto& ct : produces) {\n";
    out << "                if (ct.mime_type == token) return ct.mime_type;\n";
    out << "            }\n";
    out << "        }\n";
    out << "    }\n";
    out << "    return std::nullopt;\n";
    out << "}\n\n";

    // Add helper for formatting validation errors (reduces code duplication)
    out << "// Helper to format validation errors into problem details\n";
    out << "inline katana::http::response format_validation_error(const validation_error& err) {\n";
    out << "    std::string error_msg;\n";
    out << "    error_msg.reserve(err.field.size() + err.message().size() + 2);\n";
    out << "    error_msg.append(err.field);\n";
    out << "    error_msg.append(\": \");\n";
    out << "    error_msg.append(err.message());\n";
    out << "    return katana::http::response::error(\n";
    out << "        katana::problem_details::bad_request(std::move(error_msg))\n";
    out << "    );\n";
    out << "}\n\n";

    out << "inline const katana::http::router& make_router(api_handler& handler) {\n";
    out << "    using katana::http::route_entry;\n";
    out << "    using katana::http::path_pattern;\n";
    out << "    using katana::http::handler_fn;\n";
    out << "    static std::array<route_entry, route_count> route_entries = {\n";

    size_t route_idx = 0;
    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            if (op.operation_id.empty()) {
                continue;
            }

            const auto method_name = to_snake_case(op.operation_id);
            bool has_response_content = false;
            for (const auto& resp : op.responses) {
                if (!resp.content.empty()) {
                    has_response_content = true;
                    break;
                }
            }
            std::vector<std::string> body_schema_names;
            if (op.body && !op.body->content.empty()) {
                for (const auto& media : op.body->content) {
                    auto media_name = schema_identifier(doc, media.type);
                    if (!media_name.empty() &&
                        std::find(body_schema_names.begin(), body_schema_names.end(), media_name) ==
                            body_schema_names.end()) {
                        body_schema_names.push_back(media_name);
                    }
                }
            }
            bool has_body = op.body && !op.body->content.empty();
            bool body_is_variant = body_schema_names.size() > 1;
            std::string body_type_expr;
            if (has_body) {
                if (body_is_variant) {
                    body_type_expr = "std::variant<";
                    for (size_t i = 0; i < body_schema_names.size(); ++i) {
                        if (i > 0) {
                            body_type_expr += ", ";
                        }
                        body_type_expr += body_schema_names[i];
                    }
                    body_type_expr += ">";
                } else if (!body_schema_names.empty()) {
                    body_type_expr = body_schema_names.front();
                }
            }
            out << "        route_entry{katana::http::method::" << method_enum_literal(op.method)
                << ",\n";
            out << "                   katana::http::path_pattern::from_literal<\"" << path.path
                << "\">(),\n";
            out << "                   handler_fn([&handler](const katana::http::request& req, "
                   "katana::http::request_context& ctx) -> katana::result<katana::http::response> "
                   "{\n";

            // Content negotiation
            if (has_response_content) {
                out << "                       auto negotiated_response = "
                       "negotiate_response_type(req, "
                    << "route_" << route_idx << "_produces);\n";
                out << "                       if (!negotiated_response) {\n";
                out << "                           return katana::http::response::error("
                       "katana::problem_details::not_acceptable(\"unsupported Accept header\"));\n";
                out << "                       }\n";
            }

            // Path params extraction
            for (const auto& param : op.parameters) {
                if (param.in != katana::openapi::param_location::path || !param.type) {
                    continue;
                }
                auto param_ident = sanitize_identifier(param.name);
                out << "                       auto p_" << param_ident << " = ctx.params.get(\""
                    << param.name << "\");\n";
                out << "                       if (!p_" << param_ident
                    << ") return "
                       "katana::http::response::error(katana::problem_details::bad_request("
                       "\"missing "
                       "path param "
                    << param.name << "\"));\n";
                switch (param.type->kind) {
                case katana::openapi::schema_kind::integer:
                    out << "                       int64_t " << param_ident << " = 0;\n";
                    out << "                       {\n";
                    out << "                           auto [ptr, ec] = std::from_chars(p_"
                        << param_ident << "->data(), p_" << param_ident << "->data() + p_"
                        << param_ident << "->size(), " << param_ident << ");\n";
                    out << "                           if (ec != std::errc()) return "
                           "katana::http::response::error("
                           "katana::problem_details::bad_request(\"invalid path param "
                        << param.name << "\"));\n";
                    out << "                       }\n";
                    break;
                case katana::openapi::schema_kind::number:
                    out << "                       double " << param_ident << " = 0.0;\n";
                    out << "                       {\n";
                    out << "                           char* endp = nullptr;\n";
                    out << "                           " << param_ident << " = std::strtod(p_"
                        << param_ident << "->data(), &endp);\n";
                    out << "                           if (endp == p_" << param_ident
                        << "->data()) return "
                           "katana::http::response::error(katana::problem_details::bad_request("
                           "\"invalid path param "
                        << param.name << "\"));\n";
                    out << "                       }\n";
                    break;
                case katana::openapi::schema_kind::boolean:
                    out << "                       bool " << param_ident << " = (*p_" << param_ident
                        << " == \"true\");\n";
                    break;
                default:
                    out << "                       auto " << param_ident << " = *p_" << param_ident
                        << ";\n";
                    break;
                }
            }

            // Query/header/cookie params
            for (const auto& param : op.parameters) {
                if (param.in == katana::openapi::param_location::path || !param.type) {
                    continue;
                }

                std::string source_expr;
                auto param_ident = sanitize_identifier(param.name);
                if (param.in == katana::openapi::param_location::query) {
                    source_expr = "query_param(req.uri, \"" + std::string(param.name) + "\")";
                } else if (param.in == katana::openapi::param_location::header) {
                    source_expr = "req.headers.get(\"" + std::string(param.name) + "\")";
                } else if (param.in == katana::openapi::param_location::cookie) {
                    source_expr = "cookie_param(req, \"" + std::string(param.name) + "\")";
                }

                out << "                       auto p_" << param_ident << " = " << source_expr
                    << ";\n";
                if (param.required) {
                    out << "                       if (!p_" << param_ident
                        << ") return katana::http::response::error("
                           "katana::problem_details::bad_request(\"missing param "
                        << param.name << "\"));\n";
                }

                const bool optional_param = !param.required;
                switch (param.type->kind) {
                case katana::openapi::schema_kind::integer:
                    if (optional_param) {
                        out << "                       std::optional<int64_t> " << param_ident
                            << ";\n";
                        out << "                       if (p_" << param_ident << ") {\n";
                        out << "                           int64_t tmp = 0;\n";
                        out << "                           auto [ptr, ec] = std::from_chars(p_"
                            << param_ident << "->data(), p_" << param_ident << "->data() + p_"
                            << param_ident << "->size(), tmp);\n";
                        out << "                           if (ec != std::errc()) return "
                               "katana::http::response::error(katana::problem_details::bad_request("
                               "\"invalid param "
                            << param.name << "\"));\n";
                        out << "                           " << param_ident << " = tmp;\n";
                        out << "                       }\n";
                    } else {
                        out << "                       int64_t " << param_ident << " = 0;\n";
                        out << "                       if (p_" << param_ident << ") {\n";
                        out << "                           auto [ptr, ec] = std::from_chars(p_"
                            << param_ident << "->data(), p_" << param_ident << "->data() + p_"
                            << param_ident << "->size(), " << param_ident << ");\n";
                        out << "                           if (ec != std::errc()) return "
                               "katana::http::response::error(katana::problem_details::bad_request("
                               "\"invalid param "
                            << param.name << "\"));\n";
                        out << "                       }\n";
                    }
                    break;
                case katana::openapi::schema_kind::number:
                    if (optional_param) {
                        out << "                       std::optional<double> " << param_ident
                            << ";\n";
                        out << "                       if (p_" << param_ident << ") {\n";
                        out << "                           char* endp = nullptr;\n";
                        out << "                           double tmp = std::strtod(p_"
                            << param_ident << "->data(), &endp);\n";
                        out << "                           if (endp == p_" << param_ident
                            << "->data()) return "
                               "katana::http::response::error(katana::problem_details::bad_request("
                               "\"invalid param "
                            << param.name << "\"));\n";
                        out << "                           " << param_ident << " = tmp;\n";
                        out << "                       }\n";
                    } else {
                        out << "                       double " << param_ident << " = 0.0;\n";
                        out << "                       if (p_" << param_ident << ") {\n";
                        out << "                           char* endp = nullptr;\n";
                        out << "                           " << param_ident << " = std::strtod(p_"
                            << param_ident << "->data(), &endp);\n";
                        out << "                           if (endp == p_" << param_ident
                            << "->data()) return "
                               "katana::http::response::error(katana::problem_details::bad_request("
                               "\"invalid param "
                            << param.name << "\"));\n";
                        out << "                       }\n";
                    }
                    break;
                case katana::openapi::schema_kind::boolean:
                    if (optional_param) {
                        out << "                       std::optional<bool> " << param_ident
                            << ";\n";
                        out << "                       if (p_" << param_ident << ") {\n";
                        out << "                           if (*p_" << param_ident
                            << " == \"true\") " << param_ident << " = true;\n";
                        out << "                           else if (*p_" << param_ident
                            << " == \"false\") " << param_ident << " = false;\n";
                        out << "                           else return "
                               "katana::http::response::error(katana::problem_details::bad_request("
                               "\"invalid param "
                            << param.name << "\"));\n";
                        out << "                       }\n";
                    } else {
                        out << "                       bool " << param_ident << " = false;\n";
                        out << "                       if (!p_" << param_ident << ") {\n";
                        out << "                           return "
                               "katana::http::response::error(katana::problem_details::bad_request("
                               "\"missing param "
                            << param.name << "\"));\n";
                        out << "                       }\n";
                        out << "                       if (*p_" << param_ident << " == \"true\") "
                            << param_ident << " = true;\n";
                        out << "                       else if (*p_" << param_ident
                            << " == \"false\") " << param_ident << " = false;\n";
                        out << "                       else return "
                               "katana::http::response::error(katana::problem_details::bad_request("
                               "\"invalid param "
                            << param.name << "\"));\n";
                    }
                    break;
                default:
                    if (optional_param) {
                        out << "                       std::optional<std::string_view> "
                            << param_ident << " = std::nullopt;\n";
                        out << "                       if (p_" << param_ident << ") " << param_ident
                            << " = *p_" << param_ident << ";\n";
                    } else {
                        out << "                       auto " << param_ident << " = p_"
                            << param_ident << " ? *p_" << param_ident << " : std::string_view{};\n";
                    }
                    break;
                }
            }

            // Request body parsing
            if (has_body) {
                out << "                       auto matched_ct = find_content_type(req.headers.get("
                       "\"Content-Type\"), route_"
                    << route_idx << "_consumes);\n";
                out << "                       if (!matched_ct) return "
                       "katana::http::response::error(katana::problem_details::unsupported_media_"
                       "type("
                       "\"unsupported Content-Type\"));\n";
                if (body_is_variant) {
                    out << "                       std::optional<" << body_type_expr
                        << "> parsed_body;\n";
                } else if (!body_type_expr.empty()) {
                    out << "                       std::optional<" << body_type_expr
                        << "> parsed_body;\n";
                }
                out << "                       switch (*matched_ct) {\n";
                for (size_t media_idx = 0; media_idx < (op.body ? op.body->content.size() : 0);
                     ++media_idx) {
                    const auto& media = op.body->content[media_idx];
                    auto media_name = schema_identifier(doc, media.type);
                    out << "                       case " << media_idx << ": {\n";
                    if (!media_name.empty()) {
                        out << "                           auto candidate = parse_" << media_name
                            << "(req.body, &ctx.arena);\n";
                        out << "                           if (!candidate) return "
                               "katana::http::response::error("
                               "katana::problem_details::bad_request(\"invalid request body\"));\n";
                        if (body_is_variant || body_schema_names.size() > 1) {
                            out << "                           parsed_body = *candidate;\n";
                        } else {
                            out << "                           parsed_body = "
                                   "std::move(*candidate);\n";
                        }
                    } else {
                        out << "                           return "
                               "katana::http::response::error(katana::problem_details::unsupported_"
                               "media_type("
                               "\"unsupported Content-Type\"));\n";
                    }
                    out << "                           break;\n";
                    out << "                       }\n";
                }
                out << "                       default:\n";
                out << "                           return "
                       "katana::http::response::error(katana::problem_details::unsupported_media_"
                       "type("
                       "\"unsupported Content-Type\"));\n";
                out << "                       }\n";

                // Automatic validation after parsing
                if (body_is_variant) {
                    // For variant types, validate using std::visit
                    out << "                       // Automatic validation (optimized)\n";
                    out << "                       auto validation_result = std::visit([](const "
                           "auto& body_val) -> std::optional<std::string> {\n";
                    out << "                           using T = "
                           "std::decay_t<decltype(body_val)>;\n";
                    for (const auto& schema_name : body_schema_names) {
                        out << "                           if constexpr (std::is_same_v<T, "
                            << schema_name << ">) {\n";
                        out << "                               if (auto err = validate_"
                            << schema_name << "(body_val)) {\n";
                        out << "                                   // Optimized: single allocation "
                               "with reserve\n";
                        out << "                                   std::string msg;\n";
                        out << "                                   msg.reserve(err->field.size() + "
                               "err->message.size() + 2);\n";
                        out << "                                   msg.append(err->field);\n";
                        out << "                                   msg.append(\": \");\n";
                        out << "                                   msg.append(err->message);\n";
                        out << "                                   return msg;\n";
                        out << "                               }\n";
                        out << "                           }\n";
                    }
                    out << "                           return std::nullopt;\n";
                    out << "                       }, *parsed_body);\n";
                    out << "                       if (validation_result) {\n";
                    out << "                           return katana::http::response::error(\n";
                    out << "                               "
                           "katana::problem_details::bad_request(std::move(*validation_result))\n";
                    out << "                           );\n";
                    out << "                       }\n";
                } else if (!body_schema_names.empty()) {
                    // For single type, validate directly
                    std::string schema_name = body_schema_names.front();
                    out << "                       // Automatic validation (optimized: single "
                           "allocation)\n";
                    out << "                       if (auto validation_error = validate_"
                        << schema_name << "(*parsed_body)) {\n";
                    out << "                           return "
                           "format_validation_error(*validation_error);\n";
                    out << "                       }\n";
                }
            }

            // Set handler context scope
            out << "                       // Set handler context for zero-boilerplate access\n";
            out << "                       katana::http::handler_context::scope context_scope(req, "
                   "ctx);\n";
            out << "                       auto generated_response = handler." << method_name
                << "(";

            // path param args
            bool first_arg = true;
            for (const auto& param : op.parameters) {
                if (param.in != katana::openapi::param_location::path || !param.type) {
                    continue;
                }
                if (!first_arg)
                    out << ", ";
                first_arg = false;
                out << sanitize_identifier(param.name);
            }
            // query/header/cookie args
            for (const auto& param : op.parameters) {
                if (param.in == katana::openapi::param_location::path || !param.type) {
                    continue;
                }
                if (!first_arg)
                    out << ", ";
                first_arg = false;
                out << sanitize_identifier(param.name);
            }
            // body arg
            if (has_body) {
                if (!first_arg)
                    out << ", ";
                first_arg = false;
                out << "*parsed_body";
            }
            out << ");\n";
            if (has_response_content) {
                out << "                       if (negotiated_response && "
                       "!generated_response.headers.get(\"Content-Type\")) {\n";
                out << "                           generated_response.set_header(\"Content-Type\", "
                       "*negotiated_response);\n";
                out << "                       }\n";
            }
            out << "                       return generated_response;\n";
            out << "                   })\n";
            out << "        },\n";
            ++route_idx;
        }
    }

    out << "    };\n";
    out << "    static katana::http::router router_instance(route_entries);\n";
    out << "    return router_instance;\n";
    out << "}\n\n";

    // Generate template helper for zero-boilerplate server creation
    out << "// Zero-boilerplate server creation\n";
    out << "// Usage: return generated::serve<MyHandler>(8080);\n";
    out << "template<typename Handler, typename... Args>\n";
    out << "inline auto make_server(Args&&... args) {\n";
    out << "    static Handler handler_instance{std::forward<Args>(args)...};\n";
    out << "    const auto& router = make_router(handler_instance);\n";
    out << "    return katana::http::server(router);\n";
    out << "}\n\n";

    out << "template<typename Handler, typename... Args>\n";
    out << "inline int serve(uint16_t port, Args&&... args) {\n";
    out << "    return make_server<Handler>(std::forward<Args>(args)...)\n";
    out << "        .listen(port)\n";
    out << "        .workers(4)\n";
    out << "        .backlog(1024)\n";
    out << "        .reuseport(true)\n";
    out << "        .run();\n";
    out << "}\n\n";

    out << "} // namespace generated\n";
    return out.str();
}

std::string generate_handler_interfaces(const document& doc) {
    std::ostringstream out;
    out << "// Auto-generated handler interfaces from OpenAPI specification\n";
    out << "// \n";
    out << "// Zero-boilerplate design:\n";
    out << "//   - Clean signatures: response method(params) - no request& or context&\n";
    out << "//   - Automatic validation: schema constraints checked before handler call\n";
    out << "//   - Auto parameter binding: path/query/header/body → typed arguments\n";
    out << "//   - Context access: use katana::http::req(), ctx(), arena() for access\n";
    out << "// \n";
    out << "// Example:\n";
    out << "//   response get_user(int64_t id) override {\n";
    out << "//       auto user = db.find(id, &arena());  // arena() from context\n";
    out << "//       return response::json(serialize_User(user));\n";
    out << "//   }\n";
    out << "#pragma once\n\n";
    out << "#include \"katana/core/http.hpp\"\n";
    out << "#include \"katana/core/router.hpp\"\n";
    out << "#include \"generated_dtos.hpp\"\n";
    out << "#include <string_view>\n";
    out << "#include <optional>\n";
    out << "#include <variant>\n\n";
    out << "using katana::http::request;\n";
    out << "using katana::http::response;\n";
    out << "using katana::http::request_context;\n\n";
    out << "namespace generated {\n\n";

    // Generate handler interface class
    out << "// Base handler interface for all API operations\n";
    out << "// Implement these methods to handle requests - validation is automatic!\n";
    out << "struct api_handler {\n";
    out << "    virtual ~api_handler() = default;\n\n";

    // Generate a handler method for each operation
    for (const auto& path_item : doc.paths) {
        for (const auto& op : path_item.operations) {
            if (op.operation_id.empty()) {
                continue; // Skip operations without operation_id
            }

            std::string method_name = to_snake_case(op.operation_id);

            out << "    // " << katana::http::method_to_string(op.method) << " " << path_item.path
                << "\n";
            if (!op.summary.empty()) {
                out << "    // " << op.summary << "\n";
            }
            // Emit x-katana-* extensions as comments
            if (!op.x_katana_cache.empty()) {
                out << "    // @cache: " << op.x_katana_cache << "\n";
            }
            if (!op.x_katana_alloc.empty()) {
                out << "    // @alloc: " << op.x_katana_alloc << "\n";
            }
            if (!op.x_katana_rate_limit.empty()) {
                out << "    // @rate-limit: " << op.x_katana_rate_limit << "\n";
            }

            // Precompute body schema types
            std::vector<std::string> body_schema_names;
            if (op.body && !op.body->content.empty()) {
                for (const auto& media : op.body->content) {
                    auto media_name = schema_identifier(doc, media.type);
                    if (!media_name.empty() &&
                        std::find(body_schema_names.begin(), body_schema_names.end(), media_name) ==
                            body_schema_names.end()) {
                        body_schema_names.push_back(media_name);
                    }
                }
            }
            bool body_is_variant = body_schema_names.size() > 1;
            std::string body_type_expr;
            if (!body_schema_names.empty()) {
                if (body_is_variant) {
                    body_type_expr = "std::variant<";
                    for (size_t i = 0; i < body_schema_names.size(); ++i) {
                        if (i > 0) {
                            body_type_expr += ", ";
                        }
                        body_type_expr += body_schema_names[i];
                    }
                    body_type_expr += ">";
                } else {
                    body_type_expr = body_schema_names.front();
                }
            }

            out << "    virtual response " << method_name << "(";

            // Add path parameters
            bool first_param = true;
            for (const auto& param : op.parameters) {
                if (param.in == katana::openapi::param_location::path && param.type) {
                    if (!first_param)
                        out << ", ";
                    first_param = false;
                    auto arg_name = sanitize_identifier(param.name);
                    // Generate C++ type for parameter
                    if (param.type->kind == katana::openapi::schema_kind::string) {
                        out << "std::string_view " << arg_name;
                    } else if (param.type->kind == katana::openapi::schema_kind::integer) {
                        out << "int64_t " << arg_name;
                    } else if (param.type->kind == katana::openapi::schema_kind::number) {
                        out << "double " << arg_name;
                    } else if (param.type->kind == katana::openapi::schema_kind::boolean) {
                        out << "bool " << arg_name;
                    } else {
                        out << "std::string_view " << arg_name;
                    }
                }
            }

            // Add query/header/cookie parameters
            for (const auto& param : op.parameters) {
                if ((param.in == katana::openapi::param_location::query ||
                     param.in == katana::openapi::param_location::header ||
                     param.in == katana::openapi::param_location::cookie) &&
                    param.type) {
                    if (!first_param)
                        out << ", ";
                    first_param = false;
                    auto arg_name = sanitize_identifier(param.name);
                    auto wrap_optional = [&](std::string_view base) {
                        if (param.required) {
                            return std::string(base);
                        }
                        return std::string("std::optional<") + std::string(base) + ">";
                    };
                    if (param.type->kind == katana::openapi::schema_kind::string) {
                        out << wrap_optional("std::string_view") << " " << arg_name;
                    } else if (param.type->kind == katana::openapi::schema_kind::integer) {
                        out << wrap_optional("int64_t") << " " << arg_name;
                    } else if (param.type->kind == katana::openapi::schema_kind::number) {
                        out << wrap_optional("double") << " " << arg_name;
                    } else if (param.type->kind == katana::openapi::schema_kind::boolean) {
                        out << wrap_optional("bool") << " " << arg_name;
                    } else {
                        out << wrap_optional("std::string_view") << " " << arg_name;
                    }
                }
            }

            // Add request body parameter
            if (op.body && !op.body->content.empty()) {
                if (!body_type_expr.empty()) {
                    if (!first_param)
                        out << ", ";
                    first_param = false;
                    out << "const " << body_type_expr << "& body";
                }
            }

            out << ") = 0;\n\n";
        }
    }

    out << "};\n\n";
    out << "} // namespace generated\n";
    return out.str();
}

} // namespace katana_gen
