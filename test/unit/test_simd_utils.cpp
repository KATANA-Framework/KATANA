#include "katana/core/simd_utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <string>

using namespace katana::simd;

TEST_CASE("find_crlf basic tests", "[simd_utils]") {
    SECTION("find single CRLF") {
        const char* data = "hello\r\nworld";
        const char* result = find_crlf(data, std::strlen(data));
        REQUIRE(result != nullptr);
        REQUIRE(result == data + 5);
    }

    SECTION("find CRLF at start") {
        const char* data = "\r\nhello";
        const char* result = find_crlf(data, std::strlen(data));
        REQUIRE(result != nullptr);
        REQUIRE(result == data);
    }

    SECTION("find CRLF at end") {
        const char* data = "hello\r\n";
        const char* result = find_crlf(data, std::strlen(data));
        REQUIRE(result != nullptr);
        REQUIRE(result == data + 5);
    }

    SECTION("no CRLF found") {
        const char* data = "hello world";
        const char* result = find_crlf(data, std::strlen(data));
        REQUIRE(result == nullptr);
    }

    SECTION("only CR") {
        const char* data = "hello\rworld";
        const char* result = find_crlf(data, std::strlen(data));
        REQUIRE(result == nullptr);
    }

    SECTION("only LF") {
        const char* data = "hello\nworld";
        const char* result = find_crlf(data, std::strlen(data));
        REQUIRE(result == nullptr);
    }
}

TEST_CASE("find_crlf with long strings", "[simd_utils]") {
    SECTION("large buffer with CRLF in middle") {
        std::string data(500, 'A');
        data += "\r\n";
        data += std::string(500, 'B');

        const char* result = find_crlf(data.data(), data.size());
        REQUIRE(result != nullptr);
        REQUIRE(result == data.data() + 500);
    }

    SECTION("multiple CRLFs") {
        const char* data = "line1\r\nline2\r\nline3\r\n";
        const char* result = find_crlf(data, std::strlen(data));
        REQUIRE(result != nullptr);
        REQUIRE(result == data + 5);
    }
}

TEST_CASE("find_crlf performance comparison", "[simd_utils]") {
    std::string large_data(10000, 'X');
    large_data += "\r\n";

    const char* scalar_result = find_crlf_scalar(large_data.data(), large_data.size());
    const char* simd_result = find_crlf(large_data.data(), large_data.size());

    REQUIRE(scalar_result != nullptr);
    REQUIRE(simd_result != nullptr);
    REQUIRE(scalar_result == simd_result);
}

TEST_CASE("find_pattern tests", "[simd_utils]") {
    SECTION("find simple pattern") {
        const char* haystack = "hello world hello";
        const char* needle = "world";

        const void* result =
            find_pattern(haystack, std::strlen(haystack), needle, std::strlen(needle));
        REQUIRE(result != nullptr);
        REQUIRE(result == haystack + 6);
    }

    SECTION("pattern not found") {
        const char* haystack = "hello world";
        const char* needle = "xyz";

        const void* result =
            find_pattern(haystack, std::strlen(haystack), needle, std::strlen(needle));
        REQUIRE(result == nullptr);
    }

    SECTION("empty needle") {
        const char* haystack = "hello";
        const void* result = find_pattern(haystack, 5, "", 0);
        REQUIRE(result == nullptr);
    }
}
