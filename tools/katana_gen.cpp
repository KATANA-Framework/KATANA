#include "katana/core/arena.hpp"
#include "katana/core/openapi_loader.hpp"

#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;
using katana::error_code;
using katana::openapi::document;

namespace {

struct options {
    std::string subcommand;
    std::string input;
    fs::path output = ".";
    std::string emit = "all";      // dto,validator,serdes,router,all
    std::string layer = "flat";    // flat,layered
    std::string allocator = "pmr"; // pmr,std
    bool strict = false;
    bool dump_ast = false;
    bool json_output = false;
};

[[noreturn]] void print_usage() {
    std::cout << R"(katana_gen — OpenAPI code generator for KATANA

Usage:
  katana_gen openapi -i <spec> -o <out_dir> [options]

Options:
  -i, --input <file>         OpenAPI specification path (JSON/YAML)
  -o, --output <dir>         Output directory (default: .)
  --emit <targets>           What to generate: dto,validator,serdes,router,handler,all (default: all)
  --layer <mode>             Architecture: flat,layered (default: flat)
  --alloc <type>             Allocator: pmr,std (default: pmr)
  --json                     Output as JSON format
  --strict                   Strict validation, fail on any error
  --dump-ast                 Save AST summary to openapi_ast.json
  -h, --help                 Show this help
)";
    std::exit(1);
}

options parse_args(int argc, char** argv) {
    options opts;
    if (argc < 2) {
        print_usage();
    }
    opts.subcommand = argv[1];
    if (opts.subcommand == "-h" || opts.subcommand == "--help") {
        print_usage();
    }
    for (int i = 2; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            print_usage();
        } else if (arg == "-i" || arg == "--input") {
            if (i + 1 >= argc) {
                print_usage();
            }
            opts.input = argv[++i];
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 >= argc) {
                print_usage();
            }
            opts.output = argv[++i];
        } else if (arg == "--strict") {
            opts.strict = true;
        } else if (arg == "--dump-ast") {
            opts.dump_ast = true;
        } else if (arg == "--json") {
            opts.json_output = true;
        } else if (arg == "--emit") {
            if (i + 1 >= argc) {
                print_usage();
            }
            opts.emit = argv[++i];
        } else if (arg == "--layer") {
            if (i + 1 >= argc) {
                print_usage();
            }
            opts.layer = argv[++i];
        } else if (arg == "--alloc") {
            if (i + 1 >= argc) {
                print_usage();
            }
            opts.allocator = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            print_usage();
        }
    }
    return opts;
}

std::string error_message(const std::error_code& ec) {
    switch (static_cast<error_code>(ec.value())) {
    case error_code::openapi_parse_error:
        return "failed to parse OpenAPI document";
    case error_code::openapi_invalid_spec:
        return "invalid or unsupported OpenAPI version (expected 3.x)";
    default:
        return ec.message();
    }
}

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

