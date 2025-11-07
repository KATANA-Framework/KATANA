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
    EXPECT_EQ(req.http_method, method::GET);
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
    EXPECT_EQ(req.http_method, method::POST);
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
        {"GET", method::GET},
        {"POST", method::POST},
        {"PUT", method::PUT},
        {"DELETE", method::DELETE},
        {"PATCH", method::PATCH},
        {"HEAD", method::HEAD},
        {"OPTIONS", method::OPTIONS},
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
    EXPECT_EQ(req.http_method, method::GET);
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
    EXPECT_EQ(parse_method("GET"), method::GET);
    EXPECT_EQ(parse_method("POST"), method::POST);
    EXPECT_EQ(parse_method("PUT"), method::PUT);
    EXPECT_EQ(parse_method("DELETE"), method::DELETE);
    EXPECT_EQ(parse_method("PATCH"), method::PATCH);
    EXPECT_EQ(parse_method("HEAD"), method::HEAD);
    EXPECT_EQ(parse_method("OPTIONS"), method::OPTIONS);
    EXPECT_EQ(parse_method("INVALID"), method::UNKNOWN);
}

TEST(HttpMethod, MethodToString) {
    EXPECT_EQ(method_to_string(method::GET), "GET");
    EXPECT_EQ(method_to_string(method::POST), "POST");
    EXPECT_EQ(method_to_string(method::PUT), "PUT");
    EXPECT_EQ(method_to_string(method::DELETE), "DELETE");
    EXPECT_EQ(method_to_string(method::PATCH), "PATCH");
    EXPECT_EQ(method_to_string(method::HEAD), "HEAD");
    EXPECT_EQ(method_to_string(method::OPTIONS), "OPTIONS");
    EXPECT_EQ(method_to_string(method::UNKNOWN), "UNKNOWN");
}
