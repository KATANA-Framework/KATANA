#pragma once

// JWT (JWS compact) verification for authentication: HS256 / RS256 / ES256, standard-claim checks
// (exp/nbf/iat/iss/aud), scope extraction, and JWKS (kid → public key). Crypto is OpenSSL (the same
// dependency TLS pulls in). All parsing is allocation-light and does not throw.

#include "result.hpp"

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct evp_pkey_st; // OpenSSL EVP_PKEY

namespace katana::auth {

enum class jwt_algorithm : uint8_t { hs256, rs256, es256, unknown };

// Claims extracted from a verified token. `scopes` merges `scope` (space-delimited) / `scp` /
// `roles`. `raw_payload` keeps the decoded JSON for custom-claim access via get().
struct jwt_claims {
    std::string sub;
    std::string iss;
    std::vector<std::string> aud;
    int64_t exp = 0;
    int64_t nbf = 0;
    int64_t iat = 0;
    std::vector<std::string> scopes;
    std::string raw_payload;

    // String value of a top-level claim (empty if absent / not a string).
    [[nodiscard]] std::string get(std::string_view claim) const;
    [[nodiscard]] bool has_scope(std::string_view scope) const;
};

// A set of public keys indexed by `kid` (from a JWKS document), owning the EVP_PKEYs.
class jwks_set {
public:
    jwks_set() = default;
    ~jwks_set();
    jwks_set(jwks_set&&) noexcept;
    jwks_set& operator=(jwks_set&&) noexcept;
    jwks_set(const jwks_set&) = delete;
    jwks_set& operator=(const jwks_set&) = delete;

    // Parse a JWKS JSON document ({"keys":[{kty,kid,n,e,...}]}) into RSA/EC public keys.
    static result<jwks_set> parse(std::string_view jwks_json);

    [[nodiscard]] evp_pkey_st* find(std::string_view kid) const;
    [[nodiscard]] bool empty() const noexcept { return keys_.empty(); }

private:
    std::unordered_map<std::string, evp_pkey_st*> keys_;
};

struct jwt_verify_config {
    // Symmetric (HS256).
    std::string hs_secret;
    // Asymmetric static key (RS256/ES256), PEM SubjectPublicKeyInfo.
    std::string public_key_pem;
    // Asymmetric via JWKS (RS256/ES256), selected by the token's `kid`.
    const jwks_set* jwks = nullptr;

    // Validation (all optional except the signature + exp when require_exp).
    std::string expected_issuer;   // if set, `iss` must match
    std::string expected_audience; // if set, must be in `aud`
    int64_t leeway_seconds = 60;   // clock-skew tolerance for exp/nbf
    bool require_exp = true;
};

// Verify a compact JWS. On success returns the claims; on failure an error_code (the caller maps
// any failure to 401). `now_unix` is injectable for testing (0 = wall clock).
result<jwt_claims> verify_jwt(std::string_view token, const jwt_verify_config& cfg,
                              int64_t now_unix = 0);

// base64url (no padding) — exposed for tests and for building tokens.
std::string base64url_encode(std::span<const uint8_t> data);
result<std::vector<uint8_t>> base64url_decode(std::string_view text);

// Sign a payload (for tests / issuing simple tokens). HS256 uses hs_secret; RS256/ES256 use a PEM
// private key. Returns the compact JWS.
result<std::string> sign_jwt_hs256(std::string_view payload_json, std::string_view secret);

} // namespace katana::auth
