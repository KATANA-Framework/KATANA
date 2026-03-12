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

// Helper to convert well-known HTTP header names to field enum constants
// Returns "katana::http::field::content_type" instead of runtime string lookup
static std::optional<std::string> header_name_to_field_constant(std::string_view header_name) {
    // Map common headers to compile-time constants (avoids runtime string_to_field calls)
    static const std::unordered_map<std::string_view, std::string> known_headers = {
        {"Content-Type", "katana::http::field::content_type"},
        {"Accept", "katana::http::field::accept"},
        {"Cookie", "katana::http::field::cookie"},
        {"Authorization", "katana::http::field::authorization"},
        {"Host", "katana::http::field::host"},
        {"User-Agent", "katana::http::field::user_agent"},
        {"Content-Length", "katana::http::field::content_length"},
        {"Accept-Encoding", "katana::http::field::accept_encoding"},
        {"Accept-Language", "katana::http::field::accept_language"},
        {"Connection", "katana::http::field::connection"},
        {"Cache-Control", "katana::http::field::cache_control"},
        {"Referer", "katana::http::field::referer"},
        {"Origin", "katana::http::field::origin"},
        {"Date", "katana::http::field::date"},
        {"Server", "katana::http::field::server"},
        {"Set-Cookie", "katana::http::field::set_cookie"},
        {"ETag", "katana::http::field::etag"},
        {"Last-Modified", "katana::http::field::last_modified"},
        {"If-Modified-Since", "katana::http::field::if_modified_since"},
        {"If-None-Match", "katana::http::field::if_none_match"},
    };

    auto it = known_headers.find(header_name);
    if (it != known_headers.end()) {
        return it->second;
    }
    return std::nullopt;
}

// Helper to generate headers.get() call with compile-time constant if possible
static std::string generate_headers_get(std::string_view header_name) {
    if (auto field_const = header_name_to_field_constant(header_name)) {
        // Use compile-time field constant (O(1), no string_to_field call)
        return *field_const;
    }
    // Fallback to runtime string lookup (rare headers)
    return std::string("\"") + std::string(header_name) + "\"";
}

// Helper to convert to UPPER_SNAKE_CASE
static std::string to_upper_snake_case(std::string_view str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        if (c == '-' || c == ' ') {
            result += '_';
        } else {
            result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
    }
    return result;
}

