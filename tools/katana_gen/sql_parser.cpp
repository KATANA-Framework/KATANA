#include "sql_codegen.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace fs = std::filesystem;

namespace katana_gen {

namespace {

std::string trim_copy(std::string_view sv) {
    std::size_t start = 0;
    while (start < sv.size() && std::isspace(static_cast<unsigned char>(sv[start]))) {
        ++start;
    }
    std::size_t end = sv.size();
    while (end > start && std::isspace(static_cast<unsigned char>(sv[end - 1]))) {
        --end;
    }
    return std::string(sv.substr(start, end - start));
}

std::string to_lower_ascii(std::string_view sv) {
    std::string out;
    out.reserve(sv.size());
    for (char c : sv) {
        out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return out;
}

std::string normalize_type_name(std::string_view sv) {
    auto trimmed = trim_copy(sv);
    std::string normalized;
    normalized.reserve(trimmed.size());
    bool last_was_space = false;
    for (char c : trimmed) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!last_was_space) {
                normalized.push_back('_');
            }
            last_was_space = true;
            continue;
        }
        normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        last_was_space = false;
    }
    return normalized;
}

std::string map_pg_type_to_cpp(std::string_view pg_type) {
    const auto normalized = normalize_type_name(pg_type);
    if (normalized.empty()) {
        return "std::string";
    }
    if (normalized.ends_with("[]")) {
        return "std::vector<" + map_pg_type_to_cpp(normalized.substr(0, normalized.size() - 2)) +
               ">";
    }
    if (normalized.starts_with('_')) {
        return "std::vector<" + map_pg_type_to_cpp(normalized.substr(1)) + ">";
    }
    if (normalized == "smallint" || normalized == "int2") {
        return "int16_t";
    }
    if (normalized == "integer" || normalized == "int" || normalized == "int4") {
        return "int32_t";
    }
    if (normalized == "bigint" || normalized == "int8") {
        return "int64_t";
    }
    if (normalized == "bool" || normalized == "boolean") {
        return "bool";
    }
    if (normalized == "real" || normalized == "float4") {
        return "float";
    }
    if (normalized == "double_precision" || normalized == "float8" || normalized == "numeric" ||
        normalized == "decimal") {
        return "double";
    }
    return "std::string";
}

std::string sanitize_identifier_local(std::string_view sv) {
    std::string out;
    out.reserve(sv.size() + 2);
    for (char c : sv) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        } else {
            out.push_back('_');
        }
    }
    if (out.empty()) {
        out = "value";
    }
    if (std::isdigit(static_cast<unsigned char>(out.front()))) {
        out.insert(out.begin(), '_');
    }
    return out;
}

std::optional<std::string> read_file(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return std::nullopt;
    }
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

std::optional<std::pair<std::string, sql_query_mode>> parse_header(std::string_view content) {
    const auto first_line_end = content.find('\n');
    std::string header = trim_copy(content.substr(0, first_line_end));
    if (!header.starts_with("--")) {
        return std::nullopt;
    }

    header.erase(0, 2);
    header = trim_copy(header);
    if (!to_lower_ascii(header).starts_with("name:")) {
        return std::nullopt;
    }

    header.erase(0, 5);
    const auto mode_pos = header.rfind(':');
    if (mode_pos == std::string::npos) {
        return std::nullopt;
    }

    const std::string name = trim_copy(header.substr(0, mode_pos));
    const std::string mode_token = trim_copy(header.substr(mode_pos + 1));
    if (name.empty()) {
        return std::nullopt;
    }

    if (!std::isalpha(static_cast<unsigned char>(name.front())) && name.front() != '_') {
        return std::nullopt;
    }
    for (char c : name) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
            return std::nullopt;
        }
    }

    sql_query_mode mode = sql_query_mode::exec;
    if (mode_token == "one") {
        mode = sql_query_mode::one;
    } else if (mode_token == "many") {
        mode = sql_query_mode::many;
    } else if (mode_token != "exec") {
        return std::nullopt;
    }
    return std::make_pair(name, mode);
}

