#pragma once

#include "openapi_ast.hpp"
#include "result.hpp"

#include <string_view>

namespace katana::openapi {

// Parses minimal subset of OpenAPI 3.x (JSON) into arena-backed AST.
// `strict` promotes "unsupported construct" warnings (e.g. an unknown schema `type`) into a load
// error instead of a silent coercion. `source_name` labels those diagnostics (the file path is
// passed automatically by load_from_file; a bare string load defaults to "<spec>").
result<document> load_from_string(std::string_view spec_text, monotonic_arena& arena,
                                  bool strict = false, std::string_view source_name = "");
result<document> load_from_file(const char* path, monotonic_arena& arena, bool strict = false);

} // namespace katana::openapi
