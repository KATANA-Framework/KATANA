#pragma once

#include "openapi_ast.hpp"
#include "result.hpp"

#include <string_view>

namespace katana::openapi {

// Parses minimal subset of OpenAPI 3.x (JSON) into arena-backed AST.
result<document> load_from_string(std::string_view spec_text, monotonic_arena& arena);
result<document> load_from_file(const char* path, monotonic_arena& arena);

} // namespace katana::openapi
