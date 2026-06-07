#pragma once

// Serve several independently-generated contracts from one server. Each contract produces its
// own `katana::http::router`; this dispatcher tries them in order and the first whose path
// pattern matches owns the request (including its x-katana cache/rate-limit/idempotency
// policies, which run inside dispatch_with_info). If no router claims the path, it's a 404.
//
// The server's templated constructor accepts any type exposing
//   result<void> dispatch_to(const request&, request_context&, response&) const;
// so `server(composite)` just works.
//
// Note: the server's per-route metric labels key off a single registered router, so they're
// inactive when serving via a composite — the global request-duration histogram, connection and
// reactor gauges still apply. (A future core `composite_router` could restore per-route labels.)

#include "katana/core/router.hpp"

#include <vector>

namespace pulse {

class composite_dispatcher {
public:
    composite_dispatcher(std::initializer_list<const katana::http::router*> routers)
        : routers_(routers) {}

    katana::result<void> dispatch_to(const katana::http::request& req,
                                     katana::http::request_context& ctx,
                                     katana::http::response& out) const {
        for (const auto* router : routers_) {
            const auto info = router->dispatch_with_info(req, ctx, out);
            // dispatch_with_info outcomes:
            //   * no error            -> the handler ran and produced `out` (success).
            //   * error + path_matched -> this router owns the path but the method was wrong (405)
            //                             or the handler errored — map it; the router claims it.
            //   * error + !path_matched -> not-found here; try the next contract.
            if (!info.has_error) {
                return {}; // handled
            }
            if (info.path_matched) {
                katana::http::map_dispatch_error(info, out);
                return {};
            }
        }
        // No contract claims this path.
        katana::http::map_route_error(katana::make_error_code(katana::error_code::not_found), out);
        return {};
    }

private:
    std::vector<const katana::http::router*> routers_;
};

} // namespace pulse
