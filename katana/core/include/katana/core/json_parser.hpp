#pragma once

#include "arena.hpp"
#include "serde.hpp"
#include "validation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace katana::json {

// Constraints used during parsing. These mirror OpenAPI constraints and are
// intentionally lightweight to avoid allocations or heavy regex work on the
// hot path. Regex/pattern checks are expected to be handled by generated
// validators; the parser focuses on structural correctness and size limits.
struct string_constraints {
    size_t min_length = 0;
    size_t max_length = std::numeric_limits<size_t>::max();
};

struct number_constraints {
    double minimum = -std::numeric_limits<double>::infinity();
    double maximum = std::numeric_limits<double>::infinity();
    bool exclusive_minimum = false;
    bool exclusive_maximum = false;
};

struct array_constraints {
    size_t min_items = 0;
    size_t max_items = std::numeric_limits<size_t>::max();
};

enum class field_kind {
    string,
    integer,
    number,
    boolean,
    array_string,
    array_integer,
    array_number,
    array_boolean,
};

template <typename T> struct field_descriptor {
    std::string_view json_name;
    field_kind kind;
    bool required;
    std::ptrdiff_t offset; // byte offset to the field inside T
    string_constraints str{};
    number_constraints num{};
    array_constraints arr{};
    bool use_arena = false; // for arena_string / arena_vector

    using parse_fn = std::optional<validation_error> (*)(serde::json_cursor&,
                                                         T&,
                                                         monotonic_arena*,
                                                         const field_descriptor&);
    parse_fn parse{};
};

// Utility: compute offset of a member
template <typename T, typename Field>
inline std::ptrdiff_t member_offset(Field T::*member) noexcept {
    auto base = reinterpret_cast<std::uintptr_t>(static_cast<T*>(nullptr));
    auto mem = reinterpret_cast<std::uintptr_t>(&(static_cast<T*>(nullptr)->*member));
    return static_cast<std::ptrdiff_t>(mem - base);
}

template <typename Field> inline Field& offset_ref(void* base, std::ptrdiff_t off) noexcept {
    auto* ptr = static_cast<char*>(base) + off;
    return *reinterpret_cast<Field*>(ptr);
}

// Primitive field parsers
template <typename T, typename Field>
std::optional<validation_error> parse_string_field(serde::json_cursor& cur,
                                                   T& obj,
                                                   monotonic_arena* arena,
                                                   const field_descriptor<T>& desc) {
    auto v = cur.string();
    if (!v) {
        return validation_error{desc.json_name, validation_error_code::invalid_type};
    }
    if (v->size() < desc.str.min_length) {
        return validation_error{desc.json_name,
                                validation_error_code::string_too_short,
                                static_cast<double>(desc.str.min_length)};
    }
    if (v->size() > desc.str.max_length) {
        return validation_error{desc.json_name,
                                validation_error_code::string_too_long,
                                static_cast<double>(desc.str.max_length)};
    }

    auto& field = offset_ref<Field>(&obj, desc.offset);
    if constexpr (std::is_same_v<Field, katana::arena_string<>>) {
        field = katana::arena_string<>(v->begin(), v->end(), katana::arena_allocator<char>(arena));
    } else {
        field.assign(v->begin(), v->end());
    }
    return std::nullopt;
}