std::string extract_body(std::string_view content) {
    const auto pos = content.find('\n');
    if (pos == std::string_view::npos) {
        return {};
    }
    return trim_copy(content.substr(pos + 1));
}

std::optional<std::string> read_type_name(std::string_view expr, std::size_t start_pos) {
    static const std::unordered_set<std::string> continuation_tokens = {
        "precision",
        "varying",
        "with",
        "without",
        "time",
        "zone",
    };

    std::size_t pos = start_pos;
    while (pos < expr.size() && std::isspace(static_cast<unsigned char>(expr[pos]))) {
        ++pos;
    }

    const std::size_t first_start = pos;
    while (pos < expr.size()) {
        const char c = expr[pos];
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            ++pos;
            continue;
        }
        break;
    }
    if (pos == first_start) {
        return std::nullopt;
    }

    std::string raw(expr.substr(first_start, pos - first_start));

    while (pos < expr.size()) {
        const std::size_t space_start = pos;
        while (pos < expr.size() && std::isspace(static_cast<unsigned char>(expr[pos]))) {
            ++pos;
        }
        if (pos == space_start) {
            break;
        }

        const std::size_t token_start = pos;
        while (pos < expr.size()) {
            const char c = expr[pos];
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
                ++pos;
                continue;
            }
            break;
        }
        if (pos == token_start) {
            break;
        }

        const std::string token = to_lower_ascii(expr.substr(token_start, pos - token_start));
        if (!continuation_tokens.contains(token)) {
            break;
        }

        raw.push_back(' ');
        raw += token;
    }

    while (pos < expr.size()) {
        while (pos < expr.size() && std::isspace(static_cast<unsigned char>(expr[pos]))) {
            ++pos;
        }
        if (pos + 1 >= expr.size() || expr[pos] != '[' || expr[pos + 1] != ']') {
            break;
        }
        raw += "[]";
        pos += 2;
    }

    return normalize_type_name(raw);
}

std::optional<std::string> extract_cast_type(std::string_view expr) {
    const auto cast_pos = expr.rfind("::");
    if (cast_pos == std::string_view::npos) {
        return std::nullopt;
    }
    std::size_t pos = cast_pos + 2;
    while (pos < expr.size() && std::isspace(static_cast<unsigned char>(expr[pos]))) {
        ++pos;
    }
    return read_type_name(expr, pos);
}

std::vector<std::string> split_top_level_csv(std::string_view text) {
    std::vector<std::string> parts;
    std::string current;
    int paren_depth = 0;
    bool in_single_quote = false;
    for (char c : text) {
        if (c == '\'' && (current.empty() || current.back() != '\\')) {
            in_single_quote = !in_single_quote;
        }
        if (!in_single_quote) {
            if (c == '(') {
                ++paren_depth;
            } else if (c == ')' && paren_depth > 0) {
                --paren_depth;
            } else if (c == ',' && paren_depth == 0) {
                parts.push_back(trim_copy(current));
                current.clear();
                continue;
            }
        }
        current.push_back(c);
    }
    if (!current.empty()) {
        parts.push_back(trim_copy(current));
    }
    return parts;
}

std::optional<std::string> extract_alias(std::string_view expr) {
    const std::string lower = to_lower_ascii(expr);
    const auto as_pos = lower.rfind(" as ");
    if (as_pos != std::string::npos) {
        auto alias = trim_copy(expr.substr(as_pos + 4));
        while (!alias.empty() && !std::isalnum(static_cast<unsigned char>(alias.back())) &&
               alias.back() != '_') {
            alias.pop_back();
        }
        if (!alias.empty()) {
            return sanitize_identifier_local(alias);
        }
    }

    std::string_view base = expr;
    const auto cast_pos = base.rfind("::");
    if (cast_pos != std::string_view::npos) {
        base = base.substr(0, cast_pos);
    }
    std::string base_owned = trim_copy(base);
    while (!base_owned.empty() && !std::isalnum(static_cast<unsigned char>(base_owned.back())) &&
           base_owned.back() != '_') {
        base_owned.pop_back();
    }
    std::size_t start = base_owned.size();
    while (start > 0) {
        const char c = base_owned[start - 1];
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            --start;
            continue;
        }
        break;
    }
    if (start < base_owned.size()) {
        return sanitize_identifier_local(base_owned.substr(start));
    }
    return std::nullopt;
}

