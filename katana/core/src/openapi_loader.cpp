#include "katana/core/openapi_loader.hpp"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "katana/core/serde.hpp"

namespace katana::openapi {

namespace {

using serde::json_cursor;
using serde::parse_bool;
using serde::parse_double;
using serde::parse_size;
using serde::parse_unquoted_string;
using serde::trim_view;
using serde::yaml_to_json;

constexpr int kMaxSchemaDepth = 64;
constexpr size_t kMaxSchemaCount = 10000;

std::optional<std::string_view> extract_openapi_version(std::string_view json_view) noexcept {
    json_cursor cur{json_view.data(), json_view.data() + json_view.size()};
    cur.skip_ws();
    if (!cur.try_object_start()) {
        return std::nullopt;
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
        if (*key == "openapi") {
            if (auto v = cur.string()) {
                return trim_view(*v);
            }
            return std::nullopt;
        }
        cur.skip_value();
        cur.try_comma();
    }
    return std::nullopt;
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

struct ref_resolution_context {
    const schema_index& index;
    std::unordered_set<const schema*> visiting;
    std::unordered_set<const schema*> visited;
};

const schema* resolve_schema_ref(schema* s, ref_resolution_context& ctx) {
    if (!s || !s->is_ref || s->ref.empty()) {
        return s;
    }

    if (ctx.visiting.contains(s)) {
        return nullptr;
    }

    if (ctx.visited.contains(s)) {
        return s;
    }

    ctx.visiting.insert(s);

    constexpr std::string_view prefix = "#/components/schemas/";
    std::string_view ref_path{s->ref.data(), s->ref.size()};

    if (ref_path.starts_with(prefix)) {
        auto name_view = ref_path.substr(prefix.size());
        auto it = ctx.index.find(name_view);
        if (it != ctx.index.end()) {
            const schema* resolved = it->second;

            if (resolved && resolved->is_ref) {
                resolved = resolve_schema_ref(const_cast<schema*>(resolved), ctx);
            }

            ctx.visiting.erase(s);
            ctx.visited.insert(s);
            return resolved;
        }
    }

    ctx.visiting.erase(s);
    ctx.visited.insert(s);
    return s;
}

void resolve_all_refs_in_schema(schema* s, ref_resolution_context& ctx) {
    if (!s || ctx.visited.contains(s)) {
        return;
    }

    ctx.visited.insert(s);

    for (auto& prop : s->properties) {
        if (prop.type && prop.type->is_ref && !prop.type->ref.empty()) {
            const schema* resolved = resolve_schema_ref(const_cast<schema*>(prop.type), ctx);
            if (resolved && resolved != prop.type) {
                prop.type = resolved;
            }
        }
        if (prop.type) {
            resolve_all_refs_in_schema(const_cast<schema*>(prop.type), ctx);
        }
    }

    if (s->items && s->items->is_ref && !s->items->ref.empty()) {
        const schema* resolved = resolve_schema_ref(const_cast<schema*>(s->items), ctx);
        if (resolved && resolved != s->items) {
            s->items = resolved;
        }
    }
    if (s->items) {
        resolve_all_refs_in_schema(const_cast<schema*>(s->items), ctx);
    }

    if (s->additional_properties && s->additional_properties->is_ref &&
        !s->additional_properties->ref.empty()) {
        const schema* resolved =
            resolve_schema_ref(const_cast<schema*>(s->additional_properties), ctx);
        if (resolved && resolved != s->additional_properties) {
            s->additional_properties = resolved;
        }
    }
    if (s->additional_properties) {
        resolve_all_refs_in_schema(const_cast<schema*>(s->additional_properties), ctx);
    }

    for (size_t i = 0; i < s->one_of.size(); ++i) {
        if (s->one_of[i] && s->one_of[i]->is_ref && !s->one_of[i]->ref.empty()) {
            const schema* resolved = resolve_schema_ref(const_cast<schema*>(s->one_of[i]), ctx);
            if (resolved && resolved != s->one_of[i]) {
                s->one_of[i] = resolved;
            }
        }
        resolve_all_refs_in_schema(const_cast<schema*>(s->one_of[i]), ctx);
    }

    for (size_t i = 0; i < s->any_of.size(); ++i) {
        if (s->any_of[i] && s->any_of[i]->is_ref && !s->any_of[i]->ref.empty()) {
            const schema* resolved = resolve_schema_ref(const_cast<schema*>(s->any_of[i]), ctx);
            if (resolved && resolved != s->any_of[i]) {
                s->any_of[i] = resolved;
            }
        }
        resolve_all_refs_in_schema(const_cast<schema*>(s->any_of[i]), ctx);
    }

    for (size_t i = 0; i < s->all_of.size(); ++i) {
        if (s->all_of[i] && s->all_of[i]->is_ref && !s->all_of[i]->ref.empty()) {
            const schema* resolved = resolve_schema_ref(const_cast<schema*>(s->all_of[i]), ctx);
            if (resolved && resolved != s->all_of[i]) {
                s->all_of[i] = resolved;
            }
        }
        resolve_all_refs_in_schema(const_cast<schema*>(s->all_of[i]), ctx);
    }
}

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
                        while (!cur.eof() && !cur.try_object_end()) {
                            ++cur.ptr;
                        }
                        return const_cast<schema*>(it->second);
                    }
                }
            } else {
                cur.skip_value();
            }

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
                auto raw = parse_unquoted_string(cur);
                ensure_schema(schema_kind::object)->description =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(pool.arena));
            }
        } else if (*key == "default") {
            auto* s = ensure_schema(schema_kind::object);
            if (auto v = cur.string()) {
                s->default_value =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(pool.arena));
            } else {
                auto raw = parse_unquoted_string(cur);
                s->default_value =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(pool.arena));
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
            auto* s = ensure_schema(schema_kind::object);
            if (auto v = parse_bool(cur)) {
                s->nullable = *v;
            } else {
                cur.skip_value();
            }
        } else if (*key == "deprecated") {
            auto* s = ensure_schema(schema_kind::object);
            if (auto v = parse_bool(cur)) {
                s->deprecated = *v;
            } else {
                cur.skip_value();
            }
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
            if (auto v = parse_bool(cur)) {
                s->unique_items = *v;
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
            } else if (auto v = parse_bool(cur)) {
                obj->additional_properties_allowed = *v;
                if (!*v) {
                    obj->additional_properties = nullptr;
                }
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
            if (auto v = parse_bool(cur)) {
                param.required = *v;
                in_required = true;
            } else {
                cur.skip_value();
            }
        } else if (*key == "schema") {
            param.type = parse_schema(cur, pool, index);
        } else if (*key == "description") {
            if (auto v = cur.string()) {
                param.description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            } else {
                auto raw = parse_unquoted_string(cur);
                param.description =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(&arena));
            }
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
                                              bool is_default,
                                              monotonic_arena& arena,
                                              schema_arena_pool& pool,
                                              const schema_index& index,
                                              const response_index& rindex) {
    response resp(&arena);
    resp.status = status;
    resp.is_default = is_default;

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
            } else {
                auto raw = parse_unquoted_string(cur);
                resp.description =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(&arena));
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
                    const schema* body_schema = nullptr;
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
                                body_schema = parse_schema(cur, pool, index);
                            } else {
                                cur.skip_value();
                            }
                            cur.try_comma();
                        }
                    } else {
                        cur.skip_value();
                    }
                    media_type mt(&arena);
                    mt.content_type =
                        arena_string<>(ctype->begin(), ctype->end(), arena_allocator<char>(&arena));
                    mt.type = body_schema;
                    resp.content.push_back(std::move(mt));
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
        bool is_default = false;
        auto status_sv = *code_key;
        auto fc = std::from_chars(status_sv.data(), status_sv.data() + status_sv.size(), status);
        if (fc.ec != std::errc()) {
            if (status_sv == "default") {
                is_default = true;
                status = 0;
            } else {
                cur.skip_value();
                cur.try_comma();
                continue;
            }
        }

        if (auto resp =
                parse_response_object(cur, status, is_default, arena, pool, index, rindex)) {
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
            } else {
                auto raw = parse_unquoted_string(cur);
                body->description =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(&arena));
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
                    media_type mt(&arena);
                    mt.content_type =
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
                                mt.type = parse_schema(cur, pool, index);
                            } else {
                                cur.skip_value();
                            }
                            cur.try_comma();
                        }
                    } else {
                        cur.skip_value();
                    }
                    body->content.push_back(std::move(mt));
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
                    if (auto resp =
                            parse_response_object(cur, 0, false, arena, pool, sindex, rindex)) {
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
    auto trimmed_input = trim_view(spec_text);
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

    auto openapi_version = extract_openapi_version(json_view);
    if (!openapi_version || !openapi_version->starts_with("3.")) {
        return std::unexpected(make_error_code(error_code::openapi_invalid_spec));
    }

    document doc(arena);
    doc.openapi_version = arena_string<>(
        openapi_version->begin(), openapi_version->end(), arena_allocator<char>(&arena));
    doc.schemas.reserve(256);

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

    if (doc.schemas.size() > kMaxSchemaCount) {
        return std::unexpected(make_error_code(error_code::openapi_invalid_spec));
    }
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

    ref_resolution_context ref_ctx{index, {}, {}};
    for (auto& s : doc.schemas) {
        resolve_all_refs_in_schema(&s, ref_ctx);
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
