#pragma once

// Authentication: a small authenticator that extracts a credential from a request (Bearer JWT or an
// API key) and yields a principal, plus a require_auth() middleware that enforces it per route
// (401 missing/invalid, 403 insufficient scope) and exposes the principal to handlers.

#include "katana/core/http.hpp"
#include "katana/core/jwt.hpp"
#include "katana/core/router.hpp"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace katana::auth {

struct principal {
    std::string subject;
    std::vector<std::string> scopes;

    [[nodiscard]] bool has_scope(std::string_view s) const {
        for (const auto& sc : scopes) {
            if (sc == s) {
                return true;
            }
        }
        return false;
    }
};

struct jwt_auth_config {
    jwt_verify_config verify;      // key material + claim validation
    std::string scheme = "Bearer"; // Authorization: <scheme> <token>
};

struct api_key_config {
    std::string header = "x-api-key";                // header carrying the key
    std::string query_param;                         // optional: also accept ?<param>=<key>
    std::unordered_map<std::string, principal> keys; // key → principal (subject + scopes)
};

enum class auth_status : uint8_t { ok, missing, invalid };

struct auth_result {
    auth_status status = auth_status::missing;
    principal who;
};

// Validates credentials against whichever schemes are configured. Thread-safe after configuration
// (const authenticate()).
class authenticator {
public:
    void configure_jwt(jwt_auth_config cfg) { jwt_ = std::move(cfg); }
    void configure_api_key(api_key_config cfg) { api_key_ = std::move(cfg); }
    [[nodiscard]] bool enabled() const noexcept { return jwt_.has_value() || api_key_.has_value(); }

    // Try every configured scheme. `missing` = no credential presented; `invalid` = a credential
    // was presented but failed; `ok` = authenticated (with `who` filled).
    [[nodiscard]] auth_result authenticate(const http::request& req) const;

private:
    std::optional<jwt_auth_config> jwt_;
    std::optional<api_key_config> api_key_;
};

// Per-route middleware: require a valid credential (401 on missing/invalid) and, when
// `required_scope` is non-empty, that scope (403 otherwise). Sets ctx.principal for the handler.
[[nodiscard]] http::middleware_fn require_auth(std::shared_ptr<const authenticator> auth,
                                               std::string required_scope = {});

// Contract-first authorization: a route_policy_executor that reads the matched route's
// x-katana-auth requirement (ctx.route_policy) and enforces it before the handler — 401 on
// missing/invalid credential, 403 on insufficient scope — then exposes the principal (arena-backed)
// via ctx.principal. Compose it ahead of the Stage-5 policy executor with route_policy_executor_chain.
class auth_executor final : public http::route_policy_executor {
public:
    explicit auth_executor(std::shared_ptr<const authenticator> auth) : auth_(std::move(auth)) {}

    result<http::route_policy_resolution> before_dispatch(const http::request& req,
                                                          http::request_context& ctx,
                                                          http::response& out) override;

private:
    std::shared_ptr<const authenticator> auth_;
};

} // namespace katana::auth
