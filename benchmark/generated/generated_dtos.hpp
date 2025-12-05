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

struct UserInput {
    // Compile-time metadata for validation
    struct metadata {
        static constexpr bool NAME_REQUIRED = true;
        static constexpr size_t NAME_MIN_LENGTH = 1;
        static constexpr bool EMAIL_REQUIRED = true;
        static constexpr bool AGE_REQUIRED = false;
        static constexpr double AGE_MINIMUM = 0;
    };

    explicit UserInput(monotonic_arena* arena = nullptr)
        : arena_(arena), name(arena_allocator<char>(arena)), email(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> name;
    arena_string<> email;
    int64_t age = {};
};

using UserInput_Name_t = arena_string<>;

using UserInput_Email_t = arena_string<>;

using UserInput_Age_t = int64_t;

using getUser_param_id = int64_t;
