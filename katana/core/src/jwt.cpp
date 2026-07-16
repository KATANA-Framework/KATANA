#include "katana/core/jwt.hpp"

#include "katana/core/http_client.hpp"
#include "katana/core/serde.hpp"

#include <openssl/bn.h>
#include <openssl/core_names.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/param_build.h>
#include <openssl/pem.h>

#include <array>
#include <chrono>
#include <cstring>

namespace katana::auth {

namespace {

constexpr char kB64Url[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

int b64url_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '-') return 62;
    if (c == '_') return 63;
    return -1;
}

int64_t now_seconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

// Split "a.b.c" into its three segments; false if not exactly three.
bool split_jws(std::string_view t, std::string_view& h, std::string_view& p, std::string_view& s) {
    auto d1 = t.find('.');
    if (d1 == std::string_view::npos) return false;
    auto d2 = t.find('.', d1 + 1);
    if (d2 == std::string_view::npos) return false;
    if (t.find('.', d2 + 1) != std::string_view::npos) return false;
    h = t.substr(0, d1);
    p = t.substr(d1 + 1, d2 - d1 - 1);
    s = t.substr(d2 + 1);
    return true;
}

jwt_algorithm alg_from(std::string_view a) {
    if (a == "HS256") return jwt_algorithm::hs256;
    if (a == "RS256") return jwt_algorithm::rs256;
    if (a == "ES256") return jwt_algorithm::es256;
    return jwt_algorithm::unknown;
}

// Read {"alg":..,"kid":..} from a decoded header JSON.
void parse_header(std::string_view json, std::string& alg, std::string& kid) {
    serde::json_cursor cur(json.data(), json.data() + json.size());
    if (!cur.try_object_start()) return;
    while (!cur.try_object_end()) {
        auto key = cur.string();
        if (!key) break;
        cur.consume(':');
        if (*key == "alg") {
            if (auto v = cur.string()) alg.assign(v->begin(), v->end());
            else cur.skip_value();
        } else if (*key == "kid") {
            if (auto v = cur.string()) kid.assign(v->begin(), v->end());
            else cur.skip_value();
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
}

void split_scopes(std::string_view s, std::vector<std::string>& out) {
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && s[i] == ' ') ++i;
        size_t j = i;
        while (j < s.size() && s[j] != ' ') ++j;
        if (j > i) out.emplace_back(s.substr(i, j - i));
        i = j;
    }
}

void read_string_or_array(serde::json_cursor& cur, std::vector<std::string>& out) {
    if (cur.try_array_start()) {
        while (!cur.try_array_end()) {
            if (auto v = cur.string()) out.emplace_back(v->begin(), v->end());
            else cur.skip_value();
            cur.try_comma();
        }
    } else if (auto v = cur.string()) {
        out.emplace_back(v->begin(), v->end());
    } else {
        cur.skip_value();
    }
}

void parse_payload(std::string_view json, jwt_claims& c) {
    c.raw_payload.assign(json.begin(), json.end());
    serde::json_cursor cur(json.data(), json.data() + json.size());
    if (!cur.try_object_start()) return;
    while (!cur.try_object_end()) {
        auto key = cur.string();
        if (!key) break;
        cur.consume(':');
        if (*key == "sub") {
            if (auto v = cur.string()) c.sub.assign(v->begin(), v->end());
            else cur.skip_value();
        } else if (*key == "iss") {
            if (auto v = cur.string()) c.iss.assign(v->begin(), v->end());
            else cur.skip_value();
        } else if (*key == "aud") {
            read_string_or_array(cur, c.aud);
        } else if (*key == "exp") {
            if (auto v = serde::parse_int64(cur)) c.exp = *v;
        } else if (*key == "nbf") {
            if (auto v = serde::parse_int64(cur)) c.nbf = *v;
        } else if (*key == "iat") {
            if (auto v = serde::parse_int64(cur)) c.iat = *v;
        } else if (*key == "scope") {
            if (auto v = cur.string()) split_scopes(*v, c.scopes);
            else cur.skip_value();
        } else if (*key == "scp" || *key == "roles") {
            read_string_or_array(cur, c.scopes);
        } else {
            cur.skip_value();
        }
        cur.try_comma();
    }
}

// HMAC-SHA256 over `data` with `key`; compare to `sig` in constant time.
bool verify_hs256(std::string_view data, std::span<const uint8_t> sig, std::string_view key) {
    unsigned char mac[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    if (HMAC(EVP_sha256(), key.data(), static_cast<int>(key.size()),
             reinterpret_cast<const unsigned char*>(data.data()), data.size(), mac, &len) == nullptr) {
        return false;
    }
    if (sig.size() != len) return false;
    return CRYPTO_memcmp(mac, sig.data(), len) == 0;
}

// EVP_DigestVerify with SHA-256 for an RSA/EC public key. `sig` is DER (RSA raw, ECDSA DER).
bool verify_pkey(EVP_PKEY* pkey, std::string_view data, std::span<const uint8_t> sig) {
    if (pkey == nullptr) return false;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) return false;
    bool ok = false;
    if (EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, pkey) == 1) {
        ok = EVP_DigestVerify(ctx, sig.data(), sig.size(),
                              reinterpret_cast<const unsigned char*>(data.data()), data.size()) == 1;
    }
    EVP_MD_CTX_free(ctx);
    return ok;
}

