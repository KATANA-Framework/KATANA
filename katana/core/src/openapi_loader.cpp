#include "katana/core/openapi_loader.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <string>
#include <string_view>
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
        if (consume('\"')) {
            (void)string();
            return;
        }
        while (!eof() && *ptr != ',' && *ptr != '}' && *ptr != ']') {
            ++ptr;
        }
    }
};

std::optional<std::string_view> extract_string_field(std::string_view text,
                                                     std::string_view key) noexcept {
    json_cursor cur{text.data(), text.data() + text.size()};
    cur.skip_ws();
    if (!cur.try_object_start()) {
        return std::nullopt;
    }

    while (!cur.eof()) {
        auto maybe_key = cur.string();
        if (!maybe_key) {
            ++cur.ptr;
            continue;
        }
        cur.skip_ws();
        if (!cur.consume(':')) {
            return std::nullopt;
        }
        if (*maybe_key == key) {
            return cur.string();
        }

        cur.skip_value();
        cur.try_comma();
    }
    return std::nullopt;
}

struct schema_arena_pool {
    monotonic_arena* arena;
    std::vector<schema*> allocated;

    explicit schema_arena_pool(monotonic_arena* a) : arena(a) {}

    schema* make(schema_kind kind) {
        void* mem = arena->allocate(sizeof(schema), alignof(schema));
        if (!mem) {
            return nullptr;
        }
        auto* s = new (mem) schema(arena);
        s->kind = kind;
        allocated.push_back(s);
        return s;
    }
};

schema* parse_schema(json_cursor& cur, schema_arena_pool& pool);

