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
std::string schema_banner(const document& doc, const katana::openapi::schema& s);
bool is_free_form_object(const katana::openapi::schema* s);
std::string to_snake_case(std::string_view id);
std::string sanitize_identifier(std::string_view name);
std::string property_member_identifier(std::string_view name);
std::string metadata_constant_identifier(std::string_view name);
bool is_optional_property(const katana::openapi::property& prop);
std::string method_enum_literal(katana::http::method m);
void ensure_inline_schema_names(document& doc, std::string_view naming_style);

std::string dump_ast_summary(const document& doc);

// Wrap a generated file's body in `namespace ns { ... }`, inserted after the leading
// comment/#pragma/#include preamble so includes stay at global scope. No-op when ns is empty.
std::string inject_namespace(std::string content, const std::string& ns);

// `ns` (when non-empty) places all generated symbols in that namespace so multiple OpenAPI
// contracts can be linked into one binary without colliding. Empty preserves the legacy layout
// (DTOs/JSON/validators at global scope, router under `namespace generated`).
std::string generate_dtos(const document& doc, bool use_pmr, const std::string& ns = "");
std::string generate_json_parsers(const document& doc, bool use_pmr, const std::string& ns = "");
std::string generate_validators(const document& doc, const std::string& ns = "");
std::string generate_router_table(const document& doc, const std::string& ns = "");
std::string generate_handler_interfaces(const document& doc, const std::string& ns = "");
std::string generate_router_bindings(const document& doc, const std::string& ns = "");

} // namespace katana_gen
