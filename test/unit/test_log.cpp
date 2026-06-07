// Unit tests for katana::log — the structured JSON logger. We point the sink at a temp file,
// emit lines, then read them back and assert on shape, level filtering, field typing and
// JSON escaping.

#include "katana/core/log.hpp"

#include <algorithm>
#include <cstdio>
#include <gtest/gtest.h>
#include <string>

namespace {

// Capture everything written to the log sink during `body` and return it as a string.
template <typename Fn> std::string capture(katana::log::level min, Fn&& body) {
    std::FILE* f = std::tmpfile();
    EXPECT_NE(f, nullptr);
    const katana::log::level previous = katana::log::min_level();
    katana::log::set_sink(f);
    katana::log::set_min_level(min);

    std::forward<Fn>(body)();

    std::fflush(f);
    std::rewind(f);
    std::string out;
    char buf[4096];
    size_t n = 0;
    while ((n = std::fread(buf, 1, sizeof(buf), f)) > 0) {
        out.append(buf, n);
    }
    std::fclose(f);
    // Restore defaults so other tests aren't affected.
    katana::log::set_sink(stderr);
    katana::log::set_min_level(previous);
    return out;
}

TEST(Log, EmitsOneJsonObjectPerLineWithLevelAndMessage) {
    const std::string out = capture(katana::log::level::info, [] {
        katana::log::info("hello");
    });
    EXPECT_NE(out.find("\"level\":\"info\""), std::string::npos);
    EXPECT_NE(out.find("\"msg\":\"hello\""), std::string::npos);
    EXPECT_NE(out.find("\"ts\":"), std::string::npos);
    EXPECT_EQ(out.back(), '\n');
    // Exactly one line.
    EXPECT_EQ(std::count(out.begin(), out.end(), '\n'), 1);
}

TEST(Log, DropsLinesBelowMinLevel) {
    const std::string out = capture(katana::log::level::warn, [] {
        katana::log::debug("d");
        katana::log::info("i");
        katana::log::warn("w");
        katana::log::error("e");
    });
    EXPECT_EQ(out.find("\"msg\":\"d\""), std::string::npos);
    EXPECT_EQ(out.find("\"msg\":\"i\""), std::string::npos);
    EXPECT_NE(out.find("\"msg\":\"w\""), std::string::npos);
    EXPECT_NE(out.find("\"msg\":\"e\""), std::string::npos);
}

TEST(Log, SerializesTypedFields) {
    const std::string out = capture(katana::log::level::info, [] {
        katana::log::info("req")
            .field("method", "GET")
            .field("status", 200)
            .field("count", static_cast<int64_t>(-7))
            .field("ok", true)
            .field("cached", false);
    });
    EXPECT_NE(out.find("\"method\":\"GET\""), std::string::npos);
    EXPECT_NE(out.find("\"status\":200"), std::string::npos);
    EXPECT_NE(out.find("\"count\":-7"), std::string::npos);
    EXPECT_NE(out.find("\"ok\":true"), std::string::npos);
    EXPECT_NE(out.find("\"cached\":false"), std::string::npos);
}

TEST(Log, EscapesMessageAndStringFields) {
    const std::string out = capture(katana::log::level::info, [] {
        katana::log::info("a\"b\nc").field("path", "/x\t\"y\"");
    });
    // Quotes, newline and tab must be escaped so each record stays one valid JSON line.
    EXPECT_NE(out.find("a\\\"b\\nc"), std::string::npos);
    EXPECT_NE(out.find("/x\\t\\\"y\\\""), std::string::npos);
    EXPECT_EQ(std::count(out.begin(), out.end(), '\n'), 1); // the embedded newline was escaped
}

} // namespace
