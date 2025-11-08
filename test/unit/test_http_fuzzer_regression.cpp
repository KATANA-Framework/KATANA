#include "katana/core/http.hpp"

#include <gtest/gtest.h>
#include <vector>
#include <string>

using namespace katana::http;

TEST(HttpFuzzerRegression, EmptyInput) {
    parser p;
    std::vector<uint8_t> empty;
    auto result = p.parse(std::span<const uint8_t>(empty.data(), empty.size()));
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::request_line);
}

TEST(HttpFuzzerRegression, SingleByte) {
    parser p;
    std::vector<uint8_t> data = {'G'};
    auto result = p.parse(std::span<const uint8_t>(data));
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::request_line);
}

TEST(HttpFuzzerRegression, IncompleteRequestLine) {
    parser p;
    std::string incomplete = "GET ";
    auto data = as_bytes(incomplete);
    auto result = p.parse(data);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::request_line);
}

TEST(HttpFuzzerRegression, OnlyCRLF) {
    parser p;
    std::string crlf = "\r\n";
    auto data = as_bytes(crlf);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, RepeatedCRLF) {
    parser p;
    std::string repeated = "\r\n\r\n\r\n";
    auto data = as_bytes(repeated);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, NullBytes) {
    parser p;
    std::vector<uint8_t> null_data = {0, 0, 0, 0};
    auto result = p.parse(std::span<const uint8_t>(null_data));
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, HighBitCharacters) {
    parser p;
    std::vector<uint8_t> high_bits = {0xFF, 0xFE, 0xFD, 0xFC};
    auto result = p.parse(std::span<const uint8_t>(high_bits));
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, VeryLongMethod) {
    parser p;
    std::string long_method(10000, 'A');
    long_method += " / HTTP/1.1\r\n\r\n";
    auto data = as_bytes(long_method);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, MissingSpaceBetweenMethodAndURI) {
    parser p;
    std::string no_space = "GET/path HTTP/1.1\r\n\r\n";
    auto data = as_bytes(no_space);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, TabInsteadOfSpace) {
    parser p;
    std::string tab_request = "GET\t/path\tHTTP/1.1\r\n\r\n";
    auto data = as_bytes(tab_request);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, OnlyLFNoCarriageReturn) {
    parser p;
    std::string lf_only = "GET / HTTP/1.1\nHost: example.com\n\n";
    auto data = as_bytes(lf_only);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, MixedLineEndings) {
    parser p;
    std::string mixed = "GET / HTTP/1.1\r\nHost: example.com\n\r\n";
    auto data = as_bytes(mixed);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, HeaderWithNoValue) {
    parser p;
    std::string no_value = "GET / HTTP/1.1\r\nHost:\r\n\r\n";
    auto data = as_bytes(no_value);
    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
}

TEST(HttpFuzzerRegression, ColonInHeaderValue) {
    parser p;
    std::string colon_value = "GET / HTTP/1.1\r\nX-Header: value:with:colons\r\n\r\n";
    auto data = as_bytes(colon_value);
    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
    EXPECT_EQ(p.get_request().header("X-Header").value_or(""), "value:with:colons");
}

TEST(HttpFuzzerRegression, DuplicateHeaders) {
    parser p;
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
    parser p;
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
    parser p;
    std::string negative = "POST / HTTP/1.1\r\n"
                          "Content-Length: -1\r\n"
                          "\r\n";
    auto data = as_bytes(negative);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, HugeContentLength) {
    parser p;
    std::string huge = "POST / HTTP/1.1\r\n"
                      "Content-Length: 999999999999999\r\n"
                      "\r\n";
    auto data = as_bytes(huge);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, ChunkedEncodingInvalidSize) {
    parser p;
    std::string invalid_chunk = "POST / HTTP/1.1\r\n"
                               "Transfer-Encoding: chunked\r\n"
                               "\r\n"
                               "xyz\r\n";
    auto data = as_bytes(invalid_chunk);
    auto result = p.parse(data);
    EXPECT_TRUE(result.has_value());
}

TEST(HttpFuzzerRegression, ChunkedEncodingNegativeSize) {
    parser p;
    std::string negative_chunk = "POST / HTTP/1.1\r\n"
                                "Transfer-Encoding: chunked\r\n"
                                "\r\n"
                                "-5\r\n";
    auto data = as_bytes(negative_chunk);
    auto result = p.parse(data);
    EXPECT_TRUE(result.has_value());
}

TEST(HttpFuzzerRegression, URIWithNullByte) {
    parser p;
    std::vector<uint8_t> uri_with_null = {'G', 'E', 'T', ' ', '/', 0, ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n', '\r', '\n'};
    auto result = p.parse(std::span<const uint8_t>(uri_with_null));
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, HeaderNameWithNullByte) {
    parser p;
    std::vector<uint8_t> header_with_null = {'G', 'E', 'T', ' ', '/', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\r', '\n',
                                             'X', 0, 'H', ':', ' ', 'v', 'a', 'l', '\r', '\n', '\r', '\n'};
    auto result = p.parse(std::span<const uint8_t>(header_with_null));
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, TrailingSpacesInRequestLine) {
    parser p;
    std::string trailing = "GET / HTTP/1.1    \r\n\r\n";
    auto data = as_bytes(trailing);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, LeadingSpacesInRequestLine) {
    parser p;
    std::string leading = "    GET / HTTP/1.1\r\n\r\n";
    auto data = as_bytes(leading);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpFuzzerRegression, InvalidHTTPVersion) {
    parser p;
    std::string invalid_version = "GET / HTTP/99.99\r\n\r\n";
    auto data = as_bytes(invalid_version);
    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
    EXPECT_EQ(p.get_request().version, "HTTP/99.99");
}

TEST(HttpFuzzerRegression, MalformedHTTPVersion) {
    parser p;
    std::string malformed_version = "GET / HTTX/1.1\r\n\r\n";
    auto data = as_bytes(malformed_version);
    auto result = p.parse(data);
    EXPECT_TRUE(result.has_value());
}

TEST(HttpFuzzerRegression, CompleteRequestInMultipleChunks) {
    parser p;

    std::vector<std::string> chunks = {
        "G",
        "E",
        "T ",
        "/ ",
        "H",
        "TTP/1.1\r",
        "\n",
        "Host: ",
        "example.com",
        "\r\n",
        "\r",
        "\n"
    };

    for (const auto& chunk : chunks) {
        auto data = as_bytes(chunk);
        auto result = p.parse(data);
        ASSERT_TRUE(result.has_value());
    }

    EXPECT_TRUE(p.is_complete());
}

TEST(HttpFuzzerRegression, ZeroLengthChunk) {
    parser p;
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