template <typename T>
std::optional<validation_error> parse_integer_field(serde::json_cursor& cur,
                                                    T& obj,
                                                    monotonic_arena*,
                                                    const field_descriptor<T>& desc) {
    auto v = katana::serde::parse_size(cur);
    if (!v) {
        return validation_error{desc.json_name, validation_error_code::invalid_type};
    }
    int64_t value = static_cast<int64_t>(*v);
    const double dv = static_cast<double>(value);
    if (desc.num.exclusive_minimum) {
        if (dv <= desc.num.minimum) {
            return validation_error{desc.json_name,
                                    validation_error_code::value_below_exclusive_minimum,
                                    desc.num.minimum};
        }
    } else if (dv < desc.num.minimum) {
        return validation_error{
            desc.json_name, validation_error_code::value_too_small, desc.num.minimum};
    }
    if (desc.num.exclusive_maximum) {
        if (dv >= desc.num.maximum) {
            return validation_error{desc.json_name,
                                    validation_error_code::value_above_exclusive_maximum,
                                    desc.num.maximum};
        }
    } else if (dv > desc.num.maximum) {
        return validation_error{
            desc.json_name, validation_error_code::value_too_large, desc.num.maximum};
    }

    offset_ref<int64_t>(&obj, desc.offset) = value;
    return std::nullopt;
}

template <typename T>
std::optional<validation_error> parse_number_field(serde::json_cursor& cur,
                                                   T& obj,
                                                   monotonic_arena*,
                                                   const field_descriptor<T>& desc) {
    auto v = katana::serde::parse_double(cur);
    if (!v) {
        return validation_error{desc.json_name, validation_error_code::invalid_type};
    }
    double value = *v;
    if (desc.num.exclusive_minimum) {
        if (value <= desc.num.minimum) {
            return validation_error{desc.json_name,
                                    validation_error_code::value_below_exclusive_minimum,
                                    desc.num.minimum};
        }
    } else if (value < desc.num.minimum) {
        return validation_error{
            desc.json_name, validation_error_code::value_too_small, desc.num.minimum};
    }
    if (desc.num.exclusive_maximum) {
        if (value >= desc.num.maximum) {
            return validation_error{desc.json_name,
                                    validation_error_code::value_above_exclusive_maximum,
                                    desc.num.maximum};
        }
    } else if (value > desc.num.maximum) {
        return validation_error{
            desc.json_name, validation_error_code::value_too_large, desc.num.maximum};
    }

    offset_ref<double>(&obj, desc.offset) = value;
    return std::nullopt;
}

template <typename T>
std::optional<validation_error> parse_bool_field(serde::json_cursor& cur,
                                                 T& obj,
                                                 monotonic_arena*,
                                                 const field_descriptor<T>& desc) {
    auto v = katana::serde::parse_bool(cur);
    if (!v) {
        return validation_error{desc.json_name, validation_error_code::invalid_type};
    }
    offset_ref<bool>(&obj, desc.offset) = *v;
    return std::nullopt;
}

// Array parsing helpers
template <typename Vector, typename ValueParser>
std::optional<validation_error> parse_array(serde::json_cursor& cur,
                                            Vector& vec,
                                            ValueParser&& parse_value,
                                            const array_constraints& constraints,
                                            std::string_view field_name) {
    if (!cur.try_array_start()) {
        return validation_error{field_name, validation_error_code::invalid_type};
    }
    vec.clear();
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end()) {
            break;
        }
        const char* value_start = cur.ptr;
        cur.skip_value();
        std::string_view sv(value_start, static_cast<size_t>(cur.ptr - value_start));
        if (auto err = parse_value(sv)) {
            return err;
        }
        cur.try_comma();
    }
    if (vec.size() < constraints.min_items) {
        return validation_error{field_name,
                                validation_error_code::array_too_small,
                                static_cast<double>(constraints.min_items)};
    }
    if (vec.size() > constraints.max_items) {
        return validation_error{field_name,
                                validation_error_code::array_too_large,
                                static_cast<double>(constraints.max_items)};
    }
    return std::nullopt;
}

