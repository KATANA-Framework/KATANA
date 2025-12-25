#pragma once

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef __SSE2__
#include <emmintrin.h> // SSE2
#endif

namespace katana::serde {

// Fast JSON whitespace detection (space, tab, newline, carriage return)
// Lookup table is faster than std::isspace (avoids function call and locale checks)
constexpr bool is_json_whitespace_table[256] = {
    false, false, false, false, false, false, false, false, false, true,  true,  false, false,
    true,  false, false, // 0x09=tab, 0x0A=\n, 0x0D=\r
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, true,  false, false, false, false, false, false, false, // 0x20=space
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false,
};

inline constexpr bool is_json_whitespace(unsigned char c) noexcept {
    return is_json_whitespace_table[c];
}

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
    const char* start; // Track start for position calculation

    json_cursor(const char* p, const char* e) : ptr(p), end(e), start(p) {}

    bool eof() const noexcept { return ptr >= end; }

    size_t pos() const noexcept { return static_cast<size_t>(ptr - start); }

    void skip_ws() noexcept {
        // Fast path: if no whitespace at current position, return immediately
        // This is critical for compact JSON where whitespace is minimal
        if (eof() || !is_json_whitespace(static_cast<unsigned char>(*ptr))) {
            return;
        }

#ifdef __SSE2__
        // SIMD path: only use for large amounts of whitespace (8+ chars)
        // This amortizes the SIMD setup overhead
        constexpr size_t simd_threshold = 8;
        constexpr size_t simd_width = 16;

        // Quick scalar skip for small whitespace runs
        const char* scan_ptr = ptr;
        size_t count = 0;
        while (count < simd_threshold && scan_ptr < end &&
               is_json_whitespace(static_cast<unsigned char>(*scan_ptr))) {
            ++scan_ptr;
            ++count;
        }

        // If less than threshold, use scalar result
        if (count < simd_threshold) {
            ptr = scan_ptr;
            return;
        }

        // We have significant whitespace - use SIMD
        ptr = scan_ptr; // Start from where scalar left off

        while (static_cast<size_t>(end - ptr) >= simd_width) {
            __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));

            __m128i space = _mm_set1_epi8(' ');
            __m128i tab = _mm_set1_epi8('\t');
            __m128i lf = _mm_set1_epi8('\n');
            __m128i cr = _mm_set1_epi8('\r');

            __m128i eq_space = _mm_cmpeq_epi8(chunk, space);
            __m128i eq_tab = _mm_cmpeq_epi8(chunk, tab);
            __m128i eq_lf = _mm_cmpeq_epi8(chunk, lf);
            __m128i eq_cr = _mm_cmpeq_epi8(chunk, cr);

            __m128i is_ws =
                _mm_or_si128(_mm_or_si128(eq_space, eq_tab), _mm_or_si128(eq_lf, eq_cr));

            int mask = _mm_movemask_epi8(is_ws);

            if (mask == 0) {
                break;
            }

            if (mask == 0xFFFF) {
                ptr += simd_width;
                continue;
            }

            int first_non_ws = __builtin_ctz(~mask & 0xFFFF);
            ptr += first_non_ws;
            return;
        }
#endif

        // Scalar fallback for remaining bytes
        while (!eof() && is_json_whitespace(static_cast<unsigned char>(*ptr))) {
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
        const char* str_start = ptr;
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
        return std::string_view(str_start, static_cast<size_t>(stop - str_start));
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
        }
        return std::nullopt;
    }
    // Fast path: use std::from_chars instead of std::strtod (2-3x faster)
    const char* start = cur.ptr;
    double v = 0.0;
    auto [p, ec] = std::from_chars(start, cur.end, v);
    if (ec != std::errc() || p == start) {
        return std::nullopt;
    }
    cur.ptr = p;
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

struct yaml_diagnostic {
    size_t line = 0;
    std::string message;
};

inline void set_yaml_error(yaml_diagnostic* diag, size_t line, std::string_view message) {
    if (!diag || !diag->message.empty()) {
        return;
    }
    diag->line = line;
    diag->message.assign(message.begin(), message.end());
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

inline std::vector<std::string_view> split_top_level(std::string_view input,
                                                     char delimiter = ',') noexcept {
    std::vector<std::string_view> parts;
    size_t start = 0;
    int depth = 0;
    bool in_single = false;
    bool in_double = false;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c == '\'' && !in_double) {
            in_single = !in_single;
        } else if (c == '\"' && !in_single) {
            in_double = !in_double;
        } else if (!in_single && !in_double) {
            if (c == '{' || c == '[') {
                ++depth;
            } else if (c == '}' || c == ']') {
                --depth;
            } else if (c == delimiter && depth == 0) {
                parts.push_back(trim_view(input.substr(start, i - start)));
                start = i + 1;
            }
        }
    }

    auto tail = trim_view(input.substr(start));
    if (!tail.empty()) {
        parts.push_back(tail);
    }
    return parts;
}

inline yaml_node parse_yaml_block(const std::vector<yaml_line>& lines,
                                  size_t& idx,
                                  int indent,
                                  yaml_diagnostic* diag);

inline yaml_node parse_inline_map(std::string_view value, yaml_diagnostic* diag, size_t line_no);
inline yaml_node
parse_inline_sequence(std::string_view value, yaml_diagnostic* diag, size_t line_no);
inline yaml_node parse_inline_value(std::string_view value, yaml_diagnostic* diag, size_t line_no) {
    auto trimmed = trim_view(value);
    if (trimmed.empty()) {
        return yaml_node::scalar_node("");
    }
    if (trimmed.front() == '{' && trimmed.back() == '}') {
        return parse_inline_map(trimmed, diag, line_no);
    }
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        return parse_inline_sequence(trimmed, diag, line_no);
    }
    return yaml_node::scalar_node(std::string(trimmed));
}