std::string strip_sql_comments(std::string_view sql) {
    std::stringstream input{std::string(sql)};
    std::string line;
    std::string out;
    while (std::getline(input, line)) {
        const auto comment_pos = line.find("--");
        if (comment_pos != std::string::npos) {
            line.erase(comment_pos);
        }
        auto trimmed = trim_copy(line);
        if (!trimmed.empty()) {
            if (!out.empty()) {
                out.push_back('\n');
            }
            out += trimmed;
        }
    }
    return out;
}

std::optional<std::string> extract_projection_segment(std::string_view sql,
                                                      std::string_view start_keyword,
                                                      std::string_view end_keyword) {
    const std::string lower = to_lower_ascii(sql);
    const std::string start = to_lower_ascii(start_keyword);
    const std::string end = to_lower_ascii(end_keyword);

    // Find a keyword as a whole word at parenthesis depth 0 (skipping anything
    // nested in parens, e.g. the inner SELECT/FROM of a `WITH cte AS (...)` clause
    // or a FROM-subquery). This makes the FINAL/top-level SELECT ... FROM the one
    // whose projection columns are extracted, not a CTE's inner SELECT.
    const auto find_keyword_depth0 = [&lower](const std::string& kw,
                                              std::size_t from) -> std::size_t {
        int paren_depth = 0;
        bool in_single_quote = false;
        for (std::size_t pos = from; pos < lower.size(); ++pos) {
            const char current = lower[pos];
            if (current == '\'' && (pos == 0 || lower[pos - 1] != '\\')) {
                in_single_quote = !in_single_quote;
            }
            if (in_single_quote) {
                continue;
            }
            if (current == '(') {
                ++paren_depth;
                continue;
            }
            if (current == ')' && paren_depth > 0) {
                --paren_depth;
                continue;
            }
            if (paren_depth != 0) {
                continue;
            }
            if (pos + kw.size() > lower.size() || lower.compare(pos, kw.size(), kw) != 0) {
                continue;
            }
            const bool before_ok =
                pos == 0 || !std::isalnum(static_cast<unsigned char>(lower[pos - 1]));
            const bool after_ok =
                pos + kw.size() >= lower.size() ||
                !std::isalnum(static_cast<unsigned char>(lower[pos + kw.size()]));
            if (before_ok && after_ok) {
                return pos;
            }
        }
        return std::string::npos;
    };

    const auto start_pos = find_keyword_depth0(start, 0);
    if (start_pos == std::string::npos) {
        return std::nullopt;
    }
    const auto body_start = start_pos + start.size();
    if (end.empty()) {
        return trim_copy(sql.substr(body_start));
    }
    const auto end_pos = find_keyword_depth0(end, body_start);
    if (end_pos == std::string::npos) {
        return trim_copy(sql.substr(body_start));
    }
    return trim_copy(sql.substr(body_start, end_pos - body_start));
}

