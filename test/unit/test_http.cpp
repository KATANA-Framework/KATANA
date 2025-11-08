#include "katana/core/http.hpp"

#include <gtest/gtest.h>

using namespace katana::http;

TEST(HttpParser, ParseSimpleGetRequest) {
    parser p;

    std::string request = "GET /index.html HTTP/1.1\r\nHost: example.com\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, parser::state::complete);

    const auto& req = p.get_request();
    EXPECT_EQ(req.http_method, method::get);
    EXPECT_EQ(req.uri, "/index.html");
    EXPECT_EQ(req.version, "HTTP/1.1");
    EXPECT_EQ(req.headers.size(), 1);
    EXPECT_EQ(req.header("Host").value_or(""), "example.com");
    EXPECT_TRUE(req.body.empty());
}

TEST(HttpParser, ParsePostRequestWithBody) {
    parser p;

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
        parser p;
        std::string request = tc.method_str + " / HTTP/1.1\r\nHost: example.com\r\n\r\n";
        auto data = as_bytes(request);

        auto result = p.parse(data);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(p.get_request().http_method, tc.expected_method);
    }
}

TEST(HttpParser, ParseMultipleHeaders) {
    parser p;

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
    parser p;

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
    parser p;

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
    parser p;

    std::string request = "GETHTTP/1.1\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, InvalidRequestLineMissingVersion) {
    parser p;

    std::string request = "GET /\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, InvalidHeaderNoColon) {
    parser p;

    std::string request = "GET / HTTP/1.1\r\nInvalidHeader\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, InvalidContentLength) {
    parser p;

    std::string request = "POST / HTTP/1.1\r\nContent-Length: invalid\r\n\r\n";
    auto data = as_bytes(request);

    auto result = p.parse(data);
    EXPECT_FALSE(result.has_value());
}

TEST(HttpParser, HeaderValueWithLeadingSpaces) {
    parser p;

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
    parser p;

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
    parser p;

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
    parser p;

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
    parser p;

    std::string request = "GET / HTTP/1.1\r\n"
                         " invalid-folding\r\n"
                         "Host: example.com\r\n"
                         "\r\n";

    auto data = as_bytes(request);
    auto result = p.parse(data);

    EXPECT_FALSE(result.has_value());
}
