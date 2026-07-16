#include "katana/core/arena.hpp"
#include "katana/core/openapi_loader.hpp"
#include "katana_gen/generator.hpp"
#include "katana_gen/options.hpp"
#include "katana_gen/sql_codegen.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;
using katana::error_code;
using katana::openapi::document;
using namespace katana_gen;

namespace {

std::string generate_openapi_package_header(bool emit_dto,
                                            bool emit_validator,
                                            bool emit_serdes,
                                            bool emit_router,
                                            bool emit_handler,
                                            bool emit_bindings) {
    std::string out = "#pragma once\n\n";
    if (emit_dto) {
        out += "#include \"generated_dtos.hpp\"\n";
    }
    if (emit_validator) {
        out += "#include \"generated_validators.hpp\"\n";
    }
    if (emit_serdes) {
        out += "#include \"generated_json.hpp\"\n";
    }
    if (emit_router) {
        out += "#include \"generated_routes.hpp\"\n";
    }
    if (emit_handler) {
        out += "#include \"generated_handlers.hpp\"\n";
    }
    if (emit_bindings) {
        out += "#include \"generated_router_bindings.hpp\"\n";
    }
    return out;
}

std::string generate_sql_package_header(bool emit_models, bool emit_repository, bool emit_bridge) {
    std::string out = "#pragma once\n\n";
    if (emit_models) {
        out += "#include \"generated_sql_models.hpp\"\n";
    }
    if (emit_repository) {
        out += "#include \"generated_sql_repository.hpp\"\n";
    }
    if (emit_bridge) {
        out += "#include \"generated_bridge.hpp\"\n";
    }
    return out;
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

int run_openapi(const options& opts) {
    // In --json mode keep stdout pure JSON; informational logs go to stderr.
    std::ostream& log = opts.json_output ? std::cerr : std::cout;
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

    if (opts.verbose) {
        log << "[verbose] Loading OpenAPI spec: " << opts.input << "\n";
    }

    katana::monotonic_arena arena;
    auto loaded = katana::openapi::load_from_file(opts.input.c_str(), arena, opts.strict);
    if (!loaded) {
        std::cerr << "[openapi] " << error_message(loaded.error()) << "\n";
        if (opts.strict) {
            return 1;
        }
        return 0;
    }

    document& doc = *loaded;

    if (opts.verbose) {
        log << "[verbose] Loaded: version=" << doc.openapi_version
                  << ", schemas=" << doc.schemas.size() << ", paths=" << doc.paths.size() << "\n";
        log << "[verbose] Assigning schema names (style: " << opts.inline_naming << ")...\n";
    }

    ensure_inline_schema_names(doc, opts.inline_naming);

    if (opts.verbose) {
        log << "[verbose] Schema naming complete\n";
    }

    if (opts.json_output) {
        std::cout << dump_ast_summary(doc) << "\n";
    }

    if (opts.check_only) {
        log << "[check] OK: version=" << doc.openapi_version
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
    // The TypeScript client is opt-in only: `--emit all` feeds the C++ codegen dirs, so we do not
    // want a stray .ts file dropped in there. Request it explicitly with `--emit typescript`.
    bool emit_typescript = (opts.emit.find("typescript") != std::string::npos);

    auto with_layer = [&](std::string code) {
        return std::string("// layer: ") + opts.layer + "\n" + code;
    };

    if (emit_dto) {
        if (opts.verbose) {
            log << "[verbose] Generating DTOs (" << doc.schemas.size() << " schemas)...\n";
        }
        auto dto_code = with_layer(generate_dtos(doc, use_pmr, opts.ns));
        auto dto_path = opts.output / "generated_dtos.hpp";
        std::ofstream out(dto_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << dto_path << "\n";
            return 1;
        }
        out << dto_code;
        log << "[codegen] DTOs written to " << dto_path;
        if (opts.verbose) {
            log << " (" << dto_code.size() << " bytes)";
        }
        log << "\n";
    }

    if (emit_validator) {
        if (opts.verbose) {
            log << "[verbose] Generating validators...\n";
        }
        auto validator_code = with_layer(generate_validators(doc, opts.ns, opts.serdes));
        auto validator_path = opts.output / "generated_validators.hpp";
        std::ofstream out(validator_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << validator_path << "\n";
            return 1;
        }
        out << validator_code;
        log << "[codegen] Validators written to " << validator_path;
        if (opts.verbose) {
            log << " (" << validator_code.size() << " bytes)";
        }
        log << "\n";
    }

    if (emit_serdes) {
        if (opts.verbose) {
            log << "[verbose] Generating JSON parsers and serializers...\n";
        }
        auto json_code = with_layer(generate_json_parsers(doc, use_pmr, opts.ns, opts.serdes));
        auto json_path = opts.output / "generated_json.hpp";
        std::ofstream out(json_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << json_path << "\n";
            return 1;
        }
        out << json_code;
        log << "[codegen] JSON parsers written to " << json_path;
        if (opts.verbose) {
            log << " (" << json_code.size() << " bytes)";
        }
        log << "\n";
    }

    if (emit_router) {
        if (opts.verbose) {
            log << "[verbose] Generating route table (" << doc.paths.size() << " paths)...\n";
        }
        auto router_code = with_layer(generate_router_table(doc, opts.ns));
        auto router_path = opts.output / "generated_routes.hpp";
        std::ofstream out(router_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << router_path << "\n";
            return 1;
        }
        out << router_code;
        log << "[codegen] Route table written to " << router_path;
        if (opts.verbose) {
            log << " (" << router_code.size() << " bytes)";
        }
        log << "\n";
    }

    if (emit_handler) {
        if (opts.verbose) {
            size_t op_count = 0;
            for (const auto& p : doc.paths) {
                op_count += p.operations.size();
            }
            log << "[verbose] Generating handler interfaces (" << op_count
                      << " operations)...\n";
        }
        auto handler_code = with_layer(generate_handler_interfaces(doc, opts.ns));
        auto handler_path = opts.output / "generated_handlers.hpp";
        std::ofstream out(handler_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << handler_path << "\n";
            return 1;
        }
        out << handler_code;
        log << "[codegen] Handler interfaces written to " << handler_path;
        if (opts.verbose) {
            log << " (" << handler_code.size() << " bytes)";
        }
        log << "\n";
    }

    if (emit_bindings) {
        if (opts.verbose) {
            log << "[verbose] Generating router bindings (glue code)...\n";
        }
        auto bindings_code = with_layer(generate_router_bindings(doc, opts.ns));
        auto bindings_path = opts.output / "generated_router_bindings.hpp";
        std::ofstream out(bindings_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << bindings_path << "\n";
            return 1;
        }
        out << bindings_code;
        log << "[codegen] Router bindings written to " << bindings_path;
        if (opts.verbose) {
            log << " (" << bindings_code.size() << " bytes)";
        }
        log << "\n";
    }

    if (emit_typescript) {
        if (opts.verbose) {
            log << "[verbose] Generating TypeScript client...\n";
        }
        auto ts_code = generate_typescript_client(doc, opts.ns);
        auto ts_path = opts.output / "generated_client.ts";
        std::ofstream out(ts_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << ts_path << "\n";
            return 1;
        }
        out << ts_code;
        log << "[codegen] TypeScript client written to " << ts_path;
        if (opts.verbose) {
            log << " (" << ts_code.size() << " bytes)";
        }
        log << "\n";
    }

    if (emit_dto || emit_validator || emit_serdes || emit_router || emit_handler || emit_bindings) {
        auto package_code = generate_openapi_package_header(
            emit_dto, emit_validator, emit_serdes, emit_router, emit_handler, emit_bindings);
        auto package_path = opts.output / "generated_openapi_package.hpp";
        std::ofstream out(package_path, std::ios::binary);
        if (!out) {
            std::cerr << "[openapi] failed to write " << package_path << "\n";
            return 1;
        }
        out << package_code;
        log << "[codegen] OpenAPI package written to " << package_path;
        if (opts.verbose) {
            log << " (" << package_code.size() << " bytes)";
        }
        log << "\n";
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
        log << "[openapi] AST summary written to " << out_path << "\n";
    }

    log << "[openapi] OK: version=" << doc.openapi_version
              << ", schemas=" << doc.schemas.size() << ", paths=" << doc.paths.size() << "\n";
    return 0;
}

int run_sql(const options& opts) {
    // In --json mode keep stdout pure JSON; informational logs go to stderr.
    std::ostream& log = opts.json_output ? std::cerr : std::cout;
    if (opts.input.empty()) {
        std::cerr << "[sql] input directory is required\n";
        return 1;
    }

    if (opts.emit != "all" && opts.emit.find("models") == std::string::npos &&
        opts.emit.find("repository") == std::string::npos) {
        std::cerr << "[sql] unknown emit target: " << opts.emit
                  << " (expected: models|repository|all)\n";
        return 1;
    }

    std::error_code fs_ec;
    fs::create_directories(opts.output, fs_ec);
    if (fs_ec) {
        std::cerr << "[sql] failed to create output dir: " << fs_ec.message() << "\n";
        return 1;
    }

    auto catalog = load_sql_catalog(opts.input);
    if (!catalog) {
        std::cerr << "[sql] failed to load SQL catalog: " << catalog.error().message() << "\n";
        return 1;
    }

    const auto ast_json = dump_sql_ast_summary(*catalog);
    if (opts.json_output) {
        std::cout << ast_json << "\n";
    }

    if (opts.dump_ast) {
        auto ast_path = opts.output / "sql_ast.json";
        std::ofstream out(ast_path, std::ios::binary);
        if (!out) {
            std::cerr << "[sql] failed to write " << ast_path << "\n";
            return 1;
        }
        out << ast_json;
        log << "[sql] AST summary written to " << ast_path << "\n";
    }

    if (opts.check_only) {
        log << "[check] OK: queries=" << catalog->queries.size() << "\n";
        return 0;
    }

    const bool emit_models = opts.emit == "all" || opts.emit.find("models") != std::string::npos;
    const bool emit_repository =
        opts.emit == "all" || opts.emit.find("repository") != std::string::npos;

    if (emit_models) {
        auto models_code = generate_sql_models(*catalog, opts.ns);
        auto models_path = opts.output / "generated_sql_models.hpp";
        std::ofstream out(models_path, std::ios::binary);
        if (!out) {
            std::cerr << "[sql] failed to write " << models_path << "\n";
            return 1;
        }
        out << models_code;
        log << "[codegen] SQL models written to " << models_path << "\n";
    }

    if (emit_repository) {
        auto repository_code = generate_sql_repository(*catalog, opts.ns);
        auto repository_path = opts.output / "generated_sql_repository.hpp";
        std::ofstream out(repository_path, std::ios::binary);
        if (!out) {
            std::cerr << "[sql] failed to write " << repository_path << "\n";
            return 1;
        }
        out << repository_code;
        log << "[codegen] SQL repository written to " << repository_path << "\n";
    }

    bool emit_bridge = false;
    if (!opts.openapi_spec.empty()) {
        katana::monotonic_arena bridge_arena;
        auto bridge_doc =
            katana::openapi::load_from_file(opts.openapi_spec.c_str(), bridge_arena, opts.strict);
        if (!bridge_doc) {
            std::cerr << "[sql] --openapi: failed to load " << opts.openapi_spec << "\n";
            return 1;
        }
        // Assign the same inline schema names the DTO generator used, so DTO type names match.
        ensure_inline_schema_names(*bridge_doc, opts.inline_naming);
        auto bridge_code = generate_bridge(*catalog, *bridge_doc, opts.ns);
        auto bridge_path = opts.output / "generated_bridge.hpp";
        std::ofstream out(bridge_path, std::ios::binary);
        if (!out) {
            std::cerr << "[sql] failed to write " << bridge_path << "\n";
            return 1;
        }
        out << bridge_code;
        log << "[codegen] Row->DTO bridge written to " << bridge_path << "\n";
        emit_bridge = true;
    }

    {
        auto package_code = generate_sql_package_header(emit_models, emit_repository, emit_bridge);
        auto package_path = opts.output / "generated_sql_package.hpp";
        std::ofstream out(package_path, std::ios::binary);
        if (!out) {
            std::cerr << "[sql] failed to write " << package_path << "\n";
            return 1;
        }
        out << package_code;
        log << "[codegen] SQL package written to " << package_path << "\n";
    }

    log << "[sql] OK: queries=" << catalog->queries.size() << "\n";
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    options opts = parse_args(argc, argv);
    if (opts.subcommand == "openapi") {
        return run_openapi(opts);
    }
    if (opts.subcommand == "sql") {
        return run_sql(opts);
    }
    std::cerr << "Unknown subcommand: " << opts.subcommand << "\n";
    print_usage();
}
