#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"

#include <gtest/gtest.h>

using namespace katana;
using namespace katana::http;
using katana::monotonic_arena;

TEST(HttpParser, ParseSimpleGetRequest) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_EQ(req.http_method, method::get);
    EXPECT_EQ(req.uri, "/index.html");
    EXPECT_EQ(req.headers.size(), 1);
    EXPECT_EQ(req.header("Host").value_or(""), "example.com");
    EXPECT_TRUE(req.body.empty());
}

TEST(HttpParser, ParsePostRequestWithBody) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "POST /api/data HTTP/1.1\r\n"
                          "Host: api.example.com\r\n"
                          "Content-Type: application/json\r\n"
                          "Content-Length: 13\r\n"
                          "\r\n"
                          "{\"key\":\"val\"}";

    auto data = as_bytes(request);

    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_EQ(req.http_method, method::post);
    EXPECT_EQ(req.uri, "/api/data");
    EXPECT_EQ(req.body, "{\"key\":\"val\"}");
    EXPECT_EQ(req.header("Content-Type").value_or(""), "application/json");
}

TEST(HttpParser, ParseAllMethods) {
    struct test_case {
        std::string method_str;
        method expected_method;
    };

    std::vector<test_case> cases = {
        {"GET", method::get},
        {"POST", method::post},
        {"PUT", method::put},
        {"DELETE", method::del},
        {"PATCH", method::patch},
        {"HEAD", method::head},
        {"OPTIONS", method::options},
    };

    for (const auto& tc : cases) {
        monotonic_arena arena;
        parser p(&arena);
        std::string request = tc.method_str + " / HTTP/1.1\r\nHost: example.com\r\n\r\n";
        auto data = as_bytes(request);

        auto result = p.parse(data);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(p.get_request().http_method, tc.expected_method);
    }
}

TEST(HttpParser, ParseMultipleHeaders) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "User-Agent: TestClient/1.0\r\n"
                          "Accept: */*\r\n"
                          "Connection: keep-alive\r\n"
                          "\r\n";

    auto data = as_bytes(request);

    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_EQ(req.headers.size(), 4);
    EXPECT_EQ(req.header("Host").value_or(""), "example.com");
    EXPECT_EQ(req.header("User-Agent").value_or(""), "TestClient/1.0");
    EXPECT_EQ(req.header("Accept").value_or(""), "*/*");
    EXPECT_EQ(req.header("Connection").value_or(""), "keep-alive");
}

TEST(HttpParser, ParseIncrementalData) {
    monotonic_arena arena;
    parser p(&arena);

    std::string part1 = "GET /test HTTP/1.1\r\n";
    std::string part2 = "Host: example.com\r\n";
    std::string part3 = "\r\n";

    auto data1 = as_bytes(part1);
    auto result1 = p.parse(data1);
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(*result1, parser::state::headers);

    auto data2 = as_bytes(part2);
    auto result2 = p.parse(data2);
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(*result2, parser::state::headers);

    auto data3 = as_bytes(part3);
    auto result3 = p.parse(data3);
    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(*result3, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_EQ(req.http_method, method::get);
    EXPECT_EQ(req.uri, "/test");
}

TEST(HttpParser, ParseIncrementalBody) {
    monotonic_arena arena;
    parser p(&arena);

    std::string headers = "POST / HTTP/1.1\r\nHost: example.com\r\nContent-Length: 10\r\n\r\n";
    std::string body_part1 = "hello";
    std::string body_part2 = "world";

    auto data1 = as_bytes(headers);
    auto result1 = p.parse(data1);
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(*result1, parser::state::body);

    auto data2 = as_bytes(body_part1);
    auto result2 = p.parse(data2);
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(*result2, parser::state::body);

    auto data3 = as_bytes(body_part2);
    auto result3 = p.parse(data3);
    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(*result3, parser::state::complete);

    EXPECT_EQ(p.get_request().body, "helloworld");
}

TEST(HttpParser, InvalidRequestLineNoSpace) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GETHTTP/1.1\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, RejectUnknownMethod) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "TRACE / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, InvalidRequestLineMissingVersion) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET /\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, RejectInvalidHttpVersion) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, InvalidHeaderNoColon) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\nInvalidHeader\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, InvalidContentLength) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "POST / HTTP/1.1\r\nContent-Length: invalid\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, RejectHeaderWithIllegalTokenCharacters) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\nBad Header: value\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, RejectHeaderValueControlCharacters) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request =
        std::string("GET / HTTP/1.1\r\nHeader: value") + std::string(1, '\x01') + "\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, HeaderValueWithLeadingSpaces) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\nHost:   example.com\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
    EXPECT_EQ(p.get_request().header("Host").value_or(""), "example.com");
}

