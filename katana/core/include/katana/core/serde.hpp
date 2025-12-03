#pragma once

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace katana::serde {

inline std::string_view trim_view(std::string_view sv) noexcept {
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) {
        sv.remove_prefix(1);
    }
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.back()))) {
        sv.remove_suffix(1);
    }
    return sv;
}

struct json_cursor {
    const char* ptr;
    const char* end;

    bool eof() const noexcept { return ptr >= end; }

    void skip_ws() noexcept {
        while (!eof() && std::isspace(static_cast<unsigned char>(*ptr))) {
            ++ptr;
        }
    }

    bool consume(char c) noexcept {
        skip_ws();
        if (eof() || *ptr != c) {
            return false;
        }
        ++ptr;
        return true;
    }

    std::optional<std::string_view> string() noexcept {
        skip_ws();
        if (eof() || *ptr != '\"') {
            return std::nullopt;
        }
        ++ptr;
        const char* start = ptr;
        while (!eof() && *ptr != '\"') {
            if (*ptr == '\\' && (ptr + 1) < end) {
                ptr += 2;
                continue;
            }
            ++ptr;
        }
        if (eof()) {
            return std::nullopt;
        }
        const char* stop = ptr;
        ++ptr; // consume closing quote
        return std::string_view(start, static_cast<size_t>(stop - start));
    }

    bool try_object_start() noexcept { return consume('{'); }
    bool try_object_end() noexcept { return consume('}'); }
    bool try_array_start() noexcept { return consume('['); }
    bool try_array_end() noexcept { return consume(']'); }
    bool try_comma() noexcept { return consume(','); }

    void skip_value() noexcept {
        skip_ws();
        if (try_object_start()) {
            int depth = 1;
            while (!eof() && depth > 0) {
                if (try_object_start()) {
                    ++depth;
                } else if (try_object_end()) {
                    --depth;
                } else {
                    ++ptr;
                }
            }
            return;
        }
        if (try_array_start()) {
            int depth = 1;
            while (!eof() && depth > 0) {
                if (try_array_start()) {
                    ++depth;
                } else if (try_array_end()) {
                    --depth;
                } else {
                    ++ptr;
                }
            }
            return;
        }
        if (!eof() && *ptr == '\"') {
            (void)string();
            return;
        }
        while (!eof() && *ptr != ',' && *ptr != '}' && *ptr != ']') {
            ++ptr;
        }
    }
};

inline std::optional<size_t> parse_size(json_cursor& cur) noexcept {
    cur.skip_ws();
    if (cur.eof()) {
        return std::nullopt;
    }
    if (*cur.ptr == '\"') {
        if (auto sv = cur.string()) {
            size_t value = 0;
            auto fc = std::from_chars(sv->data(), sv->data() + sv->size(), value);
            if (fc.ec == std::errc()) {
                return value;
            }
        }
        return std::nullopt;
    }
    const char* start = cur.ptr;
    const char* p = start;
    if (p < cur.end && (*p == '+' || *p == '-')) {
        ++p;
    }
    while (p < cur.end && std::isdigit(static_cast<unsigned char>(*p))) {
        ++p;
    }
    if (p == start) {
        return std::nullopt;
    }
    size_t value = 0;
    auto fc = std::from_chars(start, p, value);
    if (fc.ec != std::errc()) {
        return std::nullopt;
    }
    cur.ptr = p;
    return value;
}

inline std::optional<double> parse_double(json_cursor& cur) noexcept {
    cur.skip_ws();
    if (cur.eof()) {
        return std::nullopt;
    }
    if (*cur.ptr == '\"') {
        if (auto sv = cur.string()) {
            double val = 0.0;
            auto [p, ec] = std::from_chars(sv->data(), sv->data() + sv->size(), val);
            if (ec == std::errc()) {
                return val;
            }
            char* endptr = nullptr;
            val = std::strtod(sv->data(), &endptr);
            if (endptr != sv->data()) {
                return val;
            }
        }
        return std::nullopt;
    }
    const char* start = cur.ptr;
    char* endptr = nullptr;
    double v = std::strtod(start, &endptr);
    if (endptr == start) {
        return std::nullopt;
    }
    cur.ptr = endptr;
    return v;
}

