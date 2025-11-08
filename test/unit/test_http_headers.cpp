#include "katana/core/http_headers.hpp"

#include <gtest/gtest.h>

using namespace katana::http;

TEST(HttpHeaders, SetAndGet) {
    headers_map headers;
    headers.set("Content-Type", "application/json");
    headers.set("Content-Length", "42");

    EXPECT_EQ(headers.get("Content-Type").value_or(""), "application/json");
    EXPECT_EQ(headers.get("Content-Length").value_or(""), "42");
}

TEST(HttpHeaders, CaseInsensitiveGet) {
    headers_map headers;
    headers.set("Content-Type", "text/html");

    EXPECT_EQ(headers.get("content-type").value_or(""), "text/html");
    EXPECT_EQ(headers.get("CONTENT-TYPE").value_or(""), "text/html");
    EXPECT_EQ(headers.get("CoNtEnT-TyPe").value_or(""), "text/html");
}

TEST(HttpHeaders, UpdateExisting) {
    headers_map headers;
    headers.set("Host", "example.com");
    headers.set("Host", "newhost.com");

    EXPECT_EQ(headers.size(), 1);
    EXPECT_EQ(headers.get("Host").value_or(""), "newhost.com");
}

TEST(HttpHeaders, Contains) {
    headers_map headers;
    headers.set("Authorization", "Bearer token");

    EXPECT_TRUE(headers.contains("Authorization"));
    EXPECT_TRUE(headers.contains("authorization"));
    EXPECT_FALSE(headers.contains("Content-Type"));
}

TEST(HttpHeaders, Remove) {
    headers_map headers;
    headers.set("X-Custom-1", "value1");
    headers.set("X-Custom-2", "value2");
    headers.set("X-Custom-3", "value3");

    EXPECT_EQ(headers.size(), 3);

    headers.remove("X-Custom-2");
    EXPECT_EQ(headers.size(), 2);
    EXPECT_FALSE(headers.contains("X-Custom-2"));
    EXPECT_TRUE(headers.contains("X-Custom-1"));
    EXPECT_TRUE(headers.contains("X-Custom-3"));
}

TEST(HttpHeaders, RemoveCaseInsensitive) {
    headers_map headers;
    headers.set("Content-Type", "application/json");

    headers.remove("content-type");
    EXPECT_FALSE(headers.contains("Content-Type"));
}

TEST(HttpHeaders, Clear) {
    headers_map headers;
    headers.set("Header1", "value1");
    headers.set("Header2", "value2");

    EXPECT_EQ(headers.size(), 2);

    headers.clear();
    EXPECT_EQ(headers.size(), 0);
    EXPECT_TRUE(headers.empty());
}

TEST(HttpHeaders, Iterator) {
    headers_map headers;
    headers.set("Host", "example.com");
    headers.set("User-Agent", "TestAgent");

    size_t count = 0;
    for (const auto& [name, value] : headers) {
        EXPECT_FALSE(name.empty());
        EXPECT_FALSE(value.empty());
        ++count;
    }

    EXPECT_EQ(count, 2);
}

TEST(HttpHeaders, GetNonExistent) {
    headers_map headers;
    auto value = headers.get("NonExistent");

    EXPECT_FALSE(value.has_value());
}

TEST(HttpHeaders, MoveSemantics) {
    headers_map headers1;
    headers1.set("X-Test", "value");

    headers_map headers2 = std::move(headers1);
    EXPECT_EQ(headers2.get("X-Test").value_or(""), "value");
}

TEST(HttpHeaders, CopySemantics) {
    headers_map headers1;
    headers1.set("X-Test", "value");

    headers_map headers2 = headers1;
    EXPECT_EQ(headers2.get("X-Test").value_or(""), "value");
    EXPECT_EQ(headers1.get("X-Test").value_or(""), "value");
}

TEST(HttpHeaders, MultipleHeadersSameNameDifferentCase) {
    headers_map headers;
    headers.set("Content-Type", "application/json");
    headers.set("content-type", "text/html");

    EXPECT_EQ(headers.size(), 1);
    EXPECT_EQ(headers.get("Content-Type").value_or(""), "text/html");
}
