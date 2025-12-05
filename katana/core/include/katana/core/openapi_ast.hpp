#pragma once

#include "arena.hpp"
#include "http.hpp"

#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace katana::openapi {

enum class schema_kind : uint8_t { object, array, string, integer, number, boolean, null_type };

enum class param_location : uint8_t { path, query, header, cookie };

struct schema;

struct property {
    arena_string<> name;
    const schema* type = nullptr;
    bool required = false;
};

struct schema {
    explicit schema(monotonic_arena* arena = nullptr)
        : name(arena_allocator<char>(arena)), format(arena_allocator<char>(arena)),
          ref(arena_allocator<char>(arena)), description(arena_allocator<char>(arena)),
          pattern(arena_allocator<char>(arena)), discriminator(arena_allocator<char>(arena)),
          default_value(arena_allocator<char>(arena)), properties(arena_allocator<property>(arena)),
          one_of(arena_allocator<const schema*>(arena)),
          any_of(arena_allocator<const schema*>(arena)),
          all_of(arena_allocator<const schema*>(arena)),
          enum_values(arena_allocator<arena_string<>>(arena)),
          parent_context(arena_allocator<char>(arena)),
          field_context(arena_allocator<char>(arena)) {}

    schema_kind kind{schema_kind::object};
    arena_string<> name;
    arena_string<> format;
    arena_string<> ref;
    arena_string<> description;
    arena_string<> pattern;
    arena_string<> discriminator;
    arena_string<> default_value;

    const schema* items = nullptr; // for arrays
    arena_vector<property> properties;
    arena_vector<const schema*> one_of;
    arena_vector<const schema*> any_of;
    arena_vector<const schema*> all_of;
    const schema* additional_properties = nullptr;
    bool additional_properties_allowed = true;

    bool nullable = false;
    bool deprecated = false;
    bool unique_items = false;
    std::optional<double> minimum;
    std::optional<double> maximum;
    std::optional<double> exclusive_minimum;
    std::optional<double> exclusive_maximum;
    std::optional<double> multiple_of;
    std::optional<size_t> min_length;
    std::optional<size_t> max_length;
    std::optional<size_t> min_items;
    std::optional<size_t> max_items;
    arena_vector<arena_string<>> enum_values;

    // Context tracking for intelligent naming (e.g., Task.title → Task_Title_t)
    arena_string<> parent_context; // Parent schema name (Task, CreateTaskRequest, etc.)
    arena_string<> field_context;  // Field name within parent (title, description, etc.)

    bool required = false;
    bool is_ref = false;
};

struct media_type {
    explicit media_type(monotonic_arena* arena = nullptr)
        : content_type(arena_allocator<char>(arena)) {}

    arena_string<> content_type;
    const schema* type = nullptr;
};

struct parameter {
    explicit parameter(monotonic_arena* arena = nullptr)
        : name(arena_allocator<char>(arena)), description(arena_allocator<char>(arena)),
          style(arena_allocator<char>(arena)) {}

    arena_string<> name;
    param_location in;
    bool required = false;
    const schema* type = nullptr;
    arena_string<> description;
    arena_string<> style;
    bool explode = false;
};

struct response {
    explicit response(monotonic_arena* arena = nullptr)
        : description(arena_allocator<char>(arena)), content(arena_allocator<media_type>(arena)) {}

    int status = 200;
    bool is_default = false; // true если "default" response
    arena_string<> description;
    arena_vector<media_type> content;

    [[nodiscard]] const media_type* first_media() const noexcept {
        if (content.empty()) {
            return nullptr;
        }
        return &content.front();
    }
};

struct request_body {
    explicit request_body(monotonic_arena* arena = nullptr)
        : description(arena_allocator<char>(arena)), content(arena_allocator<media_type>(arena)) {}

    arena_string<> description;
    arena_vector<media_type> content;

    [[nodiscard]] const media_type* first_media() const noexcept {
        if (content.empty()) {
            return nullptr;
        }
        return &content.front();
    }
};

struct operation {
    explicit operation(monotonic_arena* arena = nullptr)
        : operation_id(arena_allocator<char>(arena)), summary(arena_allocator<char>(arena)),
          description(arena_allocator<char>(arena)), parameters(arena_allocator<parameter>(arena)),
          responses(arena_allocator<response>(arena)), x_katana_cache(arena_allocator<char>(arena)),
          x_katana_alloc(arena_allocator<char>(arena)),
          x_katana_rate_limit(arena_allocator<char>(arena)) {}

    http::method method = http::method::unknown;
    arena_string<> operation_id;
    arena_string<> summary;
    arena_string<> description;
    arena_vector<parameter> parameters;
    request_body* body = nullptr;
    arena_vector<response> responses;

    // x-katana-* extensions
    arena_string<> x_katana_cache;      // e.g., "300s", "5m", "true"
    arena_string<> x_katana_alloc;      // e.g., "4096", "pool"
    arena_string<> x_katana_rate_limit; // e.g., "100/s", "1000/m"
};

struct path_item {
    explicit path_item(monotonic_arena* arena = nullptr)
        : path(arena_allocator<char>(arena)), operations(arena_allocator<operation>(arena)) {}

    arena_string<> path;
    arena_vector<operation> operations;
};

struct document {
    explicit document(monotonic_arena& arena) noexcept
        : arena_(&arena), schemas(arena_allocator<schema>(&arena)),
          paths(arena_allocator<path_item>(&arena)), openapi_version(arena_allocator<char>(&arena)),
          info_title(arena_allocator<char>(&arena)), info_version(arena_allocator<char>(&arena)) {}

    document(const document&) = delete;
    document& operator=(const document&) = delete;

    document(document&& other) noexcept
        : arena_(other.arena_), schemas(std::move(other.schemas)), paths(std::move(other.paths)),
          openapi_version(std::move(other.openapi_version)),
          info_title(std::move(other.info_title)), info_version(std::move(other.info_version)) {}

    document& operator=(document&& other) noexcept {
        if (this != &other) {
            arena_ = other.arena_;
            schemas = std::move(other.schemas);
            paths = std::move(other.paths);
            openapi_version = std::move(other.openapi_version);
            info_title = std::move(other.info_title);
            info_version = std::move(other.info_version);
        }
        return *this;
    }

    schema& add_schema(std::string_view name) {
        schemas.emplace_back(schema{arena_});
        schema& s = schemas.back();
        s.name = arena_string<>(name.begin(), name.end(), arena_allocator<char>(arena_));
        return s;
    }

    path_item& add_path(std::string_view path) {
        paths.emplace_back(path_item{arena_});
        auto& p = paths.back();
        p.path = arena_string<>(path.begin(), path.end(), arena_allocator<char>(arena_));
        return p;
    }

    schema& add_inline_schema() {
        schemas.emplace_back(schema{arena_});
        return schemas.back();
    }

    monotonic_arena* arena_;
    arena_vector<schema> schemas;
    arena_vector<path_item> paths;
    arena_string<> openapi_version{arena_allocator<char>(nullptr)};
    arena_string<> info_title{arena_allocator<char>(nullptr)};
    arena_string<> info_version{arena_allocator<char>(nullptr)};
};

} // namespace katana::openapi
