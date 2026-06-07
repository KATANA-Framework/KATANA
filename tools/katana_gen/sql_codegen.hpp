#pragma once

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
    std::string pg_type;
    std::string cpp_type;
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
// `ns` (when non-empty) places the generated SQL types in `katana::sql::<ns>` instead of the
// default `katana::sql::generated`, so multiple SQL contracts can link into one binary.
std::string generate_sql_models(const sql_catalog& catalog, const std::string& ns = "");
std::string generate_sql_repository(const sql_catalog& catalog, const std::string& ns = "");
std::string sql_mode_literal(sql_query_mode mode);

} // namespace katana_gen
