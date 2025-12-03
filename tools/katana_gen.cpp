#include "katana/core/arena.hpp"
#include "katana/core/openapi_loader.hpp"

#include <filesystem>
#include <fstream>
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
    bool strict = false;
    bool dump_ast = false;
};

[[noreturn]] void print_usage() {
    std::cout << R"(katana_gen — CLI для генераторов KATANA

Usage:
  katana_gen openapi -i <spec> -o <out_dir> [--strict] [--dump-ast]

Options:
  -i, --input <file>      Путь до OpenAPI спецификации (JSON или YAML)
  -o, --output <dir>      Директория для результатов (default: .)
  --strict                Жёсткая проверка: не заглушать parse/validation ошибки
  --dump-ast              Сохранить краткий AST-дамп в <out_dir>/openapi_ast.json
  -h, --help              Показать эту справку
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

    std::cout << "[openapi] OK: version=" << doc.openapi_version << ", paths=" << doc.paths.size()
              << "\n";
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