std::string dump_ast_summary(const document& doc) {
    std::ostringstream os;
    os << "{";
    os << "\"openapi\":\"" << escape_json(doc.openapi_version) << "\",";
    os << "\"title\":\"" << escape_json(doc.info_title) << "\",";
    os << "\"version\":\"" << escape_json(doc.info_version) << "\",";
    os << "\"paths\":[";
    bool first_path = true;
    for (const auto& p : doc.paths) {
        if (!first_path) {
            os << ",";
        }
        first_path = false;
        os << "{";
        os << "\"path\":\"" << escape_json(p.path) << "\",";
        os << "\"operations\":[";
        bool first_op = true;
        for (const auto& op : p.operations) {
            if (!first_op) {
                os << ",";
            }
            first_op = false;
            os << "{";
            os << "\"method\":\"" << escape_json(katana::http::method_to_string(op.method))
               << "\",";
            os << "\"operationId\":\"" << escape_json(op.operation_id) << "\",";
            os << "\"summary\":\"" << escape_json(op.summary) << "\",";

            os << "\"parameters\":[";
            bool first_param = true;
            for (const auto& param : op.parameters) {
                if (!first_param) {
                    os << ",";
                }
                first_param = false;
                os << "{";
                os << "\"name\":\"" << escape_json(param.name) << "\",";
                os << "\"in\":\"";
                switch (param.in) {
                case katana::openapi::param_location::path:
                    os << "path";
                    break;
                case katana::openapi::param_location::query:
                    os << "query";
                    break;
                case katana::openapi::param_location::header:
                    os << "header";
                    break;
                case katana::openapi::param_location::cookie:
                    os << "cookie";
                    break;
                }
                os << "\",";
                os << "\"required\":" << (param.required ? "true" : "false");
                os << "}";
            }
            os << "],";

            os << "\"requestBody\":";
            if (op.body && !op.body->content.empty()) {
                os << "{";
                os << "\"description\":\"" << escape_json(op.body->description) << "\",";
                os << "\"content\":[";
                bool first_media = true;
                for (const auto& media : op.body->content) {
                    if (!first_media) {
                        os << ",";
                    }
                    first_media = false;
                    os << "{";
                    os << "\"contentType\":\"" << escape_json(media.content_type) << "\"";
                    os << "}";
                }
                os << "]";
                os << "}";
            } else {
                os << "null";
            }
            os << ",";

            os << "\"responses\":[";
            bool first_resp = true;
            for (const auto& resp : op.responses) {
                if (!first_resp) {
                    os << ",";
                }
                first_resp = false;
                os << "{";
                os << "\"status\":" << resp.status << ",";
                os << "\"default\":" << (resp.is_default ? "true" : "false") << ",";
                os << "\"description\":\"" << escape_json(resp.description) << "\",";
                os << "\"content\":[";
                bool first_c = true;
                for (const auto& media : resp.content) {
                    if (!first_c) {
                        os << ",";
                    }
                    first_c = false;
                    os << "{";
                    os << "\"contentType\":\"" << escape_json(media.content_type) << "\"";
                    os << "}";
                }
                os << "]";
                os << "}";
            }
            os << "]";

            os << "}";
        }
        os << "]";
        os << "}";
    }
    os << "]";
    os << "}";
    return os.str();
}

std::string cpp_type_from_schema(const katana::openapi::schema* s, bool use_pmr) {
    if (!s) {
        return "std::monostate";
    }
    using katana::openapi::schema_kind;

    // Check if this is an enum
    if (s->kind == schema_kind::string && !s->enum_values.empty() && !s->name.empty()) {
        return std::string(s->name) + "_enum";
    }

    switch (s->kind) {
    case schema_kind::string:
        return use_pmr ? "arena_string<>" : "std::string";
    case schema_kind::integer:
        return "int64_t";
    case schema_kind::number:
        return "double";
    case schema_kind::boolean:
        return "bool";
    case schema_kind::array:
        if (s->items) {
            return (use_pmr ? "arena_vector<" : "std::vector<") +
                   cpp_type_from_schema(s->items, use_pmr) + ">";
        }
        return use_pmr ? "arena_vector<std::string>" : "std::vector<std::string>";
    case schema_kind::object:
        if (!s->name.empty()) {
            return std::string(s->name);
        }
        return "std::monostate";
    default:
        return "std::monostate";
    }
}

void generate_dto_for_schema(std::ostream& out,
                             const katana::openapi::schema& s,
                             bool use_pmr,
                             size_t indent = 0) {
    std::string ind(static_cast<size_t>(indent), ' ');
    if (s.name.empty() || s.properties.empty()) {
        return;
    }

    out << ind << "struct " << s.name << " {\n";
    if (use_pmr) {
        out << ind << "    explicit " << s.name << "(monotonic_arena* arena = nullptr)\n";
        out << ind << "        : arena_(arena)";

        for (const auto& prop : s.properties) {
            auto cpp_type = cpp_type_from_schema(prop.type, use_pmr);
            if (cpp_type.find("arena_") != std::string::npos) {
                out << ",\n"
                    << ind << "          " << prop.name << "(arena_allocator<char>(arena))";
            }
        }
        out << " {}\n\n";
        out << ind << "    monotonic_arena* arena_;\n";
    }

    for (const auto& prop : s.properties) {
        auto cpp_type = cpp_type_from_schema(prop.type, use_pmr);
        out << ind << "    " << cpp_type << " " << prop.name;
        // Don't add = {} for arena types (they're initialized in constructor)
        bool is_arena_type = use_pmr && (cpp_type.find("arena_string") != std::string::npos ||
                                         cpp_type.find("arena_vector") != std::string::npos);
        if (!prop.required && !is_arena_type) {
            out << " = {}";
        }
        out << ";\n";
    }

    out << ind << "};\n\n";
}

