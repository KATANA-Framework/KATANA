#include "katana/core/openapi_loader.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace katana::openapi {

namespace {

bool contains_version(std::string_view text, std::string_view version) noexcept {
    auto pos = text.find("\"openapi\"");
    if (pos == std::string_view::npos) {
        return false;
    }
    auto tail = text.substr(pos);
    auto vpos = tail.find(version);
    return vpos != std::string_view::npos;
}

std::string_view trim(std::string_view sv) noexcept {
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

std::optional<size_t> parse_size(json_cursor& cur) noexcept {
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

std::optional<double> parse_double(json_cursor& cur) noexcept {
    cur.skip_ws();
    if (cur.eof()) {
        return std::nullopt;
    }
    if (*cur.ptr == '\"') {
        if (auto sv = cur.string()) {
            double val = 0.0;
            auto [ptr, ec] = std::from_chars(sv->data(), sv->data() + sv->size(), val);
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

std::string_view trim_view(std::string_view sv) noexcept {
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) {
        sv.remove_prefix(1);
    }
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.back()))) {
        sv.remove_suffix(1);
    }
    return sv;
}

bool is_bool_literal(std::string_view sv) noexcept {
    return sv == "true" || sv == "false";
}

bool is_null_literal(std::string_view sv) noexcept {
    return sv == "null";
}

std::string_view parse_unquoted_string(json_cursor& cur) {
    cur.skip_ws();
    const char* start = cur.ptr;
    while (!cur.eof() && *cur.ptr != ',' && *cur.ptr != '}' && *cur.ptr != ']') {
        ++cur.ptr;
    }
    const char* end = cur.ptr;
    auto sv = std::string_view(start, static_cast<size_t>(end - start));
    return trim_view(sv);
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

std::string escape_json_string(std::string_view sv) {
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

void emit_json(const yaml_node& n, std::string& out);

void emit_scalar(const std::string& v, std::string& out) {
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

void emit_json(const yaml_node& n, std::string& out) {
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

std::vector<yaml_line> tokenize_yaml(std::string_view text) {
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

std::string normalize_key(std::string_view key) {
    auto trimmed = trim_view(key);
    if (trimmed.size() >= 2 && ((trimmed.front() == '\"' && trimmed.back() == '\"') ||
                                (trimmed.front() == '\'' && trimmed.back() == '\''))) {
        trimmed = trimmed.substr(1, trimmed.size() - 2);
    }
    return std::string(trimmed);
}

yaml_node parse_yaml_block(const std::vector<yaml_line>& lines, size_t& idx, int indent);

yaml_node parse_yaml_value(std::string_view value,
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

yaml_node parse_yaml_block(const std::vector<yaml_line>& lines, size_t& idx, int indent) {
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

std::optional<std::string> yaml_to_json(std::string_view text) {
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

struct schema_arena_pool {
    document* doc;
    monotonic_arena* arena;
    std::vector<schema*> allocated;

    explicit schema_arena_pool(document* d, monotonic_arena* a) : doc(d), arena(a) {}

    schema* make(schema_kind kind, std::optional<std::string_view> name = std::nullopt) {
        if (doc) {
            doc->schemas.emplace_back(schema{arena});
            schema* s = &doc->schemas.back();
            s->kind = kind;
            if (name) {
                s->name = arena_string<>(name->begin(), name->end(), arena_allocator<char>(arena));
            }
            allocated.push_back(s);
            return s;
        }

        void* mem = arena->allocate(sizeof(schema), alignof(schema));
        if (!mem) {
            return nullptr;
        }
        auto* s = new (mem) schema(arena);
        s->kind = kind;
        if (name) {
            s->name = arena_string<>(name->begin(), name->end(), arena_allocator<char>(arena));
        }
        allocated.push_back(s);
        return s;
    }
};

using schema_index = std::
    unordered_map<std::string_view, const schema*, std::hash<std::string_view>, std::equal_to<>>;
using parameter_index = std::
    unordered_map<std::string_view, const parameter*, std::hash<std::string_view>, std::equal_to<>>;
using response_index = std::
    unordered_map<std::string_view, const response*, std::hash<std::string_view>, std::equal_to<>>;
using request_body_index = std::unordered_map<std::string_view,
                                              const request_body*,
                                              std::hash<std::string_view>,
                                              std::equal_to<>>;

constexpr int kMaxSchemaDepth = 64;

schema*
parse_schema(json_cursor& cur, schema_arena_pool& pool, const schema_index& index, int depth = 0);

schema* parse_schema_object(json_cursor& cur,
                            schema_arena_pool& pool,
                            const schema_index& index,
                            std::optional<std::string_view> name = std::nullopt,
                            int depth = 0) {
    if (depth > kMaxSchemaDepth) {
        cur.skip_value();
        return nullptr;
    }
    if (!cur.try_object_start()) {
        cur.skip_value();
        return nullptr;
    }

    schema* result = nullptr;
    std::vector<arena_string<>> required_names;

    auto ensure_schema = [&](schema_kind kind) -> schema* {
        if (!result) {
            result = pool.make(kind, name);
        } else if (result->kind == schema_kind::object && kind != schema_kind::object) {
            result->kind = kind;
        }
        return result;
    };

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) {
            break;
        }
        auto key = cur.string();
        if (!key) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }

        if (*key == "$ref") {
            auto ref_schema = ensure_schema(schema_kind::object);
            if (auto v = cur.string()) {
                ref_schema->ref =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(pool.arena));
                ref_schema->is_ref = true;

                constexpr std::string_view prefix = "#/components/schemas/";
                if (v->starts_with(prefix)) {
                    auto name_view = v->substr(prefix.size());
                    auto it = index.find(name_view);
                    if (it != index.end()) {
                        // Return resolved schema if available.
                        cur.skip_value();
                        return const_cast<schema*>(it->second);
                    }
                }
            } else {
                cur.skip_value();
            }

            // Skip rest of object
            while (!cur.eof() && !cur.try_object_end()) {
                ++cur.ptr;
            }
            return ref_schema;
        } else if (*key == "type") {
            auto type_sv = cur.string();
            if (!type_sv) {
                cur.skip_value();
            } else {
                auto type = *type_sv;
                schema_kind kind = schema_kind::object;
                if (type == "object") {
                    kind = schema_kind::object;
                } else if (type == "array") {
                    kind = schema_kind::array;
                } else if (type == "string") {
                    kind = schema_kind::string;
                } else if (type == "integer") {
                    kind = schema_kind::integer;
                } else if (type == "number") {
                    kind = schema_kind::number;
                } else if (type == "boolean") {
                    kind = schema_kind::boolean;
                } else {
                    cur.skip_value();
                    kind = schema_kind::object;
                }
                auto* s = ensure_schema(kind);
                if (s) {
                    s->kind = kind;
                }
            }
        } else if (*key == "format") {
            if (auto fmt = cur.string()) {
                ensure_schema(schema_kind::string)->format =
                    arena_string<>(fmt->begin(), fmt->end(), arena_allocator<char>(pool.arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "description") {
            if (auto v = cur.string()) {
                ensure_schema(schema_kind::object)->description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(pool.arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "pattern") {
            auto* s = ensure_schema(schema_kind::string);
            if (auto v = cur.string()) {
                s->pattern =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(pool.arena));
            } else {
                auto raw = parse_unquoted_string(cur);
                s->pattern =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(pool.arena));
            }
        } else if (*key == "nullable") {
            ensure_schema(schema_kind::object)->nullable = true;
            cur.skip_value();
        } else if (*key == "deprecated") {
            ensure_schema(schema_kind::object)->deprecated = true;
            cur.skip_value();
        } else if (*key == "enum") {
            auto* s = ensure_schema(schema_kind::string);
            if (cur.try_array_start()) {
                bool first = true;
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) {
                        break;
                    }
                    if (auto ev = cur.string()) {
                        if (!first) {
                            s->enum_values.push_back(';');
                        }
                        s->enum_values.append(ev->data(), ev->size());
                        first = false;
                    } else {
                        ++cur.ptr;
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else if (*key == "minLength") {
            auto* s = ensure_schema(schema_kind::string);
            if (auto v = parse_size(cur)) {
                s->min_length = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "maxLength") {
            auto* s = ensure_schema(schema_kind::string);
            if (auto v = parse_size(cur)) {
                s->max_length = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "minimum") {
            auto* s = ensure_schema(schema_kind::number);
            if (auto v = parse_double(cur)) {
                s->minimum = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "exclusiveMinimum") {
            auto* s = ensure_schema(schema_kind::number);
            if (auto v = parse_double(cur)) {
                s->exclusive_minimum = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "maximum") {
            auto* s = ensure_schema(schema_kind::number);
            if (auto v = parse_double(cur)) {
                s->maximum = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "exclusiveMaximum") {
            auto* s = ensure_schema(schema_kind::number);
            if (auto v = parse_double(cur)) {
                s->exclusive_maximum = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "multipleOf") {
            auto* s = ensure_schema(schema_kind::number);
            if (auto v = parse_double(cur)) {
                s->multiple_of = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "minItems") {
            auto* s = ensure_schema(schema_kind::array);
            if (auto v = parse_size(cur)) {
                s->min_items = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "maxItems") {
            auto* s = ensure_schema(schema_kind::array);
            if (auto v = parse_size(cur)) {
                s->max_items = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "uniqueItems") {
            auto* s = ensure_schema(schema_kind::array);
            cur.skip_ws();
            if (!cur.eof() && (*cur.ptr == 't' || *cur.ptr == 'T')) {
                s->unique_items = true;
                cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(4), cur.end);
            } else {
                cur.skip_value();
            }
        } else if (*key == "items") {
            auto* s = ensure_schema(schema_kind::array);
            s->items = parse_schema(cur, pool, index, depth + 1);
        } else if (*key == "properties") {
            auto* obj = ensure_schema(schema_kind::object);
            if (!cur.try_object_start()) {
                cur.skip_value();
            } else {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_object_end()) {
                        break;
                    }
                    auto prop_name = cur.string();
                    if (!prop_name) {
                        ++cur.ptr;
                        continue;
                    }
                    if (!cur.consume(':')) {
                        break;
                    }
                    if (auto* child = parse_schema(cur, pool, index, depth + 1)) {
                        property p{arena_string<>(prop_name->begin(),
                                                  prop_name->end(),
                                                  arena_allocator<char>(pool.arena)),
                                   child,
                                   false};
                        obj->properties.push_back(std::move(p));
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
        } else if (*key == "required") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) {
                        break;
                    }
                    if (auto req_name = cur.string()) {
                        required_names.emplace_back(
                            req_name->begin(), req_name->end(), arena_allocator<char>(pool.arena));
                    } else {
                        ++cur.ptr;
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else if (*key == "oneOf" || *key == "anyOf" || *key == "allOf") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) {
                        break;
                    }
                    if (auto* sub = parse_schema(cur, pool, index, depth + 1)) {
                        auto* obj = ensure_schema(schema_kind::object);
                        if (*key == "oneOf") {
                            obj->one_of.push_back(sub);
                        } else if (*key == "anyOf") {
                            obj->any_of.push_back(sub);
                        } else {
                            obj->all_of.push_back(sub);
                        }
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else if (*key == "additionalProperties") {
            auto* obj = ensure_schema(schema_kind::object);
            cur.skip_ws();
            if (cur.try_object_start()) {
                obj->additional_properties =
                    parse_schema_object(cur, pool, index, std::nullopt, depth + 1);
            } else if (!cur.eof() && (*cur.ptr == 'f' || *cur.ptr == 'F')) {
                obj->additional_properties_allowed = false;
                cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(5), cur.end);
            } else if (!cur.eof() && (*cur.ptr == 't' || *cur.ptr == 'T')) {
                obj->additional_properties_allowed = true;
                cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(4), cur.end);
            } else {
                cur.skip_value();
            }
        } else if (*key == "discriminator") {
            auto* obj = ensure_schema(schema_kind::object);
            if (auto v = cur.string()) {
                obj->discriminator =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(pool.arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "additionalProperties") {
            auto* obj = ensure_schema(schema_kind::object);
            cur.skip_ws();
            if (cur.try_object_start()) {
                obj->additional_properties =
                    parse_schema_object(cur, pool, index, std::nullopt, depth + 1);
            } else if (!cur.eof() && (*cur.ptr == 'f' || *cur.ptr == 'F')) {
                obj->additional_properties_allowed = false;
                cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(5), cur.end);
            } else if (!cur.eof() && (*cur.ptr == 't' || *cur.ptr == 'T')) {
                obj->additional_properties_allowed = true;
                cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(4), cur.end);
            } else {
                cur.skip_value();
            }
        } else if (*key == "discriminator") {
            auto* obj = ensure_schema(schema_kind::object);
            if (auto v = cur.string()) {
                obj->discriminator =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(pool.arena));
            } else {
                auto raw = parse_unquoted_string(cur);
                obj->discriminator =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(pool.arena));
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }

    if (!result) {
        result = pool.make(schema_kind::object, name);
    }

    if (!required_names.empty()) {
        for (auto& prop : result->properties) {
            for (const auto& req : required_names) {
                if (prop.name == req) {
                    prop.required = true;
                    break;
                }
            }
        }
    }

    return result;
}

schema*
parse_schema(json_cursor& cur, schema_arena_pool& pool, const schema_index& index, int depth) {
    cur.skip_ws();
    if (cur.eof() || depth > kMaxSchemaDepth) {
        return nullptr;
    }
    if (*cur.ptr == '{') {
        return parse_schema_object(cur, pool, index, std::nullopt, depth);
    }
    cur.skip_value();
    return nullptr;
}

std::optional<param_location> param_location_from_string(std::string_view sv) noexcept {
    if (sv == "path")
        return param_location::path;
    if (sv == "query")
        return param_location::query;
    if (sv == "header")
        return param_location::header;
    if (sv == "cookie")
        return param_location::cookie;
    return std::nullopt;
}

std::optional<parameter> parse_parameter_object(json_cursor& cur,
                                                monotonic_arena& arena,
                                                schema_arena_pool& pool,
                                                const schema_index& index,
                                                const parameter_index& pindex) {
    // precondition: object start already consumed
    bool in_required = false;
    parameter param(&arena);
    bool has_name = false;
    bool has_in = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) {
            break;
        }
        auto key = cur.string();
        if (!key) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }
        if (*key == "name") {
            if (auto v = cur.string()) {
                param.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
                has_name = true;
            }
        } else if (*key == "in") {
            if (auto v = cur.string()) {
                auto loc = param_location_from_string(*v);
                if (loc) {
                    param.in = *loc;
                    has_in = true;
                }
            }
        } else if (*key == "required") {
            cur.skip_ws();
            if (!cur.eof() && (*cur.ptr == 't' || *cur.ptr == 'T')) {
                param.required = true;
                // advance over true
                cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(4), cur.end);
            } else {
                param.required = false;
            }
            in_required = true;
        } else if (*key == "schema") {
            param.type = parse_schema(cur, pool, index);
        } else if (*key == "$ref") {
            if (auto v = cur.string()) {
                constexpr std::string_view prefix = "#/components/parameters/";
                if (v->starts_with(prefix)) {
                    auto name_view = v->substr(prefix.size());
                    auto it = pindex.find(name_view);
                    if (it != pindex.end()) {
                        return *it->second;
                    }
                }
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }

    if (!has_name || !has_in) {
        return std::nullopt;
    }
    if (!in_required && param.in == param_location::path) {
        param.required = true;
    }
    return param;
}

std::optional<response> parse_response_object(json_cursor& cur,
                                              int status,
                                              monotonic_arena& arena,
                                              schema_arena_pool& pool,
                                              const schema_index& index,
                                              const response_index& rindex) {
    response resp(&arena);
    resp.status = status;

    cur.skip_ws();
    if (!cur.try_object_start()) {
        cur.skip_value();
        return std::nullopt;
    }

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) {
            break;
        }
        auto rkey = cur.string();
        if (!rkey) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }
        if (*rkey == "$ref") {
            if (auto ref = cur.string()) {
                constexpr std::string_view prefix = "#/components/responses/";
                if (ref->starts_with(prefix)) {
                    auto name_view = ref->substr(prefix.size());
                    auto it = rindex.find(name_view);
                    if (it != rindex.end()) {
                        resp = *it->second;
                        resp.status = status;
                        // consume remaining object
                        while (!cur.eof() && !cur.try_object_end()) {
                            ++cur.ptr;
                        }
                        break;
                    }
                }
            } else {
                cur.skip_value();
            }
        } else if (*rkey == "description") {
            if (auto desc = cur.string()) {
                resp.description =
                    arena_string<>(desc->begin(), desc->end(), arena_allocator<char>(&arena));
            }
        } else if (*rkey == "content") {
            cur.skip_ws();
            if (cur.try_object_start()) {
                int depth = 1;
                while (!cur.eof() && depth > 0) {
                    cur.skip_ws();
                    if (cur.try_object_end()) {
                        --depth;
                        continue;
                    }
                    auto ctype = cur.string();
                    if (!ctype) {
                        ++cur.ptr;
                        continue;
                    }
                    if (!cur.consume(':')) {
                        break;
                    }
                    resp.body = nullptr;
                    auto media_depth = 1;
                    if (cur.try_object_start()) {
                        while (!cur.eof() && media_depth > 0) {
                            cur.skip_ws();
                            if (cur.try_object_end()) {
                                --media_depth;
                                continue;
                            }
                            auto mkey = cur.string();
                            if (!mkey) {
                                ++cur.ptr;
                                continue;
                            }
                            if (!cur.consume(':')) {
                                break;
                            }
                            if (*mkey == "schema") {
                                resp.body = parse_schema(cur, pool, index);
                            } else {
                                cur.skip_value();
                            }
                            cur.try_comma();
                        }
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }

    return resp;
}

void parse_responses(json_cursor& cur,
                     operation& op,
                     monotonic_arena& arena,
                     schema_arena_pool& pool,
                     const schema_index& index,
                     const response_index& rindex) {
    if (!cur.try_object_start()) {
        cur.skip_value();
        return;
    }

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) {
            break;
        }
        auto code_key = cur.string();
        if (!code_key) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }

        int status = 0;
        auto status_sv = *code_key;
        auto fc = std::from_chars(status_sv.data(), status_sv.data() + status_sv.size(), status);
        if (fc.ec != std::errc()) {
            cur.skip_value();
            cur.try_comma();
            continue;
        }

        if (auto resp = parse_response_object(cur, status, arena, pool, index, rindex)) {
            op.responses.push_back(std::move(*resp));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
}

request_body* parse_request_body(json_cursor& cur,
                                 monotonic_arena& arena,
                                 schema_arena_pool& pool,
                                 const schema_index& index,
                                 const request_body_index& rbindex) {
    cur.skip_ws();
    if (cur.eof()) {
        return nullptr;
    }

    // Reference case
    if (*cur.ptr == '\"') {
        auto ref = cur.string();
        if (ref) {
            constexpr std::string_view prefix = "#/components/requestBodies/";
            if (ref->starts_with(prefix)) {
                auto name_view = ref->substr(prefix.size());
                auto it = rbindex.find(name_view);
                if (it != rbindex.end()) {
                    return const_cast<request_body*>(it->second);
                }
            }
        }
        return nullptr;
    }

    if (!cur.try_object_start()) {
        cur.skip_value();
        return nullptr;
    }

    request_body* body = nullptr;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) {
            break;
        }
        auto key = cur.string();
        if (!key) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }

        if (*key == "description") {
            if (!body) {
                void* mem = arena.allocate(sizeof(request_body), alignof(request_body));
                if (!mem) {
                    cur.skip_value();
                    cur.try_comma();
                    continue;
                }
                body = new (mem) request_body(&arena);
            }
            if (auto v = cur.string()) {
                body->description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "content") {
            if (!body) {
                void* mem = arena.allocate(sizeof(request_body), alignof(request_body));
                if (!mem) {
                    cur.skip_value();
                    cur.try_comma();
                    continue;
                }
                body = new (mem) request_body(&arena);
            }
            cur.skip_ws();
            if (cur.try_object_start()) {
                int depth = 1;
                while (!cur.eof() && depth > 0) {
                    cur.skip_ws();
                    if (cur.try_object_end()) {
                        --depth;
                        continue;
                    }
                    auto ctype = cur.string();
                    if (!ctype) {
                        ++cur.ptr;
                        continue;
                    }
                    if (!cur.consume(':')) {
                        break;
                    }
                    body->content_type =
                        arena_string<>(ctype->begin(), ctype->end(), arena_allocator<char>(&arena));
                    if (cur.try_object_start()) {
                        int media_depth = 1;
                        while (!cur.eof() && media_depth > 0) {
                            cur.skip_ws();
                            if (cur.try_object_end()) {
                                --media_depth;
                                continue;
                            }
                            auto mkey = cur.string();
                            if (!mkey) {
                                ++cur.ptr;
                                continue;
                            }
                            if (!cur.consume(':')) {
                                break;
                            }
                            if (*mkey == "schema") {
                                body->body = parse_schema(cur, pool, index);
                            } else {
                                cur.skip_value();
                            }
                            cur.try_comma();
                        }
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }

    return body;
}

void parse_operation_object(json_cursor& cur,
                            operation& op,
                            monotonic_arena& arena,
                            schema_arena_pool& pool,
                            const schema_index& index,
                            const parameter_index& pindex,
                            const response_index& rindex,
                            const request_body_index& rbindex) {
    if (!cur.try_object_start()) {
        cur.skip_value();
        return;
    }
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) {
            break;
        }
        auto key = cur.string();
        if (!key) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }
        if (*key == "operationId") {
            if (auto v = cur.string()) {
                op.operation_id =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "summary") {
            if (auto v = cur.string()) {
                op.summary = arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "parameters") {
            cur.skip_ws();
            if (!cur.try_array_start()) {
                cur.skip_value();
            } else {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) {
                        break;
                    }
                    if (cur.try_object_start()) {
                        if (auto param = parse_parameter_object(cur, arena, pool, index, pindex)) {
                            op.parameters.push_back(std::move(*param));
                        }
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
        } else if (*key == "responses") {
            parse_responses(cur, op, arena, pool, index, rindex);
        } else if (*key == "requestBody") {
            op.body = parse_request_body(cur, arena, pool, index, rbindex);
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
}

void parse_info_object(json_cursor& cur, document& doc, monotonic_arena& arena) {
    if (!cur.try_object_start()) {
        cur.skip_value();
        return;
    }

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) {
            break;
        }
        auto key = cur.string();
        if (!key) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }

        if (*key == "title") {
            if (auto v = cur.string()) {
                doc.info_title =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "version") {
            if (auto v = cur.string()) {
                doc.info_version =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
}

void parse_components(json_cursor& cur,
                      monotonic_arena& arena,
                      schema_arena_pool& pool,
                      schema_index& sindex,
                      parameter_index& pindex,
                      response_index& rindex,
                      request_body_index& rbindex) {
    cur.skip_ws();
    if (!cur.try_object_start()) {
        return;
    }

    while (!cur.eof()) {
        auto key = cur.string();
        if (!key) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }
        if (*key == "schemas") {
            cur.skip_ws();
            if (!cur.try_object_start()) {
                cur.skip_value();
            } else {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_object_end()) {
                        break;
                    }
                    auto schema_name = cur.string();
                    if (!schema_name) {
                        ++cur.ptr;
                        continue;
                    }
                    if (!cur.consume(':')) {
                        break;
                    }
                    if (auto* s = parse_schema_object(cur, pool, sindex, *schema_name, 1)) {
                        sindex.emplace(s->name, s);
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
        } else if (*key == "parameters") {
            cur.skip_ws();
            if (!cur.try_object_start()) {
                cur.skip_value();
            } else {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_object_end()) {
                        break;
                    }
                    auto pname = cur.string();
                    if (!pname) {
                        ++cur.ptr;
                        continue;
                    }
                    if (!cur.consume(':')) {
                        break;
                    }
                    if (cur.try_object_start()) {
                        if (auto param = parse_parameter_object(cur, arena, pool, sindex, pindex)) {
                            void* mem = arena.allocate(sizeof(parameter), alignof(parameter));
                            if (mem) {
                                auto* stored = new (mem) parameter(*param);
                                pindex.emplace(stored->name, stored);
                            }
                        } else {
                            cur.skip_value();
                        }
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
        } else if (*key == "responses") {
            cur.skip_ws();
            if (!cur.try_object_start()) {
                cur.skip_value();
            } else {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_object_end()) {
                        break;
                    }
                    auto rname = cur.string();
                    if (!rname) {
                        ++cur.ptr;
                        continue;
                    }
                    if (!cur.consume(':')) {
                        break;
                    }
                    if (auto resp = parse_response_object(cur, 0, arena, pool, sindex, rindex)) {
                        void* mem = arena.allocate(sizeof(response), alignof(response));
                        if (mem) {
                            auto* stored = new (mem) response(*resp);
                            rindex.emplace(*rname, stored);
                        }
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
        } else if (*key == "requestBodies") {
            cur.skip_ws();
            if (!cur.try_object_start()) {
                cur.skip_value();
            } else {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_object_end()) {
                        break;
                    }
                    auto rbname = cur.string();
                    if (!rbname) {
                        ++cur.ptr;
                        continue;
                    }
                    if (!cur.consume(':')) {
                        break;
                    }
                    if (auto* body = parse_request_body(cur, arena, pool, sindex, rbindex)) {
                        rbindex.emplace(*rbname, body);
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
}

} // namespace

result<document> load_from_string(std::string_view spec_text, monotonic_arena& arena) {
    auto trimmed_input = trim(spec_text);
    if (trimmed_input.empty()) {
        return std::unexpected(make_error_code(error_code::openapi_parse_error));
    }

    std::string storage;
    std::string_view json_view = trimmed_input;
    bool is_json =
        !trimmed_input.empty() && (trimmed_input.front() == '{' || trimmed_input.front() == '[');
    if (!is_json) {
        auto maybe_json = yaml_to_json(trimmed_input);
        if (!maybe_json) {
            return std::unexpected(make_error_code(error_code::openapi_parse_error));
        }
        storage = std::move(*maybe_json);
        json_view = trim_view(storage);
    }

    // Extremely small guard: ensure this looks like an OpenAPI 3 document.
    if (!contains_version(json_view, "3.")) {
        return std::unexpected(make_error_code(error_code::openapi_invalid_spec));
    }

    document doc(arena);
    doc.openapi_version = arena_string<>("3.x", arena_allocator<char>(&arena));
    schema_arena_pool pool(&doc, &arena);
    schema_index index;
    parameter_index pindex;
    response_index rindex;
    request_body_index rbindex;

    // Pass 1: components (schemas) to allow basic $ref resolution.
    {
        json_cursor components_cur{json_view.data(), json_view.data() + json_view.size()};
        components_cur.skip_ws();
        components_cur.try_object_start();
        while (!components_cur.eof()) {
            auto key = components_cur.string();
            if (!key) {
                ++components_cur.ptr;
                continue;
            }
            if (!components_cur.consume(':')) {
                break;
            }
            if (*key == "components") {
                parse_components(components_cur, arena, pool, index, pindex, rindex, rbindex);
            } else {
                components_cur.skip_value();
            }
            components_cur.try_comma();
        }
    }

    // Minimal paths walker: adds path stubs and single GET/POST operations if present.
    json_cursor cur{json_view.data(), json_view.data() + json_view.size()};
    cur.skip_ws();
    cur.try_object_start();

    while (!cur.eof()) {
        auto key = cur.string();
        if (!key) {
            ++cur.ptr;
            continue;
        }
        if (!cur.consume(':')) {
            break;
        }
        if (*key == "paths") {
            cur.skip_ws();
            if (!cur.try_object_start()) {
                break;
            }
            int depth = 1;
            while (!cur.eof() && depth > 0) {
                cur.skip_ws();
                if (cur.try_object_end()) {
                    --depth;
                    continue;
                }
                auto path_key = cur.string();
                if (!path_key) {
                    ++cur.ptr;
                    continue;
                }
                if (!cur.consume(':')) {
                    break;
                }
                auto& path_item = doc.add_path(*path_key);
                arena_vector<parameter> path_params{arena_allocator<parameter>(&arena)};
                cur.skip_ws();
                if (cur.try_object_start()) {
                    int op_depth = 1;
                    while (!cur.eof() && op_depth > 0) {
                        cur.skip_ws();
                        if (cur.try_object_end()) {
                            --op_depth;
                            continue;
                        }
                        auto method_key = cur.string();
                        if (!method_key) {
                            ++cur.ptr;
                            continue;
                        }
                        if (!cur.consume(':')) {
                            break;
                        }
                        if (*method_key == "parameters") {
                            cur.skip_ws();
                            if (!cur.try_array_start()) {
                                cur.skip_value();
                            } else {
                                while (!cur.eof()) {
                                    cur.skip_ws();
                                    if (cur.try_array_end()) {
                                        break;
                                    }
                                    if (cur.try_object_start()) {
                                        if (auto param = parse_parameter_object(
                                                cur, arena, pool, index, pindex)) {
                                            path_params.push_back(std::move(*param));
                                        }
                                    } else {
                                        cur.skip_value();
                                    }
                                    cur.try_comma();
                                }
                            }
                            cur.try_comma();
                            continue;
                        }
                        if (*method_key == "get" || *method_key == "post" || *method_key == "put" ||
                            *method_key == "delete" || *method_key == "patch" ||
                            *method_key == "head" || *method_key == "options") {
                            path_item.operations.emplace_back(&arena);
                            auto& op = path_item.operations.back();
                            op.parameters.insert(
                                op.parameters.end(), path_params.begin(), path_params.end());
                            op.method = [&]() {
                                if (*method_key == "get")
                                    return http::method::get;
                                if (*method_key == "post")
                                    return http::method::post;
                                if (*method_key == "put")
                                    return http::method::put;
                                if (*method_key == "delete")
                                    return http::method::del;
                                if (*method_key == "patch")
                                    return http::method::patch;
                                if (*method_key == "head")
                                    return http::method::head;
                                if (*method_key == "options")
                                    return http::method::options;
                                return http::method::unknown;
                            }();
                            parse_operation_object(
                                cur, op, arena, pool, index, pindex, rindex, rbindex);
                            continue;
                        }
                        cur.skip_value();
                        cur.try_comma();
                    }
                }
                cur.try_comma();
            }
        } else if (*key == "info") {
            parse_info_object(cur, doc, arena);
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }

    return doc;
}

result<document> load_from_file(const char* path, monotonic_arena& arena) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return std::unexpected(make_error_code(error_code::openapi_parse_error));
    }
    std::string content;
    in.seekg(0, std::ios::end);
    content.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0, std::ios::beg);
    in.read(content.data(), static_cast<std::streamsize>(content.size()));
    return load_from_string(content, arena);
}

} // namespace katana::openapi