// JOSE ES256 signatures are raw R||S (32+32 bytes); OpenSSL wants a DER ECDSA-Sig.
bool verify_es256(EVP_PKEY* pkey, std::string_view data, std::span<const uint8_t> raw) {
    if (raw.size() != 64) return false;
    ECDSA_SIG* sig = ECDSA_SIG_new();
    BIGNUM* r = BN_bin2bn(raw.data(), 32, nullptr);
    BIGNUM* s = BN_bin2bn(raw.data() + 32, 32, nullptr);
    if (sig == nullptr || r == nullptr || s == nullptr) {
        ECDSA_SIG_free(sig);
        BN_free(r);
        BN_free(s);
        return false;
    }
    ECDSA_SIG_set0(sig, r, s); // takes ownership of r, s
    unsigned char* der = nullptr;
    const int der_len = i2d_ECDSA_SIG(sig, &der);
    bool ok = false;
    if (der_len > 0) {
        ok = verify_pkey(pkey, data, {der, static_cast<size_t>(der_len)});
        OPENSSL_free(der);
    }
    ECDSA_SIG_free(sig);
    return ok;
}

EVP_PKEY* load_pubkey_pem(std::string_view pem) {
    if (pem.empty()) return nullptr;
    BIO* bio = BIO_new_mem_buf(pem.data(), static_cast<int>(pem.size()));
    if (bio == nullptr) return nullptr;
    EVP_PKEY* key = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    return key;
}

} // namespace

// ---------------------------------------------------------------------------- base64url

std::string base64url_encode(std::span<const uint8_t> data) {
    std::string out;
    out.reserve((data.size() + 2) / 3 * 4);
    size_t i = 0;
    for (; i + 3 <= data.size(); i += 3) {
        const uint32_t n = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
        out.push_back(kB64Url[(n >> 18) & 63]);
        out.push_back(kB64Url[(n >> 12) & 63]);
        out.push_back(kB64Url[(n >> 6) & 63]);
        out.push_back(kB64Url[n & 63]);
    }
    if (data.size() - i == 1) {
        const uint32_t n = data[i] << 16;
        out.push_back(kB64Url[(n >> 18) & 63]);
        out.push_back(kB64Url[(n >> 12) & 63]);
    } else if (data.size() - i == 2) {
        const uint32_t n = (data[i] << 16) | (data[i + 1] << 8);
        out.push_back(kB64Url[(n >> 18) & 63]);
        out.push_back(kB64Url[(n >> 12) & 63]);
        out.push_back(kB64Url[(n >> 6) & 63]);
    }
    return out;
}

result<std::vector<uint8_t>> base64url_decode(std::string_view text) {
    std::vector<uint8_t> out;
    out.reserve(text.size() * 3 / 4 + 3);
    uint32_t buf = 0;
    int bits = 0;
    for (char c : text) {
        if (c == '=') break;
        const int v = b64url_value(c);
        if (v < 0) {
            return std::unexpected(make_error_code(error_code::openapi_parse_error));
        }
        buf = (buf << 6) | static_cast<uint32_t>(v);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back(static_cast<uint8_t>((buf >> bits) & 0xFF));
        }
    }
    return out;
}

// ---------------------------------------------------------------------------- jwt_claims