inline std::optional<bool> parse_bool(json_cursor& cur) noexcept {
    cur.skip_ws();
    if (cur.eof()) {
        return std::nullopt;
    }
    if (*cur.ptr == 't' || *cur.ptr == 'T') {
        cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(4), cur.end); // true
        return true;
    }
    if (*cur.ptr == 'f' || *cur.ptr == 'F') {
        cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(5), cur.end); // false
        return false;
    }
    if (*cur.ptr == '\"') {
        if (auto sv = cur.string()) {
            auto v = trim_view(*sv);
            if (v == "true") {
                return true;
            }
            if (v == "false") {
                return false;
            }
        }
    }
    return std::nullopt;
}

inline std::string_view parse_unquoted_string(json_cursor& cur) {
    cur.skip_ws();
    const char* start = cur.ptr;
    while (!cur.eof() && *cur.ptr != ',' && *cur.ptr != '}' && *cur.ptr != ']') {
        ++cur.ptr;
    }
    const char* end = cur.ptr;
    auto sv = std::string_view(start, static_cast<size_t>(end - start));
    return trim_view(sv);
}

inline bool is_bool_literal(std::string_view sv) noexcept {
    return sv == "true" || sv == "false";
}
inline bool is_null_literal(std::string_view sv) noexcept {
    return sv == "null";
}

struct yaml_node {
    enum class kind { scalar, object, array };

    kind k{kind::scalar};
    std::string scalar;
    std::vector<std::pair<std::string, std::unique_ptr<yaml_node>>> object;
    std::vector<std::unique_ptr<yaml_node>> array;

    static yaml_node scalar_node(std::string value) {
        yaml_node n;
        n.k = kind::scalar;
        n.scalar = std::move(value);
        return n;
    }

    static yaml_node object_node() {
        yaml_node n;
        n.k = kind::object;
        return n;
    }

    static yaml_node array_node() {
        yaml_node n;
        n.k = kind::array;
        return n;
    }
};

inline std::string escape_json_string(std::string_view sv) {
    std::string out;
    out.reserve(sv.size() + 8);
    for (char c : sv) {
        switch (c) {
        case '\\':
            out += "\\\\";
            break;
        case '\"':
            out += "\\\"";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            out.push_back(c);
            break;
        }
    }
    return out;
}

inline void emit_json(const yaml_node& n, std::string& out);

inline void emit_scalar(const std::string& v, std::string& out) {
    std::string_view sv = trim_view(v);
    if (is_bool_literal(sv) || is_null_literal(sv)) {
        out.append(sv);
        return;
    }
    if (sv.size() >= 2 &&
        ((sv.front() == '\"' && sv.back() == '\"') || (sv.front() == '\'' && sv.back() == '\''))) {
        sv = sv.substr(1, sv.size() - 2);
    }
    out.push_back('\"');
    out.append(escape_json_string(sv));
    out.push_back('\"');
}

inline void emit_json(const yaml_node& n, std::string& out) {
    switch (n.k) {
    case yaml_node::kind::scalar:
        emit_scalar(n.scalar, out);
        break;
    case yaml_node::kind::object: {
        out.push_back('{');
        for (size_t i = 0; i < n.object.size(); ++i) {
            if (i != 0) {
                out.push_back(',');
            }
            const auto& kv = n.object[i];
            out.push_back('\"');
            out.append(escape_json_string(kv.first));
            out.push_back('\"');
            out.push_back(':');
            emit_json(*kv.second, out);
        }
        out.push_back('}');
        break;
    }
    case yaml_node::kind::array: {
        out.push_back('[');
        for (size_t i = 0; i < n.array.size(); ++i) {
            if (i != 0) {
                out.push_back(',');
            }
            emit_json(*n.array[i], out);
        }
        out.push_back(']');
        break;
    }
    }
}

struct yaml_line {
    int indent;
    std::string_view content;
};

