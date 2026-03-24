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

std::string raw_sql_literal(const std::string& sql) {
    return "R\"__KATANA_SQL__(\n" + sql + "\n)__KATANA_SQL__\"";
}

std::string param_cpp_type(const sql_parameter& parameter) {
    if (parameter.cpp_type == "std::string") {
        return "std::string_view";
    }
    return parameter.cpp_type;
}

std::string optional_cpp_type(const std::string& cpp_type) {
    return "std::optional<" + cpp_type + ">";
}

} // namespace

std::string generate_sql_models(const sql_catalog& catalog) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include <cstdint>\n";
    out << "#include <optional>\n";
    out << "#include <string>\n\n";
    out << "namespace katana::sql::generated {\n\n";
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
    out << "} // namespace katana::sql::generated\n";
    return out.str();
}

std::string generate_sql_repository(const sql_catalog& catalog) {
    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "#include \"generated_sql_models.hpp\"\n";
    out << "#include \"katana/sql/runtime.hpp\"\n\n";
    out << "#include <optional>\n";
    out << "#include <string_view>\n";
    out << "#include <vector>\n\n";
    out << "namespace katana::sql::generated {\n\n";
    out << "class generated_repository {\n";
    out << "public:\n";
    out << "    explicit generated_repository(katana::sql::executor& executor) noexcept\n";
    out << "        : executor_(executor) {}\n\n";

    for (const auto& query : catalog.queries) {
        std::string return_type;
        if (query.mode == sql_query_mode::exec) {
            return_type = "katana::result<katana::sql::exec_result>";
        } else if (query.mode == sql_query_mode::one) {
            return_type = "katana::result<std::optional<" + row_type_name(query) + ">>";
        } else {
            return_type = "katana::result<std::vector<" + row_type_name(query) + ">>";
        }

        out << "    " << return_type << " " << query.name << "(";
        for (std::size_t i = 0; i < query.parameters.size(); ++i) {
            const auto& parameter = query.parameters[i];
            if (i != 0) {
                out << ", ";
            }
            out << param_cpp_type(parameter) << " p" << parameter.index;
        }
        out << ") const {\n";
        out << "        katana::sql::parameters params;\n";
        out << "        params.reserve(" << query.parameters.size() << ");\n";
        for (const auto& parameter : query.parameters) {
            out << "        params.push_back(katana::sql::encode_value(p" << parameter.index
                << "));\n";
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
                out << "            [&](const katana::sql::row_view& row) -> katana::result<void> {\n";
                out << "                ++row_count;\n";
                out << "                if (row_count != 1) {\n";
                out << "                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));\n";
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
                out << "            [&](const katana::sql::row_view& row) -> katana::result<void> {\n";
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
    }

    out << "private:\n";
    out << "    katana::sql::executor& executor_;\n\n";

    for (const auto& query : catalog.queries) {
        out << "    static constexpr std::string_view " << query.name << "_sql = "
            << raw_sql_literal(query.sql) << ";\n\n";
        if (query.mode == sql_query_mode::exec) {
            continue;
        }
        out << "    static katana::result<" << row_type_name(query) << "> map_" << query.name
            << "(const katana::sql::row_view& row) {\n";
        out << "        " << row_type_name(query) << " out;\n";
        for (std::size_t column_index = 0; column_index < query.columns.size(); ++column_index) {
            const auto& column = query.columns[column_index];
            out << "        {\n";
            out << "            if (row.size() <= " << column_index << ") {\n";
            out << "                return std::unexpected(std::make_error_code(std::errc::invalid_argument));\n";
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
    out << "} // namespace katana::sql::generated\n";
    return out.str();
}

} // namespace katana_gen
