#include "options.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace katana_gen {

[[noreturn]] void print_usage() {
    std::cout << R"(katana_gen — OpenAPI code generator for KATANA

Usage:
  katana_gen openapi -i <spec> -o <out_dir> [options]
  katana_gen examples

Options:
  -i, --input <file>         OpenAPI specification path (JSON/YAML)
  -o, --output <dir>         Output directory (default: .)
  --emit <targets>           What to generate: dto,validator,serdes,router,handler,all (default: all)
  --layer <mode>             Architecture: flat,layered (default: flat)
  --alloc <type>             Allocator: pmr,std (default: pmr)
  --inline-naming <style>    Inline schema naming: operation,flat (default: operation)
  --json                     Output as JSON format
  --check                    Validate spec only, no files written
  --strict                   Strict validation, fail on any error
  --dump-ast                 Save AST summary to openapi_ast.json
  -h, --help                 Show this help
)";
    std::exit(1);
}

[[noreturn]] void print_examples() {
    std::cout << R"(katana_gen examples:

  # Validate spec only
  katana_gen openapi -i api/openapi.yaml --check --strict

  # Generate everything (DTOs, serdes, router, handlers)
  katana_gen openapi -i api/openapi.yaml -o gen --emit all --inline-naming operation

  # Flat inline schema names (deterministic snapshots)
  katana_gen openapi -i api/openapi.yaml -o gen --emit dto,serdes,router --inline-naming flat

  # Dump AST for debugging
  katana_gen openapi -i api/openapi.yaml -o gen --dump-ast --json
)";
    std::exit(0);
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
    if (opts.subcommand == "examples") {
        print_examples();
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
        } else if (arg == "--inline-naming") {
            if (i + 1 >= argc) {
                print_usage();
            }
            opts.inline_naming = argv[++i];
        } else if (arg == "--check") {
            opts.check_only = true;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            print_usage();
        }
    }
    return opts;
}

} // namespace katana_gen