TEST(HttpResponse, SerializeOk) {
    auto resp = response::ok("Hello, World!", "text/plain");
    std::string serialized = resp.serialize();

    EXPECT_TRUE(serialized.find("HTTP/1.1 200 OK") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Type: text/plain") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Length: 13") != std::string::npos);
    EXPECT_TRUE(serialized.find("Hello, World!") != std::string::npos);
}

TEST(HttpResponse, SerializeJson) {
    auto resp = response::json("{\"status\":\"ok\"}");
    std::string serialized = resp.serialize();

    EXPECT_TRUE(serialized.find("Content-Type: application/json") != std::string::npos);
    EXPECT_TRUE(serialized.find("{\"status\":\"ok\"}") != std::string::npos);
}

TEST(HttpResponse, SerializeError) {
    katana::problem_details problem;
    problem.status = 404;
    problem.title = "Not Found";
    problem.detail = "The requested resource was not found";

    auto resp = response::error(problem);
    std::string serialized = resp.serialize();

    EXPECT_TRUE(serialized.find("HTTP/1.1 404 Not Found") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Type: application/problem+json") != std::string::npos);
}

TEST(HttpResponse, CustomHeaders) {
    response resp;
    resp.status = 200;
    resp.reason = "OK";
    resp.set_header("X-Custom-Header", "custom-value");
    resp.set_header("X-Request-ID", "12345");

    std::string serialized = resp.serialize();

    EXPECT_TRUE(serialized.find("X-Custom-Header: custom-value") != std::string::npos);
    EXPECT_TRUE(serialized.find("X-Request-ID: 12345") != std::string::npos);
}

TEST(HttpMethod, ParseMethod) {
    EXPECT_EQ(parse_method("GET"), method::get);
    EXPECT_EQ(parse_method("POST"), method::post);
    EXPECT_EQ(parse_method("PUT"), method::put);
    EXPECT_EQ(parse_method("DELETE"), method::del);
    EXPECT_EQ(parse_method("PATCH"), method::patch);
    EXPECT_EQ(parse_method("HEAD"), method::head);
    EXPECT_EQ(parse_method("OPTIONS"), method::options);
    EXPECT_EQ(parse_method("INVALID"), method::unknown);
}

TEST(HttpMethod, MethodToString) {
    EXPECT_EQ(method_to_string(method::get), "GET");
    EXPECT_EQ(method_to_string(method::post), "POST");
    EXPECT_EQ(method_to_string(method::put), "PUT");
    EXPECT_EQ(method_to_string(method::del), "DELETE");
    EXPECT_EQ(method_to_string(method::patch), "PATCH");
    EXPECT_EQ(method_to_string(method::head), "HEAD");
    EXPECT_EQ(method_to_string(method::options), "OPTIONS");
    EXPECT_EQ(method_to_string(method::unknown), "UNKNOWN");
}

TEST(HttpParser, ParseMultilineHeaderFoldingSpace) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "X-Custom-Header: value-line1\r\n"
                          " value-line2\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_EQ(req.header("X-Custom-Header").value_or(""), "value-line1 value-line2");
}

TEST(HttpParser, ParseMultilineHeaderFoldingTab) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "X-Long-Header: first-part\r\n"
                          "\tsecond-part\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_EQ(req.header("X-Long-Header").value_or(""), "first-part second-part");
}

TEST(HttpParser, ParseMultilineHeaderMultipleFolds) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "X-Very-Long-Header: part1\r\n"
                          " part2\r\n"
                          "\tpart3\r\n"
                          "  part4\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    auto value = req.header("X-Very-Long-Header").value_or("");
    EXPECT_TRUE(value.find("part1") != std::string::npos);
    EXPECT_TRUE(value.find("part2") != std::string::npos);
    EXPECT_TRUE(value.find("part3") != std::string::npos);
    EXPECT_TRUE(value.find("part4") != std::string::npos);
}

TEST(HttpParser, RejectFoldingWithoutPriorHeader) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          " invalid-folding\r\n"
                          "Host: example.com\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, ChunkedEncodingSimple) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "POST /data HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "5\r\n"
                          "hello\r\n"
                          "6\r\n"
                          "world!\r\n"
                          "0\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_EQ(req.body, "helloworld!");
}

