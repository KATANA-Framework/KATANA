// layer: flat
#pragma once

#include "katana/core/arena.hpp"
using katana::arena_allocator;
using katana::arena_string;
using katana::arena_vector;
using katana::monotonic_arena;

#include <cctype>
#include <optional>
#include <string_view>

struct RegisterUserRequest {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool EMAIL_REQUIRED = true;
        static constexpr bool PASSWORD_REQUIRED = true;
        static constexpr size_t PASSWORD_MIN_LENGTH = 8;
        static constexpr size_t PASSWORD_MAX_LENGTH = 128;
        static constexpr bool AGE_REQUIRED = false;
        static constexpr double AGE_MINIMUM = 0;
        static constexpr double AGE_MAXIMUM = 120;
    };

    static_assert(metadata::PASSWORD_MIN_LENGTH <= metadata::PASSWORD_MAX_LENGTH,
                  "password: min_length must be <= max_length");
    static_assert(metadata::AGE_MINIMUM <= metadata::AGE_MAXIMUM,
                  "age: minimum must be <= maximum");

    explicit RegisterUserRequest(monotonic_arena* arena = nullptr)
        : arena_(arena), email(arena_allocator<char>(arena)),
          password(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> email;
    arena_string<> password;
    std::optional<int64_t> age = {};
};

using RegisterUserRequest_Email_t = arena_string<>;

using RegisterUserRequest_Password_t = arena_string<>;

using RegisterUserRequest_Age_t = std::optional<int64_t>;

using register_user_resp_200_0 = arena_string<>;
