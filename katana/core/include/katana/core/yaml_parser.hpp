#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "katana/core/serde.hpp"

namespace katana::serde {

struct yaml_node {
    enum class kind { scalar, object, array };

    kind k{kind::scalar};
    std::string scalar;
    std::vector<std::pair<std::string, std::unique_ptr<yaml_node>>> object;
    std::vector<std::unique_ptr<yaml_node>> array;
    // 1-based source line this node was produced from (0 if unknown). Carried into
    // the emitted JSON via emit_json's checkpoint map so the OpenAPI loader can map a
    // schema's byte offset back to its YAML line.
    size_t source_line = 0;

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

// Maps a byte offset into the emitted JSON string to the 1-based YAML source line
// that produced the construct starting there. Built in emit-order, so it is sorted
// ascending by offset and supports binary search.
using yaml_line_map = std::vector<std::pair<size_t, size_t>>;

inline void emit_json(const yaml_node& n, std::string& out, yaml_line_map* line_map = nullptr);

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
    escape_json_string_into(sv, out);
    out.push_back('\"');
}

inline void emit_json(const yaml_node& n, std::string& out, yaml_line_map* line_map) {
    // Record where this node's JSON begins so the loader can recover its YAML line
    // from a byte offset. The offset is the position of the opening '{' / '[' / value.
    if (line_map != nullptr && n.source_line != 0) {
        line_map->emplace_back(out.size(), n.source_line);
    }
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
            escape_json_string_into(kv.first, out);
            out.push_back('\"');
            out.push_back(':');
            emit_json(*kv.second, out, line_map);
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
            emit_json(*n.array[i], out, line_map);
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
    size_t line_number; // 1-based physical line in the source text (blanks/comments counted)
};

inline std::vector<yaml_line> tokenize_yaml(std::string_view text) {
    std::vector<yaml_line> lines;
    size_t pos = 0;
    size_t phys_line = 0; // physical line counter; blank/comment lines are skipped but still counted
    while (pos < text.size()) {
        ++phys_line;
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
        lines.push_back({indent, content, phys_line});
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
    const size_t line_no =
        current_line ? current_line : (idx < lines.size() ? lines[idx].line_number : 0);
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
            size_t line_no = lines[idx].line_number;
            ++idx;

            if (item.empty()) {
                node.array.push_back(
                    std::make_unique<yaml_node>(parse_yaml_block(lines, idx, indent + 2, diag)));
                continue;
            }

            // Flow-style sequence item, e.g. `- { name: q1, in: query, schema: {...} }`
            // or `- [a, b]`. Without this the `{`/`[` body would be mis-split on its
            // first ':' as a block mapping, silently corrupting the item (e.g. an
            // OpenAPI parameter losing its `schema`, hence its type).
            if (item.front() == '{' || item.front() == '[') {
                node.array.push_back(
                    std::make_unique<yaml_node>(parse_inline_value(item, diag, line_no)));
                continue;
            }

            auto colon_pos = item.find(':');
            if (colon_pos != std::string_view::npos) {
                std::string key = normalize_key(item.substr(0, colon_pos));
                std::string_view val = trim_view(item.substr(colon_pos + 1));
                yaml_node obj = yaml_node::object_node();
                obj.source_line = line_no; // array item (e.g. a parameter) is at its '-' line
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
            size_t line_no = lines[idx].line_number;
            ++idx;

            if (node.k != yaml_node::kind::object) {
                node.k = yaml_node::kind::object;
                node.object.clear();
            }

            yaml_node child = parse_yaml_value(val, lines, idx, indent, diag, line_no);
            child.source_line = line_no; // the value (e.g. a schema) is defined at its key's line
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

inline std::optional<std::string>
yaml_to_json(std::string_view text, std::string* error = nullptr, yaml_line_map* line_map = nullptr) {
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
    emit_json(root, out, line_map);
    return out;
}

} // namespace katana::serde