std::string jwt_claims::get(std::string_view claim) const {
    serde::json_cursor cur(raw_payload.data(), raw_payload.data() + raw_payload.size());
    if (!cur.try_object_start()) return {};
    while (!cur.try_object_end()) {
        auto key = cur.string();
        if (!key) break;
        cur.consume(':');
        if (*key == claim) {
            if (auto v = cur.string()) return std::string(v->begin(), v->end());
            return {};
        }
        cur.skip_value();
        cur.try_comma();
    }
    return {};
}

bool jwt_claims::has_scope(std::string_view scope) const {
    for (const auto& s : scopes) {
        if (s == scope) return true;
    }
    return false;
}

// ---------------------------------------------------------------------------- jwks_set

jwks_set::~jwks_set() {
    for (auto& [kid, key] : keys_) {
        EVP_PKEY_free(key);
    }
}

jwks_set::jwks_set(jwks_set&& other) noexcept : keys_(std::move(other.keys_)) {
    other.keys_.clear();
}

jwks_set& jwks_set::operator=(jwks_set&& other) noexcept {
    if (this != &other) {
        for (auto& [kid, key] : keys_) {
            EVP_PKEY_free(key);
        }
        keys_ = std::move(other.keys_);
        other.keys_.clear();
    }
    return *this;
}

evp_pkey_st* jwks_set::find(std::string_view kid) const {
    auto it = keys_.find(std::string(kid));
    return it == keys_.end() ? nullptr : it->second;
}

namespace {

// Build an RSA public EVP_PKEY from base64url n (modulus) and e (exponent).
EVP_PKEY* rsa_pubkey_from(std::string_view n_b64, std::string_view e_b64) {
    auto n = base64url_decode(n_b64);
    auto e = base64url_decode(e_b64);
    if (!n || !e || n->empty() || e->empty()) return nullptr;
    BIGNUM* bn_n = BN_bin2bn(n->data(), static_cast<int>(n->size()), nullptr);
    BIGNUM* bn_e = BN_bin2bn(e->data(), static_cast<int>(e->size()), nullptr);
    EVP_PKEY* pkey = nullptr;
    OSSL_PARAM_BLD* bld = OSSL_PARAM_BLD_new();
    if (bn_n && bn_e && bld && OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, bn_n) &&
        OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, bn_e)) {
        OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(bld);
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr);
        if (params && ctx && EVP_PKEY_fromdata_init(ctx) == 1) {
            EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_PUBLIC_KEY, params);
        }
        EVP_PKEY_CTX_free(ctx);
        OSSL_PARAM_free(params);
    }
    OSSL_PARAM_BLD_free(bld);
    BN_free(bn_n);
    BN_free(bn_e);
    return pkey;
}

} // namespace

result<jwks_set> jwks_set::parse(std::string_view jwks_json) {
    jwks_set set;
    serde::json_cursor cur(jwks_json.data(), jwks_json.data() + jwks_json.size());
    if (!cur.try_object_start()) {
        return std::unexpected(make_error_code(error_code::openapi_parse_error));
    }
    while (!cur.try_object_end()) {
        auto key = cur.string();
        if (!key) break;
        cur.consume(':');
        if (*key != "keys") {
            cur.skip_value();
            cur.try_comma();
            continue;
        }
        if (!cur.try_array_start()) {
            cur.skip_value();
            cur.try_comma();
            continue;
        }
        while (!cur.try_array_end()) {
            std::string kty, kid, n, e;
            if (cur.try_object_start()) {
                while (!cur.try_object_end()) {
                    auto k = cur.string();
                    if (!k) break;
                    cur.consume(':');
                    auto* dst = *k == "kty" ? &kty : *k == "kid" ? &kid : *k == "n" ? &n
                                : *k == "e"                                         ? &e
                                                                                    : nullptr;
                    if (dst != nullptr) {
                        if (auto v = cur.string()) dst->assign(v->begin(), v->end());
                        else cur.skip_value();
                    } else {
                        cur.skip_value();
                    }
                    cur.try_comma();
                }
            }
            if (kty == "RSA" && !kid.empty() && !n.empty() && !e.empty()) {
                if (EVP_PKEY* pk = rsa_pubkey_from(n, e)) {
                    set.keys_.emplace(kid, pk);
                }
            }
            cur.try_comma();
        }
        cur.try_comma();
    }
    return set;
}

