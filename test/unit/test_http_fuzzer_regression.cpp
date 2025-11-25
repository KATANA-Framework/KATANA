#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace katana;
using namespace katana::http;
using katana::monotonic_arena;

TEST(HttpFuzzerRegression, EmptyInput) {
    monotonic_arena arena;
    parser p(&arena);
    std::vector<uint8_t> empty;
    auto result = p.parse(std::span<const uint8_t>(empty.data(), empty.size()));
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::request_line);
}

TEST(HttpFuzzerRegression, SingleByte) {
    monotonic_arena arena;
    parser p(&arena);
    std::vector<uint8_t> data = {'G'};
    auto result = p.parse(std::span<const uint8_t>(data));
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::request_line);
}

TEST(HttpFuzzerRegression, IncompleteRequestLine) {
    monotonic_arena arena;
    parser p(&arena);
    std::string incomplete = "GET ";
    auto data = as_bytes(incomplete);
    auto result = p.parse(data);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::request_line);
}

TEST(HttpFuzzerRegression, OnlyCRLF) {
    monotonic_arena arena;
    parser p(&arena);
    std::string crlf = "\r\n";
    auto data = as_bytes(crlf);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, RepeatedCRLF) {
    monotonic_arena arena;
    parser p(&arena);
    std::string repeated = "\r\n\r\n\r\n";
    auto data = as_bytes(repeated);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, NullBytes) {
    monotonic_arena arena;
    parser p(&arena);
    std::vector<uint8_t> null_data = {0, 0, 0, 0};
    auto result = p.parse(std::span<const uint8_t>(null_data));
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, HighBitCharacters) {
    monotonic_arena arena;
    parser p(&arena);
    std::vector<uint8_t> high_bits = {0xFF, 0xFE, 0xFD, 0xFC};
    auto result = p.parse(std::span<const uint8_t>(high_bits));
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, VeryLongMethod) {
    monotonic_arena arena;
    parser p(&arena);
    std::string long_method(10000, 'A');
    long_method += " / HTTP/1.1\r\n\r\n";
    auto data = as_bytes(long_method);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, MissingSpaceBetweenMethodAndURI) {
    monotonic_arena arena;
    parser p(&arena);
    std::string no_space = "GET/path HTTP/1.1\r\n\r\n";
    auto data = as_bytes(no_space);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, TabInsteadOfSpace) {
    monotonic_arena arena;
    parser p(&arena);
    std::string tab_request = "GET\t/path\tHTTP/1.1\r\n\r\n";
    auto data = as_bytes(tab_request);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, OnlyLFNoCarriageReturn) {
    monotonic_arena arena;
    parser p(&arena);
    std::string lf_only = "GET / HTTP/1.1\nHost: example.com\n\n";
    auto data = as_bytes(lf_only);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, MixedLineEndings) {
    monotonic_arena arena;
    parser p(&arena);
    std::string mixed = "GET / HTTP/1.1\r\nHost: example.com\n\r\n";
    auto data = as_bytes(mixed);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, HeaderWithNoValue) {
    monotonic_arena arena;
    parser p(&arena);
    std::string no_value = "GET / HTTP/1.1\r\nHost:\r\n\r\n";
    auto data = as_bytes(no_value);
    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
}

TEST(HttpFuzzerRegression, ColonInHeaderValue) {
    monotonic_arena arena;
    parser p(&arena);
    std::string colon_value = "GET / HTTP/1.1\r\nX-Header: value:with:colons\r\n\r\n";
    auto data = as_bytes(colon_value);
    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
    EXPECT_EQ(p.get_request().header("X-Header").value_or(""), "value:with:colons");
}

TEST(HttpFuzzerRegression, DuplicateHeaders) {
    monotonic_arena arena;
    parser p(&arena);
    std::string duplicate = "GET / HTTP/1.1\r\n"
                            "Host: first.com\r\n"
                            "Host: second.com\r\n"
                            "\r\n";
    auto data = as_bytes(duplicate);
    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
}

TEST(HttpFuzzerRegression, ContentLengthMismatch) {
    monotonic_arena arena;
    parser p(&arena);
    std::string mismatch = "POST / HTTP/1.1\r\n"
                           "Content-Length: 100\r\n"
                           "\r\n"
                           "short";
    auto data = as_bytes(mismatch);
    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::body);
}

