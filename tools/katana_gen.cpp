#include "katana/core/arena.hpp"
#include "katana/core/openapi_loader.hpp"
#include "katana_gen/generator.hpp"
#include "katana_gen/options.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;
using katana::error_code;
using katana::openapi::document;
using namespace katana_gen;

namespace {

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

int run_openapi(const options& opts) {
    if (opts.input.empty()) {
        std::cerr << "[openapi] input spec is required\n";
        return 1;
    }

    if (opts.inline_naming != "operation" && opts.inline_naming != "flat" &&
        opts.inline_naming != "short" && opts.inline_naming != "sequential") {
        std::cerr << "[openapi] unknown inline naming style: " << opts.inline_naming
                  << " (expected: operation|flat)\n";
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

    document& doc = *loaded;

    ensure_inline_schema_names(doc, opts.inline_naming);

    if (opts.json_output) {
        std::cout << dump_ast_summary(doc) << "\n";
    }

    if (opts.check_only) {
        std::cout << "[check] OK: version=" << doc.openapi_version
                  << ", schemas=" << doc.schemas.size() << ", paths=" << doc.paths.size() << "\n";
        return 0;
    }

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

    auto with_layer = [&](std::string code) {
        return std::string("// layer: ") + opts.layer + "\n" + code;
    };

    if (emit_dto) {
        auto dto_code = with_layer(generate_dtos(doc, use_pmr));
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
        auto validator_code = with_layer(generate_validators(doc));
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
        auto json_code = with_layer(generate_json_parsers(doc, use_pmr));
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
        auto router_code = with_layer(generate_router_table(doc));
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
        auto handler_code = with_layer(generate_handler_interfaces(doc));
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
        auto bindings_code = with_layer(generate_router_bindings(doc));
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
