#pragma once

#include "arena.hpp"
#include "function_ref.hpp"
#include "http.hpp"
#include "inplace_function.hpp"
#include "problem.hpp"
#include "result.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace katana::http {

constexpr size_t MAX_ROUTE_SEGMENTS = 16;
constexpr size_t MAX_PATH_PARAMS = 16;

template <size_t N> struct fixed_string {
    constexpr fixed_string(const char (&str)[N]) { std::copy_n(str, N, value); }
    constexpr operator std::string_view() const { return std::string_view{value, N - 1}; }
    char value[N];
};

enum class segment_kind : uint8_t { literal, parameter };

struct path_segment {
    segment_kind kind{segment_kind::literal};
    std::string_view value{};
};

struct path_params {
    using param_entry = std::pair<std::string_view, std::string_view>;

    void add(std::string_view name, std::string_view value) noexcept {
        if (size_ < MAX_PATH_PARAMS) {
            entries_[size_] = param_entry{name, value};
            ++size_;
        }
    }

    [[nodiscard]] std::optional<std::string_view> get(std::string_view name) const noexcept {
        for (size_t i = 0; i < size_; ++i) {
            if (entries_[i].first == name) {
                return entries_[i].second;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] size_t size() const noexcept { return size_; }
    [[nodiscard]] std::span<const param_entry> entries() const noexcept {
        return std::span<const param_entry>(entries_.data(), size_);
    }

private:
    std::array<param_entry, MAX_PATH_PARAMS> entries_{};
    size_t size_{0};
};

struct request_context {
    monotonic_arena& arena;
    path_params params{};
};

struct path_pattern {
    std::array<path_segment, MAX_ROUTE_SEGMENTS> segments{};
    std::array<std::string_view, MAX_PATH_PARAMS> param_names{};
    size_t segment_count{0};
    size_t param_count{0};
    size_t literal_count{0};

    template <fixed_string Str> static consteval path_pattern from_literal() {
        path_pattern pattern{};
        constexpr auto raw = std::string_view(Str);

        if (raw.empty()) {
            throw "route path cannot be empty";
        }
        if (raw.front() != '/') {
            throw "route path must start with '/'";
        }

        size_t pos = 1; // skip leading '/'
        size_t param_index = 0;
        size_t segment_index = 0;

        while (pos < raw.size()) {
            size_t next_slash = raw.size();
            for (size_t i = pos; i < raw.size(); ++i) {
                if (raw[i] == '/') {
                    next_slash = i;
                    break;
                }
            }

            const size_t len = next_slash - pos;
            if (len == 0) {
                throw "empty path segment is not allowed";
            }

            if (segment_index >= MAX_ROUTE_SEGMENTS) {
                throw "too many path segments";
            }

            std::string_view segment = raw.substr(pos, len);
            if (segment.front() == '{') {
                if (segment.back() != '}') {
                    throw "parameter segment must end with '}'";
                }
                if (segment.size() <= 2) {
                    throw "parameter name cannot be empty";
                }
                if (param_index >= MAX_PATH_PARAMS) {
                    throw "too many path parameters";
                }

                auto name = segment.substr(1, segment.size() - 2);
                pattern.segments[segment_index] =
                    path_segment{segment_kind::parameter, std::string_view{name}};
                pattern.param_names[param_index] = name;
                ++param_index;
                ++pattern.param_count;
            } else {
                pattern.segments[segment_index] = path_segment{segment_kind::literal, segment};
                ++pattern.literal_count;
            }

            ++segment_index;
            pos = next_slash + 1;
        }

        pattern.segment_count = segment_index;
        return pattern;
    }

    struct split_result {
        std::array<std::string_view, MAX_ROUTE_SEGMENTS> parts{};
        size_t count{0};
        bool overflow{false};
    };

    [[nodiscard]] static split_result split_path(std::string_view path) noexcept {
        split_result out{};
        size_t pos = 0;
        while (pos < path.size()) {
            if (path[pos] == '/') {
                ++pos;
                continue;
            }
            size_t next = path.find('/', pos);
            if (next == std::string_view::npos) {
                next = path.size();
            }
            if (out.count >= MAX_ROUTE_SEGMENTS) {
                out.overflow = true;
                return out;
            }
            out.parts[out.count++] = path.substr(pos, next - pos);
            pos = next;
        }
        return out;
    }

    [[nodiscard]] bool match_segments(std::span<const std::string_view> path_segments,
                                      size_t path_segment_count,
                                      path_params& out) const noexcept {
        if (segment_count == 0 && path_segment_count == 0) {
            return true;
        }
        if (path_segment_count != segment_count) {
            return false;
        }

        size_t param_index = 0;
        for (size_t i = 0; i < segment_count; ++i) {
            const auto& segment = segments[i];
            const auto& actual = path_segments[i];

            if (segment.kind == segment_kind::literal) {
                if (segment.value != actual) {
                    return false;
                }
            } else {
                if (actual.empty()) {
                    return false;
                }
                out.add(param_names[param_index], actual);
                ++param_index;
            }
        }

        return true;
    }

    [[nodiscard]] bool match(std::string_view path, path_params& out) const noexcept {
        if (segment_count == 0 && (path == "/" || path.empty())) {
            return true;
        }

        auto split = split_path(path);
        if (split.overflow) {
            return false;
        }
        if (split.count != segment_count) {
            return false;
        }
        std::span<const std::string_view> parts(split.parts.data(), split.count);
        return match_segments(parts, split.count, out);
    }

    [[nodiscard]] int specificity_score() const noexcept {
        return static_cast<int>(literal_count * 16 + (MAX_ROUTE_SEGMENTS - param_count));
    }
};

using handler_fn = inplace_function<result<response>(const request&, request_context&), 160>;
using next_fn = function_ref<result<response>()>;
using middleware_fn =
    inplace_function<result<response>(const request&, request_context&, next_fn), 160>;

struct middleware_chain {
    const middleware_fn* ptr{nullptr};
    size_t size{0};

    [[nodiscard]] bool empty() const noexcept { return size == 0 || ptr == nullptr; }

    result<response>
    run(const request& req, request_context& ctx, const handler_fn& handler) const {
        if (empty()) {
            return handler(req, ctx);
        }

        struct invoker {
            const middleware_fn* ptr;
            size_t size;
            const handler_fn& terminal;
            const request& req;
            request_context& ctx;

            result<response> call(size_t index) const {
                if (index >= size) {
                    return terminal(req, ctx);
                }
                const middleware_fn& mw = ptr[index];
                auto next_lambda = [this, index]() -> result<response> { return call(index + 1); };
                next_fn next{next_lambda};
                return mw(req, ctx, next);
            }
        };

        invoker inv{ptr, size, handler, req, ctx};
        return inv.call(0);
    }
};

template <size_t N>
constexpr middleware_chain make_middleware_chain(const std::array<middleware_fn, N>& middlewares) {
    return middleware_chain{middlewares.data(), N};
}

struct route_entry {
    http::method method;
    path_pattern pattern;
    handler_fn handler;
    middleware_chain middleware{};
};

inline constexpr uint32_t method_bit(http::method m) noexcept {
    auto idx = static_cast<uint32_t>(m);
    if (idx >= 31 || m == http::method::unknown) {
        return 0;
    }
    return 1u << idx;
}

inline std::string allow_header_from_mask(uint32_t mask) {
    if (mask == 0) {
        return {};
    }

    constexpr std::array<http::method, 7> order = {http::method::get,
                                                   http::method::head,
                                                   http::method::post,
                                                   http::method::put,
                                                   http::method::del,
                                                   http::method::patch,
                                                   http::method::options};

    std::string allow;
    allow.reserve(32);

    bool first = true;
    for (auto m : order) {
        auto bit = method_bit(m);
        if ((mask & bit) == 0) {
            continue;
        }
        if (!first) {
            allow.append(", ");
        }
        allow.append(http::method_to_string(m));
        first = false;
    }

    return allow;
}

struct dispatch_result {
    result<response> route_response;
    bool path_matched{false};
    uint32_t allowed_methods_mask{0};
};

class router {
public:
    explicit router(std::span<const route_entry> routes) : routes_(routes) {}

    dispatch_result dispatch_with_info(const request& req, request_context& ctx) const {
        auto path = strip_query(req.uri);
        auto split = path_pattern::split_path(path);
        if (split.overflow) {
            return dispatch_result{
                std::unexpected(make_error_code(error_code::not_found)), false, 0};
        }
        std::span<const std::string_view> path_segments(split.parts.data(), split.count);

        const route_entry* best_route = nullptr;
        path_params best_params;
        int best_score = -1;
        bool path_matched = false;
        uint32_t allowed_methods_mask = 0;

        for (const auto& entry : routes_) {
            path_params candidate_params{};
            if (!entry.pattern.match_segments(path_segments, split.count, candidate_params)) {
                continue;
            }

            path_matched = true;
            allowed_methods_mask |= method_bit(entry.method);
            if (entry.method != req.http_method) {
                continue;
            }

            int score = entry.pattern.specificity_score();
            if (!best_route || score > best_score) {
                best_route = &entry;
                best_score = score;
                best_params = candidate_params;
            }
        }

        if (!best_route) {
            if (path_matched) {
                return dispatch_result{
                    std::unexpected(make_error_code(error_code::method_not_allowed)),
                    true,
                    allowed_methods_mask};
            }
            return dispatch_result{
                std::unexpected(make_error_code(error_code::not_found)), false, 0};
        }

        ctx.params = best_params;
        return dispatch_result{
            best_route->middleware.run(req, ctx, best_route->handler), true, allowed_methods_mask};
    }

    result<response> dispatch(const request& req, request_context& ctx) const {
        return dispatch_with_info(req, ctx).route_response;
    }

private:
    static std::string_view strip_query(std::string_view uri) noexcept {
        size_t pos = uri.find('?');
        if (pos == std::string_view::npos) {
            pos = uri.find('#');
        }
        if (pos == std::string_view::npos) {
            return uri;
        }
        return uri.substr(0, pos);
    }

    std::span<const route_entry> routes_;
};

inline response map_dispatch_error(dispatch_result result) {
    if (result.route_response) {
        return std::move(*result.route_response);
    }

    auto ec = result.route_response.error();
    switch (static_cast<error_code>(ec.value())) {
    case error_code::not_found:
        return response::error(problem_details::not_found());
    case error_code::method_not_allowed: {
        auto res = response::error(problem_details::method_not_allowed());
        auto allow = allow_header_from_mask(result.allowed_methods_mask);
        if (!allow.empty()) {
            res.set_header("Allow", allow);
        }
        return res;
    }
    default:
        return response::error(problem_details::internal_server_error());
    }
}

inline response dispatch_or_problem(const router& r, const request& req, request_context& ctx) {
    return map_dispatch_error(r.dispatch_with_info(req, ctx));
}

// Helper functor to plug router into existing handler harnesses or server code.
class router_handler {
public:
    explicit router_handler(const router& r) : router_(&r) {}

    response operator()(const request& req, monotonic_arena& arena) const {
        request_context ctx{arena};
        return dispatch_or_problem(*router_, req, ctx);
    }

private:
    const router* router_;
};

} // namespace katana::http
