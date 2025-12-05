#pragma once

#include <filesystem>
#include <string>

namespace katana_gen {

struct options {
    std::string subcommand;
    std::string input;
    std::filesystem::path output = ".";
    std::string emit = "all";                // dto,validator,serdes,router,all
    std::string layer = "flat";              // flat,layered
    std::string allocator = "pmr";           // pmr,std
    std::string inline_naming = "operation"; // operation,flat
    bool strict = false;
    bool dump_ast = false;
    bool json_output = false;
    bool check_only = false;
};

[[noreturn]] void print_usage();
[[noreturn]] void print_examples();
options parse_args(int argc, char** argv);

} // namespace katana_gen