// Rewrite `@name` placeholders to positional `$N` in first-appearance order (repeats reuse the same
// index), returning index→name. Postgres only understands `$N`, so the emitted SQL string uses the
// positional form; the names are kept only to generate readable C++ parameter names. A query that
// uses raw `$N` has no `@` tokens, so this is a no-op there (back-compat: those stay `pN`).
//
// The scan is lexer-aware: `@name` is only treated as a placeholder in "code" state — never inside a
// `--` line comment, a `/* */` block comment, a `'...'` string literal (with `''` escape), or a
// `$tag$...$tag$` dollar-quoted string. (Previously an `@word` in a comment minted a phantom
// parameter — a real bug this fixes.)
std::unordered_map<std::size_t, std::string> rewrite_named_parameters(std::string& sql) {
    std::unordered_map<std::string, std::size_t> index_by_name;
    std::unordered_map<std::size_t, std::string> name_by_index;
    std::string out;
    out.reserve(sql.size());

    enum class lex { code, line_comment, block_comment, single_quote, dollar_quote };
    lex st = lex::code;
    std::string dollar_tag; // active `$tag$` delimiter while in dollar_quote

    for (std::size_t pos = 0; pos < sql.size();) {
        const char c = sql[pos];
        switch (st) {
            case lex::line_comment:
                out += c;
                ++pos;
                if (c == '\n') {
                    st = lex::code;
                }
                continue;
            case lex::block_comment:
                if (c == '*' && pos + 1 < sql.size() && sql[pos + 1] == '/') {
                    out += "*/";
                    pos += 2;
                    st = lex::code;
                } else {
                    out += c;
                    ++pos;
                }
                continue;
            case lex::single_quote:
                out += c;
                ++pos;
                if (c == '\'') {
                    if (pos < sql.size() && sql[pos] == '\'') {
                        out += '\''; // doubled quote is an escaped quote — stay in the string
                        ++pos;
                    } else {
                        st = lex::code;
                    }
                }
                continue;
            case lex::dollar_quote:
                if (!dollar_tag.empty() && sql.compare(pos, dollar_tag.size(), dollar_tag) == 0) {
                    out += dollar_tag;
                    pos += dollar_tag.size();
                    st = lex::code;
                } else {
                    out += c;
                    ++pos;
                }
                continue;
            case lex::code:
                break;
        }

        // ── code state: enter comment/string states, else detect @name ──────────────────────
        if (c == '-' && pos + 1 < sql.size() && sql[pos + 1] == '-') {
            out += "--";
            pos += 2;
            st = lex::line_comment;
            continue;
        }
        if (c == '/' && pos + 1 < sql.size() && sql[pos + 1] == '*') {
            out += "/*";
            pos += 2;
            st = lex::block_comment;
            continue;
        }
        if (c == '\'') {
            out += c;
            ++pos;
            st = lex::single_quote;
            continue;
        }
        if (c == '$') { // dollar-quote open: `$$` or `$tag$` (tag: empty or ident) — not a `$N` param
            std::size_t e = pos + 1;
            if (e < sql.size() &&
                (std::isalpha(static_cast<unsigned char>(sql[e])) || sql[e] == '_')) {
                ++e;
                while (e < sql.size() &&
                       (std::isalnum(static_cast<unsigned char>(sql[e])) || sql[e] == '_')) {
                    ++e;
                }
            }
            if (e < sql.size() && sql[e] == '$') {
                dollar_tag = sql.substr(pos, e - pos + 1);
                out += dollar_tag;
                pos = e + 1;
                st = lex::dollar_quote;
                continue;
            }
        }

        const bool is_name_start =
            c == '@' && pos + 1 < sql.size() &&
            (std::isalpha(static_cast<unsigned char>(sql[pos + 1])) || sql[pos + 1] == '_');
        if (!is_name_start) {
            out += c;
            ++pos;
            continue;
        }
        std::size_t start = pos + 1;
        std::size_t end = start;
        while (end < sql.size() &&
               (std::isalnum(static_cast<unsigned char>(sql[end])) || sql[end] == '_')) {
            ++end;
        }
        std::string name = sql.substr(start, end - start);
        auto [it, inserted] = index_by_name.try_emplace(name, index_by_name.size() + 1);
        if (inserted) {
            name_by_index.emplace(it->second, name);
        }
        out += '$';
        out += std::to_string(it->second);
        pos = end;
    }
    sql = std::move(out);
    return name_by_index;
}

