#include "katana/sql/runtime.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <string>
#include <string_view>
#include <vector>

namespace {

TEST(SqlRuntimeTest, ParsesValuesFromCellView) {
    const auto id = katana::sql::parse_value<int64_t>(katana::sql::cell_view::borrowed("42"));
    const auto active = katana::sql::parse_value<bool>(katana::sql::cell_view::borrowed("true"));
    const auto name =
        katana::sql::parse_value<std::string>(katana::sql::cell_view::borrowed("Ada"));
    const auto ratio = katana::sql::parse_value<double>(katana::sql::cell_view::borrowed("3.25"));

    ASSERT_TRUE(id);
    ASSERT_TRUE(active);
    ASSERT_TRUE(name);
    ASSERT_TRUE(ratio);
    EXPECT_EQ(*id, 42);
    EXPECT_TRUE(*active);
    EXPECT_EQ(*name, "Ada");
    EXPECT_TRUE(std::abs(*ratio - 3.25) < 1e-12);
}

TEST(SqlRuntimeTest, RejectsNullCellViewParse) {
    auto parsed = katana::sql::parse_value<int64_t>(katana::sql::cell_view::null());
    ASSERT_FALSE(parsed);
}

TEST(SqlRuntimeTest, PreservesInlineParameterStorageAcrossCopies) {
    auto original = katana::sql::parameter::from_integer(123456789LL);
    auto copied = original;
    auto moved = std::move(original);

    ASSERT_TRUE(copied.has_value());
    ASSERT_TRUE(moved.has_value());
    EXPECT_EQ(copied.value(), std::string_view("123456789"));
    EXPECT_EQ(moved.value(), std::string_view("123456789"));
}

TEST(SqlRuntimeTest, OwnsStringViewParameterStorage) {
    std::string source = "tools";
    auto parameter = katana::sql::encode_value(std::string_view(source));
    source[0] = 'b';

    ASSERT_TRUE(parameter.has_value());
    EXPECT_EQ(parameter.value(), std::string_view("tools"));
}

TEST(SqlRuntimeTest, EncodesPgArrayParameters) {
    const auto ids = katana::sql::encode_value(std::vector<int64_t>{1, 2, 3});
    const auto names =
        katana::sql::encode_value(std::vector<std::string>{"Ada", "Linus", "Grace"});

    ASSERT_TRUE(ids.has_value());
    ASSERT_TRUE(names.has_value());
    EXPECT_EQ(ids.value(), std::string_view("{1,2,3}"));
    EXPECT_EQ(names.value(), std::string_view("{\"Ada\",\"Linus\",\"Grace\"}"));
}

TEST(SqlRuntimeTest, ParsesPgArrayValues) {
    const auto ids =
        katana::sql::parse_value<std::vector<int64_t>>(katana::sql::cell_view::borrowed("{1,2,3}"));
    const auto names = katana::sql::parse_value<std::vector<std::string>>(
        katana::sql::cell_view::borrowed("{\"Ada\",\"Linus\",\"Grace Hopper\"}"));

    ASSERT_TRUE(ids);
    ASSERT_TRUE(names);
    EXPECT_EQ(*ids, (std::vector<int64_t>{1, 2, 3}));
    EXPECT_EQ(*names, (std::vector<std::string>{"Ada", "Linus", "Grace Hopper"}));
}

} // namespace
