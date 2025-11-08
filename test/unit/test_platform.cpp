#include "katana/core/platform.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstring>

using namespace katana::platform;

TEST_CASE("platform find_pattern tests", "[platform]") {
    SECTION("find CRLF") {
        const char* data = "GET / HTTP/1.1\r\nHost: example.com\r\n";
        const void* result = find_pattern(data, std::strlen(data), "\r\n", 2);
        REQUIRE(result != nullptr);
        REQUIRE(static_cast<const char*>(result) == data + 14);
    }

    SECTION("find at beginning") {
        const char* data = "test find me";
        const void* result = find_pattern(data, std::strlen(data), "test", 4);
        REQUIRE(result != nullptr);
        REQUIRE(result == data);
    }

    SECTION("find at end") {
        const char* data = "find at end!";
        const void* result = find_pattern(data, std::strlen(data), "end!", 4);
        REQUIRE(result != nullptr);
        REQUIRE(static_cast<const char*>(result) == data + 8);
    }

    SECTION("pattern not found") {
        const char* data = "hello world";
        const void* result = find_pattern(data, std::strlen(data), "xyz", 3);
        REQUIRE(result == nullptr);
    }

    SECTION("haystack too small") {
        const char* data = "ab";
        const void* result = find_pattern(data, 2, "abc", 3);
        REQUIRE(result == nullptr);
    }

    SECTION("single character pattern") {
        const char* data = "hello world";
        const void* result = find_pattern(data, std::strlen(data), "w", 1);
        REQUIRE(result != nullptr);
        REQUIRE(static_cast<const char*>(result) == data + 6);
    }
}
