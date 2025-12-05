#include "katana/core/openapi_loader.hpp"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
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

// Merge allOf schemas into parent schema (most restrictive constraints win)
void merge_all_of_schemas(schema* s, monotonic_arena* arena) {
    if (!s || s->all_of.empty()) {
        return;
    }

    // Recursively merge nested allOf first
    for (const auto* child : s->all_of) {
        merge_all_of_schemas(const_cast<schema*>(child), arena);
    }

    // Merge properties from all schemas
    for (const auto* child : s->all_of) {
        if (!child) {
            continue;
        }

        // Merge properties - child props take precedence, but keep required flags if already set
        for (const auto& child_prop : child->properties) {
            bool found = false;
            for (auto& existing : s->properties) {
                if (existing.name == child_prop.name) {
                    existing.type = child_prop.type;
                    existing.required = existing.required || child_prop.required;
                    found = true;
                    break;
                }
            }
            if (!found) {
                s->properties.push_back(child_prop);
            }
        }

        // Merge format
        if (!child->format.empty() && s->format.empty()) {
            s->format = child->format;
        }

        // Merge description if parent doesn't have one
        if (!child->description.empty() && s->description.empty()) {
            s->description = child->description;
        }

        // Type: if child has type and parent doesn't, use child's
        if (child->kind != schema_kind::object && s->kind == schema_kind::object) {
            s->kind = child->kind;
        }

        // String constraints: most restrictive wins
        if (child->min_length > s->min_length) {
            s->min_length = child->min_length;
        }
        if (child->max_length > 0 && (s->max_length == 0 || child->max_length < s->max_length)) {
            s->max_length = child->max_length;
        }
        if (!child->pattern.empty()) {
            s->pattern = child->pattern;
        }

        // Numeric constraints: most restrictive wins
        if (child->minimum.has_value()) {
            if (!s->minimum.has_value() || *child->minimum > *s->minimum) {
                s->minimum = child->minimum;
            }
        }
        if (child->maximum.has_value()) {
            if (!s->maximum.has_value() || *child->maximum < *s->maximum) {
                s->maximum = child->maximum;
            }
        }
        if (child->exclusive_minimum.has_value()) {
            if (!s->exclusive_minimum.has_value() ||
                *child->exclusive_minimum > *s->exclusive_minimum) {
                s->exclusive_minimum = child->exclusive_minimum;
            }
        }
        if (child->exclusive_maximum.has_value()) {
            if (!s->exclusive_maximum.has_value() ||
                *child->exclusive_maximum < *s->exclusive_maximum) {
                s->exclusive_maximum = child->exclusive_maximum;
            }
        }
        if (child->multiple_of.has_value()) {
            s->multiple_of = child->multiple_of;
        }

        // Array constraints: most restrictive wins
        if (child->min_items > s->min_items) {
            s->min_items = child->min_items;
        }
        if (child->max_items > 0 && (s->max_items == 0 || child->max_items < s->max_items)) {
            s->max_items = child->max_items;
        }
        if (child->unique_items) {
            s->unique_items = true;
        }
        if (child->items && !s->items) {
            s->items = child->items;
        }

        // Additional properties
        if (child->additional_properties && !s->additional_properties) {
            s->additional_properties = child->additional_properties;
        }
        if (!child->additional_properties_allowed) {
            s->additional_properties_allowed = false;
        }

        // Boolean flags
        if (child->nullable) {
            s->nullable = true;
        }
        if (child->deprecated) {
            s->deprecated = true;
        }

        // Enum values: merge if compatible
        if (!child->enum_values.empty()) {
            if (s->enum_values.empty()) {
                s->enum_values = child->enum_values;
            }
        }
    }

    s->all_of.clear();
}

schema* parse_schema(json_cursor& cur,
                     schema_arena_pool& pool,
                     const schema_index& index,
                     int depth = 0,
                     std::optional<std::string_view> parent_ctx = std::nullopt,
                     std::optional<std::string_view> field_ctx = std::nullopt);

