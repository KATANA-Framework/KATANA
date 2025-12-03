#pragma once

#include "katana/core/arena.hpp"
using katana::arena_allocator;
using katana::arena_string;
using katana::arena_vector;
using katana::monotonic_arena;

#include <cctype>
#include <optional>
#include <string_view>

struct Task {
    explicit Task(monotonic_arena* arena = nullptr)
        : arena_(arena), title(arena_allocator<char>(arena)),
          description(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    int64_t id;
    arena_string<> title;
    arena_string<> description;
    bool completed;
};

struct CreateTaskRequest {
    explicit CreateTaskRequest(monotonic_arena* arena = nullptr)
        : arena_(arena), title(arena_allocator<char>(arena)),
          description(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> title;
    arena_string<> description;
    bool completed = {};
};

struct UpdateTaskRequest {
    explicit UpdateTaskRequest(monotonic_arena* arena = nullptr)
        : arena_(arena), title(arena_allocator<char>(arena)),
          description(arena_allocator<char>(arena)) {}

    monotonic_arena* arena_;
    arena_string<> title;
    arena_string<> description;
    bool completed = {};
};