inline std::vector<yaml_line> tokenize_yaml(std::string_view text) {
    std::vector<yaml_line> lines;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find('\n', pos);
        if (end == std::string_view::npos) {
            end = text.size();
        }
        std::string_view line = text.substr(pos, end - pos);
        pos = end + 1;
        if (line.empty()) {
            continue;
        }
        int indent = 0;
        for (char c : line) {
            if (c == ' ') {
                ++indent;
            } else {
                break;
            }
        }
        std::string_view content = line.substr(static_cast<size_t>(indent));
        content = trim_view(content);
        if (content.empty() || content.front() == '#') {
            continue;
        }
        lines.push_back({indent, content});
    }
    return lines;
}

inline std::string normalize_key(std::string_view key) {
    auto trimmed = trim_view(key);
    if (trimmed.size() >= 2 && ((trimmed.front() == '\"' && trimmed.back() == '\"') ||
                                (trimmed.front() == '\'' && trimmed.back() == '\''))) {
        trimmed = trimmed.substr(1, trimmed.size() - 2);
    }
    return std::string(trimmed);
}

inline yaml_node parse_yaml_block(const std::vector<yaml_line>& lines, size_t& idx, int indent);

inline yaml_node parse_yaml_value(std::string_view value,
                                  const std::vector<yaml_line>& lines,
                                  size_t& idx,
                                  int indent) {
    yaml_node child;
    auto trimmed_val = trim_view(value);
    if (trimmed_val.empty()) {
        child = parse_yaml_block(lines, idx, indent + 2);
    } else {
        child = yaml_node::scalar_node(std::string(trimmed_val));
    }
    return child;
}

inline yaml_node parse_yaml_block(const std::vector<yaml_line>& lines, size_t& idx, int indent) {
    yaml_node node = yaml_node::object_node();

    while (idx < lines.size()) {
        const auto& ln = lines[idx];
        if (ln.indent < indent) {
            break;
        }
        if (ln.indent > indent) {
            ++idx;
            continue;
        }

        std::string_view content = ln.content;
        if (content.size() >= 2 && content[0] == '-' && content[1] == ' ') {
            if (node.k != yaml_node::kind::array) {
                node.k = yaml_node::kind::array;
                node.array.clear();
            }

            std::string_view item = trim_view(content.substr(2));
            ++idx;

            if (item.empty()) {
                node.array.push_back(
                    std::make_unique<yaml_node>(parse_yaml_block(lines, idx, indent + 2)));
                continue;
            }

            auto colon_pos = item.find(':');
            if (colon_pos != std::string_view::npos) {
                std::string key = normalize_key(item.substr(0, colon_pos));
                std::string_view val = trim_view(item.substr(colon_pos + 1));
                yaml_node obj = yaml_node::object_node();
                if (val.empty()) {
                    obj.object.emplace_back(
                        std::move(key),
                        std::make_unique<yaml_node>(parse_yaml_block(lines, idx, indent + 2)));
                } else {
                    obj.object.emplace_back(
                        std::move(key),
                        std::make_unique<yaml_node>(yaml_node::scalar_node(std::string(val))));
                }
                node.array.push_back(std::make_unique<yaml_node>(std::move(obj)));
            } else {
                node.array.push_back(
                    std::make_unique<yaml_node>(yaml_node::scalar_node(std::string(item))));
            }
        } else {
            auto colon_pos = content.find(':');
            if (colon_pos == std::string_view::npos) {
                ++idx;
                continue;
            }
            std::string key = normalize_key(content.substr(0, colon_pos));
            std::string_view val = trim_view(content.substr(colon_pos + 1));
            ++idx;

            if (node.k != yaml_node::kind::object) {
                node.k = yaml_node::kind::object;
                node.object.clear();
            }

            yaml_node child = parse_yaml_value(val, lines, idx, indent);
            node.object.emplace_back(std::move(key), std::make_unique<yaml_node>(std::move(child)));
        }
    }

    return node;
}

inline std::optional<std::string> yaml_to_json(std::string_view text) {
    auto lines = tokenize_yaml(text);
    if (lines.empty()) {
        return std::nullopt;
    }
    size_t idx = 0;
    yaml_node root = parse_yaml_block(lines, idx, lines.front().indent);
    std::string out;
    emit_json(root, out);
    return out;
}

} // namespace katana::serde
