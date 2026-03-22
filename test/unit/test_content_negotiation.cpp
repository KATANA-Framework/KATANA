#include "katana/core/content_negotiation.hpp"

#include <gtest/gtest.h>

using namespace katana;
using namespace katana::http;

namespace {

request make_request(method m, std::string_view uri) {
    request req;
    req.http_method = m;
    req.uri = uri;
    req.headers = headers_map(nullptr);
    return req;
}

} // namespace

TEST(ContentNegotiation, RegistryValidateContentTypeAcceptsRegisteredMime) {
    auto req = make_request(method::post, "/items");
    req.headers.set_known(field::content_type, "application/json; charset=utf-8");

    EXPECT_TRUE(validate_content_type(req, media_type_registry::default_instance()));
}

TEST(ContentNegotiation, RegistryValidateContentTypeRejectsUnknownMime) {
    auto req = make_request(method::post, "/items");
    req.headers.set_known(field::content_type, "application/xml");

    EXPECT_FALSE(validate_content_type(req, media_type_registry::default_instance()));
}

TEST(ContentNegotiation, RegistryValidateAcceptHonorsRegisteredCandidates) {
    auto req = make_request(method::get, "/items");
    req.headers.set_known(field::accept, "application/xml;q=0.9, application/cbor;q=0.1");

    EXPECT_TRUE(validate_accept(req, media_type_registry::default_instance()));
}

TEST(ContentNegotiation, RegistryValidateAcceptRejectsUnsupportedMime) {
    auto req = make_request(method::get, "/items");
    req.headers.set_known(field::accept, "application/xml");

    EXPECT_FALSE(validate_accept(req, media_type_registry::default_instance()));
}

TEST(ContentNegotiation, RegistryMiddlewarePassesThroughRegisteredHeaders) {
    monotonic_arena arena;
    request_context ctx{arena};
    auto req = make_request(method::post, "/items");
    req.headers.set_known(field::content_type, "application/json");
    req.headers.set_known(field::accept, "application/cbor;q=0.3, application/json;q=0.8");

    response out;
    bool called = false;
    auto middleware = make_content_negotiation_middleware(media_type_registry::default_instance());

    auto middleware_result =
        middleware(req, ctx, out, [&](response& next_out) -> result<void> {
        called = true;
        next_out = response::ok("ok", "text/plain");
        return {};
    });

    ASSERT_TRUE(middleware_result);
    EXPECT_TRUE(called);
    EXPECT_EQ(out.status, 200);
    EXPECT_EQ(out.body, "ok");
}

TEST(ContentNegotiation, RegistryMiddlewareRejectsUnsupportedContentType) {
    monotonic_arena arena;
    request_context ctx{arena};
    auto req = make_request(method::post, "/items");
    req.headers.set_known(field::content_type, "application/xml");

    response out;
    bool called = false;
    auto middleware = make_content_negotiation_middleware(media_type_registry::default_instance());

    auto middleware_result = middleware(req, ctx, out, [&](response&) -> result<void> {
        called = true;
        return {};
    });

    ASSERT_TRUE(middleware_result);
    EXPECT_FALSE(called);
    EXPECT_EQ(out.status, 415);
}

TEST(ContentNegotiation, RegistryMiddlewareRejectsUnsupportedAccept) {
    monotonic_arena arena;
    request_context ctx{arena};
    auto req = make_request(method::get, "/items");
    req.headers.set_known(field::accept, "application/xml");

    response out;
    bool called = false;
    auto middleware = make_content_negotiation_middleware(media_type_registry::default_instance());

    auto middleware_result = middleware(req, ctx, out, [&](response&) -> result<void> {
        called = true;
        return {};
    });

    ASSERT_TRUE(middleware_result);
    EXPECT_FALSE(called);
    EXPECT_EQ(out.status, 406);
}

TEST(ContentNegotiation, RegistryMiddlewareAllowsRequestsWithoutNegotiationHeaders) {
    monotonic_arena arena;
    request_context ctx{arena};
    auto req = make_request(method::get, "/items");

    response out;
    bool called = false;
    auto middleware = make_content_negotiation_middleware(media_type_registry::default_instance());

    auto middleware_result = middleware(req, ctx, out, [&](response&) -> result<void> {
        called = true;
        return {};
    });

    ASSERT_TRUE(middleware_result);
    EXPECT_TRUE(called);
}
