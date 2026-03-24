#include "katana/sql/runtime.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <string>
#include <string_view>

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

} // namespace