TEST(HttpParser, ChunkedEncodingIncremental) {
    monotonic_arena arena;
    parser p(&arena);

    std::string part1 =
        "POST /data HTTP/1.1\r\nHost: example.com\r\nTransfer-Encoding: chunked\r\n\r\n";
    std::string part2 = "3\r\nfoo\r\n";
    std::string part3 = "3\r\nbar\r\n";
    std::string part4 = "0\r\n\r\n";

    auto data1 = as_bytes(part1);
    auto result1 = p.parse(data1);
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(*result1, parser::state::chunk_size);

    auto data2 = as_bytes(part2);
    auto result2 = p.parse(data2);
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(*result2, parser::state::chunk_size);

    auto data3 = as_bytes(part3);
    auto result3 = p.parse(data3);
    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(*result3, parser::state::chunk_size);

    auto data4 = as_bytes(part4);
    auto result4 = p.parse(data4);
    ASSERT_TRUE(result4.has_value());
    EXPECT_EQ(*result4, parser::state::complete);

    EXPECT_EQ(p.get_request().body, "foobar");
}

TEST(HttpParser, RejectChunkWithoutTrailingCrlf) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "POST /data HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "5\r\n"
                          "hello"
                          "0\r\n\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, ChunkedEncodingWithTrailer) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "POST /data HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n"
                          "4\r\n"
                          "test\r\n"
                          "0\r\n"
                          "X-Trailer: value\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
    EXPECT_EQ(p.get_request().body, "test");
}

TEST(HttpParser, ExcessiveHeaderCount) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n";
    for (int i = 0; i < 150; ++i) {
        request += "X-Header-" + std::to_string(i) + ": value\r\n";
    }
    request += "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, ExcessivelyLongURI) {
    monotonic_arena arena;
    parser p(&arena);

    std::string long_uri(10000, 'a');
    std::string request = "GET /" + long_uri +
                          " HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, ExcessivelyLongHeader) {
    monotonic_arena arena;
    parser p(&arena);

    std::string long_value(100000, 'x');
    std::string request = "GET / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "X-Long-Header: " +
                          long_value +
                          "\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, MalformedRequestLineNoHTTP) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET /path\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, MalformedRequestLineInvalidMethod) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "INVALID@METHOD /path HTTP/1.1\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, MalformedRequestLineExtraSpaces) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET  /path  HTTP/1.1\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, MalformedHeaderMissingColon) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          "InvalidHeaderNoColon\r\n"
                          "\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, MalformedHeaderInvalidCharacters) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          "X-Header\x01\x02: value\r\n"
                          "\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, RecoveryAfterError) {
    monotonic_arena arena;
    parser p(&arena);

    std::string bad_request = "INVALID\r\n\r\n";
    auto bad_data = as_bytes(bad_request);
    auto bad_result = p.parse(bad_data);
    EXPECT_FALSE(bad_result.has_value());

    arena.reset();
    p = parser(&arena);

    std::string good_request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto good_data = as_bytes(good_request);
    auto good_result = p.parse(good_data);
    ASSERT_TRUE(good_result.has_value());
    EXPECT_EQ(*good_result, parser::state::complete);
}

TEST(HttpParser, ContentLengthZero) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "POST / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "Content-Length: 0\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
    EXPECT_TRUE(p.get_request().body.empty());
}

TEST(HttpParser, LargeValidContentLength) {
    monotonic_arena arena;
    parser p(&arena);

    std::string body(1024 * 1024, 'x');
    std::string request = "POST / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "Content-Length: " +
                          std::to_string(body.size()) +
                          "\r\n"
                          "\r\n" +
                          body;

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);
    EXPECT_EQ(p.get_request().body.size(), 1024 * 1024);
}

TEST(HttpParser, CaseInsensitiveHeaders) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          "CoNtEnT-tYpE: text/plain\r\n"
                          "HoSt: example.com\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_TRUE(req.header("content-type").has_value());
    EXPECT_TRUE(req.header("HOST").has_value());
    EXPECT_TRUE(req.header("Content-Type").has_value());
}

TEST(HttpParser, EmptyHeaderValue) {
    monotonic_arena arena;
    parser p(&arena);

    std::string request = "GET / HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "X-Empty-Header:\r\n"
                          "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    auto empty_header = req.header("X-Empty-Header");
    EXPECT_TRUE(empty_header.has_value());
    EXPECT_TRUE(empty_header->empty());
}