// ---------------------------------------------------------------------------- verify_jwt

result<jwt_claims> verify_jwt(std::string_view token, const jwt_verify_config& cfg,
                              int64_t now_unix) {
    std::string_view h, p, s;
    if (!split_jws(token, h, p, s)) {
        return std::unexpected(make_error_code(error_code::openapi_parse_error));
    }
    auto header_bytes = base64url_decode(h);
    auto payload_bytes = base64url_decode(p);
    auto sig = base64url_decode(s);
    if (!header_bytes || !payload_bytes || !sig) {
        return std::unexpected(make_error_code(error_code::openapi_parse_error));
    }

    std::string alg_str, kid;
    parse_header({reinterpret_cast<const char*>(header_bytes->data()), header_bytes->size()}, alg_str,
                 kid);
    const jwt_algorithm alg = alg_from(alg_str);

    const std::string_view signing_input = token.substr(0, h.size() + 1 + p.size());
    const auto fail = std::unexpected(make_error_code(error_code::method_not_allowed)); // → 401

    bool signature_ok = false;
    switch (alg) {
    case jwt_algorithm::hs256:
        if (cfg.hs_secret.empty()) return fail;
        signature_ok = verify_hs256(signing_input, *sig, cfg.hs_secret);
        break;
    case jwt_algorithm::rs256:
    case jwt_algorithm::es256: {
        EVP_PKEY* pkey = nullptr;
        bool owned = false;
        if (cfg.jwks != nullptr && !kid.empty()) {
            pkey = cfg.jwks->find(kid);
        }
        if (pkey == nullptr && !cfg.public_key_pem.empty()) {
            pkey = load_pubkey_pem(cfg.public_key_pem);
            owned = true;
        }
        if (pkey == nullptr) return fail;
        signature_ok = alg == jwt_algorithm::rs256 ? verify_pkey(pkey, signing_input, *sig)
                                                   : verify_es256(pkey, signing_input, *sig);
        if (owned) EVP_PKEY_free(pkey);
        break;
    }
    case jwt_algorithm::unknown:
        return fail;
    }
    if (!signature_ok) {
        return fail;
    }

    jwt_claims claims;
    parse_payload({reinterpret_cast<const char*>(payload_bytes->data()), payload_bytes->size()},
                  claims);

    const int64_t now = now_unix != 0 ? now_unix : now_seconds();
    if (cfg.require_exp && claims.exp == 0) return fail;
    if (claims.exp != 0 && now > claims.exp + cfg.leeway_seconds) return fail;
    if (claims.nbf != 0 && now + cfg.leeway_seconds < claims.nbf) return fail;
    if (!cfg.expected_issuer.empty() && claims.iss != cfg.expected_issuer) return fail;
    if (!cfg.expected_audience.empty()) {
        bool aud_ok = false;
        for (const auto& a : claims.aud) {
            if (a == cfg.expected_audience) {
                aud_ok = true;
                break;
            }
        }
        if (!aud_ok) return fail;
    }
    return claims;
}

// ---------------------------------------------------------------------------- sign (HS256, tests)

result<std::string> sign_jwt_hs256(std::string_view payload_json, std::string_view secret) {
    static constexpr std::string_view header = R"({"alg":"HS256","typ":"JWT"})";
    std::string token = base64url_encode(
        {reinterpret_cast<const uint8_t*>(header.data()), header.size()});
    token.push_back('.');
    token += base64url_encode(
        {reinterpret_cast<const uint8_t*>(payload_json.data()), payload_json.size()});

    unsigned char mac[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    if (HMAC(EVP_sha256(), secret.data(), static_cast<int>(secret.size()),
             reinterpret_cast<const unsigned char*>(token.data()), token.size(), mac,
             &len) == nullptr) {
        return std::unexpected(make_error_code(error_code::openapi_parse_error));
    }
    token.push_back('.');
    token += base64url_encode({mac, len});
    return token;
}

result<jwks_set> jwks_set::fetch(std::string_view url, bool verify_tls) {
    auto resp = http::http_get(url, {.verify_tls = verify_tls});
    if (!resp) {
        return std::unexpected(resp.error());
    }
    if (resp->status != 200 || resp->body.empty()) {
        return std::unexpected(make_error_code(error_code::openapi_parse_error));
    }
    return parse(resp->body);
}

} // namespace katana::auth