std::string generate_router_table(const document& doc) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include \"katana/core/http.hpp\"\n";
    out << "#include \"katana/core/http_utils.hpp\"\n";
    out << "#include \"katana/core/router.hpp\"\n";
    out << "#include <array>\n";
    out << "#include <span>\n";
    out << "#include <string_view>\n\n";
    out << "namespace generated {\n\n";
    out << "using katana::http_utils::content_type_info;\n\n";
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
    out << "#include \"katana/core/http_utils.hpp\"\n";
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

    // Import framework utilities instead of generating them inline
    out << "using katana::http_utils::query_param;\n";
    out << "using katana::http_utils::cookie_param;\n";
    out << "using katana::http_utils::find_content_type;\n";
    out << "using katana::http_utils::negotiate_response_type;\n";
    out << "using katana::http_utils::format_validation_error;\n";
    out << "using katana::http_utils::format_validation_error_into;\n";
    out << "using katana::http_utils::hash_string;\n";
    out << "using katana::http_utils::content_type_info;\n";
    out << "\n";

    // Collect all static routes (routes without path parameters)
    struct static_route {
        std::string path;
        std::string method;
        std::string operation_id;
        std::string method_name; // snake_case method name for dispatch_FUNCTION
        size_t route_idx;
    };
    std::vector<static_route> static_routes;

    {
        size_t idx = 0;
        for (const auto& path : doc.paths) {
            // Check if path has parameters
            bool has_params = path.path.find('{') != std::string::npos;

            for (const auto& op : path.operations) {
                if (op.operation_id.empty()) {
                    continue;
                }

                if (!has_params) {
                    std::string method_name_str = to_snake_case(op.operation_id);
                    static_routes.emplace_back(static_route{std::string(path.path),
                                                            method_enum_literal(op.method),
                                                            method_name_str,
                                                            method_name_str, // same value for both
                                                            idx});
                }
                ++idx;
            }
        }
    }

    // Generate pre-computed hash constants for static routes (deduplicated by path)
    if (!static_routes.empty()) {
        out << "// Pre-computed path hashes for static routes\n";
        std::unordered_set<std::string> emitted_paths;
        for (const auto& route : static_routes) {
            if (emitted_paths.insert(route.path).second) {
                out << "constexpr uint64_t HASH_" << to_upper_snake_case(route.operation_id)
                    << " = hash_string(\"" << route.path << "\");\n";
            }
        }
        out << "\n";
    }

    // ============================================================
    // Route Dispatch Functions
    // ============================================================
    out << "// ============================================================\n";
    out << "// Route Dispatch Functions\n";
    out << "// ============================================================\n\n";

    // Generate dispatch functions for ALL routes (static and dynamic)
    std::ostringstream dispatch_functions;

    // Use separate stream for make_router so we can output dispatch functions first
    std::ostringstream make_router_stream;

    make_router_stream << "// ============================================================\n";
    make_router_stream << "// Router Configuration\n";
    make_router_stream << "// ============================================================\n\n";

    make_router_stream
        << "inline const katana::http::router& make_router(api_handler& handler) {\n";
    make_router_stream << "    using katana::http::route_entry;\n";
    make_router_stream << "    using katana::http::path_pattern;\n";
    make_router_stream << "    using katana::http::handler_fn;\n";
    make_router_stream << "    static std::array<route_entry, route_count> route_entries = {\n";

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

            bool is_static_route = path.path.find('{') == std::string::npos;

            // Generate dispatch function for ALL routes (both static and dynamic)
            dispatch_functions << "// Dispatch for " << path.path << "\n";
            dispatch_functions << "inline katana::result<void> dispatch_" << method_name
                               << "(const katana::http::request& req, "
                               << "katana::http::request_context& ctx, api_handler& handler, "
                               << "katana::http::response& out) {\n";

            // Content negotiation
            if (has_response_content) {
                std::optional<std::string> single_produces_type;
                for (const auto& resp : op.responses) {
                    for (const auto& media : resp.content) {
                        const std::string_view media_type_view(media.content_type.data(),
                                                               media.content_type.size());
                        if (!single_produces_type) {
                            single_produces_type = std::string(media_type_view);
                        } else if (std::string_view(*single_produces_type) != media_type_view) {
                            single_produces_type.reset();
                            break;
                        }
                    }
                    if (!single_produces_type && !resp.content.empty()) {
                        break;
                    }
                }

                if (single_produces_type) {
                    dispatch_functions
                        << "    constexpr std::string_view response_content_type = \""
                        << *single_produces_type << "\";\n";
                    dispatch_functions << "    auto accept_header = req.headers.get("
                                       << generate_headers_get("Accept") << ");\n";
                    dispatch_functions
                        << "    if (accept_header && !accept_header->empty() && *accept_header != "
                           "\"*/*\" && *accept_header != response_content_type) {\n";
                    dispatch_functions << "        auto comma = accept_header->find(',');\n";
                    dispatch_functions << "        auto semicolon = accept_header->find(';');\n";
                    dispatch_functions << "        auto token_end = std::min(comma, semicolon);\n";
                    dispatch_functions
                        << "        auto simple_accept = accept_header->substr(0, token_end);\n";
                    dispatch_functions << "        if (simple_accept != response_content_type) {\n";
                    dispatch_functions << "            out.assign_error("
                                       << "katana::problem_details::not_acceptable("
                                          "\"unsupported Accept header\"));\n";
                    dispatch_functions << "            return {};\n";
                    dispatch_functions << "        }\n";
                    dispatch_functions << "    }\n";
                } else {
                    dispatch_functions
                        << "    auto negotiated_content_type = negotiate_response_type(req, route_"
                        << route_idx << "_produces);\n";
                    dispatch_functions << "    if (!negotiated_content_type) {\n";
                    dispatch_functions << "        out.assign_error("
                                       << "katana::problem_details::not_acceptable("
                                          "\"unsupported Accept header\"));\n";
                    dispatch_functions << "        return {};\n";
                    dispatch_functions << "    }\n";
                    dispatch_functions << "    std::string_view response_content_type = "
                                          "*negotiated_content_type;\n";
                }
            }

            // Path params extraction (for dynamic routes)
            if (!is_static_route) {
                for (const auto& param : op.parameters) {
                    if (param.in != katana::openapi::param_location::path || !param.type) {
                        continue;
                    }
                    auto param_ident = sanitize_identifier(param.name);
                    dispatch_functions << "    auto p_" << param_ident << " = ctx.params.get(\""
                                       << param.name << "\");\n";
                    dispatch_functions
                        << "    if (!p_" << param_ident
                        << ") { out = "
                           "katana::http::response::error(katana::problem_details::bad_request("
                           "\"missing "
                           "path param "
                        << param.name << "\")); return {}; }\n";
                    switch (param.type->kind) {
                    case katana::openapi::schema_kind::integer:
                        dispatch_functions << "    int64_t " << param_ident << " = 0;\n";
                        dispatch_functions << "    {\n";
                        dispatch_functions << "        auto [ptr, ec] = std::from_chars(p_"
                                           << param_ident << "->data(), p_" << param_ident
                                           << "->data() + p_" << param_ident << "->size(), "
                                           << param_ident << ");\n";
                        dispatch_functions
                            << "        if (ec != std::errc() || ptr != p_" << param_ident
                            << "->data() + p_" << param_ident
                            << "->size()) { out = "
                               "katana::http::response::error("
                               "katana::problem_details::bad_request(\"invalid path param "
                            << param.name << "\")); return {}; }\n";
                        dispatch_functions << "    }\n";
                        break;
                    case katana::openapi::schema_kind::number:
                        dispatch_functions << "    double " << param_ident << " = 0.0;\n";
                        dispatch_functions << "    {\n";
                        dispatch_functions << "        auto [ptr, ec] = std::from_chars(p_"
                                           << param_ident << "->data(), p_" << param_ident
                                           << "->data() + p_" << param_ident << "->size(), "
                                           << param_ident << ");\n";
                        dispatch_functions
                            << "        if (ec != std::errc() || ptr != p_" << param_ident
                            << "->data() + p_" << param_ident
                            << "->size()) { out = "
                               "katana::http::response::error(katana::problem_details::bad_request("
                               "\"invalid path param "
                            << param.name << "\")); return {}; }\n";
                        dispatch_functions << "    }\n";
                        break;
                    case katana::openapi::schema_kind::boolean:
                        dispatch_functions << "    bool " << param_ident << " = false;\n";
                        dispatch_functions << "    if (*p_" << param_ident << " == \"true\") "
                                           << param_ident << " = true;\n";
                        dispatch_functions << "    else if (*p_" << param_ident << " == \"false\") "
                                           << param_ident << " = false;\n";
                        dispatch_functions << "    else { out = "
                                              "katana::http::response::error(katana::problem_"
                                              "details::bad_request("
                                              "\"invalid path param "
                                           << param.name << "\")); return {}; }\n";
                        break;
                    default:
                        dispatch_functions << "    auto " << param_ident << " = *p_" << param_ident
                                           << ";\n";
                        break;
                    }
                }
            }

            // Query/header/cookie params (for ALL routes, not just dynamic ones)
            {
                size_t query_param_count = 0;
                size_t cookie_param_count = 0;
                for (const auto& param : op.parameters) {
                    if (param.in == katana::openapi::param_location::query && param.type) {
                        ++query_param_count;
                    } else if (param.in == katana::openapi::param_location::cookie && param.type) {
                        ++cookie_param_count;
                    }
                }

                if (query_param_count > 1) {
                    for (const auto& param : op.parameters) {
                        if (param.in != katana::openapi::param_location::query || !param.type) {
                            continue;
                        }
                        auto param_ident = sanitize_identifier(param.name);
                        dispatch_functions << "    std::optional<std::string_view> p_"
                                           << param_ident << " = std::nullopt;\n";
                    }
                    dispatch_functions << "    auto query_view = req.uri;\n";
                    dispatch_functions << "    auto query_qpos = query_view.find('?');\n";
                    dispatch_functions << "    if (query_qpos != std::string_view::npos) {\n";
                    dispatch_functions
                        << "        query_view = query_view.substr(query_qpos + 1);\n";
                    dispatch_functions << "        while (!query_view.empty()) {\n";
                    dispatch_functions << "            auto amp = query_view.find('&');\n";
                    dispatch_functions << "            auto part = query_view.substr(0, amp);\n";
                    dispatch_functions << "            auto eq = part.find('=');\n";
                    dispatch_functions << "            auto name = part.substr(0, eq);\n";
                    dispatch_functions << "            auto value = eq == std::string_view::npos ? "
                                          "std::string_view{} : part.substr(eq + 1);\n";
                    bool first_query_branch = true;
                    for (const auto& param : op.parameters) {
                        if (param.in != katana::openapi::param_location::query || !param.type) {
                            continue;
                        }
                        auto param_ident = sanitize_identifier(param.name);
                        dispatch_functions
                            << "            " << (first_query_branch ? "if" : "else if") << " (!p_"
                            << param_ident << " && name == \"" << param.name << "\") {\n";
                        dispatch_functions << "                p_" << param_ident << " = value;\n";
                        dispatch_functions << "            }\n";
                        first_query_branch = false;
                    }
                    dispatch_functions << "            if (";
                    bool first_found = true;
                    for (const auto& param : op.parameters) {
                        if (param.in != katana::openapi::param_location::query || !param.type) {
                            continue;
                        }
                        auto param_ident = sanitize_identifier(param.name);
                        if (!first_found) {
                            dispatch_functions << " && ";
                        }
                        dispatch_functions << "p_" << param_ident;
                        first_found = false;
                    }
                    dispatch_functions << ") {\n";
                    dispatch_functions << "                break;\n";
                    dispatch_functions << "            }\n";
                    dispatch_functions << "            if (amp == std::string_view::npos) {\n";
                    dispatch_functions << "                break;\n";
                    dispatch_functions << "            }\n";
                    dispatch_functions << "            query_view.remove_prefix(amp + 1);\n";
                    dispatch_functions << "        }\n";
                    dispatch_functions << "    }\n";
                }

                if (cookie_param_count > 1) {
                    for (const auto& param : op.parameters) {
                        if (param.in != katana::openapi::param_location::cookie || !param.type) {
                            continue;
                        }
                        auto param_ident = sanitize_identifier(param.name);
                        dispatch_functions << "    std::optional<std::string_view> p_"
                                           << param_ident << " = std::nullopt;\n";
                    }
                    dispatch_functions << "    auto cookie_header = req.headers.get("
                                       << generate_headers_get("Cookie") << ");\n";
                    dispatch_functions << "    if (cookie_header) {\n";
                    dispatch_functions
                        << "        std::string_view cookie_view = *cookie_header;\n";
                    dispatch_functions << "        while (!cookie_view.empty()) {\n";
                    dispatch_functions << "            auto sep = cookie_view.find(';');\n";
                    dispatch_functions << "            auto token = cookie_view.substr(0, sep);\n";
                    dispatch_functions << "            auto eq = token.find('=');\n";
                    dispatch_functions << "            if (eq != std::string_view::npos) {\n";
                    dispatch_functions
                        << "                auto name = katana::serde::trim_view(token.substr(0, "
                           "eq));\n";
                    dispatch_functions
                        << "                auto value = katana::serde::trim_view(token.substr(eq "
                           "+ 1));\n";
                    bool first_cookie_branch = true;
                    for (const auto& param : op.parameters) {
                        if (param.in != katana::openapi::param_location::cookie || !param.type) {
                            continue;
                        }
                        auto param_ident = sanitize_identifier(param.name);
                        dispatch_functions << "                "
                                           << (first_cookie_branch ? "if" : "else if") << " (!p_"
                                           << param_ident << " && name == \"" << param.name
                                           << "\") {\n";
                        dispatch_functions << "                    p_" << param_ident
                                           << " = value;\n";
                        dispatch_functions << "                }\n";
                        first_cookie_branch = false;
                    }
                    dispatch_functions << "            }\n";
                    dispatch_functions << "            if (";
                    bool first_cookie_found = true;
                    for (const auto& param : op.parameters) {
                        if (param.in != katana::openapi::param_location::cookie || !param.type) {
                            continue;
                        }
                        auto param_ident = sanitize_identifier(param.name);
                        if (!first_cookie_found) {
                            dispatch_functions << " && ";
                        }
                        dispatch_functions << "p_" << param_ident;
                        first_cookie_found = false;
                    }
                    dispatch_functions << ") {\n";
                    dispatch_functions << "                break;\n";
                    dispatch_functions << "            }\n";
                    dispatch_functions << "            if (sep == std::string_view::npos) {\n";
                    dispatch_functions << "                break;\n";
                    dispatch_functions << "            }\n";
                    dispatch_functions << "            cookie_view.remove_prefix(sep + 1);\n";
                    dispatch_functions << "        }\n";
                    dispatch_functions << "    }\n";
                }

                for (const auto& param : op.parameters) {
                    if (param.in == katana::openapi::param_location::path || !param.type) {
                        continue;
                    }

                    std::string source_expr;
                    auto param_ident = sanitize_identifier(param.name);
                    if (param.in == katana::openapi::param_location::query) {
                        if (query_param_count <= 1) {
                            source_expr =
                                "query_param(req.uri, \"" + std::string(param.name) + "\")";
                        }
                    } else if (param.in == katana::openapi::param_location::header) {
                        source_expr = "req.headers.get(" + generate_headers_get(param.name) + ")";
                    } else if (param.in == katana::openapi::param_location::cookie) {
                        if (cookie_param_count <= 1) {
                            source_expr = "cookie_param(req, \"" + std::string(param.name) + "\")";
                        }
                    }

                    if (!source_expr.empty()) {
                        dispatch_functions << "    auto p_" << param_ident << " = " << source_expr
                                           << ";\n";
                    }
                    if (param.required) {
                        dispatch_functions
                            << "    if (!p_" << param_ident
                            << ") { out = katana::http::response::error("
                               "katana::problem_details::bad_request(\"missing param "
                            << param.name << "\")); return {}; }\n";
                    }

                    const bool optional_param = !param.required;
                    switch (param.type->kind) {
                    case katana::openapi::schema_kind::integer:
                        if (optional_param) {
                            dispatch_functions << "    std::optional<int64_t> " << param_ident
                                               << ";\n";
                            dispatch_functions << "    if (p_" << param_ident << ") {\n";
                            dispatch_functions << "        int64_t tmp = 0;\n";
                            dispatch_functions << "        auto [ptr, ec] = std::from_chars(p_"
                                               << param_ident << "->data(), p_" << param_ident
                                               << "->data() + p_" << param_ident
                                               << "->size(), tmp);\n";
                            dispatch_functions << "        if (ec != std::errc() || ptr != p_"
                                               << param_ident << "->data() + p_" << param_ident
                                               << "->size()) { out = "
                                                  "katana::http::response::error(katana::problem_"
                                                  "details::bad_request("
                                                  "\"invalid param "
                                               << param.name << "\")); return {}; }\n";
                            dispatch_functions << "        " << param_ident << " = tmp;\n";
                            dispatch_functions << "    }\n";
                        } else {
                            dispatch_functions << "    int64_t " << param_ident << " = 0;\n";
                            dispatch_functions << "    if (p_" << param_ident << ") {\n";
                            dispatch_functions << "        auto [ptr, ec] = std::from_chars(p_"
                                               << param_ident << "->data(), p_" << param_ident
                                               << "->data() + p_" << param_ident << "->size(), "
                                               << param_ident << ");\n";
                            dispatch_functions << "        if (ec != std::errc() || ptr != p_"
                                               << param_ident << "->data() + p_" << param_ident
                                               << "->size()) { out = "
                                                  "katana::http::response::error(katana::problem_"
                                                  "details::bad_request("
                                                  "\"invalid param "
                                               << param.name << "\")); return {}; }\n";
                            dispatch_functions << "    }\n";
                        }
                        break;
                    case katana::openapi::schema_kind::number:
                        if (optional_param) {
                            dispatch_functions << "    std::optional<double> " << param_ident
                                               << ";\n";
                            dispatch_functions << "    if (p_" << param_ident << ") {\n";
                            dispatch_functions << "        double tmp = 0.0;\n";
                            dispatch_functions << "        auto [ptr, ec] = std::from_chars(p_"
                                               << param_ident << "->data(), p_" << param_ident
                                               << "->data() + p_" << param_ident
                                               << "->size(), tmp);\n";
                            dispatch_functions << "        if (ec != std::errc() || ptr != p_"
                                               << param_ident << "->data() + p_" << param_ident
                                               << "->size()) { out = "
                                                  "katana::http::response::error(katana::problem_"
                                                  "details::bad_request("
                                                  "\"invalid param "
                                               << param.name << "\")); return {}; }\n";
                            dispatch_functions << "        " << param_ident << " = tmp;\n";
                            dispatch_functions << "    }\n";
                        } else {
                            dispatch_functions << "    double " << param_ident << " = 0.0;\n";
                            dispatch_functions << "    if (p_" << param_ident << ") {\n";
                            dispatch_functions << "        auto [ptr, ec] = std::from_chars(p_"
                                               << param_ident << "->data(), p_" << param_ident
                                               << "->data() + p_" << param_ident << "->size(), "
                                               << param_ident << ");\n";
                            dispatch_functions << "        if (ec != std::errc() || ptr != p_"
                                               << param_ident << "->data() + p_" << param_ident
                                               << "->size()) { out = "
                                                  "katana::http::response::error(katana::problem_"
                                                  "details::bad_request("
                                                  "\"invalid param "
                                               << param.name << "\")); return {}; }\n";
                            dispatch_functions << "    }\n";
                        }
                        break;
                    case katana::openapi::schema_kind::boolean:
                        if (optional_param) {
                            dispatch_functions << "    std::optional<bool> " << param_ident
                                               << ";\n";
                            dispatch_functions << "    if (p_" << param_ident << ") {\n";
                            dispatch_functions << "        if (*p_" << param_ident
                                               << " == \"true\") " << param_ident << " = true;\n";
                            dispatch_functions << "        else if (*p_" << param_ident
                                               << " == \"false\") " << param_ident << " = false;\n";
                            dispatch_functions << "        else { out = "
                                                  "katana::http::response::error(katana::problem_"
                                                  "details::bad_request("
                                                  "\"invalid param "
                                               << param.name << "\")); return {}; }\n";
                            dispatch_functions << "    }\n";
                        } else {
                            dispatch_functions << "    bool " << param_ident << " = false;\n";
                            dispatch_functions << "    if (!p_" << param_ident << ") {\n";
                            dispatch_functions << "        out = "
                                                  "katana::http::response::error(katana::problem_"
                                                  "details::bad_request("
                                                  "\"missing param "
                                               << param.name << "\")); return {};\n";
                            dispatch_functions << "    }\n";
                            dispatch_functions << "    if (*p_" << param_ident << " == \"true\") "
                                               << param_ident << " = true;\n";
                            dispatch_functions << "    else if (*p_" << param_ident
                                               << " == \"false\") " << param_ident << " = false;\n";
                            dispatch_functions << "    else { out = "
                                                  "katana::http::response::error(katana::problem_"
                                                  "details::bad_request("
                                                  "\"invalid param "
                                               << param.name << "\")); return {}; }\n";
                        }
                        break;
                    default:
                        if (optional_param) {
                            dispatch_functions << "    std::optional<std::string_view> "
                                               << param_ident << " = std::nullopt;\n";
                            dispatch_functions << "    if (p_" << param_ident << ") " << param_ident
                                               << " = *p_" << param_ident << ";\n";
                        } else {
                            dispatch_functions << "    auto " << param_ident << " = p_"
                                               << param_ident << " ? *p_" << param_ident
                                               << " : std::string_view{};\n";
                        }
                        break;
                    }
                }
            }

            // Request body parsing (only if route has body)
            if (has_body) {
                const bool has_single_content_type = op.body && op.body->content.size() == 1;
                if (has_single_content_type) {
                    const auto& only_media_type = op.body->content.front().content_type;
                    dispatch_functions << "    auto content_type = req.headers.get("
                                       << generate_headers_get("Content-Type") << ");\n";
                    dispatch_functions << "    if (!content_type || "
                                       << "content_type->substr(0, " << only_media_type.size()
                                       << ") != \"" << only_media_type << "\") {\n";
                    dispatch_functions << "        out.assign_error("
                                       << "katana::problem_details::unsupported_media_type("
                                          "\"unsupported Content-Type\")); return {};\n";
                    dispatch_functions << "    }\n";
                } else {
                    dispatch_functions
                        << "    auto content_type_index = find_content_type(req.headers.get("
                        << generate_headers_get("Content-Type") << "), route_" << route_idx
                        << "_consumes);\n";
                    dispatch_functions
                        << "    if (!content_type_index) { out.assign_error("
                        << "katana::problem_details::unsupported_media_type(\"unsupported "
                           "Content-Type\")); return {}; }\n";
                }

                const bool has_single_body_schema =
                    !body_is_variant && body_schema_names.size() == 1;

                if (body_is_variant) {
                    dispatch_functions << "    std::optional<" << body_type_expr
                                       << "> parsed_body;\n";
                } else if (!body_type_expr.empty() && !has_single_body_schema) {
                    dispatch_functions << "    std::optional<" << body_type_expr
                                       << "> parsed_body;\n";
                }

                if (has_single_body_schema) {
                    const std::string schema_name = body_schema_names.front();
                    dispatch_functions << "    auto parsed_body = parse_" << schema_name
                                       << "(req.body, &ctx.arena);\n";
                    dispatch_functions << "    if (!parsed_body) {\n";
                    dispatch_functions << "        out.assign_error("
                                       << "katana::problem_details::bad_request(\"invalid request "
                                          "body\")); return {};\n";
                    dispatch_functions << "    }\n\n";
                } else {
                    dispatch_functions << "    switch (*content_type_index) {\n";
                    for (size_t media_idx = 0; media_idx < (op.body ? op.body->content.size() : 0);
                         ++media_idx) {
                        const auto& media = op.body->content[media_idx];
                        auto media_name = schema_identifier(doc, media.type);
                        dispatch_functions << "    case " << media_idx << ": {\n";
                        if (!media_name.empty()) {
                            dispatch_functions << "        auto parsed_body_candidate = parse_"
                                               << media_name << "(req.body, &ctx.arena);\n";
                            dispatch_functions
                                << "        if (!parsed_body_candidate) { out.assign_error("
                                << "katana::problem_details::bad_request(\"invalid request "
                                   "body\")); return {}; }\n";
                            dispatch_functions
                                << "        parsed_body = std::move(*parsed_body_candidate);\n";
                        } else {
                            dispatch_functions
                                << "        out.assign_error("
                                << "katana::problem_details::unsupported_media_type(\"unsupported "
                                   "Content-Type\")); return {};\n";
                        }
                        dispatch_functions << "        break;\n";
                        dispatch_functions << "    }\n";
                    }
                    dispatch_functions << "    default:\n";
                    dispatch_functions << "        out.assign_error("
                                       << "katana::problem_details::unsupported_media_type("
                                          "\"unsupported Content-Type\")); return {};\n";
                    dispatch_functions << "    }\n\n";
                }

                // Validation
                if (body_is_variant) {
                    // Variant validation
                    dispatch_functions << "    // Automatic validation (optimized)\n";
                    dispatch_functions
                        << "    auto validation_result = std::visit([](const auto& body_val) "
                           "-> std::optional<std::string> {\n";
                    dispatch_functions << "        using T = std::decay_t<decltype(body_val)>;\n";
                    for (const auto& schema_name : body_schema_names) {
                        dispatch_functions << "        if constexpr (std::is_same_v<T, "
                                           << schema_name << ">) {\n";
                        dispatch_functions << "            if (auto err = validate_" << schema_name
                                           << "(body_val)) {\n";
                        dispatch_functions << "                std::string msg;\n";
                        dispatch_functions << "                msg.reserve(err->field.size() + "
                                              "err->message.size() + 2);\n";
                        dispatch_functions << "                msg.append(err->field);\n";
                        dispatch_functions << "                msg.append(\": \");\n";
                        dispatch_functions << "                msg.append(err->message);\n";
                        dispatch_functions << "                return msg;\n";
                        dispatch_functions << "            }\n";
                        dispatch_functions << "        }\n";
                    }
                    dispatch_functions << "        return std::nullopt;\n";
                    dispatch_functions << "    }, *parsed_body);\n";
                    dispatch_functions << "    if (validation_result) {\n";
                    dispatch_functions << "        out.assign_error(\n";
                    dispatch_functions
                        << "            "
                           "katana::problem_details::bad_request(std::move(*validation_result))\n";
                    dispatch_functions << "        );\n";
                    dispatch_functions << "        return {};\n";
                    dispatch_functions << "    }\n";
                } else if (!body_schema_names.empty()) {
                    // Single type validation
                    std::string schema_name = body_schema_names.front();
                    dispatch_functions
                        << "    // Automatic validation (optimized: single allocation)\n";
                    dispatch_functions << "    if (auto validation_error = validate_" << schema_name
                                       << "(*parsed_body)) {\n";
                    dispatch_functions
                        << "        format_validation_error_into(out, *validation_error);\n";
                    dispatch_functions << "        return {};\n";
                    dispatch_functions << "    }\n";
                }
            }

            // Handler context scope
            dispatch_functions << "    // Set handler context for zero-boilerplate access\n";
            dispatch_functions
                << "    katana::http::handler_context::scope context_scope(req, ctx);\n";

            // Handler invocation
            dispatch_functions << "    auto handler_result = handler." << method_name << "(";

            // Arguments: path param args (for dynamic routes)
            bool first_arg = true;
            if (!is_static_route) {
                for (const auto& param : op.parameters) {
                    if (param.in != katana::openapi::param_location::path || !param.type) {
                        continue;
                    }
                    if (!first_arg)
                        dispatch_functions << ", ";
                    first_arg = false;
                    dispatch_functions << sanitize_identifier(param.name);
                }
            }
            // query/header/cookie args (for ALL routes)
            for (const auto& param : op.parameters) {
                if (param.in == katana::openapi::param_location::path || !param.type) {
                    continue;
                }
                if (!first_arg)
                    dispatch_functions << ", ";
                first_arg = false;
                dispatch_functions << sanitize_identifier(param.name);
            }
            // body arg
            if (has_body) {
                if (!first_arg)
                    dispatch_functions << ", ";
                first_arg = false;
                dispatch_functions << "*parsed_body";
            }
            if (!first_arg)
                dispatch_functions << ", ";
            dispatch_functions << "out);\n";

            // Set Content-Type header if needed
            dispatch_functions << "    if (!handler_result) {\n";
            dispatch_functions << "        return std::unexpected(handler_result.error());\n";
            dispatch_functions << "    }\n";
            if (has_response_content) {
                dispatch_functions << "    if (!out.headers.get("
                                   << generate_headers_get("Content-Type") << ")) {\n";
                dispatch_functions << "        out.set_header(\"Content-Type\", "
                                      "response_content_type);\n";
                dispatch_functions << "    }\n";
            }

            dispatch_functions << "    return {};\n";
            dispatch_functions << "}\n\n";

            // Lambda in make_router now simply calls the dispatch function
            make_router_stream << "        route_entry{katana::http::method::"
                               << method_enum_literal(op.method) << ",\n";
            make_router_stream << "                   katana::http::path_pattern::from_literal<\""
                               << path.path << "\">(),\n";
            make_router_stream
                << "                   handler_fn([&handler](const katana::http::request& req, "
                   "katana::http::request_context& ctx, katana::http::response& out) -> "
                   "katana::result<void> "
                   "{\n";
            make_router_stream << "                       return dispatch_" << method_name
                               << "(req, ctx, handler, out);\n";
            make_router_stream << "                   })\n";
            make_router_stream << "        },\n";
            ++route_idx;
        }
    }

    make_router_stream << "    };\n";
    make_router_stream << "    static katana::http::router router_instance(route_entries);\n";
    make_router_stream << "    return router_instance;\n";
    make_router_stream << "}\n\n";

    // Output in correct order: dispatch functions -> make_router -> fast_router
    out << dispatch_functions.str();
    out << make_router_stream.str();

    // Generate optimized router wrapper with compile-time hash dispatch
    // This provides O(1) routing for static paths (no parameters)
    out << "// Optimized router with hash-based O(1) dispatch for static routes\n";
    out << "class fast_router {\n";
    out << "public:\n";
    out << "    explicit fast_router(api_handler& handler, const katana::http::router& fallback)\n";
    out << "        : handler_(handler), fallback_router_(fallback) {}\n\n";

    out << "    katana::result<void> dispatch_to(\n";
    out << "        const katana::http::request& req,\n";
    out << "        katana::http::request_context& ctx,\n";
    out << "        katana::http::response& out) const {\n";
    out << "        // Strip query string for matching\n";
    out << "        std::string_view path = req.uri;\n";
    out << "        auto query_pos = path.find('?');\n";
    out << "        if (query_pos != std::string_view::npos) {\n";
    out << "            path = path.substr(0, query_pos);\n";
    out << "        }\n\n";

    if (!static_routes.empty()) {
        out << "        // Fast path: O(1) hash-based dispatch for static routes\n";
        out << "        uint64_t path_hash = hash_string(path);\n";
        out << "        switch (path_hash) {\n";

        // Group static routes by path to handle multiple methods per path
        // (e.g., GET /items and POST /items share the same hash)
        std::unordered_map<std::string, std::vector<const static_route*>> routes_by_path;
        for (const auto& route : static_routes) {
            routes_by_path[route.path].push_back(&route);
        }

        std::unordered_set<std::string> emitted_case_paths;
        for (const auto& route : static_routes) {
            if (!emitted_case_paths.insert(route.path).second) {
                continue; // Already emitted case for this path
            }
            out << "            case HASH_"
                << to_upper_snake_case(routes_by_path[route.path][0]->operation_id) << ":\n";
            out << "                if (path == \"" << route.path << "\") {\n";
            for (const auto* r : routes_by_path[route.path]) {
                out << "                    if (req.http_method == katana::http::method::"
                    << r->method << ")\n";
                out << "                        { return dispatch_" << r->method_name
                    << "(req, ctx, handler_, out); }\n";
            }
            out << "                }\n";
            out << "                break;\n";
        }

        out << "            default:\n";
        out << "                break;\n";
        out << "        }\n\n";
    }

    out << "        // Fallback to standard router for:\n";
    out << "        // - Dynamic routes (with path parameters)\n";
    out << "        // - Hash collisions\n";
    out << "        // - Method mismatches\n";
    out << "        return fallback_router_.dispatch(req, ctx, out);\n";
    out << "    }\n\n";

    out << "    katana::result<katana::http::response> operator()(\n";
    out << "        const katana::http::request& req,\n";
    out << "        katana::http::request_context& ctx) const {\n";
    out << "        katana::http::response out;\n";
    out << "        auto status = dispatch_to(req, ctx, out);\n";
    out << "        if (!status) {\n";
    out << "            return std::unexpected(status.error());\n";
    out << "        }\n";
    out << "        return out;\n";
    out << "    }\n\n";

    out << "private:\n";
    out << "    api_handler& handler_;\n";
    out << "    const katana::http::router& fallback_router_;\n";
    out << "};\n\n";

    out << "// Create optimized router (recommended for production)\n";
    out << "inline fast_router make_fast_router(api_handler& handler) {\n";
    out << "    return fast_router(handler, make_router(handler));\n";
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
    out << "//   - Clean signatures: result<void> method(params, response& out)\n";
    out << "//   - Automatic validation: schema constraints checked before handler call\n";
    out << "//   - Auto parameter binding: path/query/header/body → typed arguments\n";
    out << "//   - Context access: use katana::http::req(), ctx(), arena() for access\n";
    out << "// \n";
    out << "// Example:\n";
    out << "//   katana::result<void> get_user(int64_t id, response& out) override {\n";
    out << "//       auto user = db.find(id, &arena());  // arena() from context\n";
    out << "//       out = response::json(serialize_User(user));\n";
    out << "//       return {};\n";
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

            out << "    virtual katana::result<void> " << method_name << "(";

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

            if (!first_param)
                out << ", ";
            out << "response& out) = 0;\n\n";
        }
    }

    out << "};\n\n";

    // Add usage examples
    out << "// ============================================================================\n";
    out << "// USAGE EXAMPLES\n";
    out << "// ============================================================================\n";
    out << "//\n";
    out << "// Example implementation of api_handler:\n";
    out << "//\n";
    out << "// class my_api : public generated::api_handler {\n";
    out << "// public:\n";
    out << "//     // Example 1: Simple request/response with arena allocator\n";

    // Find first operation to use as example
    for (const auto& path_item : doc.paths) {
        for (const auto& op : path_item.operations) {
            if (op.operation_id.empty() || !op.body || op.body->content.empty()) {
                continue;
            }

            std::string method_name = to_snake_case(op.operation_id);

            // Get body type
            std::string body_type;
            if (!op.body->content.empty()) {
                body_type = schema_identifier(doc, op.body->content[0].type);
            }

            // Get response type (first 200 response)
            std::string response_type;
            for (const auto& resp : op.responses) {
                if (resp.status == 200 && !resp.content.empty()) {
                    response_type = schema_identifier(doc, resp.content[0].type);
                    break;
                }
            }

            if (!body_type.empty() && !response_type.empty()) {
                out << "//     response " << method_name << "(const " << body_type
                    << "& req) override {\n";
                out << "//         // Access request fields\n";
                out << "//         auto input = req.text;\n";
                out << "//\n";
                out << "//         // Create response using arena allocator\n";
                out << "//         " << response_type << " resp(&katana::http::arena());\n";
                out << "//\n";
                out << "//         // Process and set response fields\n";
                out << "//         // resp.result = ...your logic here...\n";
                out << "//\n";
                out << "//         // Serialize and return\n";
                out << "//         return response::json(serialize_" << response_type
                    << "(resp));\n";
                out << "//     }\n";
                out << "//\n";
                break;
            }
        }
        break;
    }

    out << "//     // Example 2: Error handling\n";
    out << "//     response handle_request(const some_request& req) override {\n";
    out << "//         if (req.value < 0) {\n";
    out << "//             return response::bad_request(\"value must be positive\");\n";
    out << "//         }\n";
    out << "//         // ... normal processing ...\n";
    out << "//         return response::json(serialize_some_response(resp));\n";
    out << "//     }\n";
    out << "//\n";
    out << "//     // Example 3: Different response status codes\n";
    out << "//     response create_item(const create_request& req) override {\n";
    out << "//         auto item = db.create(req, &katana::http::arena());\n";
    out << "//         if (!item) {\n";
    out << "//             return response::internal_error(\"failed to create item\");\n";
    out << "//         }\n";
    out << "//         return response::created(serialize_item(*item));\n";
    out << "//     }\n";
    out << "//\n";
    out << "//     // Example 4: Enum handling\n";
    out << "//     response transform_text(const text_transform_request& req) override {\n";
    out << "//         std::string result;\n";
    out << "//         switch (req.operation) {\n";
    out << "//             case text_transform_operation::upper:\n";
    out << "//                 result = to_upper(req.text);\n";
    out << "//                 break;\n";
    out << "//             case text_transform_operation::lower:\n";
    out << "//                 result = to_lower(req.text);\n";
    out << "//                 break;\n";
    out << "//             // ... other cases ...\n";
    out << "//         }\n";
    out << "//         text_transform_response resp(&katana::http::arena());\n";
    out << "//         resp.result = result;\n";
    out << "//         return response::json(serialize_text_transform_response(resp));\n";
    out << "//     }\n";
    out << "// };\n";
    out << "//\n";
    out << "// Available response helpers:\n";
    out << "//   - respond::into(out).text(...)\n";
    out << "//   - respond::into(out).json(...)\n";
    out << "//   - respond::into(out).created_json(...)\n";
    out << "//   - respond::into(out).no_content()\n";
    out << "//   - out = response::bad_request(message)\n";
    out << "//   - out = response::unauthorized(message)\n";
    out << "//   - out = response::forbidden(message)\n";
    out << "//   - response::not_found(message)\n";
    out << "//   - response::internal_error(message)\n";
    out << "//\n";
    out << "// Context access functions (available in handler methods):\n";
    out << "//   - katana::http::req()    - Get current request\n";
    out << "//   - katana::http::ctx()    - Get request context\n";
    out << "//   - katana::http::arena()  - Get arena allocator for zero-copy strings\n";
    out << "//\n";

    out << "} // namespace generated\n";
    return out.str();
}

} // namespace katana_gen
