#include "katana/core/json_parser.hpp"

#include <gtest/gtest.h>

#include <array>
#include <optional>
#include <string>
#include <vector>

using katana::validation_error;
using katana::validation_error_code;
using katana::json::array_constraints;
using katana::json::integer_array_field;
using katana::json::integer_field;
using katana::json::parse_object;
using katana::json::string_array_field;
using katana::json::string_constraints;
using katana::json::string_field;

namespace {

struct User {
    katana::arena_string<> name;
    int64_t id = 0;
    katana::arena_string<> email;
    katana::arena_vector<int64_t> scores;

    explicit User(katana::monotonic_arena* arena)
        : name(katana::arena_allocator<char>(arena)), email(katana::arena_allocator<char>(arena)),
          scores(katana::arena_allocator<int64_t>(arena)) {}
};

const auto kUserFields = std::to_array<katana::json::field_descriptor<User>>(
    {string_field<User, katana::arena_string<>>(
         "name", &User::name, true, string_constraints{.min_length = 1, .max_length = 64}),
     integer_field<User>("id", &User::id, true, {.minimum = 1, .maximum = 1'000'000}),
     string_field<User, katana::arena_string<>>(
         "email", &User::email, true, string_constraints{.min_length = 3, .max_length = 120}),
     integer_array_field<User, katana::arena_vector<int64_t>>(
         "scores", &User::scores, false, array_constraints{.min_items = 1, .max_items = 5})});

} // namespace

TEST(JsonParser, ParsesValidObject) {
    const std::string json =
        R"({"name":"alice","id":42,"email":"alice@example.com","scores":[1,2,3]})";
    katana::monotonic_arena arena;
    validation_error err;
    auto parsed = parse_object<User>(json, kUserFields, &arena, &err);
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->name, "alice");
    EXPECT_EQ(parsed->id, 42);
    EXPECT_EQ(parsed->email, "alice@example.com");
    ASSERT_EQ(parsed->scores.size(), 3u);
    EXPECT_EQ(parsed->scores[0], 1);
    EXPECT_EQ(parsed->scores[2], 3);
}

TEST(JsonParser, FailsOnMissingRequired) {
    const std::string json = R"({"id":7})";
    katana::monotonic_arena arena;
    validation_error err;
    auto parsed = parse_object<User>(json, kUserFields, &arena, &err);
    EXPECT_FALSE(parsed.has_value());
    EXPECT_EQ(err.code, validation_error_code::required_field_missing);
    EXPECT_EQ(err.field, "name");
}

TEST(JsonParser, EnforcesBounds) {
    const std::string json = R"({"name":"","id":0,"email":"x","scores":[1,2,3,4,5,6]})";
    katana::monotonic_arena arena;
    validation_error err;
    auto parsed = parse_object<User>(json, kUserFields, &arena, &err);
    EXPECT_FALSE(parsed.has_value());
    EXPECT_EQ(err.code, validation_error_code::string_too_short);
    EXPECT_EQ(err.field, "name");
}
