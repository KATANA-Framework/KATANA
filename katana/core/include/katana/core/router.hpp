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

    path_params() noexcept : size_(0) {}

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

    void reset() noexcept { size_ = 0; }

private:
    // Uninitialized — only entries_[0..size_) are valid (written before read)
    std::array<param_entry, MAX_PATH_PARAMS> entries_;
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
        const char* ptr = path.data();
        const char* end = ptr + path.size();
        while (ptr < end) {
            // Skip leading slashes
            while (ptr < end && *ptr == '/')
                ++ptr;
            if (ptr >= end)
                break;
            // Find next slash or end
            const char* seg_start = ptr;
            while (ptr < end && *ptr != '/')
                ++ptr;
            if (out.count >= MAX_ROUTE_SEGMENTS) {
                out.overflow = true;
                return out;
            }
            out.parts[out.count++] =
                std::string_view(seg_start, static_cast<size_t>(ptr - seg_start));
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

using handler_fn = inplace_function<result<void>(const request&, request_context&, response&), 160>;
using next_fn = function_ref<result<void>(response&)>;
using middleware_fn =
    inplace_function<result<void>(const request&, request_context&, response&, next_fn), 160>;

struct middleware_chain {
    const middleware_fn* ptr{nullptr};
    size_t size{0};

    [[nodiscard]] bool empty() const noexcept { return size == 0 || ptr == nullptr; }

    result<void>
    run(const request& req, request_context& ctx, const handler_fn& handler, response& out) const {
        if (empty()) {
            return handler(req, ctx, out);
        }

        struct invoker {
            const middleware_fn* ptr;
            size_t size;
            const handler_fn& terminal;
            const request& req;
            request_context& ctx;

            result<void> call(size_t index, response& out) const {
                if (index >= size) {
                    return terminal(req, ctx, out);
                }
                const middleware_fn& mw = ptr[index];
                auto next_lambda = [this, index](response& next_out) -> result<void> {
                    return call(index + 1, next_out);
                };
                next_fn next{next_lambda};
                return mw(req, ctx, out, next);
            }
        };

        invoker inv{ptr, size, handler, req, ctx};
        return inv.call(0, out);
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
    std::error_code route_error{};
    bool has_error{false};
    bool path_matched{false};
    uint32_t allowed_methods_mask{0};
};

class router {
public:
    explicit router(std::span<const route_entry> routes) : routes_(routes) {}

    dispatch_result
    dispatch_with_info(const request& req, request_context& ctx, response& out) const {
        auto path = strip_query(req.uri);
        auto split = path_pattern::split_path(path);
        if (split.overflow) {
            return dispatch_result{make_error_code(error_code::not_found), true, false, 0};
        }
        std::span<const std::string_view> path_segments(split.parts.data(), split.count);

        const route_entry* best_route = nullptr;
        path_params best_params;
        int best_score = -1;
        bool path_matched = false;
        uint32_t allowed_methods_mask = 0;

        // Two-phase dispatch for better cache locality:
        // Phase 1: Find matching method routes (fast path)
        // Phase 2: Only collect allowed methods if no exact match found

        // Phase 1: Try to find an exact method match first
        for (const auto& entry : routes_) {
            // Fast reject: segment count must match
            if (entry.pattern.segment_count != split.count) {
                continue;
            }

            // Fast reject: method must match for phase 1
            if (entry.method != req.http_method) {
                continue;
            }

            path_params candidate_params;
            if (!entry.pattern.match_segments(path_segments, split.count, candidate_params)) {
                continue;
            }

            // Found a match!
            int score = entry.pattern.specificity_score();
            if (!best_route || score > best_score) {
                best_route = &entry;
                best_score = score;
                best_params = candidate_params;
            }
        }

        // If we found a matching route, return immediately
        if (best_route) {
            ctx.params = best_params;
            auto route_result = best_route->middleware.run(req, ctx, best_route->handler, out);
            if (!route_result) {
                return dispatch_result{route_result.error(), true, true, 0};
            }
            return dispatch_result{};
        }

        // Phase 2: No method match - check if path exists with other methods
        for (const auto& entry : routes_) {
            if (entry.pattern.segment_count != split.count) {
                continue;
            }

            path_params candidate_params;
            if (!entry.pattern.match_segments(path_segments, split.count, candidate_params)) {
                continue;
            }

            path_matched = true;
            allowed_methods_mask |= method_bit(entry.method);
        }

        if (path_matched) {
            return dispatch_result{
                make_error_code(error_code::method_not_allowed), true, true, allowed_methods_mask};
        }
        return dispatch_result{make_error_code(error_code::not_found), true, false, 0};
    }

    result<response> dispatch(const request& req, request_context& ctx) const {
        response out;
        auto dispatch_info = dispatch_with_info(req, ctx, out);
        if (dispatch_info.has_error) {
            return std::unexpected(dispatch_info.route_error);
        }
        return out;
    }

    result<void> dispatch(const request& req, request_context& ctx, response& out) const {
        auto dispatch_info = dispatch_with_info(req, ctx, out);
        if (dispatch_info.has_error) {
            return std::unexpected(dispatch_info.route_error);
        }
        return {};
    }

private:
    static std::string_view strip_query(std::string_view uri) noexcept {
        // Scan for '?' or '#' in a single pass (common case: no query string)
        for (size_t i = 0; i < uri.size(); ++i) {
            char c = uri[i];
            if (c == '?' || c == '#') {
                return uri.substr(0, i);
            }
        }
        return uri;
    }

    std::span<const route_entry> routes_;
};

inline void map_route_error(std::error_code ec, response& out) {
    switch (static_cast<error_code>(ec.value())) {
    case error_code::not_found:
        respond::into(out).problem(problem_details::not_found());
        return;
    case error_code::method_not_allowed:
        respond::into(out).problem(problem_details::method_not_allowed());
        return;
    default:
        respond::into(out).problem(problem_details::internal_server_error());
        return;
    }
}

inline void map_dispatch_error(const dispatch_result& result, response& out) {
    auto ec = result.route_error;
    switch (static_cast<error_code>(ec.value())) {
    case error_code::method_not_allowed: {
        respond::into(out).problem(problem_details::method_not_allowed());
        auto allow = allow_header_from_mask(result.allowed_methods_mask);
        if (!allow.empty()) {
            out.set_header("Allow", allow);
        }
        return;
    }
    default:
        map_route_error(ec, out);
        return;
    }
}

inline void
dispatch_or_problem(const router& r, const request& req, request_context& ctx, response& out) {
    auto dispatch_info = r.dispatch_with_info(req, ctx, out);
    if (dispatch_info.has_error) {
        map_dispatch_error(dispatch_info, out);
    }
}

inline response dispatch_or_problem(const router& r, const request& req, request_context& ctx) {
    response out;
    dispatch_or_problem(r, req, ctx, out);
    return out;
}

// Helper functor to plug router into existing handler harnesses or server code.
class router_handler {
public:
    explicit router_handler(const router& r) : router_(&r) {}

    response operator()(const request& req, monotonic_arena& arena) const {
        request_context ctx{arena};
        response out;
        dispatch_or_problem(*router_, req, ctx, out);
        return out;
    }

private:
    const router* router_;
};

} // namespace katana::http