schema* parse_object_schema(json_cursor& cur, schema_arena_pool& pool) {
    auto* obj = pool.make(schema_kind::object);
    if (!obj) {
        return nullptr;
    }

    if (!cur.try_object_start()) {
        cur.skip_value();
        return obj;
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

        if (*key == "\"properties\"") {
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
                    if (auto* child = parse_schema(cur, pool)) {
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
        } else if (*key == "\"required\"") {
            if (cur.try_array_start()) {
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) {
                        break;
                    }
                    auto req_name = cur.string();
                    if (req_name) {
                        for (auto& p : obj->properties) {
                            if (p.name == *req_name) {
                                p.required = true;
                            }
                        }
                    } else {
                        ++cur.ptr;
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
        } else if (*key == "\"description\"") {
            if (auto v = cur.string()) {
                obj->description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(pool.arena));
            }
        } else if (*key == "\"nullable\"") {
            obj->nullable = true;
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }

    return obj;
}

schema* parse_schema(json_cursor& cur, schema_arena_pool& pool) {
    cur.skip_ws();
    if (cur.try_object_start()) {
        // Peek for $ref or type
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
            if (*key == "\"$ref\"") {
                auto* ref_schema = pool.make(schema_kind::object);
                if (auto v = cur.string()) {
                    ref_schema->ref =
                        arena_string<>(v->begin(), v->end(), arena_allocator<char>(pool.arena));
                    ref_schema->is_ref = true;
                }
                while (!cur.eof() && !cur.try_object_end()) {
                    ++cur.ptr;
                }
                return ref_schema;
            }
            if (*key == "\"type\"") {
                auto type_sv = cur.string();
                if (!type_sv) {
                    return nullptr;
                }
                auto type = *type_sv;
                if (type == "object") {
                    return parse_object_schema(cur, pool);
                } else if (type == "array") {
                    auto* arr = pool.make(schema_kind::array);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_object_end()) {
                            break;
                        }
                        auto k = cur.string();
                        if (!k) {
                            ++cur.ptr;
                            continue;
                        }
                        if (!cur.consume(':')) {
                            break;
                        }
                        if (*k == "\"items\"") {
                            arr->items = parse_schema(cur, pool);
                        } else {
                            cur.skip_value();
                        }
                        cur.try_comma();
                    }
                    return arr;
                } else if (type == "string") {
                    auto* s = pool.make(schema_kind::string);
                    while (!cur.eof()) {
                        cur.skip_ws();
                        if (cur.try_object_end()) {
                            break;
                        }
                        auto k = cur.string();
                        if (!k) {
                            ++cur.ptr;
                            continue;
                        }
                        if (!cur.consume(':')) {
                            break;
                        }
                        if (*k == "\"format\"") {
                            if (auto fmt = cur.string()) {
                                s->format = arena_string<>(
                                    fmt->begin(), fmt->end(), arena_allocator<char>(pool.arena));
                            }
                        } else if (*k == "\"enum\"") {
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
                        } else if (*k == "\"nullable\"") {
                            s->nullable = true;
                        } else if (*k == "\"minLength\"") {
                            auto num_sv = cur.string();
                            if (num_sv) {
                                size_t v = 0;
                                auto fc = std::from_chars(
                                    num_sv->data(), num_sv->data() + num_sv->size(), v);
                                if (fc.ec == std::errc()) {
                                    s->min_length = v;
                                }
                            } else {
                                cur.skip_value();
                            }
                        } else if (*k == "\"maxLength\"") {
                            auto num_sv = cur.string();
                            if (num_sv) {
                                size_t v = 0;
                                auto fc = std::from_chars(
                                    num_sv->data(), num_sv->data() + num_sv->size(), v);
                                if (fc.ec == std::errc()) {
                                    s->max_length = v;
                                }
                            } else {
                                cur.skip_value();
                            }
                        } else {
                            cur.skip_value();
                        }
                        cur.try_comma();
                    }
                    return s;
                } else if (type == "integer" || type == "number") {
                    return pool.make(type == "integer" ? schema_kind::integer
                                                       : schema_kind::number);
                } else if (type == "boolean") {
                    return pool.make(schema_kind::boolean);
                }
            } else {
                cur.skip_value();
            }
            cur.try_comma();
        }
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

void parse_parameter(json_cursor& cur, operation& op, monotonic_arena& arena) {
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
        if (*key == "\"name\"") {
            if (auto v = cur.string()) {
                param.name = arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
                has_name = true;
            }
        } else if (*key == "\"in\"") {
            if (auto v = cur.string()) {
                auto loc = param_location_from_string(*v);
                if (loc) {
                    param.in = *loc;
                    has_in = true;
                }
            }
        } else if (*key == "\"required\"") {
            cur.skip_ws();
            if (!cur.eof() && (*cur.ptr == 't' || *cur.ptr == 'T')) {
                param.required = true;
                // advance over true
                cur.ptr = std::min(cur.ptr + static_cast<ptrdiff_t>(4), cur.end);
            } else {
                param.required = false;
            }
            in_required = true;
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }

    if (has_name && has_in) {
        if (!in_required && param.in == param_location::path) {
            param.required = true;
        }
        op.parameters.push_back(std::move(param));
    }
}

void parse_responses(json_cursor& cur, operation& op, monotonic_arena& arena) {
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
        status_sv = status_sv.substr(1, status_sv.size() - 2);
        auto fc = std::from_chars(status_sv.data(), status_sv.data() + status_sv.size(), status);
        if (fc.ec == std::errc()) {
            response resp(&arena);
            resp.status = status;

            cur.skip_ws();
            if (cur.try_object_start()) {
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
                    if (*rkey == "\"description\"") {
                        if (auto desc = cur.string()) {
                            resp.description = arena_string<>(
                                desc->begin(), desc->end(), arena_allocator<char>(&arena));
                        }
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            } else {
                cur.skip_value();
            }
            op.responses.push_back(std::move(resp));
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
}

void parse_request_body(json_cursor& cur, operation& op, monotonic_arena& arena) {
    if (!cur.try_object_start()) {
        cur.skip_value();
        return;
    }

    auto body = std::make_unique<request_body>(&arena);

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

        if (*key == "\"description\"") {
            if (auto v = cur.string()) {
                body->description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "\"content\"") {
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
                    cur.skip_value(); // skip schema
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

    op.body = body.release();
}

void parse_operation_object(json_cursor& cur, operation& op, monotonic_arena& arena) {
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
        if (*key == "\"operationId\"") {
            if (auto v = cur.string()) {
                op.operation_id =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "\"summary\"") {
            if (auto v = cur.string()) {
                op.summary = arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "\"parameters\"") {
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
                        parse_parameter(cur, op, arena);
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
        } else if (*key == "\"responses\"") {
            parse_responses(cur, op, arena);
        } else if (*key == "\"requestBody\"") {
            parse_request_body(cur, op, arena);
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
}

} // namespace

result<document> load_from_string(std::string_view spec_text, monotonic_arena& arena) {
    auto trimmed = trim(spec_text);
    if (trimmed.empty()) {
        return std::unexpected(make_error_code(error_code::openapi_parse_error));
    }

    // Extremely small guard: ensure this looks like an OpenAPI 3 document.
    if (!contains_version(trimmed, "3.")) {
        return std::unexpected(make_error_code(error_code::openapi_invalid_spec));
    }

    document doc(arena);
    doc.openapi_version = arena_string<>("3.x", arena_allocator<char>(&arena));

    if (auto title = extract_string_field(trimmed, "\"title\"")) {
        doc.info_title =
            arena_string<>(title->begin(), title->end(), arena_allocator<char>(&arena));
    }
    if (auto version = extract_string_field(trimmed, "\"version\"")) {
        doc.info_version =
            arena_string<>(version->begin(), version->end(), arena_allocator<char>(&arena));
    }

    // Minimal paths walker: adds path stubs and single GET/POST operations if present.
    json_cursor cur{trimmed.data(), trimmed.data() + trimmed.size()};
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
        if (*key == "\"paths\"") {
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
                auto& path_item = doc.add_path(path_key->substr(1, path_key->size() - 2));
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
                        if (*method_key == "\"get\"" || *method_key == "\"post\"" ||
                            *method_key == "\"put\"" || *method_key == "\"delete\"" ||
                            *method_key == "\"patch\"" || *method_key == "\"head\"" ||
                            *method_key == "\"options\"") {
                            path_item.operations.emplace_back(&arena);
                            auto& op = path_item.operations.back();
                            op.method = [&]() {
                                if (*method_key == "\"get\"")
                                    return http::method::get;
                                if (*method_key == "\"post\"")
                                    return http::method::post;
                                if (*method_key == "\"put\"")
                                    return http::method::put;
                                if (*method_key == "\"delete\"")
                                    return http::method::del;
                                if (*method_key == "\"patch\"")
                                    return http::method::patch;
                                if (*method_key == "\"head\"")
                                    return http::method::head;
                                if (*method_key == "\"options\"")
                                    return http::method::options;
                                return http::method::unknown;
                            }();
                            parse_operation_object(cur, op, arena);
                            continue;
                        }
                        cur.skip_value();
                        cur.try_comma();
                    }
                }
                cur.try_comma();
            }
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
