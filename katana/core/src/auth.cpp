#include "katana/core/auth.hpp"

#include "katana/core/problem.hpp"

#include <new>

namespace katana::auth {

auth_result authenticator::authenticate(const http::request& req) const {
    // 1) Bearer JWT.
    if (jwt_) {
        if (auto header = req.header("authorization")) {
            const std::string prefix = jwt_->scheme + " ";
            if (header->size() > prefix.size() && header->substr(0, prefix.size()) == prefix) {
                const std::string_view token = header->substr(prefix.size());
                auto claims = verify_jwt(token, jwt_->verify);
                if (!claims) {
                    return {auth_status::invalid, {}};
                }
                principal who;
                who.subject = std::move(claims->sub);
                who.scopes = std::move(claims->scopes);
                return {auth_status::ok, std::move(who)};
            }
        }
    }

    // 2) API key (header). Static map first, then the optional dynamic resolver (e.g. DB lookup).
    if (api_key_) {
        if (auto key = req.header(api_key_->header)) {
            auto it = api_key_->keys.find(std::string(*key));
            if (it != api_key_->keys.end()) {
                return {auth_status::ok, it->second};
            }
            if (api_key_->resolver) {
                if (auto who = api_key_->resolver(*key)) {
                    return {auth_status::ok, std::move(*who)};
                }
            }
            return {auth_status::invalid, {}};
        }
    }

    return {auth_status::missing, {}};
}

http::middleware_fn require_auth(std::shared_ptr<const authenticator> auth,
                                 std::string required_scope) {
    return http::middleware_fn([auth = std::move(auth), required_scope = std::move(required_scope)](
                                   const http::request& req, http::request_context& ctx,
                                   http::response& out, http::next_fn next) -> result<void> {
        auto res = auth->authenticate(req);
        if (res.status != auth_status::ok) {
            out.assign_error(problem_details::unauthorized(
                res.status == auth_status::missing ? "authentication required" : "invalid credentials"));
            return result<void>{};
        }
        if (!required_scope.empty() && !res.who.has_scope(required_scope)) {
            out.assign_error(problem_details::forbidden("insufficient scope"));
            return result<void>{};
        }
        // Build a non-owning view over the principal; both live on this stack frame across next()
        // (the handler runs synchronously inside it).
        std::vector<std::string_view> scope_views(res.who.scopes.begin(), res.who.scopes.end());
        principal_view view{res.who.subject, scope_views};
        ctx.principal = &view;
        auto result = next(out);
        ctx.principal = nullptr;
        return result;
    });
}

result<http::route_policy_resolution> auth_executor::before_dispatch(const http::request& req,
                                                                     http::request_context& ctx,
                                                                     http::response& out) {
    if (ctx.route_policy == nullptr || !ctx.route_policy->auth_required) {
        return http::route_policy_resolution::continue_request;
    }
    auto res = auth_->authenticate(req);
    if (res.status != auth_status::ok) {
        out.assign_error(problem_details::unauthorized(
            res.status == auth_status::missing ? "authentication required" : "invalid credentials"));
        return http::route_policy_resolution::short_circuit;
    }
    if (!ctx.route_policy->auth_scope.empty() && !res.who.has_scope(ctx.route_policy->auth_scope)) {
        out.assign_error(problem_details::forbidden("insufficient scope"));
        return http::route_policy_resolution::short_circuit;
    }
    // Copy the principal into the request arena so it outlives before_dispatch until the handler.
    monotonic_arena& arena = ctx.arena;
    const std::string_view subject{arena.allocate_string(res.who.subject), res.who.subject.size()};
    const size_t n = res.who.scopes.size();
    std::string_view* scope_arr = nullptr;
    if (n > 0) {
        scope_arr = arena.allocate_array<std::string_view>(n);
        for (size_t i = 0; i < n; ++i) {
            const auto& s = res.who.scopes[i];
            scope_arr[i] = std::string_view{arena.allocate_string(s), s.size()};
        }
    }
    auto* view = new (arena.allocate_array<principal_view>(1))
        principal_view{subject, std::span<const std::string_view>{scope_arr, n}};
    ctx.principal = view;
    return http::route_policy_resolution::continue_request;
}

} // namespace katana::auth