std::vector<sql_parameter> parse_parameters(std::string_view sql) {
    std::unordered_map<std::size_t, std::string> type_by_index;
    for (std::size_t pos = 0; pos < sql.size(); ++pos) {
        if (sql[pos] != '$') {
            continue;
        }
        std::size_t index_start = pos + 1;
        std::size_t index_end = index_start;
        while (index_end < sql.size() && std::isdigit(static_cast<unsigned char>(sql[index_end]))) {
            ++index_end;
        }
        if (index_end == index_start) {
            continue;
        }

        const auto index = static_cast<std::size_t>(
            std::stoul(std::string(sql.substr(index_start, index_end - index_start))));

        std::string type_name;
        std::size_t cursor = index_end;
        while (cursor < sql.size() && std::isspace(static_cast<unsigned char>(sql[cursor]))) {
            ++cursor;
        }
        if (cursor + 1 < sql.size() && sql[cursor] == ':' && sql[cursor + 1] == ':') {
            cursor += 2;
            while (cursor < sql.size() && std::isspace(static_cast<unsigned char>(sql[cursor]))) {
                ++cursor;
            }
            auto resolved_type = read_type_name(sql, cursor);
            if (resolved_type) {
                type_name = std::move(*resolved_type);
            }
        }

        auto& slot = type_by_index[index];
        if (slot.empty() && !type_name.empty()) {
            slot = std::move(type_name);
        }
    }

    std::vector<sql_parameter> parameters;
    parameters.reserve(type_by_index.size());
    for (const auto& [index, type_name] : type_by_index) {
        const auto resolved_type = type_name.empty() ? std::string("text") : type_name;
        parameters.push_back(sql_parameter{
            .index = index,
            .name = {}, // filled in by parse_sql_source from @name placeholders, if any
            .pg_type = resolved_type,
            .cpp_type = map_pg_type_to_cpp(resolved_type),
        });
    }

    std::sort(parameters.begin(), parameters.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.index < rhs.index;
    });
    return parameters;
}

std::vector<sql_column> parse_columns(std::string_view sql, sql_query_mode mode) {
    if (mode == sql_query_mode::exec &&
        to_lower_ascii(sql).find("returning") == std::string::npos) {
        return {};
    }

    const auto without_comments = strip_sql_comments(sql);
    const auto lowered = to_lower_ascii(trim_copy(without_comments));
    const bool prefer_returning = lowered.starts_with("insert") || lowered.starts_with("update") ||
                                  lowered.starts_with("delete");

    std::optional<std::string> segment;
    if (prefer_returning) {
        segment = extract_projection_segment(without_comments, "returning", "");
    }
    if (!segment) {
        segment = extract_projection_segment(without_comments, "select", "from");
    }
    if (!segment) {
        segment = extract_projection_segment(without_comments, "returning", "");
    }
    if (!segment || segment->empty()) {
        return {};
    }

    auto expressions = split_top_level_csv(*segment);
    std::vector<sql_column> columns;
    columns.reserve(expressions.size());
    for (std::size_t i = 0; i < expressions.size(); ++i) {
        const auto& expr = expressions[i];
        auto alias = extract_alias(expr);
        const auto type_name = extract_cast_type(expr).value_or("text");
        columns.push_back(sql_column{
            .name = alias.value_or("column_" + std::to_string(i + 1)),
            .pg_type = type_name,
            .cpp_type = map_pg_type_to_cpp(type_name),
        });
    }
    return columns;
}

katana::result<sql_query> parse_sql_source(const sql_source& source) {
    auto header = parse_header(source.content);
    if (!header) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    auto body = extract_body(source.content);
    if (body.empty()) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    sql_query query;
    query.name = header->first;
    query.source_path = source.path;
    query.mode = header->second;
    query.sql = std::move(body);
    auto param_names = rewrite_named_parameters(query.sql); // @name → $N, in place
    query.parameters = parse_parameters(query.sql);
    for (auto& param : query.parameters) {
        if (auto it = param_names.find(param.index); it != param_names.end()) {
            param.name = it->second;
        }
    }
    query.columns = parse_columns(query.sql, query.mode);
    if (query.mode == sql_query_mode::exec && !query.columns.empty()) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
    return query;
}

} // namespace

