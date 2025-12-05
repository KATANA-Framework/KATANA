#pragma once

#include "katana/core/http.hpp"
#include "katana/core/openapi_loader.hpp"

#include <string>
#include <string_view>

namespace katana_gen {

using katana::openapi::document;

std::string escape_json(std::string_view sv);
std::string escape_cpp_string(std::string_view sv);
std::string schema_identifier(const document& doc, const katana::openapi::schema* s);
std::string to_snake_case(std::string_view id);
std::string sanitize_identifier(std::string_view name);
std::string method_enum_literal(katana::http::method m);
void ensure_inline_schema_names(document& doc, std::string_view naming_style);

std::string dump_ast_summary(const document& doc);

std::string generate_dtos(const document& doc, bool use_pmr);
std::string generate_json_parsers(const document& doc, bool use_pmr);
std::string generate_validators(const document& doc);
std::string generate_router_table(const document& doc);
std::string generate_handler_interfaces(const document& doc);
std::string generate_router_bindings(const document& doc);

} // namespace katana_gen