void generate_enum_for_schema(std::ostream& out, const katana::openapi::schema& s) {
    if (s.kind != katana::openapi::schema_kind::string || s.enum_values.empty() || s.name.empty()) {
        return;
    }

    out << "enum class " << s.name << "_enum {\n";
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
    out << "inline std::string_view to_string(" << s.name << "_enum e) {\n";
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
        out << "    case " << s.name << "_enum::" << identifier << ": return \"" << val << "\";\n";
    }
    out << "    }\n";
    out << "    return \"\";\n";
    out << "}\n\n";

    // Add from_string function
    out << "inline std::optional<" << s.name << "_enum> " << s.name
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
        out << "    if (s == \"" << val << "\") return " << s.name << "_enum::" << identifier
            << ";\n";
    }
    out << "    return std::nullopt;\n";
    out << "}\n\n";
}

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
        generate_enum_for_schema(out, schema);
    }

    // Then generate DTOs
    for (const auto& schema : doc.schemas) {
        generate_dto_for_schema(out, schema, use_pmr);
    }

    return out.str();
}

void generate_json_parser_for_schema(std::ostream& out,
                                     const katana::openapi::schema& s,
                                     bool use_pmr) {
    if (s.name.empty() || s.properties.empty()) {
        return;
    }

    out << "inline std::optional<" << s.name << "> parse_" << s.name
        << "(std::string_view json, monotonic_arena* arena) {\n";
    out << "    using katana::serde::json_cursor;\n";
    out << "    json_cursor cur{json.data(), json.data() + json.size()};\n";
    if (!use_pmr) {
        out << "    (void)arena;\n";
    }
    out << "    if (!cur.try_object_start()) return std::nullopt;\n\n";
    out << "    " << s.name << " obj(arena);\n";

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

            if (is_enum && !prop.type->name.empty()) {
                out << "            if (auto v = cur.string()) {\n";
                out << "                auto enum_val = " << prop.type->name
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
                        case schema_kind::object:
                            if (!item->name.empty()) {
                                out << "                    {\n";
                                out << "                        const char* value_start = "
                                       "cur.ptr;\n";
                                out << "                        cur.skip_value();\n";
                                out << "                        std::string_view sv(value_start, "
                                       "static_cast<size_t>(cur.ptr - value_start));\n";
                                out << "                        if (auto nested = parse_"
                                    << item->name << "(sv, arena)) { obj." << prop.name
                                    << ".push_back(*nested); }\n";
                                out << "                    }\n";
                            } else {
                                out << "                    cur.skip_value();\n";
                            }
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
                case schema_kind::object:
                    if (!prop.type->name.empty()) {
                        out << "            {\n";
                        out << "                const char* value_start = cur.ptr;\n";
                        out << "                cur.skip_value();\n";
                        out << "                std::string_view sv(value_start, "
                               "static_cast<size_t>(cur.ptr - value_start));\n";
                        out << "                auto nested = parse_" << prop.type->name
                            << "(sv, arena);\n";
                        out << "                if (nested) obj." << prop.name << " = *nested;\n";
                        out << "            }\n";
                    } else {
                        out << "            cur.skip_value();\n";
                    }
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

void generate_json_serializer_for_schema(std::ostream& out, const katana::openapi::schema& s) {
    if (s.name.empty() || s.properties.empty()) {
        return;
    }

    out << "inline std::string serialize_" << s.name << "(const " << s.name << "& obj) {\n";
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

            if (is_enum && !prop.type->name.empty()) {
                out << "    json.push_back('\"');\n";
                out << "    json.append(to_string(obj." << prop.name << "));\n";
                out << "    json.push_back('\"');\n";
            } else {
                switch (prop.type->kind) {
                case schema_kind::string:
                    out << "    json.push_back('\"');\n";
                    out << "    json.append(katana::serde::escape_json_string(obj." << prop.name
                        << "));\n";
                    out << "    json.push_back('\"');\n";
                    break;
                case schema_kind::integer:
                    out << "    {\n";
                    out << "        char buf[32];\n";
                    out << "        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj."
                        << prop.name << ");\n";
                    out << "        json.append(buf, static_cast<size_t>(ptr - buf));\n";
                    out << "    }\n";
                    break;
                case schema_kind::number:
                    out << "    {\n";
                    out << "        char buf[64];\n";
                    out << "        auto res = std::to_chars(buf, buf + sizeof(buf), obj."
                        << prop.name << ");\n";
                    out << "        if (res.ec == std::errc()) json.append(buf, "
                           "static_cast<size_t>(res.ptr - buf));\n";
                    out << "    }\n";
                    break;
                case schema_kind::boolean:
                    out << "    json.append(obj." << prop.name << " ? \"true\" : \"false\");\n";
                    break;
                case schema_kind::array:
                    out << "    json.push_back('[');\n";
                    out << "    for (size_t i = 0; i < obj." << prop.name << ".size(); ++i) {\n";
                    out << "        if (i > 0) json.push_back(',');\n";
                    if (prop.type->items) {
                        switch (prop.type->items->kind) {
                        case schema_kind::string:
                            out << "        json.push_back('\"');\n";
                            out << "        json.append(katana::serde::escape_json_string(obj."
                                << prop.name << "[i]));\n";
                            out << "        json.push_back('\"');\n";
                            break;
                        case schema_kind::integer:
                            out << "        {\n";
                            out << "            char buf[32];\n";
                            out << "            auto [ptr, ec] = std::to_chars(buf, buf + "
                                   "sizeof(buf), obj."
                                << prop.name << "[i]);\n";
                            out << "            json.append(buf, static_cast<size_t>(ptr - "
                                   "buf));\n";
                            out << "        }\n";
                            break;
                        case schema_kind::number:
                            out << "        {\n";
                            out << "            char buf[64];\n";
                            out << "            auto res = std::to_chars(buf, buf + sizeof(buf), "
                                   "obj."
                                << prop.name << "[i]);\n";
                            out << "            if (res.ec == std::errc()) json.append(buf, "
                                   "static_cast<size_t>(res.ptr - buf));\n";
                            out << "        }\n";
                            break;
                        case schema_kind::boolean:
                            out << "        json.append(obj." << prop.name
                                << "[i] ? \"true\" : \"false\");\n";
                            break;
                        case schema_kind::object:
                            if (!prop.type->items->name.empty()) {
                                out << "        json.append(serialize_" << prop.type->items->name
                                    << "(obj." << prop.name << "[i]));\n";
                            } else {
                                out << "        json.append(\"null\");\n";
                            }
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
                    if (!prop.type->name.empty()) {
                        out << "    json.append(serialize_" << prop.type->name << "(obj."
                            << prop.name << "));\n";
                    } else {
                        out << "    json.append(\"{}\");\n";
                    }
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
                                const katana::openapi::schema& s,
                                [[maybe_unused]] bool use_pmr) {
    if (s.name.empty() || s.properties.empty()) {
        return;
    }

    // Generate array parser: parse_Type_array(string_view, arena*) -> optional<vector<Type>>
    out << "inline std::optional<std::vector<" << s.name << ">> parse_" << s.name
        << "_array(std::string_view json, monotonic_arena* arena) {\n";
    out << "    using katana::serde::json_cursor;\n";
    out << "    json_cursor cur{json.data(), json.data() + json.size()};\n";
    out << "    if (!cur.try_array_start()) return std::nullopt;\n\n";
    out << "    std::vector<" << s.name << "> result;\n";
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
    out << "        auto obj = parse_" << s.name << "(obj_json, arena);\n";
    out << "        if (!obj) return std::nullopt;\n";
    out << "        result.push_back(std::move(*obj));\n";
    out << "        \n";
    out << "        cur.try_comma();\n";
    out << "    }\n";
    out << "    return result;\n";
    out << "}\n\n";
}

void generate_json_array_serializer(std::ostream& out, const katana::openapi::schema& s) {
    if (s.name.empty() || s.properties.empty()) {
        return;
    }

    // Generate array serializer: serialize_Type_array(const std::vector<Type>&)
    out << "inline std::string serialize_" << s.name << "_array(const std::vector<" << s.name
        << ">& arr) {\n";
    out << "    std::string json = \"[\";\n";
    out << "    for (size_t i = 0; i < arr.size(); ++i) {\n";
    out << "        json += serialize_" << s.name << "(arr[i]);\n";
    out << "        if (i < arr.size() - 1) json += \",\";\n";
    out << "    }\n";
    out << "    json += \"]\";\n";
    out << "    return json;\n";
    out << "}\n\n";
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

    for (const auto& schema : doc.schemas) {
        generate_json_parser_for_schema(out, schema, use_pmr);
    }
    for (const auto& schema : doc.schemas) {
        generate_json_serializer_for_schema(out, schema);
    }
    // Generate array parsers and serializers for all schemas
    for (const auto& schema : doc.schemas) {
        generate_json_array_parser(out, schema, use_pmr);
    }
    for (const auto& schema : doc.schemas) {
        generate_json_array_serializer(out, schema);
    }

    return out.str();
}
void generate_validator_for_schema(std::ostream& out, const katana::openapi::schema& s) {
    if (s.name.empty() || s.properties.empty()) {
        return;
    }

    out << "struct " << s.name << "_validation_error {\n";
    out << "    std::string_view field;\n";
    out << "    std::string_view message;\n";
    out << "};\n\n";

    out << "inline std::optional<" << s.name << "_validation_error> validate_" << s.name
        << "(const " << s.name << "& obj) {\n";

    for (const auto& prop : s.properties) {
        if (!prop.type) {
            continue;
        }
        using katana::openapi::schema_kind;

        if (prop.required && prop.type->kind == schema_kind::string) {
            out << "    if (obj." << prop.name << ".empty()) {\n";
            out << "        return " << s.name << "_validation_error{\"" << prop.name
                << "\", \"required field is missing\"};\n";
            out << "    }\n";
        }
        if (prop.required && prop.type->kind == schema_kind::array && prop.type->min_items &&
            *prop.type->min_items > 0) {
            out << "    if (obj." << prop.name << ".empty()) {\n";
            out << "        return " << s.name << "_validation_error{\"" << prop.name
                << "\", \"required field is missing\"};\n";
            out << "    }\n";
        }

        if (prop.type->kind == schema_kind::string) {
            if (prop.type->min_length) {
                out << "    if (!obj." << prop.name << ".empty() && obj." << prop.name
                    << ".size() < " << *prop.type->min_length << ") {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"string too short (min: " << *prop.type->min_length << ")\"};\n";
                out << "    }\n";
            }
            if (prop.type->max_length) {
                out << "    if (obj." << prop.name << ".size() > " << *prop.type->max_length
                    << ") {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"string too long (max: " << *prop.type->max_length << ")\"};\n";
                out << "    }\n";
            }
            if (!prop.type->enum_values.empty()) {
                out << "    {\n";
                out << "        bool valid = false;\n";
                for (const auto& enum_val : prop.type->enum_values) {
                    out << "        if (obj." << prop.name << " == \"" << enum_val
                        << "\") valid = true;\n";
                }
                out << "        if (!valid) {\n";
                out << "            return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"invalid enum value\"};\n";
                out << "        }\n";
                out << "    }\n";
            }
        }

        if (prop.type->kind == schema_kind::integer || prop.type->kind == schema_kind::number) {
            if (prop.type->minimum) {
                out << "    if (obj." << prop.name << " < " << *prop.type->minimum << ") {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"value too small (min: " << *prop.type->minimum << ")\"};\n";
                out << "    }\n";
            }
            if (prop.type->maximum) {
                out << "    if (obj." << prop.name << " > " << *prop.type->maximum << ") {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"value too large (max: " << *prop.type->maximum << ")\"};\n";
                out << "    }\n";
            }
            if (prop.type->exclusive_minimum) {
                out << "    if (obj." << prop.name << " <= " << *prop.type->exclusive_minimum
                    << ") {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"value must be greater than " << *prop.type->exclusive_minimum
                    << "\"};\n";
                out << "    }\n";
            }
            if (prop.type->exclusive_maximum) {
                out << "    if (obj." << prop.name << " >= " << *prop.type->exclusive_maximum
                    << ") {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"value must be less than " << *prop.type->exclusive_maximum
                    << "\"};\n";
                out << "    }\n";
            }
            if (prop.type->multiple_of) {
                out << "    if (std::fmod(static_cast<double>(obj." << prop.name << "), "
                    << *prop.type->multiple_of << ") != 0.0) {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"value must be multiple of " << *prop.type->multiple_of << "\"};\n";
                out << "    }\n";
            }
        }

        if (prop.type->kind == schema_kind::array) {
            if (prop.type->min_items) {
                out << "    if (!obj." << prop.name << ".empty() && obj." << prop.name
                    << ".size() < " << *prop.type->min_items << ") {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"array too small (min items: " << *prop.type->min_items << ")\"};\n";
                out << "    }\n";
            }
            if (prop.type->max_items) {
                out << "    if (obj." << prop.name << ".size() > " << *prop.type->max_items
                    << ") {\n";
                out << "        return " << s.name << "_validation_error{\"" << prop.name
                    << "\", \"array too large (max items: " << *prop.type->max_items << ")\"};\n";
                out << "    }\n";
            }
        }
    }

    out << "    return std::nullopt;\n";
    out << "}\n\n";
}

std::string generate_validators(const document& doc) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include \"generated_dtos.hpp\"\n";
    out << "#include <optional>\n";
    out << "#include <string_view>\n";
    out << "#include <string>\n";
    out << "#include <cmath>\n\n";

    for (const auto& schema : doc.schemas) {
        generate_validator_for_schema(out, schema);
    }

    return out.str();
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

std::string generate_router_bindings(const document& doc) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include \"katana/core/router.hpp\"\n";
    out << "#include \"katana/core/problem.hpp\"\n";
    out << "#include \"generated_routes.hpp\"\n";
    out << "#include \"generated_handlers.hpp\"\n";
    out << "#include \"generated_json.hpp\"\n";
    out << "#include <array>\n";
    out << "#include <charconv>\n";
    out << "#include <string_view>\n";
    out << "\n";
    out << "namespace generated {\n\n";

    out << "inline const katana::http::router& make_router(api_handler& handler) {\n";
    out << "    using katana::http::route_entry;\n";
    out << "    using katana::http::path_pattern;\n";
    out << "    using katana::http::handler_fn;\n";
    out << "    static std::array<route_entry, route_count> route_entries = {\n";

    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            if (op.operation_id.empty()) {
                continue;
            }

            const auto method_name = to_snake_case(op.operation_id);
            out << "        route_entry{katana::http::method::" << method_enum_literal(op.method)
                << ",\n";
            out << "                   katana::http::path_pattern::from_literal<\"" << path.path
                << "\">(),\n";
            out << "                   handler_fn([&handler](const katana::http::request& req, "
                   "katana::http::request_context& ctx) -> katana::result<katana::http::response> "
                   "{\n";

            // Path params extraction
            for (const auto& param : op.parameters) {
                if (param.in != katana::openapi::param_location::path || !param.type) {
                    continue;
                }
                out << "                       auto p_" << param.name << " = ctx.params.get(\""
                    << param.name << "\");\n";
                out << "                       if (!p_" << param.name
                    << ") return "
                       "katana::http::response::error(katana::problem_details::bad_request("
                       "\"missing "
                       "path param "
                    << param.name << "\"));\n";
                switch (param.type->kind) {
                case katana::openapi::schema_kind::integer:
                    out << "                       int64_t " << param.name << " = 0;\n";
                    out << "                       {\n";
                    out << "                           auto [ptr, ec] = std::from_chars(p_"
                        << param.name << "->data(), p_" << param.name << "->data() + p_"
                        << param.name << "->size(), " << param.name << ");\n";
                    out << "                           if (ec != std::errc()) return "
                           "katana::http::response::error("
                           "katana::problem_details::bad_request(\"invalid path param "
                        << param.name << "\"));\n";
                    out << "                       }\n";
                    break;
                case katana::openapi::schema_kind::number:
                    out << "                       double " << param.name << " = 0.0;\n";
                    out << "                       {\n";
                    out << "                           char* endp = nullptr;\n";
                    out << "                           " << param.name << " = std::strtod(p_"
                        << param.name << "->data(), &endp);\n";
                    out << "                           if (endp == p_" << param.name
                        << "->data()) return "
                           "katana::http::response::error(katana::problem_details::bad_request("
                           "\"invalid path param "
                        << param.name << "\"));\n";
                    out << "                       }\n";
                    break;
                case katana::openapi::schema_kind::boolean:
                    out << "                       bool " << param.name << " = (*p_" << param.name
                        << " == \"true\");\n";
                    break;
                default:
                    out << "                       auto " << param.name << " = *p_" << param.name
                        << ";\n";
                    break;
                }
            }

            // Request body parsing
            bool has_body = op.body && !op.body->content.empty();
            if (has_body) {
                const auto* media = op.body->first_media();
                if (media && media->type && !media->type->name.empty()) {
                    out << "                       auto parsed_body = parse_" << media->type->name
                        << "(req.body, &ctx.arena);\n";
                    out << "                       if (!parsed_body) return "
                           "katana::http::response::error("
                           "katana::problem_details::bad_request(\"invalid request body\"));\n";
                }
            }

            out << "                       return handler." << method_name << "(req, ctx";

            // path param args
            for (const auto& param : op.parameters) {
                if (param.in != katana::openapi::param_location::path || !param.type) {
                    continue;
                }
                out << ", " << param.name;
            }
            // body arg
            if (has_body) {
                const auto* media = op.body->first_media();
                if (media && media->type) {
                    out << ", ";
                    if (!media->type->name.empty()) {
                        out << "*parsed_body";
                    } else {
                        out << "req.body";
                    }
                }
            }
            out << ");\n";
            out << "                   })\n";
            out << "        },\n";
        }
    }

    out << "    };\n";
    out << "    static katana::http::router router_instance(route_entries);\n";
    out << "    return router_instance;\n";
    out << "}\n\n";
    out << "} // namespace generated\n";
    return out.str();
}

std::string generate_handler_interfaces(const document& doc) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include \"katana/core/http.hpp\"\n";
    out << "#include \"katana/core/router.hpp\"\n";
    out << "#include \"generated_dtos.hpp\"\n";
    out << "#include <string_view>\n";
    out << "#include <optional>\n\n";
    out << "using katana::http::request;\n";
    out << "using katana::http::response;\n";
    out << "using katana::http::request_context;\n\n";
    out << "namespace generated {\n\n";

    // Generate handler interface class
    out << "// Base handler interface for all API operations\n";
    out << "struct api_handler {\n";
    out << "    virtual ~api_handler() = default;\n\n";

    // Generate a handler method for each operation
    for (const auto& path_item : doc.paths) {
        for (const auto& op : path_item.operations) {
            if (op.operation_id.empty()) {
                continue; // Skip operations without operation_id
            }

            // Convert operationId from camelCase to snake_case
            std::string method_name;
            for (char c : op.operation_id) {
                if (std::isupper(static_cast<unsigned char>(c)) && !method_name.empty()) {
                    method_name += '_';
                    method_name += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                } else {
                    method_name += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                }
            }

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
            out << "    virtual response " << method_name
                << "(const request& req, request_context& ctx";

            // Add path parameters
            for (const auto& param : op.parameters) {
                if (param.in == katana::openapi::param_location::path && param.type) {
                    out << ", ";
                    // Generate C++ type for parameter
                    if (param.type->kind == katana::openapi::schema_kind::string) {
                        out << "std::string_view " << param.name;
                    } else if (param.type->kind == katana::openapi::schema_kind::integer) {
                        out << "int64_t " << param.name;
                    } else if (param.type->kind == katana::openapi::schema_kind::number) {
                        out << "double " << param.name;
                    } else if (param.type->kind == katana::openapi::schema_kind::boolean) {
                        out << "bool " << param.name;
                    } else {
                        out << "std::string_view " << param.name;
                    }
                }
            }

            // Add request body parameter
            if (op.body && !op.body->content.empty()) {
                const auto* media = op.body->first_media();
                if (media && media->type) {
                    out << ", ";
                    if (!media->type->name.empty()) {
                        out << "const " << media->type->name << "& body";
                    } else {
                        out << "std::string_view body";
                    }
                }
            }

            out << ") = 0;\n\n";
        }
    }

    out << "};\n\n";
    out << "} // namespace generated\n";
    return out.str();
}

int run_openapi(const options& opts) {
    if (opts.input.empty()) {
        std::cerr << "[openapi] input spec is required\n";
        return 1;
    }

    std::error_code fs_ec;
    fs::create_directories(opts.output, fs_ec);
    if (fs_ec) {
        std::cerr << "[openapi] failed to create output dir: " << fs_ec.message() << "\n";
        return 1;
    }

    katana::monotonic_arena arena;
    auto loaded = katana::openapi::load_from_file(opts.input.c_str(), arena);
    if (!loaded) {
        std::cerr << "[openapi] " << error_message(loaded.error()) << "\n";
        if (opts.strict) {
            return 1;
        }
        return 0;
    }

    const document& doc = *loaded;

    bool use_pmr = (opts.allocator == "pmr");
    bool emit_dto = (opts.emit == "all" || opts.emit.find("dto") != std::string::npos);
    bool emit_validator = (opts.emit == "all" || opts.emit.find("validator") != std::string::npos);
    bool emit_serdes = (opts.emit == "all" || opts.emit.find("serdes") != std::string::npos);
    bool emit_router = (opts.emit == "all" || opts.emit.find("router") != std::string::npos);
    bool emit_handler = (opts.emit == "all" || opts.emit.find("handler") != std::string::npos);
    bool emit_bindings = emit_router && emit_handler;
    if (emit_handler || emit_bindings) {
        emit_serdes = true; // нужно для парсинга body в glue
    }

    if (emit_dto) {
        auto dto_code = generate_dtos(doc, use_pmr);
        auto dto_path = opts.output / "generated_dtos.hpp";
        std::ofstream out(dto_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << dto_path << "\n";
            return 1;
        }
        out << dto_code;
        std::cout << "[codegen] DTOs written to " << dto_path << "\n";
    }

    if (emit_validator) {
        auto validator_code = generate_validators(doc);
        auto validator_path = opts.output / "generated_validators.hpp";
        std::ofstream out(validator_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << validator_path << "\n";
            return 1;
        }
        out << validator_code;
        std::cout << "[codegen] Validators written to " << validator_path << "\n";
    }

    if (emit_serdes) {
        auto json_code = generate_json_parsers(doc, use_pmr);
        auto json_path = opts.output / "generated_json.hpp";
        std::ofstream out(json_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << json_path << "\n";
            return 1;
        }
        out << json_code;
        std::cout << "[codegen] JSON parsers written to " << json_path << "\n";
    }

    if (emit_router) {
        auto router_code = generate_router_table(doc);
        auto router_path = opts.output / "generated_routes.hpp";
        std::ofstream out(router_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << router_path << "\n";
            return 1;
        }
        out << router_code;
        std::cout << "[codegen] Route table written to " << router_path << "\n";
    }

    if (emit_handler) {
        auto handler_code = generate_handler_interfaces(doc);
        auto handler_path = opts.output / "generated_handlers.hpp";
        std::ofstream out(handler_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << handler_path << "\n";
            return 1;
        }
        out << handler_code;
        std::cout << "[codegen] Handler interfaces written to " << handler_path << "\n";
    }

    if (emit_bindings) {
        auto bindings_code = generate_router_bindings(doc);
        auto bindings_path = opts.output / "generated_router_bindings.hpp";
        std::ofstream out(bindings_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << bindings_path << "\n";
            return 1;
        }
        out << bindings_code;
        std::cout << "[codegen] Router bindings written to " << bindings_path << "\n";
    }

    if (opts.dump_ast) {
        auto json = dump_ast_summary(doc);
        auto out_path = opts.output / "openapi_ast.json";
        std::ofstream out(out_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << out_path << "\n";
            return 1;
        }
        out << json;
        std::cout << "[openapi] AST summary written to " << out_path << "\n";
    }

    std::cout << "[openapi] OK: version=" << doc.openapi_version
              << ", schemas=" << doc.schemas.size() << ", paths=" << doc.paths.size() << "\n";
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    options opts = parse_args(argc, argv);
    if (opts.subcommand != "openapi") {
        std::cerr << "Unknown subcommand: " << opts.subcommand << "\n";
        print_usage();
    }
    return run_openapi(opts);
}
