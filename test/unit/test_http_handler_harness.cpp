#include "support/http_handler_harness.hpp"

#include <gtest/gtest.h>

using katana::http::method;
using katana::http::response;
using katana::test_support::HttpHandlerHarness;

TEST(HttpHandlerHarness, ParsesRawRequestAndCallsHandler) {
    HttpHandlerHarness harness([](const katana::http::request& req, katana::monotonic_arena&) {
        EXPECT_EQ(req.http_method, method::post);
        EXPECT_EQ(req.uri, "/echo");
        EXPECT_EQ(req.header("Content-Type").value_or(""), "application/json");
        EXPECT_EQ(req.body, R"({"ping":"pong"})");
        auto resp = response::ok("ok", "text/plain");
        resp.set_header("X-Handled", "true");
        return resp;
    });

    std::string raw = "POST /echo HTTP/1.1\r\n"
                      "Host: localhost\r\n"
                      "Content-Type: application/json\r\n"
                      "Content-Length: 15\r\n"
                      "\r\n"
                      R"({"ping":"pong"})";

    auto resp = harness.run_raw(raw);
    EXPECT_EQ(resp.status, 200);
    EXPECT_EQ(resp.headers.get("X-Handled").value_or(""), "true");
    EXPECT_EQ(resp.body, "ok");
}

TEST(HttpHandlerHarness, RunsOnExistingRequestCopy) {
    katana::monotonic_arena arena;
    katana::http::request req;
    req.http_method = method::get;
    req.uri = "/ping";
    req.headers = katana::http::headers_map(&arena);
    req.headers.set_view("X-Test", "yes");
    req.body = "body";

    HttpHandlerHarness harness([](const katana::http::request& r, katana::monotonic_arena&) {
        EXPECT_EQ(r.header("X-Test").value_or(""), "yes");
        auto resp = response::ok("pong");
        resp.set_header("Content-Length", "4");
        return resp;
    });

    auto resp = harness.run(req);
    EXPECT_EQ(resp.status, 200);
    EXPECT_EQ(resp.body, "pong");
}
