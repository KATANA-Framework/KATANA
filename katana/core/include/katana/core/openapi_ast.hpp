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

    // 1-based line in the source spec where this schema is defined (0 if unknown,
    // e.g. when the input was JSON rather than YAML). Used for provenance comments
    // in generated code so a parser/serializer can be traced back to the spec.
    size_t source_line = 0;
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

enum class cache_policy_kind : uint8_t { none, disabled, enabled, ttl };

struct cache_policy {
    explicit cache_policy(monotonic_arena* arena = nullptr)
        : raw_value(arena_allocator<char>(arena)), ttl(arena_allocator<char>(arena)) {}

    cache_policy_kind kind{cache_policy_kind::none};
    arena_string<> raw_value;
    arena_string<> ttl;

    [[nodiscard]] bool present() const noexcept { return kind != cache_policy_kind::none; }
    [[nodiscard]] bool enabled() const noexcept {
        return kind == cache_policy_kind::enabled || kind == cache_policy_kind::ttl;
    }
    [[nodiscard]] std::string_view display_value() const noexcept {
        if (!raw_value.empty()) {
            return {raw_value.data(), raw_value.size()};
        }
        switch (kind) {
        case cache_policy_kind::disabled:
            return "false";
        case cache_policy_kind::enabled:
            return "true";
        case cache_policy_kind::ttl:
            return {ttl.data(), ttl.size()};
        case cache_policy_kind::none:
            break;
        }
        return {};
    }
};

enum class alloc_policy_kind : uint8_t { none, named_mode, bytes };

struct alloc_policy {
    explicit alloc_policy(monotonic_arena* arena = nullptr)
        : raw_value(arena_allocator<char>(arena)), mode(arena_allocator<char>(arena)) {}

    alloc_policy_kind kind{alloc_policy_kind::none};
    arena_string<> raw_value;
    arena_string<> mode;
    std::optional<size_t> bytes;

    [[nodiscard]] bool present() const noexcept { return kind != alloc_policy_kind::none; }
    [[nodiscard]] std::string_view display_value() const noexcept {
        if (!raw_value.empty()) {
            return {raw_value.data(), raw_value.size()};
        }
        return {mode.data(), mode.size()};
    }
};

enum class rate_limit_unit : uint8_t { unknown, second, minute, hour };

struct rate_limit_policy {
    explicit rate_limit_policy(monotonic_arena* arena = nullptr)
        : raw_value(arena_allocator<char>(arena)) {}

    bool present = false;
    arena_string<> raw_value;
    std::optional<size_t> count;
    rate_limit_unit unit{rate_limit_unit::unknown};

    [[nodiscard]] bool parsed() const noexcept {
        return present && count.has_value() && unit != rate_limit_unit::unknown;
    }
    [[nodiscard]] std::string_view display_value() const noexcept {
        return {raw_value.data(), raw_value.size()};
    }
};

enum class idempotency_policy_kind : uint8_t { none, disabled, enabled, mode };

struct idempotency_policy {
    explicit idempotency_policy(monotonic_arena* arena = nullptr)
        : raw_value(arena_allocator<char>(arena)), mode(arena_allocator<char>(arena)) {}

    idempotency_policy_kind kind{idempotency_policy_kind::none};
    arena_string<> raw_value;
    arena_string<> mode;

    [[nodiscard]] bool present() const noexcept { return kind != idempotency_policy_kind::none; }
    [[nodiscard]] bool enabled() const noexcept {
        return kind == idempotency_policy_kind::enabled || kind == idempotency_policy_kind::mode;
    }
    [[nodiscard]] std::string_view display_value() const noexcept {
        if (!raw_value.empty()) {
            return {raw_value.data(), raw_value.size()};
        }
        switch (kind) {
        case idempotency_policy_kind::disabled:
            return "false";
        case idempotency_policy_kind::enabled:
            return "true";
        case idempotency_policy_kind::mode:
            return {mode.data(), mode.size()};
        case idempotency_policy_kind::none:
            break;
        }
        return {};
    }
};

struct operation {
    explicit operation(monotonic_arena* arena = nullptr)
        : operation_id(arena_allocator<char>(arena)), summary(arena_allocator<char>(arena)),
          description(arena_allocator<char>(arena)), parameters(arena_allocator<parameter>(arena)),
          responses(arena_allocator<response>(arena)), cache(arena), alloc(arena),
          rate_limit(arena), idempotency(arena) {}

    http::method method = http::method::unknown;
    arena_string<> operation_id;
    arena_string<> summary;
    arena_string<> description;
    arena_vector<parameter> parameters;
    request_body* body = nullptr;
    arena_vector<response> responses;

    // x-katana-* extensions
    cache_policy cache;           // e.g., false, true, "300s", "5m"
    alloc_policy alloc;           // e.g., "4096", "pool"
    rate_limit_policy rate_limit; // e.g., "100/s", "1000/m"
    idempotency_policy idempotency; // e.g., false, true, "required"
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
          info_title(arena_allocator<char>(&arena)), info_version(arena_allocator<char>(&arena)),
          source_file(arena_allocator<char>(&arena)) {}

    document(const document&) = delete;
    document& operator=(const document&) = delete;

    document(document&& other) noexcept
        : arena_(other.arena_), schemas(std::move(other.schemas)), paths(std::move(other.paths)),
          openapi_version(std::move(other.openapi_version)),
          info_title(std::move(other.info_title)), info_version(std::move(other.info_version)),
          source_file(std::move(other.source_file)) {}

    document& operator=(document&& other) noexcept {
        if (this != &other) {
            arena_ = other.arena_;
            schemas = std::move(other.schemas);
            paths = std::move(other.paths);
            openapi_version = std::move(other.openapi_version);
            info_title = std::move(other.info_title);
            info_version = std::move(other.info_version);
            source_file = std::move(other.source_file);
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
    // Basename of the source spec file (e.g. "api.yaml"); empty when loaded from a
    // string or JSON. Paired with schema::source_line for generated provenance comments.
    arena_string<> source_file{arena_allocator<char>(nullptr)};
};

} // namespace katana::openapi