template <typename T, typename Vector>
std::optional<validation_error> parse_string_array(serde::json_cursor& cur,
                                                   T& obj,
                                                   monotonic_arena* arena,
                                                   const field_descriptor<T>& desc) {
    using Value = typename Vector::value_type;
    auto& target = offset_ref<Vector>(&obj, desc.offset);
    auto parse_elem = [&](std::string_view sv) -> std::optional<validation_error> {
        serde::json_cursor item_cur{sv.data(), sv.data() + sv.size()};
        auto val = item_cur.string();
        if (!val) {
            return validation_error{desc.json_name, validation_error_code::invalid_type};
        }
        if constexpr (std::is_same_v<Value, katana::arena_string<>>) {
            target.emplace_back(val->begin(), val->end(), katana::arena_allocator<char>(arena));
        } else {
            target.emplace_back(val->begin(), val->end());
        }
        return std::nullopt;
    };
    return parse_array(cur, target, parse_elem, desc.arr, desc.json_name);
}

template <typename T, typename Vector>
std::optional<validation_error> parse_integer_array(serde::json_cursor& cur,
                                                    T& obj,
                                                    monotonic_arena*,
                                                    const field_descriptor<T>& desc) {
    using Value = typename Vector::value_type;
    auto& target = offset_ref<Vector>(&obj, desc.offset);
    auto parse_elem = [&](std::string_view sv) -> std::optional<validation_error> {
        serde::json_cursor item_cur{sv.data(), sv.data() + sv.size()};
        auto val = katana::serde::parse_size(item_cur);
        if (!val) {
            return validation_error{desc.json_name, validation_error_code::invalid_type};
        }
        target.emplace_back(static_cast<Value>(*val));
        return std::nullopt;
    };
    return parse_array(cur, target, parse_elem, desc.arr, desc.json_name);
}

template <typename T, typename Vector>
std::optional<validation_error> parse_number_array(serde::json_cursor& cur,
                                                   T& obj,
                                                   monotonic_arena*,
                                                   const field_descriptor<T>& desc) {
    using Value = typename Vector::value_type;
    auto& target = offset_ref<Vector>(&obj, desc.offset);
    auto parse_elem = [&](std::string_view sv) -> std::optional<validation_error> {
        serde::json_cursor item_cur{sv.data(), sv.data() + sv.size()};
        auto val = katana::serde::parse_double(item_cur);
        if (!val) {
            return validation_error{desc.json_name, validation_error_code::invalid_type};
        }
        target.emplace_back(static_cast<Value>(*val));
        return std::nullopt;
    };
    return parse_array(cur, target, parse_elem, desc.arr, desc.json_name);
}

template <typename T, typename Vector>
std::optional<validation_error> parse_bool_array(serde::json_cursor& cur,
                                                 T& obj,
                                                 monotonic_arena*,
                                                 const field_descriptor<T>& desc) {
    using Value = typename Vector::value_type;
    auto& target = offset_ref<Vector>(&obj, desc.offset);
    auto parse_elem = [&](std::string_view sv) -> std::optional<validation_error> {
        serde::json_cursor item_cur{sv.data(), sv.data() + sv.size()};
        auto val = katana::serde::parse_bool(item_cur);
        if (!val) {
            return validation_error{desc.json_name, validation_error_code::invalid_type};
        }
        target.emplace_back(static_cast<Value>(*val));
        return std::nullopt;
    };
    return parse_array(cur, target, parse_elem, desc.arr, desc.json_name);
}

// Factories for field descriptors
template <typename T, typename Field>
inline field_descriptor<T>
string_field(std::string_view name, Field T::*member, bool required, string_constraints c = {}) {
    return field_descriptor<T>{name,
                               field_kind::string,
                               required,
                               member_offset(member),
                               c,
                               {},
                               {},
                               std::is_same_v<Field, katana::arena_string<>>,
                               &parse_string_field<T, Field>};
}

template <typename T>
inline field_descriptor<T>
integer_field(std::string_view name, int64_t T::*member, bool required, number_constraints c = {}) {
    return field_descriptor<T>{name,
                               field_kind::integer,
                               required,
                               member_offset(member),
                               {},
                               c,
                               {},
                               false,
                               &parse_integer_field<T>};
}