schema* parse_schema_object(json_cursor& cur,
                            schema_arena_pool& pool,
                            const schema_index& index,
                            std::optional<std::string_view> name = std::nullopt,
                            int depth = 0,
                            std::optional<std::string_view> parent_ctx = std::nullopt,
                            std::optional<std::string_view> field_ctx = std::nullopt) {
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
            // Set context for intelligent naming immediately upon creation
            if (parent_ctx && !parent_ctx->empty()) {
                result->parent_context = arena_string<>(
                    parent_ctx->begin(), parent_ctx->end(), arena_allocator<char>(pool.arena));
            }
            if (field_ctx && !field_ctx->empty()) {
                result->field_context = arena_string<>(
                    field_ctx->begin(), field_ctx->end(), arena_allocator<char>(pool.arena));
            }
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
                while (!cur.eof()) {
                    cur.skip_ws();
                    if (cur.try_array_end()) {
                        break;
                    }
                    if (auto ev = cur.string()) {
                        s->enum_values.emplace_back(
                            ev->begin(), ev->end(), arena_allocator<char>(pool.arena));
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
            // Pass parent context for array items (e.g., Tasks → Tasks_Item_t)
            std::optional<std::string_view> items_parent = name ? name : parent_ctx;
            s->items = parse_schema(cur, pool, index, depth + 1, items_parent, "item");
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
                    // Pass parent schema name and property name for context-aware naming
                    std::optional<std::string_view> prop_parent = name ? name : parent_ctx;
                    if (auto* child =
                            parse_schema(cur, pool, index, depth + 1, prop_parent, *prop_name)) {
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
                    // Pass context for polymorphic types
                    std::optional<std::string_view> poly_parent = name ? name : parent_ctx;
                    if (auto* sub =
                            parse_schema(cur, pool, index, depth + 1, poly_parent, std::nullopt)) {
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
                std::optional<std::string_view> addl_parent = name ? name : parent_ctx;
                obj->additional_properties = parse_schema_object(
                    cur, pool, index, std::nullopt, depth + 1, addl_parent, "additionalProperty");
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
        // Set context for intelligent naming
        if (parent_ctx && !parent_ctx->empty()) {
            result->parent_context = arena_string<>(
                parent_ctx->begin(), parent_ctx->end(), arena_allocator<char>(pool.arena));
        }
        if (field_ctx && !field_ctx->empty()) {
            result->field_context = arena_string<>(
                field_ctx->begin(), field_ctx->end(), arena_allocator<char>(pool.arena));
        }
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

schema* parse_schema(json_cursor& cur,
                     schema_arena_pool& pool,
                     const schema_index& index,
                     int depth,
                     std::optional<std::string_view> parent_ctx,
                     std::optional<std::string_view> field_ctx) {
    cur.skip_ws();
    if (cur.eof() || depth > kMaxSchemaDepth) {
        return nullptr;
    }
    if (*cur.ptr == '{') {
        return parse_schema_object(cur, pool, index, std::nullopt, depth, parent_ctx, field_ctx);
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
            // Use parameter name as context if available
            std::optional<std::string_view> param_ctx =
                param.name.empty() ? std::nullopt
                                   : std::optional<std::string_view>(
                                         std::string_view(param.name.data(), param.name.size()));
            param.type = parse_schema(cur, pool, index, 0, param_ctx, std::nullopt);
        } else if (*key == "description") {
            if (auto v = cur.string()) {
                param.description =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            } else {
                auto raw = parse_unquoted_string(cur);
                param.description =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "style") {
            if (auto v = cur.string()) {
                param.style = arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "explode") {
            if (auto v = parse_bool(cur)) {
                param.explode = *v;
            } else {
                cur.skip_value();
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
                                body_schema =
                                    parse_schema(cur, pool, index, 0, std::nullopt, std::nullopt);
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
                                mt.type =
                                    parse_schema(cur, pool, index, 0, std::nullopt, std::nullopt);
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
        } else if (*key == "x-katana-cache") {
            if (auto v = cur.string()) {
                op.x_katana_cache =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            } else if (auto b = parse_bool(cur)) {
                op.x_katana_cache =
                    arena_string<>(*b ? "true" : "false", arena_allocator<char>(&arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "x-katana-alloc") {
            if (auto v = cur.string()) {
                op.x_katana_alloc =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(&arena));
            } else {
                // Could be a number
                auto raw = parse_unquoted_string(cur);
                op.x_katana_alloc =
                    arena_string<>(raw.begin(), raw.end(), arena_allocator<char>(&arena));
            }
        } else if (*key == "x-katana-rate-limit") {
            if (auto v = cur.string()) {
                op.x_katana_rate_limit =
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
        std::string yaml_error;
        auto maybe_json = yaml_to_json(trimmed_input, &yaml_error);
        if (!maybe_json) {
            if (!yaml_error.empty()) {
                std::cerr << "[openapi][yaml] " << yaml_error << "\n";
            }
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
    // Reserve full allowed schema budget to keep pointers stable in indexes.
    doc.schemas.reserve(kMaxSchemaCount);

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

    // Pass 2: Resolve all $refs
    ref_resolution_context ref_ctx{index, {}, {}};
    for (auto& s : doc.schemas) {
        resolve_all_refs_in_schema(&s, ref_ctx);
    }

    // Pass 3: Merge allOf schemas after refs are resolved
    for (auto& s : doc.schemas) {
        merge_all_of_schemas(&s, &arena);
    }

    // Pass 4: Ensure path params exist in operations even if missing in OpenAPI (fallback)
    for (auto& path : doc.paths) {
        std::vector<std::string_view> path_param_names;
        std::string_view path_str{path.path.data(), path.path.size()};
        size_t pos = 0;
        while (pos < path_str.size()) {
            auto open = path_str.find('{', pos);
            if (open == std::string_view::npos) {
                break;
            }
            auto close = path_str.find('}', open);
            if (close == std::string_view::npos || close <= open + 1) {
                break;
            }
            path_param_names.push_back(path_str.substr(open + 1, close - open - 1));
            pos = close + 1;
        }

        for (auto& op : path.operations) {
            for (auto name : path_param_names) {
                bool exists = false;
                for (const auto& p : op.parameters) {
                    if (p.in == param_location::path && p.name == name) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    auto& inline_schema = doc.add_inline_schema();
                    inline_schema.kind = schema_kind::string;

                    op.parameters.emplace_back(doc.arena_);
                    auto& p = op.parameters.back();
                    p.name =
                        arena_string<>(name.begin(), name.end(), arena_allocator<char>(doc.arena_));
                    p.in = param_location::path;
                    p.required = true;
                    p.type = &inline_schema;
                }
            }
        }
    }

    // Pass 5: Validate specification
    std::unordered_set<std::string_view> operation_ids;
    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            // Check operationId uniqueness
            if (!op.operation_id.empty()) {
                std::string_view op_id_view{op.operation_id.data(), op.operation_id.size()};
                if (operation_ids.contains(op_id_view)) {
                    return std::unexpected(make_error_code(error_code::openapi_invalid_spec));
                }
                operation_ids.insert(op_id_view);
            }

            // Validate HTTP response codes
            for (const auto& resp : op.responses) {
                if (!resp.is_default && resp.status != 0) {
                    int code = resp.status;
                    if (code < 100 || code >= 600) {
                        return std::unexpected(make_error_code(error_code::openapi_invalid_spec));
                    }
                }
            }
        }
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
