#pragma once

// Non-owning view of an authenticated caller, stored on request_context. Kept dependency-free so
// both router.hpp (which holds a request_context) and auth.hpp can share it without a cycle. The
// viewed strings are owned elsewhere for the request's lifetime — the auth middleware keeps them on
// its stack frame across the handler call; the auth executor copies them into the request arena.

#include <span>
#include <string_view>

namespace katana::auth {

struct principal_view {
    std::string_view subject;
    std::span<const std::string_view> scopes;

    [[nodiscard]] bool has_scope(std::string_view s) const noexcept {
        for (const auto& sc : scopes) {
            if (sc == s) {
                return true;
            }
        }
        return false;
    }
};

} // namespace katana::auth