TEST(HttpFuzzerRegression, NegativeContentLength) {
    monotonic_arena arena;
    parser p(&arena);
    std::string negative = "POST / HTTP/1.1\r\n"
                           "Content-Length: -1\r\n"
                           "\r\n";
    auto data = as_bytes(negative);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, HugeContentLength) {
    monotonic_arena arena;
    parser p(&arena);
    std::string huge = "POST / HTTP/1.1\r\n"
                       "Content-Length: 999999999999999\r\n"
                       "\r\n";
    auto data = as_bytes(huge);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, ChunkedEncodingInvalidSize) {
    monotonic_arena arena;
    parser p(&arena);
    std::string invalid_chunk = "POST / HTTP/1.1\r\n"
                                "Transfer-Encoding: chunked\r\n"
                                "\r\n"
                                "xyz\r\n";
    auto data = as_bytes(invalid_chunk);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, ChunkedEncodingNegativeSize) {
    monotonic_arena arena;
    parser p(&arena);
    std::string negative_chunk = "POST / HTTP/1.1\r\n"
                                 "Transfer-Encoding: chunked\r\n"
                                 "\r\n"
                                 "-5\r\n";
    auto data = as_bytes(negative_chunk);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, URIWithNullByte) {
    monotonic_arena arena;
    parser p(&arena);
    std::vector<uint8_t> uri_with_null = {'G',
                                          'E',
                                          'T',
                                          ' ',
                                          '/',
                                          0,
                                          ' ',
                                          'H',
                                          'T',
                                          'T',
                                          'P',
                                          '/',
                                          '1',
                                          '.',
                                          '1',
                                          '\r',
                                          '\n',
                                          '\r',
                                          '\n'};
    auto result = p.parse(std::span<const uint8_t>(uri_with_null));
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, HeaderNameWithNullByte) {
    monotonic_arena arena;
    parser p(&arena);
    std::vector<uint8_t> header_with_null = {'G', 'E', 'T', ' ', '/',  ' ',  'H',  'T', 'T', 'P',
                                             '/', '1', '.', '1', '\r', '\n', 'X',  0,   'H', ':',
                                             ' ', 'v', 'a', 'l', '\r', '\n', '\r', '\n'};
    auto result = p.parse(std::span<const uint8_t>(header_with_null));
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, TrailingSpacesInRequestLine) {
    monotonic_arena arena;
    parser p(&arena);
    std::string trailing = "GET / HTTP/1.1    \r\n\r\n";
    auto data = as_bytes(trailing);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, LeadingSpacesInRequestLine) {
    monotonic_arena arena;
    parser p(&arena);
    std::string leading = "    GET / HTTP/1.1\r\n\r\n";
    auto data = as_bytes(leading);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, InvalidHTTPVersion) {
    monotonic_arena arena;
    parser p(&arena);
    std::string invalid_version = "GET / HTTP/99.99\r\n\r\n";
    auto data = as_bytes(invalid_version);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, MalformedHTTPVersion) {
    monotonic_arena arena;
    parser p(&arena);
    std::string malformed_version = "GET / HTTX/1.1\r\n\r\n";
    auto data = as_bytes(malformed_version);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, CompleteRequestInMultipleChunks) {
    monotonic_arena arena;
    parser p(&arena);

    std::vector<std::string> chunks = {
        "G", "E", "T ", "/ ", "H", "TTP/1.1\r", "\n", "Host: ", "example.com", "\r\n", "\r", "\n"};

    for (const auto& chunk : chunks) {
        auto data = as_bytes(chunk);
        auto result = p.parse(data);
        ASSERT_TRUE(result.has_value());
    }

    EXPECT_TRUE(p.is_complete());
}

TEST(HttpFuzzerRegression, ZeroLengthChunk) {
    monotonic_arena arena;
    parser p(&arena);
    std::string zero_chunk = "POST / HTTP/1.1\r\n"
                             "Transfer-Encoding: chunked\r\n"
                             "\r\n"
                             "0\r\n"
                             "\r\n";
    auto data = as_bytes(zero_chunk);
    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
    EXPECT_TRUE(p.get_request().body.empty());
}
