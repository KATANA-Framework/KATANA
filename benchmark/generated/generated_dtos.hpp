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
    explicit UserInput(monotonic_arena* arena = nullptr)
        : arena_(arena), name(arena_allocator<char>(arena)), email(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> name;
    arena_string<> email;
    int64_t age = {};
};
