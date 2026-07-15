#include <gtest/gtest.h>

#ifdef KATANA_HAS_OPENSSL

#include "katana/core/jwt.hpp"

#include <string>

using namespace katana::auth;

namespace {
std::span<const uint8_t> bytes(const std::string& s) {
    return {reinterpret_cast<const uint8_t*>(s.data()), s.size()};
}
} // namespace

TEST(Jwt, Base64UrlRoundTripAndUrlSafe) {
    const std::string s = std::string("Hello, JWT!") + '\xff' + '\x00' + '\x10' + '\x3e' + '\x3f';
    const std::string enc = base64url_encode(bytes(s));
    EXPECT_EQ(enc.find('='), std::string::npos);
    EXPECT_EQ(enc.find('+'), std::string::npos);
    EXPECT_EQ(enc.find('/'), std::string::npos);
    auto dec = base64url_decode(enc);
    ASSERT_TRUE(dec.has_value());
    EXPECT_EQ(std::string(reinterpret_cast<const char*>(dec->data()), dec->size()), s);
}

TEST(Jwt, Hs256SignVerifyAndScopes) {
    auto tok = sign_jwt_hs256(
        R"({"sub":"alice","iss":"pulse","exp":4102444800,"scope":"read write"})", "secret");
    ASSERT_TRUE(tok.has_value());

    jwt_verify_config cfg;
    cfg.hs_secret = "secret";
    cfg.expected_issuer = "pulse";
    auto c = verify_jwt(*tok, cfg);
    ASSERT_TRUE(c.has_value());
    EXPECT_EQ(c->sub, "alice");
    EXPECT_EQ(c->iss, "pulse");
    EXPECT_TRUE(c->has_scope("read"));
    EXPECT_TRUE(c->has_scope("write"));
    EXPECT_FALSE(c->has_scope("admin"));
}

TEST(Jwt, RejectsTamperedSignature) {
    auto tok = sign_jwt_hs256(R"({"sub":"a","exp":4102444800})", "secret");
    ASSERT_TRUE(tok.has_value());
    std::string bad = *tok;
    bad.back() = bad.back() == 'A' ? 'B' : 'A';
    jwt_verify_config cfg;
    cfg.hs_secret = "secret";
    EXPECT_FALSE(verify_jwt(bad, cfg).has_value());
}

TEST(Jwt, RejectsWrongSecret) {
    auto tok = sign_jwt_hs256(R"({"sub":"a","exp":4102444800})", "secret");
    ASSERT_TRUE(tok.has_value());
    jwt_verify_config cfg;
    cfg.hs_secret = "different";
    EXPECT_FALSE(verify_jwt(*tok, cfg).has_value());
}

TEST(Jwt, RejectsExpiredAndFutureNbf) {
    jwt_verify_config cfg;
    cfg.hs_secret = "secret";

    auto expired = sign_jwt_hs256(R"({"sub":"a","exp":100})", "secret");
    ASSERT_TRUE(expired.has_value());
    EXPECT_FALSE(verify_jwt(*expired, cfg).has_value());

    auto future = sign_jwt_hs256(R"({"sub":"a","exp":4102444800,"nbf":4102444800})", "secret");
    ASSERT_TRUE(future.has_value());
    EXPECT_FALSE(verify_jwt(*future, cfg).has_value());
}

TEST(Jwt, RejectsWrongIssuerAndAudience) {
    auto tok = sign_jwt_hs256(
        R"({"sub":"a","iss":"pulse","aud":"api","exp":4102444800})", "secret");
    ASSERT_TRUE(tok.has_value());

    jwt_verify_config bad_iss;
    bad_iss.hs_secret = "secret";
    bad_iss.expected_issuer = "other";
    EXPECT_FALSE(verify_jwt(*tok, bad_iss).has_value());

    jwt_verify_config bad_aud;
    bad_aud.hs_secret = "secret";
    bad_aud.expected_audience = "other";
    EXPECT_FALSE(verify_jwt(*tok, bad_aud).has_value());

    jwt_verify_config good;
    good.hs_secret = "secret";
    good.expected_issuer = "pulse";
    good.expected_audience = "api";
    EXPECT_TRUE(verify_jwt(*tok, good).has_value());
}

TEST(Jwt, RequireExpRejectsTokenWithoutExp) {
    auto tok = sign_jwt_hs256(R"({"sub":"a"})", "secret");
    ASSERT_TRUE(tok.has_value());
    jwt_verify_config cfg;
    cfg.hs_secret = "secret";
    cfg.require_exp = true;
    EXPECT_FALSE(verify_jwt(*tok, cfg).has_value());
    cfg.require_exp = false;
    EXPECT_TRUE(verify_jwt(*tok, cfg).has_value());
}

#endif // KATANA_HAS_OPENSSL
