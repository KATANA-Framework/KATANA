// layer: flat
#pragma once

#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"
#include <charconv>
#include <optional>
#include <string>
#include <vector>

using katana::monotonic_arena;

inline std::optional<text_uppercase_body_0> parse_text_uppercase_body_0(std::string_view json,
                                                                        monotonic_arena* arena);
inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena);
inline std::optional<text_uppercase_resp_200_0>
parse_text_uppercase_resp_200_0(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena);
inline std::optional<text_lowercase_body_0> parse_text_lowercase_body_0(std::string_view json,
                                                                        monotonic_arena* arena);
inline std::optional<schema_2> parse_schema_2(std::string_view json, monotonic_arena* arena);
inline std::optional<text_lowercase_resp_200_0>
parse_text_lowercase_resp_200_0(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_3> parse_schema_3(std::string_view json, monotonic_arena* arena);
inline std::optional<text_reverse_body_0> parse_text_reverse_body_0(std::string_view json,
                                                                    monotonic_arena* arena);
inline std::optional<schema_4> parse_schema_4(std::string_view json, monotonic_arena* arena);
inline std::optional<text_reverse_resp_200_0> parse_text_reverse_resp_200_0(std::string_view json,
                                                                            monotonic_arena* arena);
inline std::optional<schema_5> parse_schema_5(std::string_view json, monotonic_arena* arena);
inline std::optional<text_stats_body_0> parse_text_stats_body_0(std::string_view json,
                                                                monotonic_arena* arena);
inline std::optional<schema_6> parse_schema_6(std::string_view json, monotonic_arena* arena);
inline std::optional<text_stats_resp_200_0> parse_text_stats_resp_200_0(std::string_view json,
                                                                        monotonic_arena* arena);
inline std::optional<schema_7> parse_schema_7(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_8> parse_schema_8(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_9> parse_schema_9(std::string_view json, monotonic_arena* arena);
inline std::optional<text_transform_body_0> parse_text_transform_body_0(std::string_view json,
                                                                        monotonic_arena* arena);
inline std::optional<schema_10> parse_schema_10(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_11> parse_schema_11(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_12> parse_schema_12(std::string_view json, monotonic_arena* arena);
inline std::optional<text_transform_resp_200_0>
parse_text_transform_resp_200_0(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_13> parse_schema_13(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_14> parse_schema_14(std::string_view json, monotonic_arena* arena);
inline std::optional<schema_15> parse_schema_15(std::string_view json, monotonic_arena* arena);

inline std::string serialize_text_uppercase_body_0(const text_uppercase_body_0& obj);
inline std::string serialize_schema(const schema& obj);
inline std::string serialize_text_uppercase_resp_200_0(const text_uppercase_resp_200_0& obj);
inline std::string serialize_schema_1(const schema_1& obj);
inline std::string serialize_text_lowercase_body_0(const text_lowercase_body_0& obj);
inline std::string serialize_schema_2(const schema_2& obj);
inline std::string serialize_text_lowercase_resp_200_0(const text_lowercase_resp_200_0& obj);
inline std::string serialize_schema_3(const schema_3& obj);
inline std::string serialize_text_reverse_body_0(const text_reverse_body_0& obj);
inline std::string serialize_schema_4(const schema_4& obj);
inline std::string serialize_text_reverse_resp_200_0(const text_reverse_resp_200_0& obj);
inline std::string serialize_schema_5(const schema_5& obj);
inline std::string serialize_text_stats_body_0(const text_stats_body_0& obj);
inline std::string serialize_schema_6(const schema_6& obj);
inline std::string serialize_text_stats_resp_200_0(const text_stats_resp_200_0& obj);
inline std::string serialize_schema_7(const schema_7& obj);
inline std::string serialize_schema_8(const schema_8& obj);
inline std::string serialize_schema_9(const schema_9& obj);
inline std::string serialize_text_transform_body_0(const text_transform_body_0& obj);
inline std::string serialize_schema_10(const schema_10& obj);
inline std::string serialize_schema_11(const schema_11& obj);
inline std::string serialize_schema_12(const schema_12& obj);
inline std::string serialize_text_transform_resp_200_0(const text_transform_resp_200_0& obj);
inline std::string serialize_schema_13(const schema_13& obj);
inline std::string serialize_schema_14(const schema_14& obj);
inline std::string serialize_schema_15(const schema_15& obj);

inline std::optional<std::vector<text_uppercase_body_0>>
parse_text_uppercase_body_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json,
                                                             monotonic_arena* arena);
inline std::optional<std::vector<text_uppercase_resp_200_0>>
parse_text_uppercase_resp_200_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<text_lowercase_body_0>>
parse_text_lowercase_body_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_2>> parse_schema_2_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<text_lowercase_resp_200_0>>
parse_text_lowercase_resp_200_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_3>> parse_schema_3_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<text_reverse_body_0>>
parse_text_reverse_body_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_4>> parse_schema_4_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<text_reverse_resp_200_0>>
parse_text_reverse_resp_200_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_5>> parse_schema_5_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<text_stats_body_0>>
parse_text_stats_body_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_6>> parse_schema_6_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<text_stats_resp_200_0>>
parse_text_stats_resp_200_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_7>> parse_schema_7_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<schema_8>> parse_schema_8_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<schema_9>> parse_schema_9_array(std::string_view json,
                                                                 monotonic_arena* arena);
inline std::optional<std::vector<text_transform_body_0>>
parse_text_transform_body_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_10>> parse_schema_10_array(std::string_view json,
                                                                   monotonic_arena* arena);
inline std::optional<std::vector<schema_11>> parse_schema_11_array(std::string_view json,
                                                                   monotonic_arena* arena);
inline std::optional<std::vector<schema_12>> parse_schema_12_array(std::string_view json,
                                                                   monotonic_arena* arena);
inline std::optional<std::vector<text_transform_resp_200_0>>
parse_text_transform_resp_200_0_array(std::string_view json, monotonic_arena* arena);
inline std::optional<std::vector<schema_13>> parse_schema_13_array(std::string_view json,
                                                                   monotonic_arena* arena);
inline std::optional<std::vector<schema_14>> parse_schema_14_array(std::string_view json,
                                                                   monotonic_arena* arena);
inline std::optional<std::vector<schema_15>> parse_schema_15_array(std::string_view json,
                                                                   monotonic_arena* arena);

inline std::string
serialize_text_uppercase_body_0_array(const std::vector<text_uppercase_body_0>& arr);
inline std::string
serialize_text_uppercase_body_0_array(const arena_vector<text_uppercase_body_0>& arr);
inline std::string serialize_schema_array(const std::vector<schema>& arr);
inline std::string serialize_schema_array(const arena_vector<schema>& arr);
inline std::string
serialize_text_uppercase_resp_200_0_array(const std::vector<text_uppercase_resp_200_0>& arr);
inline std::string
serialize_text_uppercase_resp_200_0_array(const arena_vector<text_uppercase_resp_200_0>& arr);
inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr);
inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr);
inline std::string
serialize_text_lowercase_body_0_array(const std::vector<text_lowercase_body_0>& arr);
inline std::string
serialize_text_lowercase_body_0_array(const arena_vector<text_lowercase_body_0>& arr);
inline std::string serialize_schema_2_array(const std::vector<schema_2>& arr);
inline std::string serialize_schema_2_array(const arena_vector<schema_2>& arr);
inline std::string
serialize_text_lowercase_resp_200_0_array(const std::vector<text_lowercase_resp_200_0>& arr);
inline std::string
serialize_text_lowercase_resp_200_0_array(const arena_vector<text_lowercase_resp_200_0>& arr);
inline std::string serialize_schema_3_array(const std::vector<schema_3>& arr);
inline std::string serialize_schema_3_array(const arena_vector<schema_3>& arr);
inline std::string serialize_text_reverse_body_0_array(const std::vector<text_reverse_body_0>& arr);
inline std::string
serialize_text_reverse_body_0_array(const arena_vector<text_reverse_body_0>& arr);
inline std::string serialize_schema_4_array(const std::vector<schema_4>& arr);
inline std::string serialize_schema_4_array(const arena_vector<schema_4>& arr);
inline std::string
serialize_text_reverse_resp_200_0_array(const std::vector<text_reverse_resp_200_0>& arr);
inline std::string
serialize_text_reverse_resp_200_0_array(const arena_vector<text_reverse_resp_200_0>& arr);
inline std::string serialize_schema_5_array(const std::vector<schema_5>& arr);
inline std::string serialize_schema_5_array(const arena_vector<schema_5>& arr);
inline std::string serialize_text_stats_body_0_array(const std::vector<text_stats_body_0>& arr);
inline std::string serialize_text_stats_body_0_array(const arena_vector<text_stats_body_0>& arr);
inline std::string serialize_schema_6_array(const std::vector<schema_6>& arr);
inline std::string serialize_schema_6_array(const arena_vector<schema_6>& arr);
inline std::string
serialize_text_stats_resp_200_0_array(const std::vector<text_stats_resp_200_0>& arr);
inline std::string
serialize_text_stats_resp_200_0_array(const arena_vector<text_stats_resp_200_0>& arr);
inline std::string serialize_schema_7_array(const std::vector<schema_7>& arr);
inline std::string serialize_schema_7_array(const arena_vector<schema_7>& arr);
inline std::string serialize_schema_8_array(const std::vector<schema_8>& arr);
inline std::string serialize_schema_8_array(const arena_vector<schema_8>& arr);
inline std::string serialize_schema_9_array(const std::vector<schema_9>& arr);
inline std::string serialize_schema_9_array(const arena_vector<schema_9>& arr);
inline std::string
serialize_text_transform_body_0_array(const std::vector<text_transform_body_0>& arr);
inline std::string
serialize_text_transform_body_0_array(const arena_vector<text_transform_body_0>& arr);
inline std::string serialize_schema_10_array(const std::vector<schema_10>& arr);
inline std::string serialize_schema_10_array(const arena_vector<schema_10>& arr);
inline std::string serialize_schema_11_array(const std::vector<schema_11>& arr);
inline std::string serialize_schema_11_array(const arena_vector<schema_11>& arr);
inline std::string serialize_schema_12_array(const std::vector<schema_12>& arr);
inline std::string serialize_schema_12_array(const arena_vector<schema_12>& arr);
inline std::string
serialize_text_transform_resp_200_0_array(const std::vector<text_transform_resp_200_0>& arr);
inline std::string
serialize_text_transform_resp_200_0_array(const arena_vector<text_transform_resp_200_0>& arr);
inline std::string serialize_schema_13_array(const std::vector<schema_13>& arr);
inline std::string serialize_schema_13_array(const arena_vector<schema_13>& arr);
inline std::string serialize_schema_14_array(const std::vector<schema_14>& arr);
inline std::string serialize_schema_14_array(const arena_vector<schema_14>& arr);
inline std::string serialize_schema_15_array(const std::vector<schema_15>& arr);
inline std::string serialize_schema_15_array(const arena_vector<schema_15>& arr);

inline std::optional<text_uppercase_body_0> parse_text_uppercase_body_0(std::string_view json,
                                                                        monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_uppercase_body_0 obj(arena);
    bool has_text = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text)
        return std::nullopt;
    return obj;
}

inline std::optional<schema> parse_schema(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<text_uppercase_resp_200_0>
parse_text_uppercase_resp_200_0(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_uppercase_resp_200_0 obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

inline std::optional<schema_1> parse_schema_1(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_1{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<text_lowercase_body_0> parse_text_lowercase_body_0(std::string_view json,
                                                                        monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_lowercase_body_0 obj(arena);
    bool has_text = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text)
        return std::nullopt;
    return obj;
}

inline std::optional<schema_2> parse_schema_2(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_2{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<text_lowercase_resp_200_0>
parse_text_lowercase_resp_200_0(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_lowercase_resp_200_0 obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

inline std::optional<schema_3> parse_schema_3(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_3{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<text_reverse_body_0> parse_text_reverse_body_0(std::string_view json,
                                                                    monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_reverse_body_0 obj(arena);
    bool has_text = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text)
        return std::nullopt;
    return obj;
}

inline std::optional<schema_4> parse_schema_4(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_4{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<text_reverse_resp_200_0>
parse_text_reverse_resp_200_0(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_reverse_resp_200_0 obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

inline std::optional<schema_5> parse_schema_5(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_5{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<text_stats_body_0> parse_text_stats_body_0(std::string_view json,
                                                                monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_stats_body_0 obj(arena);
    bool has_text = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text)
        return std::nullopt;
    return obj;
}

inline std::optional<schema_6> parse_schema_6(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_6{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<text_stats_resp_200_0> parse_text_stats_resp_200_0(std::string_view json,
                                                                        monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_stats_resp_200_0 obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "chars") {
            if (auto v = katana::serde::parse_size(cur)) {
                obj.chars = static_cast<int64_t>(*v);
            } else {
                cur.skip_value();
            }
        } else if (*key == "words") {
            if (auto v = katana::serde::parse_size(cur)) {
                obj.words = static_cast<int64_t>(*v);
            } else {
                cur.skip_value();
            }
        } else if (*key == "lines") {
            if (auto v = katana::serde::parse_size(cur)) {
                obj.lines = static_cast<int64_t>(*v);
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

inline std::optional<schema_7> parse_schema_7(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = katana::serde::parse_size(cur))
        return schema_7{static_cast<int64_t>(*v)};
    return std::nullopt;
}

inline std::optional<schema_8> parse_schema_8(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = katana::serde::parse_size(cur))
        return schema_8{static_cast<int64_t>(*v)};
    return std::nullopt;
}

inline std::optional<schema_9> parse_schema_9(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = katana::serde::parse_size(cur))
        return schema_9{static_cast<int64_t>(*v)};
    return std::nullopt;
}

inline std::optional<text_transform_body_0> parse_text_transform_body_0(std::string_view json,
                                                                        monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_transform_body_0 obj(arena);
    bool has_text = false;
    bool has_operation = false;

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "text") {
            has_text = true;
            if (auto v = cur.string()) {
                obj.text = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "operation") {
            has_operation = true;
            if (auto v = cur.string()) {
                auto enum_val = schema_11_enum_from_string(std::string_view(v->begin(), v->end()));
                if (enum_val)
                    obj.operation = *enum_val;
            } else {
                cur.skip_value();
            }
        } else if (*key == "trim") {
            if (auto v = katana::serde::parse_bool(cur)) {
                obj.trim = *v;
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    if (!has_text)
        return std::nullopt;
    if (!has_operation)
        return std::nullopt;
    return obj;
}

inline std::optional<schema_10> parse_schema_10(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_10{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<schema_11> parse_schema_11(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = cur.string()) {
        return schema_11_enum_from_string(std::string_view(v->begin(), v->end()));
    }
    return std::nullopt;
}

inline std::optional<schema_12> parse_schema_12(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = katana::serde::parse_bool(cur))
        return schema_12{*v};
    return std::nullopt;
}

inline std::optional<text_transform_resp_200_0>
parse_text_transform_resp_200_0(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_object_start())
        return std::nullopt;

    text_transform_resp_200_0 obj(arena);

    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_object_end())
            break;
        auto key = cur.string();
        if (!key || !cur.consume(':'))
            break;

        if (*key == "original_length") {
            if (auto v = katana::serde::parse_size(cur)) {
                obj.original_length = static_cast<int64_t>(*v);
            } else {
                cur.skip_value();
            }
        } else if (*key == "result") {
            if (auto v = cur.string()) {
                obj.result = arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else if (*key == "operation_applied") {
            if (auto v = cur.string()) {
                obj.operation_applied =
                    arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena));
            } else {
                cur.skip_value();
            }
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
    return obj;
}

inline std::optional<schema_13> parse_schema_13(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    (void)arena;
    if (auto v = katana::serde::parse_size(cur))
        return schema_13{static_cast<int64_t>(*v)};
    return std::nullopt;
}

inline std::optional<schema_14> parse_schema_14(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_14{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::optional<schema_15> parse_schema_15(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (auto v = cur.string()) {
        return schema_15{arena_string<>(v->begin(), v->end(), arena_allocator<char>(arena))};
    }
    return std::nullopt;
}

inline std::string serialize_text_uppercase_body_0(const text_uppercase_body_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"text\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.text));
    json.push_back('"');
    json.push_back('}');
    return json;
}

inline std::string serialize_schema(const schema& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_text_uppercase_resp_200_0(const text_uppercase_resp_200_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"result\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.result));
    json.push_back('"');
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_1(const schema_1& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_text_lowercase_body_0(const text_lowercase_body_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"text\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.text));
    json.push_back('"');
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_2(const schema_2& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_text_lowercase_resp_200_0(const text_lowercase_resp_200_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"result\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.result));
    json.push_back('"');
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_3(const schema_3& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_text_reverse_body_0(const text_reverse_body_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"text\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.text));
    json.push_back('"');
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_4(const schema_4& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_text_reverse_resp_200_0(const text_reverse_resp_200_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"result\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.result));
    json.push_back('"');
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_5(const schema_5& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_text_stats_body_0(const text_stats_body_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"text\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.text));
    json.push_back('"');
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_6(const schema_6& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_text_stats_resp_200_0(const text_stats_resp_200_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"chars\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.chars);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"words\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.words);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"lines\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.lines);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_7(const schema_7& obj) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    return std::string(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_schema_8(const schema_8& obj) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    return std::string(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_schema_9(const schema_9& obj) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    return std::string(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_text_transform_body_0(const text_transform_body_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"text\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.text));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"operation\":");
    json.push_back('"');
    json.append(to_string(obj.operation));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"trim\":");
    json.append(obj.trim ? "true" : "false");
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_10(const schema_10& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_schema_11(const schema_11& obj) {
    std::string json;
    auto str = to_string(obj);
    json.reserve(str.size() + 2);
    json.push_back('"');
    json.append(str);
    json.push_back('"');
    return json;
}

inline std::string serialize_schema_12(const schema_12& obj) {
    return obj ? "true" : "false";
}

inline std::string serialize_text_transform_resp_200_0(const text_transform_resp_200_0& obj) {
    std::string json;
    json.reserve(256);
    json.push_back('{');
    bool first = true;

    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"original_length\":");
    {
        char buf[32];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj.original_length);
        json.append(buf, static_cast<size_t>(ptr - buf));
    }
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"result\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.result));
    json.push_back('"');
    if (!first)
        json.push_back(',');
    first = false;
    json.append("\"operation_applied\":");
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj.operation_applied));
    json.push_back('"');
    json.push_back('}');
    return json;
}

inline std::string serialize_schema_13(const schema_13& obj) {
    char buf[32];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), obj);
    return std::string(buf, static_cast<size_t>(ptr - buf));
}

inline std::string serialize_schema_14(const schema_14& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::string serialize_schema_15(const schema_15& obj) {
    std::string json;
    json.reserve(obj.size() + 16);
    json.push_back('"');
    json.append(katana::serde::escape_json_string(obj));
    json.push_back('"');
    return json;
}

inline std::optional<std::vector<text_uppercase_body_0>>
parse_text_uppercase_body_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_uppercase_body_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_uppercase_body_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema>> parse_schema_array(std::string_view json,
                                                             monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_uppercase_resp_200_0>>
parse_text_uppercase_resp_200_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_uppercase_resp_200_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_uppercase_resp_200_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_1>> parse_schema_1_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_1> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_1(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_lowercase_body_0>>
parse_text_lowercase_body_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_lowercase_body_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_lowercase_body_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_2>> parse_schema_2_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_2> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_2(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_lowercase_resp_200_0>>
parse_text_lowercase_resp_200_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_lowercase_resp_200_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_lowercase_resp_200_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_3>> parse_schema_3_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_3> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_3(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_reverse_body_0>>
parse_text_reverse_body_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_reverse_body_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_reverse_body_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_4>> parse_schema_4_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_4> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_4(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_reverse_resp_200_0>>
parse_text_reverse_resp_200_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_reverse_resp_200_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_reverse_resp_200_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_5>> parse_schema_5_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_5> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_5(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_stats_body_0>>
parse_text_stats_body_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_stats_body_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_stats_body_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_6>> parse_schema_6_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_6> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_6(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_stats_resp_200_0>>
parse_text_stats_resp_200_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_stats_resp_200_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_stats_resp_200_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_7>> parse_schema_7_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_7> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_7(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_8>> parse_schema_8_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_8> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_8(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_9>> parse_schema_9_array(std::string_view json,
                                                                 monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_9> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_9(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_transform_body_0>>
parse_text_transform_body_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_transform_body_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_transform_body_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_10>> parse_schema_10_array(std::string_view json,
                                                                   monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_10> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_10(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_11>> parse_schema_11_array(std::string_view json,
                                                                   monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_11> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_11(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_12>> parse_schema_12_array(std::string_view json,
                                                                   monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_12> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_12(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<text_transform_resp_200_0>>
parse_text_transform_resp_200_0_array(std::string_view json, monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<text_transform_resp_200_0> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_text_transform_resp_200_0(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_13>> parse_schema_13_array(std::string_view json,
                                                                   monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_13> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_13(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_14>> parse_schema_14_array(std::string_view json,
                                                                   monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_14> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_14(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::optional<std::vector<schema_15>> parse_schema_15_array(std::string_view json,
                                                                   monotonic_arena* arena) {
    using katana::serde::json_cursor;
    json_cursor cur{json.data(), json.data() + json.size()};
    if (!cur.try_array_start())
        return std::nullopt;

    std::vector<schema_15> result;
    while (!cur.eof()) {
        cur.skip_ws();
        if (cur.try_array_end())
            break;

        // Parse object at current position
        size_t obj_start = cur.pos();
        cur.skip_value();
        size_t obj_end = cur.pos();
        std::string_view obj_json(json.data() + obj_start, obj_end - obj_start);

        auto obj = parse_schema_15(obj_json, arena);
        if (!obj)
            return std::nullopt;
        result.push_back(std::move(*obj));

        cur.try_comma();
    }
    return result;
}

inline std::string
serialize_text_uppercase_body_0_array(const std::vector<text_uppercase_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_uppercase_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_uppercase_body_0_array(const arena_vector<text_uppercase_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_uppercase_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_array(const std::vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_array(const arena_vector<schema>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_uppercase_resp_200_0_array(const std::vector<text_uppercase_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_uppercase_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_uppercase_resp_200_0_array(const arena_vector<text_uppercase_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_uppercase_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_1_array(const std::vector<schema_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_1(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_1_array(const arena_vector<schema_1>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_1(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_lowercase_body_0_array(const std::vector<text_lowercase_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_lowercase_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_lowercase_body_0_array(const arena_vector<text_lowercase_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_lowercase_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_2_array(const std::vector<schema_2>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_2(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_2_array(const arena_vector<schema_2>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_2(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_lowercase_resp_200_0_array(const std::vector<text_lowercase_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_lowercase_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_lowercase_resp_200_0_array(const arena_vector<text_lowercase_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_lowercase_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_3_array(const std::vector<schema_3>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_3(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_3_array(const arena_vector<schema_3>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_3(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_reverse_body_0_array(const std::vector<text_reverse_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_reverse_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_reverse_body_0_array(const arena_vector<text_reverse_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_reverse_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_4_array(const std::vector<schema_4>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_4(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_4_array(const arena_vector<schema_4>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_4(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_reverse_resp_200_0_array(const std::vector<text_reverse_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_reverse_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_reverse_resp_200_0_array(const arena_vector<text_reverse_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_reverse_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_5_array(const std::vector<schema_5>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_5(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_5_array(const arena_vector<schema_5>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_5(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_text_stats_body_0_array(const std::vector<text_stats_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_stats_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_text_stats_body_0_array(const arena_vector<text_stats_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_stats_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_6_array(const std::vector<schema_6>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_6(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_6_array(const arena_vector<schema_6>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_6(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_stats_resp_200_0_array(const std::vector<text_stats_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_stats_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_stats_resp_200_0_array(const arena_vector<text_stats_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_stats_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_7_array(const std::vector<schema_7>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_7(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_7_array(const arena_vector<schema_7>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_7(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_8_array(const std::vector<schema_8>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_8(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_8_array(const arena_vector<schema_8>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_8(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_9_array(const std::vector<schema_9>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_9(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_9_array(const arena_vector<schema_9>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_9(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_transform_body_0_array(const std::vector<text_transform_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_transform_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_transform_body_0_array(const arena_vector<text_transform_body_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_transform_body_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_10_array(const std::vector<schema_10>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_10(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_10_array(const arena_vector<schema_10>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_10(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_11_array(const std::vector<schema_11>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_11(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_11_array(const arena_vector<schema_11>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_11(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_12_array(const std::vector<schema_12>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_12(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_12_array(const arena_vector<schema_12>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_12(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_transform_resp_200_0_array(const std::vector<text_transform_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_transform_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string
serialize_text_transform_resp_200_0_array(const arena_vector<text_transform_resp_200_0>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_text_transform_resp_200_0(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_13_array(const std::vector<schema_13>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_13(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_13_array(const arena_vector<schema_13>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_13(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_14_array(const std::vector<schema_14>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_14(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_14_array(const arena_vector<schema_14>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_14(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_15_array(const std::vector<schema_15>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_15(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}

inline std::string serialize_schema_15_array(const arena_vector<schema_15>& arr) {
    std::string json;
    json.reserve(arr.size() * 32 + 2);
    json.push_back('[');
    for (size_t i = 0; i < arr.size(); ++i) {
        json.append(serialize_schema_15(arr[i]));
        if (i < arr.size() - 1)
            json.push_back(',');
    }
    json.push_back(']');
    return json;
}
