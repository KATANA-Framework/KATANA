#include "sql_codegen.hpp"

#include <cctype>
#include <optional>
#include <sstream>
#include <string_view>

namespace katana_gen {

namespace {

std::string to_pascal_case_local(std::string_view sv) {
    std::string out;
    bool upper = true;
    for (char c : sv) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            if (upper) {
                out.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
                upper = false;
            } else {
                out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            }
        } else {
            upper = true;
        }
    }
    if (out.empty()) {
        return "Query";
    }
    return out;
}

std::string row_type_name(const sql_query& query) {
    return to_pascal_case_local(query.name) + "Row";
}

// C++ identifier for a query parameter: an `arg_`-prefixed form of the @name when present, else the
// positional fallback pN. The `arg_` prefix keeps the name readable while guaranteeing it never
// collides with the method's generated internal locals (params, row, status, mapped, out_row, …),
// none of which start with `arg_`. Raw $N queries keep their historical pN names.
std::string param_identifier(const sql_parameter& parameter) {
    if (parameter.name.empty()) {
        return "p" + std::to_string(parameter.index);
    }
    std::string id = "arg_";
    for (char c : parameter.name) {
        id.push_back((std::isalnum(static_cast<unsigned char>(c)) || c == '_') ? c : '_');
    }
    return id;
}

std::string raw_sql_literal(const std::string& sql) {
    return "R\"__KATANA_SQL__(\n" + sql + "\n)__KATANA_SQL__\"";
}

std::string param_cpp_type(const sql_parameter& parameter) {
    if (parameter.nullable) {
        // `@name?::type` → std::optional<T> by value; encode_value binds SQL NULL when empty.
        return "std::optional<" + parameter.cpp_type + ">";
    }
    if (parameter.cpp_type == "std::string") {
        return "std::string_view";
    }
    if (parameter.cpp_type.starts_with("std::vector<")) {
        return "const " + parameter.cpp_type + "&";
    }
    return parameter.cpp_type;
}

std::string optional_cpp_type(const std::string& cpp_type) {
    return "std::optional<" + cpp_type + ">";
}

std::string repository_return_type(const sql_query& query) {
    if (query.mode == sql_query_mode::exec) {
        return "katana::result<katana::sql::exec_result>";
    }
    if (query.mode == sql_query_mode::one) {
        return "katana::result<std::optional<" + row_type_name(query) + ">>";
    }
    return "katana::result<std::vector<" + row_type_name(query) + ">>";
}

// A named-args params struct is emitted only when every parameter carries a `@name` — i.e. a
// fully-named query. Raw `$N`/`pN` queries keep only the positional method (back-compat).
bool all_params_named(const sql_query& query) {
    if (query.parameters.empty()) {
        return false;
    }
    for (const auto& p : query.parameters) {
        if (p.name.empty()) {
            return false;
        }
    }
    return true;
}

std::string param_struct_type_name(const sql_query& query) {
    return to_pascal_case_local(query.name) + "Params";
}

// Owning member type for the params struct (so callers can aggregate-init with temporaries): strings
// own, vectors own, scalars pass through.
std::string param_owning_type(const sql_parameter& parameter) {
    if (parameter.nullable) {
        return "std::optional<" + parameter.cpp_type + ">";
    }
    return parameter.cpp_type; // std::string / vector / scalar are already owning
}

} // namespace

std::string generate_sql_models(const sql_catalog& catalog, const std::string& ns) {
    const std::string sql_ns = ns.empty() ? "generated" : ns;
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include <cstdint>\n";
    out << "#include <optional>\n";
    out << "#include <string>\n\n";
    out << "#include <vector>\n\n";
    out << "namespace " << sql_ns << " {\n\n";
    for (const auto& query : catalog.queries) {
        if (query.columns.empty()) {
            continue;
        }
        out << "struct " << row_type_name(query) << " {\n";
        for (const auto& column : query.columns) {
            out << "    " << optional_cpp_type(column.cpp_type) << " " << column.name << ";\n";
        }
        out << "};\n\n";
    }
    // Named-args params structs — enable `repo.query({.a = …, .b = …})` so same-type args can't be
    // silently swapped at the call site. Members are in first-appearance (SQL) order, matching the
    // positional method's argument order for designated-initializer parity.
    for (const auto& query : catalog.queries) {
        if (!all_params_named(query)) {
            continue;
        }
        out << "struct " << param_struct_type_name(query) << " {\n";
        for (const auto& parameter : query.parameters) {
            out << "    " << param_owning_type(parameter) << " " << parameter.name << ";\n";
        }
        out << "};\n\n";
    }
    out << "} // namespace " << sql_ns << "\n";
    return out.str();
}