template <typename T>
inline field_descriptor<T>
number_field(std::string_view name, double T::*member, bool required, number_constraints c = {}) {
    return field_descriptor<T>{name,
                               field_kind::number,
                               required,
                               member_offset(member),
                               {},
                               c,
                               {},
                               false,
                               &parse_number_field<T>};
}

template <typename T>
inline field_descriptor<T> boolean_field(std::string_view name, bool T::*member, bool required) {
    return field_descriptor<T>{name,
                               field_kind::boolean,
                               required,
                               member_offset(member),
                               {},
                               {},
                               {},
                               false,
                               &parse_bool_field<T>};
}

template <typename T, typename Vector>
inline field_descriptor<T> string_array_field(std::string_view name,
                                              Vector T::*member,
                                              bool required,
                                              array_constraints c = {}) {
    return field_descriptor<T>{name,
                               field_kind::array_string,
                               required,
                               member_offset(member),
                               {},
                               {},
                               c,
                               std::is_same_v<typename Vector::value_type, katana::arena_string<>>,
                               &parse_string_array<T, Vector>};
}

template <typename T, typename Vector>
inline field_descriptor<T> integer_array_field(std::string_view name,
                                               Vector T::*member,
                                               bool required,
                                               array_constraints c = {}) {
    return field_descriptor<T>{name,
                               field_kind::array_integer,
                               required,
                               member_offset(member),
                               {},
                               {},
                               c,
                               false,
                               &parse_integer_array<T, Vector>};
}

template <typename T, typename Vector>
inline field_descriptor<T> number_array_field(std::string_view name,
                                              Vector T::*member,
                                              bool required,
                                              array_constraints c = {}) {
    return field_descriptor<T>{name,
                               field_kind::array_number,
                               required,
                               member_offset(member),
                               {},
                               {},
                               c,
                               false,
                               &parse_number_array<T, Vector>};
}

template <typename T, typename Vector>
inline field_descriptor<T> boolean_array_field(std::string_view name,
                                               Vector T::*member,
                                               bool required,
                                               array_constraints c = {}) {
    return field_descriptor<T>{name,
                               field_kind::array_boolean,
                               required,
                               member_offset(member),
                               {},
                               {},
                               c,
                               false,
                               &parse_bool_array<T, Vector>};
}

// Main object parser
template <typename T, size_t N>
std::optional<validation_error> parse_object(std::string_view json,
                                             const std::array<field_descriptor<T>, N>& fields,
                                             T& out,
                                             monotonic_arena* arena,
                                             std::array<bool, N>& seen) {
    serde::json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start()) {
        return validation_error{"", validation_error_code::invalid_type};
    }

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end()) {
            break;
        }
        auto key = cur.string();
        if (!key || !cur.consume(':')) {
            return validation_error{"", validation_error_code::invalid_type};
        }

        bool matched = false;
        for (size_t i = 0; i < N; ++i) {
            const auto& desc = fields[i];
            if (*key == desc.json_name) {
                matched = true;
                seen[i] = true;
                if (auto err = desc.parse(cur, out, arena, desc)) {
                    return err;
                }
                break;
            }
        }
        if (!matched) {
            cur.skip_value();
        }
        cur.try_comma();
    }

    for (size_t i = 0; i < N; ++i) {
        if (fields[i].required && !seen[i]) {
            return validation_error{fields[i].json_name,
                                    validation_error_code::required_field_missing};
        }
    }
    return std::nullopt;
}

template <typename T, size_t N>
std::optional<T> parse_object(std::string_view json,
                              const std::array<field_descriptor<T>, N>& fields,
                              monotonic_arena* arena,
                              validation_error* error_out = nullptr) {
    std::array<bool, N> seen{};
    T obj(arena);
    if (auto err = parse_object(json, fields, obj, arena, seen)) {
        if (error_out) {
            *error_out = *err;
        }
        return std::nullopt;
    }
    return obj;
}

} // namespace katana::json