inline yaml_node parse_inline_map(std::string_view value, yaml_diagnostic* diag, size_t line_no) {
    yaml_node obj = yaml_node::object_node();
    auto inner = trim_view(value.substr(1, value.size() - 2));
    std::unordered_set<std::string> seen;
    for (auto part : split_top_level(inner)) {
        if (part.empty()) {
            continue;
        }
        auto colon = part.find(':');
        if (colon == std::string_view::npos) {
            continue;
        }
        auto key = normalize_key(part.substr(0, colon));
        auto val = trim_view(part.substr(colon + 1));
        if (seen.contains(key)) {
            set_yaml_error(diag, line_no, std::string("duplicate key '") + key + "' in inline map");
            auto it = std::find_if(obj.object.begin(), obj.object.end(), [&](const auto& kv) {
                return kv.first == key;
            });
            if (it != obj.object.end()) {
                obj.object.erase(it);
            }
        }
        seen.insert(key);
        obj.object.emplace_back(
            std::move(key), std::make_unique<yaml_node>(parse_inline_value(val, diag, line_no)));
    }
    return obj;
}

inline yaml_node
parse_inline_sequence(std::string_view value, yaml_diagnostic* diag, size_t line_no) {
    yaml_node arr = yaml_node::array_node();
    auto inner = trim_view(value.substr(1, value.size() - 2));
    for (auto part : split_top_level(inner)) {
        if (part.empty()) {
            continue;
        }
        arr.array.push_back(std::make_unique<yaml_node>(parse_inline_value(part, diag, line_no)));
    }
    return arr;
}

inline yaml_node parse_yaml_value(std::string_view value,
                                  const std::vector<yaml_line>& lines,
                                  size_t& idx,
                                  int indent,
                                  yaml_diagnostic* diag,
                                  size_t current_line = 0) {
    const size_t line_no = current_line ? current_line : (idx + 1);
    yaml_node child;
    auto trimmed_val = trim_view(value);
    if (trimmed_val.empty()) {
        child = parse_yaml_block(lines, idx, indent + 2, diag);
    } else if ((trimmed_val.front() == '{' && trimmed_val.back() == '}') ||
               (trimmed_val.front() == '[' && trimmed_val.back() == ']')) {
        child = parse_inline_value(trimmed_val, diag, line_no);
    } else {
        child = yaml_node::scalar_node(std::string(trimmed_val));
    }
    return child;
}

inline yaml_node parse_yaml_block(const std::vector<yaml_line>& lines,
                                  size_t& idx,
                                  int indent,
                                  yaml_diagnostic* diag) {
    yaml_node node = yaml_node::object_node();
    std::unordered_set<std::string> seen_keys;

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
            size_t line_no = idx + 1;
            ++idx;

            if (item.empty()) {
                node.array.push_back(
                    std::make_unique<yaml_node>(parse_yaml_block(lines, idx, indent + 2, diag)));
                continue;
            }

            auto colon_pos = item.find(':');
            if (colon_pos != std::string_view::npos) {
                std::string key = normalize_key(item.substr(0, colon_pos));
                std::string_view val = trim_view(item.substr(colon_pos + 1));
                yaml_node obj = yaml_node::object_node();
                yaml_node parsed_val = parse_yaml_value(val, lines, idx, indent, diag, line_no);
                obj.object.emplace_back(std::move(key),
                                        std::make_unique<yaml_node>(std::move(parsed_val)));
                if (idx < lines.size() && lines[idx].indent > indent) {
                    int child_indent = lines[idx].indent;
                    yaml_node extra = parse_yaml_block(lines, idx, child_indent, diag);
                    if (extra.k == yaml_node::kind::object) {
                        for (auto& kv : extra.object) {
                            obj.object.emplace_back(std::move(kv.first), std::move(kv.second));
                        }
                    }
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
            size_t line_no = idx + 1;
            ++idx;

            if (node.k != yaml_node::kind::object) {
                node.k = yaml_node::kind::object;
                node.object.clear();
            }

            yaml_node child = parse_yaml_value(val, lines, idx, indent, diag, line_no);
            if (seen_keys.contains(key)) {
                set_yaml_error(diag, line_no, std::string("duplicate key '") + key + "'");
                auto it = std::find_if(node.object.begin(), node.object.end(), [&](const auto& kv) {
                    return kv.first == key;
                });
                if (it != node.object.end()) {
                    node.object.erase(it);
                }
            }
            seen_keys.insert(key);
            node.object.emplace_back(std::move(key), std::make_unique<yaml_node>(std::move(child)));
        }
    }

    return node;
}

inline std::optional<std::string> yaml_to_json(std::string_view text,
                                               std::string* error = nullptr) {
    auto lines = tokenize_yaml(text);
    if (lines.empty()) {
        return std::nullopt;
    }
    size_t idx = 0;
    yaml_diagnostic diag;
    yaml_node root = parse_yaml_block(lines, idx, lines.front().indent, &diag);
    if (!diag.message.empty()) {
        if (error) {
            *error = "line " + std::to_string(diag.line == 0 ? 1 : diag.line) + ": " + diag.message;
        }
        return std::nullopt;
    }
    std::string out;
    emit_json(root, out);
    return out;
}

} // namespace katana::serde