std::string generate_sql_repository(const sql_catalog& catalog, const std::string& ns) {
    const std::string sql_ns = ns.empty() ? "generated" : ns;
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include \"generated_sql_models.hpp\"\n";
    out << "#include \"katana/sql/gather.hpp\"\n";
    out << "#include \"katana/sql/runtime.hpp\"\n\n";
    out << "#include <optional>\n";
    out << "#include <string_view>\n";
    out << "#include <utility>\n";
    out << "#include <vector>\n\n";
    out << "namespace " << sql_ns << " {\n\n";
    out << "class generated_repository {\n";
    out << "public:\n";
    for (const auto& query : catalog.queries) {
        out << "    using " << query.name << "_async_handler = katana::inplace_function<void("
            << repository_return_type(query) << "), 256>;\n";
    }
    if (!catalog.queries.empty()) {
        out << "\n";
    }
    out << "    explicit generated_repository(katana::sql::executor& executor) noexcept\n";
    out << "        : executor_(executor),\n";
    out << "          async_executor_(dynamic_cast<katana::sql::async_executor*>(&executor)) {}\n\n";

    for (const auto& query : catalog.queries) {
        const std::string return_type = repository_return_type(query);

        out << "    " << return_type << " " << query.name << "(";
        for (std::size_t i = 0; i < query.parameters.size(); ++i) {
            const auto& parameter = query.parameters[i];
            if (i != 0) {
                out << ", ";
            }
            out << param_cpp_type(parameter) << " " << param_identifier(parameter);
        }
        out << ") const {\n";
        out << "        katana::sql::parameters params;\n";
        out << "        params.reserve(" << query.parameters.size() << ");\n";
        for (const auto& parameter : query.parameters) {
            out << "        params.push_back(katana::sql::encode_value("
                << param_identifier(parameter) << "));\n";
        }
        if (query.mode == sql_query_mode::exec) {
            out << "        return executor_.exec(\"" << query.name << "\", " << query.name
                << "_sql, params);\n";
        } else {
            if (query.mode == sql_query_mode::one) {
                out << "        std::optional<" << row_type_name(query) << "> out_row;\n";
                out << "        std::size_t row_count = 0;\n";
                out << "        auto status = executor_.query_each(\"" << query.name << "\", "
                    << query.name << "_sql, params,\n";
                out << "            [&](const katana::sql::row_view& row) -> katana::result<void> "
                       "{\n";
                out << "                ++row_count;\n";
                out << "                if (row_count != 1) {\n";
                out << "                    return "
                       "std::unexpected(std::make_error_code(std::errc::invalid_argument));\n";
                out << "                }\n";
                out << "                auto mapped = map_" << query.name << "(row);\n";
                out << "                if (!mapped) {\n";
                out << "                    return std::unexpected(mapped.error());\n";
                out << "                }\n";
                out << "                out_row = std::move(*mapped);\n";
                out << "                return {};\n";
                out << "            });\n";
                out << "        if (!status) {\n";
                out << "            return std::unexpected(status.error());\n";
                out << "        }\n";
                out << "        if (row_count == 0) {\n";
                out << "            return std::optional<" << row_type_name(query) << ">{};\n";
                out << "        }\n";
                out << "        return out_row;\n";
            } else {
                out << "        std::vector<" << row_type_name(query) << "> out_rows;\n";
                out << "        auto status = executor_.query_each(\"" << query.name << "\", "
                    << query.name << "_sql, params,\n";
                out << "            [&](const katana::sql::row_view& row) -> katana::result<void> "
                       "{\n";
                out << "                auto mapped = map_" << query.name << "(row);\n";
                out << "                if (!mapped) {\n";
                out << "                    return std::unexpected(mapped.error());\n";
                out << "                }\n";
                out << "                out_rows.push_back(std::move(*mapped));\n";
                out << "                return {};\n";
                out << "            });\n";
                out << "        if (!status) {\n";
                out << "            return std::unexpected(status.error());\n";
                out << "        }\n";
                out << "        return out_rows;\n";
            }
        }
        out << "    }\n\n";

        out << "    bool " << query.name << "_async(";
        for (std::size_t i = 0; i < query.parameters.size(); ++i) {
            const auto& parameter = query.parameters[i];
            if (i != 0) {
                out << ", ";
            }
            out << param_cpp_type(parameter) << " " << param_identifier(parameter);
        }
        if (!query.parameters.empty()) {
            out << ", ";
        }
        out << query.name << "_async_handler handler) const {\n";
        out << "        if (!handler) {\n";
        out << "            return false;\n";
        out << "        }\n";
        out << "        if (async_executor_ == nullptr) {\n";
        out << "            // No async executor: run synchronously and deliver the result inline\n";
        out << "            // so the completion always fires (returning false here would hang a\n";
        out << "            // deferred response).\n";
        out << "            handler(" << query.name << "(";
        for (std::size_t i = 0; i < query.parameters.size(); ++i) {
            if (i != 0) {
                out << ", ";
            }
            out << param_identifier(query.parameters[i]);
        }
        out << "));\n";
        out << "            return true;\n";
        out << "        }\n";
        out << "        katana::sql::parameters params;\n";
        out << "        params.reserve(" << query.parameters.size() << ");\n";
        for (const auto& parameter : query.parameters) {
            out << "        params.push_back(katana::sql::encode_value("
                << param_identifier(parameter) << "));\n";
        }
        if (query.mode == sql_query_mode::exec) {
            out << "        return async_executor_->exec_async(\"" << query.name << "\", "
                << query.name << "_sql, std::move(params), std::move(handler));\n";
        } else if (query.mode == sql_query_mode::one) {
            out << "        return async_executor_->query_async(\"" << query.name << "\", "
                << query.name << "_sql, std::move(params),\n";
            out << "            [handler = std::move(handler)](katana::result<katana::sql::rows> "
                   "rows_result) {\n";
            out << "                if (!rows_result) {\n";
            out << "                    handler(std::unexpected(rows_result.error()));\n";
            out << "                    return;\n";
            out << "                }\n";
            out << "                if (rows_result->size() > 1) {\n";
            out << "                    "
                   "handler(std::unexpected(std::make_error_code(std::errc::invalid_argument)));\n";
            out << "                    return;\n";
            out << "                }\n";
            out << "                if (rows_result->empty()) {\n";
            out << "                    handler(std::optional<" << row_type_name(query)
                << ">{});\n";
            out << "                    return;\n";
            out << "                }\n";
            out << "                auto mapped = map_" << query.name
                << "(rows_result->front());\n";
            out << "                if (!mapped) {\n";
            out << "                    handler(std::unexpected(mapped.error()));\n";
            out << "                    return;\n";
            out << "                }\n";
            out << "                handler(std::optional<" << row_type_name(query)
                << ">(std::move(*mapped)));\n";
            out << "            });\n";
        } else {
            out << "        return async_executor_->query_async(\"" << query.name << "\", "
                << query.name << "_sql, std::move(params),\n";
            out << "            [handler = std::move(handler)](katana::result<katana::sql::rows> "
                   "rows_result) {\n";
            out << "                if (!rows_result) {\n";
            out << "                    handler(std::unexpected(rows_result.error()));\n";
            out << "                    return;\n";
            out << "                }\n";
            out << "                std::vector<" << row_type_name(query) << "> out_rows;\n";
            out << "                out_rows.reserve(rows_result->size());\n";
            out << "                for (const auto& row : *rows_result) {\n";
            out << "                    auto mapped = map_" << query.name << "(row);\n";
            out << "                    if (!mapped) {\n";
            out << "                        handler(std::unexpected(mapped.error()));\n";
            out << "                        return;\n";
            out << "                    }\n";
            out << "                    out_rows.push_back(std::move(*mapped));\n";
            out << "                }\n";
            out << "                handler(std::move(out_rows));\n";
            out << "            });\n";
        }
        out << "    }\n\n";

        // `<name>_step(args...)` packages this query as a katana::sql::query_step so several queries
        // can be fanned out with katana::sql::gather() over one pooled connection (pipelined on
        // Postgres) instead of hand-nesting the *_async callbacks (F14). exec-mode has no rows to
        // join, so it gets no step.
        if (query.mode != sql_query_mode::exec) {
            const std::string step_value = query.mode == sql_query_mode::one
                                               ? "std::optional<" + row_type_name(query) + ">"
                                               : "std::vector<" + row_type_name(query) + ">";
            out << "    katana::sql::query_step<" << step_value << "> " << query.name << "_step(";
            for (std::size_t i = 0; i < query.parameters.size(); ++i) {
                if (i != 0) {
                    out << ", ";
                }
                out << param_cpp_type(query.parameters[i]) << " "
                    << param_identifier(query.parameters[i]);
            }
            out << ") const {\n";
            out << "        katana::sql::parameters step_params;\n";
            out << "        step_params.reserve(" << query.parameters.size() << ");\n";
            for (const auto& parameter : query.parameters) {
                out << "        step_params.push_back(katana::sql::encode_value("
                    << param_identifier(parameter) << "));\n";
            }
            out << "        return katana::sql::query_step<" << step_value << ">{\"" << query.name
                << "\", " << query.name << "_sql, std::move(step_params),\n";
            out << "            [](katana::sql::rows rows_result) -> katana::result<" << step_value
                << "> {\n";
            if (query.mode == sql_query_mode::one) {
                out << "                if (rows_result.size() > 1) {\n";
                out << "                    return "
                       "std::unexpected(std::make_error_code(std::errc::invalid_argument));\n";
                out << "                }\n";
                out << "                if (rows_result.empty()) {\n";
                out << "                    return std::optional<" << row_type_name(query)
                    << ">{};\n";
                out << "                }\n";
                out << "                auto mapped = map_" << query.name
                    << "(rows_result.front());\n";
                out << "                if (!mapped) {\n";
                out << "                    return std::unexpected(mapped.error());\n";
                out << "                }\n";
                out << "                return std::optional<" << row_type_name(query)
                    << ">(std::move(*mapped));\n";
            } else {
                out << "                std::vector<" << row_type_name(query) << "> out_rows;\n";
                out << "                out_rows.reserve(rows_result.size());\n";
                out << "                for (const auto& row : rows_result) {\n";
                out << "                    auto mapped = map_" << query.name << "(row);\n";
                out << "                    if (!mapped) {\n";
                out << "                        return std::unexpected(mapped.error());\n";
                out << "                    }\n";
                out << "                    out_rows.push_back(std::move(*mapped));\n";
                out << "                }\n";
                out << "                return out_rows;\n";
            }
            out << "            }};\n";
            out << "    }\n\n";
        }

        // Named-args overloads — forward to the positional methods above (which stay the API of
        // record). Callers use `repo.query({.a = …, .b = …})` so same-type arguments can't swap.
        if (all_params_named(query)) {
            const std::string params_type = param_struct_type_name(query);
            out << "    " << return_type << " " << query.name << "(const " << params_type
                << "& args) const {\n";
            out << "        return " << query.name << "(";
            for (std::size_t i = 0; i < query.parameters.size(); ++i) {
                if (i != 0) {
                    out << ", ";
                }
                out << "args." << query.parameters[i].name;
            }
            out << ");\n";
            out << "    }\n\n";

            out << "    bool " << query.name << "_async(const " << params_type << "& args, "
                << query.name << "_async_handler handler) const {\n";
            out << "        return " << query.name << "_async(";
            for (const auto& parameter : query.parameters) {
                out << "args." << parameter.name << ", ";
            }
            out << "std::move(handler));\n";
            out << "    }\n\n";

            if (query.mode != sql_query_mode::exec) {
                const std::string step_value = query.mode == sql_query_mode::one
                                                   ? "std::optional<" + row_type_name(query) + ">"
                                                   : "std::vector<" + row_type_name(query) + ">";
                out << "    katana::sql::query_step<" << step_value << "> " << query.name
                    << "_step(const " << params_type << "& args) const {\n";
                out << "        return " << query.name << "_step(";
                for (std::size_t i = 0; i < query.parameters.size(); ++i) {
                    if (i != 0) {
                        out << ", ";
                    }
                    out << "args." << query.parameters[i].name;
                }
                out << ");\n";
                out << "    }\n\n";
            }
        }
    }

    out << "private:\n";
    out << "    katana::sql::executor& executor_;\n";
    out << "    katana::sql::async_executor* const async_executor_;\n\n";

    for (const auto& query : catalog.queries) {
        out << "    static constexpr std::string_view " << query.name
            << "_sql = " << raw_sql_literal(query.sql) << ";\n\n";
        if (query.mode == sql_query_mode::exec) {
            continue;
        }
        out << "    template <typename Row>\n";
        out << "    static katana::result<" << row_type_name(query) << "> map_" << query.name
            << "(const Row& row) {\n";
        out << "        " << row_type_name(query) << " out;\n";
        for (std::size_t column_index = 0; column_index < query.columns.size(); ++column_index) {
            const auto& column = query.columns[column_index];
            out << "        {\n";
            out << "            if (row.size() <= " << column_index << ") {\n";
            out << "                return "
                   "std::unexpected(std::make_error_code(std::errc::invalid_argument));\n";
            out << "            }\n";
            out << "            const auto& cell = row.at(" << column_index << ");\n";
            out << "            if (cell.has_value()) {\n";
            out << "                auto parsed = katana::sql::parse_value<" << column.cpp_type
                << ">(cell);\n";
            out << "                if (!parsed) {\n";
            out << "                    return std::unexpected(parsed.error());\n";
            out << "                }\n";
            out << "                out." << column.name << " = std::move(*parsed);\n";
            out << "            }\n";
            out << "        }\n";
        }
        out << "        return out;\n";
        out << "    }\n\n";
    }

    out << "};\n\n";
    out << "} // namespace " << sql_ns << "\n";
    return out.str();
}

} // namespace katana_gen