std::string sql_mode_literal(sql_query_mode mode) {
    switch (mode) {
    case sql_query_mode::one:
        return "one";
    case sql_query_mode::many:
        return "many";
    case sql_query_mode::exec:
        return "exec";
    }
    return "exec";
}

katana::result<sql_catalog> load_sql_catalog(const fs::path& input_root) {
    std::error_code fs_ec;
    if (!fs::exists(input_root, fs_ec) || fs_ec) {
        return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
    }
    if (!fs::is_directory(input_root, fs_ec) || fs_ec) {
        return std::unexpected(std::make_error_code(std::errc::not_a_directory));
    }

    std::vector<fs::path> files;
    for (fs::recursive_directory_iterator it(input_root, fs_ec), end; it != end && !fs_ec; ++it) {
        if (it->is_regular_file() && it->path().extension() == ".sql") {
            files.push_back(it->path());
        }
    }
    if (fs_ec) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }

    std::sort(files.begin(), files.end());

    std::vector<sql_source> sources;
    sources.reserve(files.size());
    for (const auto& file : files) {
        auto content = read_file(file);
        if (!content) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }
        sources.push_back(sql_source{.path = fs::relative(file, input_root), .content = *content});
    }

    return load_sql_catalog_from_sources(std::move(sources));
}

katana::result<sql_catalog> load_sql_catalog_from_sources(std::vector<sql_source> sources) {
    std::sort(sources.begin(), sources.end(), [](const sql_source& lhs, const sql_source& rhs) {
        return lhs.path.generic_string() < rhs.path.generic_string();
    });

    sql_catalog catalog;
    catalog.queries.reserve(sources.size());
    std::unordered_set<std::string> seen_names;
    for (const auto& source : sources) {
        auto query = parse_sql_source(source);
        if (!query) {
            std::cerr << "[sql] " << source.path.generic_string()
                      << ": failed to parse (" << query.error().message()
                      << "). Expected a `-- name: <name> :one|:many|:exec` header and a "
                         "non-empty query body; an :exec query must not RETURN columns.\n";
            return std::unexpected(query.error());
        }
        if (!seen_names.insert(query->name).second) {
            std::cerr << "[sql] " << source.path.generic_string() << ": duplicate query name '"
                      << query->name << "' (already defined in another file)\n";
            return std::unexpected(std::make_error_code(std::errc::file_exists));
        }
        catalog.queries.push_back(std::move(*query));
    }
    return catalog;
}

std::string dump_sql_ast_summary(const sql_catalog& catalog) {
    std::string out;
    out += "{\"queries\":[";
    for (std::size_t i = 0; i < catalog.queries.size(); ++i) {
        const auto& query = catalog.queries[i];
        if (i != 0) {
            out += ',';
        }
        out += "{\"name\":\"" + query.name + "\",\"mode\":\"" + sql_mode_literal(query.mode) +
               "\",\"path\":\"" + query.source_path.generic_string() + "\",\"params\":[";
        for (std::size_t p = 0; p < query.parameters.size(); ++p) {
            const auto& param = query.parameters[p];
            if (p != 0) {
                out += ',';
            }
            out += "{\"index\":" + std::to_string(param.index) + ",\"name\":\"" + param.name +
                   "\",\"type\":\"" + param.pg_type + "\",\"cpp\":\"" + param.cpp_type + "\"}";
        }
        out += "],\"columns\":[";
        for (std::size_t c = 0; c < query.columns.size(); ++c) {
            const auto& column = query.columns[c];
            if (c != 0) {
                out += ',';
            }
            out += "{\"name\":\"" + column.name + "\",\"type\":\"" + column.pg_type +
                   "\",\"cpp\":\"" + column.cpp_type + "\"}";
        }
        out += "]}";
    }
    out += "]}";
    return out;
}

} // namespace katana_gen
