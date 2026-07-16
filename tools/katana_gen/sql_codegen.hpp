#pragma once

#include "katana/core/openapi_loader.hpp"
#include "katana/core/result.hpp"

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

namespace katana_gen {

enum class sql_query_mode {
    one,
    many,
    exec,
};

struct sql_parameter {
    std::size_t index = 0;
    std::string name; // from an @name placeholder; empty for positional $N (falls back to pN)
    std::string pg_type;
    std::string cpp_type;
    bool nullable = false; // from `@name?::type` — generated arg is std::optional<T> (binds SQL NULL)
};

struct sql_column {
    std::string name;
    std::string pg_type;
    std::string cpp_type;
};

struct sql_query {
    std::string name;
    std::filesystem::path source_path;
    sql_query_mode mode = sql_query_mode::exec;
    std::string sql;
    std::vector<sql_parameter> parameters;
    std::vector<sql_column> columns;
    // Header flags (`-- name: foo :one :no-async :no-step`) — opt out of generated variants a
    // service never calls. The positional sync method is always emitted (it is the API of record
    // and the only variant the others build on).
    bool gen_async = true;
    bool gen_step = true;
};

struct sql_catalog {
    std::vector<sql_query> queries;
};

struct sql_source {
    std::filesystem::path path;
    std::string content;
};

katana::result<sql_catalog> load_sql_catalog(const std::filesystem::path& input_root);
katana::result<sql_catalog> load_sql_catalog_from_sources(std::vector<sql_source> sources);
std::string dump_sql_ast_summary(const sql_catalog& catalog);
// `ns` places the generated SQL types in `namespace <ns>` (default `generated`) — the same bare
// namespace the OpenAPI generator uses, so a contract's DTOs and its SQL rows/repository share one
// namespace (and multiple contracts can link into one binary via distinct namespaces).
std::string generate_sql_models(const sql_catalog& catalog, const std::string& ns = "");
std::string generate_sql_repository(const sql_catalog& catalog, const std::string& ns = "");
// Row→DTO bridge: for each generated <Name>Row whose field set exactly matches an OpenAPI DTO
// (by normalized name + compatible scalar type), emit a `to_<Dto>(row, arena)` converter.
// Ambiguous/partial matches are skipped with a warning (never a wrong mapping).
std::string generate_bridge(const sql_catalog& catalog, const katana::openapi::document& doc,
                            const std::string& ns = "");
std::string sql_mode_literal(sql_query_mode mode);

} // namespace katana_gen
