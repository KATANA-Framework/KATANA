#include <gtest/gtest.h>

#ifdef KATANA_HAS_OPENSSL

#include "katana/core/auth.hpp"
#include "katana/core/jwt.hpp"

#include <string>

using namespace katana;

TEST(Auth, JwtBearerAuthenticates) {
    auth::authenticator a;
    auth::jwt_auth_config jc;
    jc.verify.hs_secret = "s";
    jc.verify.require_exp = false;
    a.configure_jwt(jc);

    auto tok = auth::sign_jwt_hs256(R"({"sub":"alice","scope":"admin read"})", "s");
    ASSERT_TRUE(tok.has_value());
    const std::string header = "Bearer " + *tok; // keep alive: headers store views

    http::request req;
    req.headers.set_view("authorization", header);
    auto res = a.authenticate(req);

    EXPECT_EQ(static_cast<int>(res.status), static_cast<int>(auth::auth_status::ok));
    EXPECT_EQ(res.who.subject, "alice");
    EXPECT_TRUE(res.who.has_scope("admin"));
    EXPECT_FALSE(res.who.has_scope("write"));
}

TEST(Auth, MissingCredentialIsMissing) {
    auth::authenticator a;
    auth::jwt_auth_config jc;
    jc.verify.hs_secret = "s";
    a.configure_jwt(jc);

    http::request req;
    EXPECT_EQ(static_cast<int>(a.authenticate(req).status),
              static_cast<int>(auth::auth_status::missing));
}

TEST(Auth, InvalidJwtIsInvalid) {
    auth::authenticator a;
    auth::jwt_auth_config jc;
    jc.verify.hs_secret = "s";
    a.configure_jwt(jc);

    const std::string header = "Bearer not.a.jwt";
    http::request req;
    req.headers.set_view("authorization", header);
    EXPECT_EQ(static_cast<int>(a.authenticate(req).status),
              static_cast<int>(auth::auth_status::invalid));
}

TEST(Auth, ApiKeyAuthenticatesAndRejects) {
    auth::authenticator a;
    auth::api_key_config kc;
    kc.header = "x-api-key";
    kc.keys["k-live-123"] = auth::principal{"service-account", {"ingest"}};
    a.configure_api_key(kc);

    const std::string good = "k-live-123";
    http::request ok_req;
    ok_req.headers.set_view("x-api-key", good);
    auto ok = a.authenticate(ok_req);
    EXPECT_EQ(static_cast<int>(ok.status), static_cast<int>(auth::auth_status::ok));
    EXPECT_EQ(ok.who.subject, "service-account");
    EXPECT_TRUE(ok.who.has_scope("ingest"));

    const std::string bad = "nope";
    http::request bad_req;
    bad_req.headers.set_view("x-api-key", bad);
    EXPECT_EQ(static_cast<int>(a.authenticate(bad_req).status),
              static_cast<int>(auth::auth_status::invalid));
}

#endif // KATANA_HAS_OPENSSL
