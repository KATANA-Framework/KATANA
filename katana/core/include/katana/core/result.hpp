#pragma once

#if __has_include(<expected>)
#include <expected>
#endif
#include <string>
#include <string_view>
#include <system_error>

#ifndef __cpp_lib_expected
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

namespace std {

template <class E> class unexpected {
public:
    using error_type = E;

    constexpr explicit unexpected(const E& error) : error_(error) {}
    constexpr explicit unexpected(E&& error) : error_(std::move(error)) {}

    [[nodiscard]] constexpr const E& error() const& noexcept { return error_; }
    constexpr E& error() & noexcept { return error_; }
    constexpr E&& error() && noexcept { return std::move(error_); }

private:
    E error_;
};

template <class E> unexpected(E) -> unexpected<E>;

template <class T, class E> class expected {
public:
    using value_type = T;
    using error_type = E;

    constexpr expected(const T& value) : storage_(value) {}
    constexpr expected(T&& value) : storage_(std::in_place_index<0>, std::move(value)) {}
    constexpr expected(unexpected<E> error)
        : storage_(std::in_place_index<1>, std::move(error.error())) {}

    template <class... Args>
    constexpr explicit expected(std::in_place_t, Args&&... args)
        : storage_(std::in_place_index<0>, std::forward<Args>(args)...) {}

    constexpr expected(const expected&) = default;
    constexpr expected(expected&&) = default;
    constexpr expected& operator=(const expected&) = default;
    constexpr expected& operator=(expected&&) = default;

    [[nodiscard]] constexpr bool has_value() const noexcept { return storage_.index() == 0; }
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return has_value(); }

    constexpr T& operator*() & { return value(); }
    constexpr const T& operator*() const& { return value(); }
    constexpr T&& operator*() && { return std::move(*this).value(); }
    constexpr const T&& operator*() const&& { return std::move(*this).value(); }

    constexpr T* operator->() { return &value(); }
    constexpr const T* operator->() const { return &value(); }

    constexpr T& value() & {
        if (!has_value())
            throw std::logic_error("bad expected access");
        return std::get<0>(storage_);
    }

    constexpr const T& value() const& {
        if (!has_value())
            throw std::logic_error("bad expected access");
        return std::get<0>(storage_);
    }

    constexpr T&& value() && {
        if (!has_value())
            throw std::logic_error("bad expected access");
        return std::get<0>(std::move(storage_));
    }

    constexpr const E& error() const& {
        if (has_value())
            throw std::logic_error("bad expected error access");
        return std::get<1>(storage_);
    }

    constexpr E& error() & {
        if (has_value())
            throw std::logic_error("bad expected error access");
        return std::get<1>(storage_);
    }

    constexpr E&& error() && {
        if (has_value())
            throw std::logic_error("bad expected error access");
        return std::get<1>(std::move(storage_));
    }

    template <class F> constexpr auto and_then(F&& f) & {
        using return_type = std::invoke_result_t<F, T&>;
        if (has_value())
            return std::invoke(std::forward<F>(f), value());
        return return_type(unexpected<E>(error()));
    }

    template <class F> constexpr auto and_then(F&& f) const& {
        using return_type = std::invoke_result_t<F, const T&>;
        if (has_value())
            return std::invoke(std::forward<F>(f), value());
        return return_type(unexpected<E>(error()));
    }

    template <class F> constexpr auto and_then(F&& f) && {
        using return_type = std::invoke_result_t<F, T&&>;
        if (has_value())
            return std::invoke(std::forward<F>(f), std::move(*this).value());
        return return_type(unexpected<E>(std::move(*this).error()));
    }

    template <class F> constexpr expected or_else(F&& f) & {
        if (has_value())
            return *this;
        return std::invoke(std::forward<F>(f), error());
    }

    template <class F> constexpr expected or_else(F&& f) const& {
        if (has_value())
            return *this;
        return std::invoke(std::forward<F>(f), error());
    }

    template <class F> constexpr expected or_else(F&& f) && {
        if (has_value())
            return expected(std::in_place, std::move(*this).value());
        return std::invoke(std::forward<F>(f), std::move(*this).error());
    }

private:
    std::variant<T, E> storage_;
};

template <class E> class expected<void, E> {
public:
    using value_type = void;
    using error_type = E;

    constexpr expected() noexcept = default;
    constexpr expected(unexpected<E> error) : has_value_(false), error_(std::move(error.error())) {}

    constexpr expected(const expected&) = default;
    constexpr expected(expected&&) = default;
    constexpr expected& operator=(const expected&) = default;
    constexpr expected& operator=(expected&&) = default;

    [[nodiscard]] constexpr bool has_value() const noexcept { return has_value_; }
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return has_value(); }

    constexpr void value() const {
        if (!has_value_)
            throw std::logic_error("bad expected access");
    }

    [[nodiscard]] constexpr const E& error() const& {
        if (has_value_)
            throw std::logic_error("bad expected error access");
        return error_;
    }

    constexpr E& error() & {
        if (has_value_)
            throw std::logic_error("bad expected error access");
        return error_;
    }

    constexpr E&& error() && {
        if (has_value_)
            throw std::logic_error("bad expected error access");
        return std::move(error_);
    }

    template <class F> constexpr auto and_then(F&& f) & {
        using return_type = std::invoke_result_t<F>;
        if (has_value_)
            return std::invoke(std::forward<F>(f));
        return return_type(unexpected<E>(error_));
    }

    template <class F> constexpr auto and_then(F&& f) const& {
        using return_type = std::invoke_result_t<F>;
        if (has_value_)
            return std::invoke(std::forward<F>(f));
        return return_type(unexpected<E>(error_));
    }

    template <class F> constexpr auto and_then(F&& f) && {
        using return_type = std::invoke_result_t<F>;
        if (has_value_)
            return std::invoke(std::forward<F>(f));
        return return_type(unexpected<E>(std::move(error_)));
    }

    template <class F> constexpr expected or_else(F&& f) & {
        if (has_value_)
            return *this;
        return std::invoke(std::forward<F>(f), error_);
    }

    template <class F> constexpr expected or_else(F&& f) const& {
        if (has_value_)
            return *this;
        return std::invoke(std::forward<F>(f), error_);
    }

    template <class F> constexpr expected or_else(F&& f) && {
        if (has_value_)
            return expected();
        return std::invoke(std::forward<F>(f), std::move(error_));
    }

private:
    bool has_value_ = true;
    E error_{};
};

} // namespace std

#endif // __cpp_lib_expected

namespace katana {

template <typename T> using result = std::expected<T, std::error_code>;

enum class error_code : int {
    ok = 0,
    epoll_create_failed = 1,
    epoll_ctl_failed = 2,
    epoll_wait_failed = 3,
    invalid_fd = 4,
    reactor_stopped = 5,
    timeout = 6,
    not_found = 7,
    method_not_allowed = 8,
    openapi_parse_error = 9,
    openapi_invalid_spec = 10,
};

class error_category : public std::error_category {
public:
    [[nodiscard]] const char* name() const noexcept override { return "katana"; }

    [[nodiscard]] std::string message(int ev) const override {
        using ec = error_code;
        switch (static_cast<ec>(ev)) {
        case ec::ok:
            return "success";
        case ec::epoll_create_failed:
            return "epoll_create failed";
        case ec::epoll_ctl_failed:
            return "epoll_ctl failed";
        case ec::epoll_wait_failed:
            return "epoll_wait failed";
        case ec::invalid_fd:
            return "invalid file descriptor";
        case ec::reactor_stopped:
            return "reactor is stopped";
        case ec::timeout:
            return "operation timed out";
        case ec::not_found:
            return "route not found";
        case ec::method_not_allowed:
            return "method not allowed";
        case ec::openapi_parse_error:
            return "failed to parse OpenAPI document";
        case ec::openapi_invalid_spec:
            return "invalid or unsupported OpenAPI document";
        default:
            return "unknown error";
        }
    }
};

inline const error_category& get_error_category() {
    static error_category const instance;
    return instance;
}

inline std::error_code make_error_code(error_code e) {
    return {static_cast<int>(e), get_error_category()};
}

} // namespace katana

namespace std {
template <> struct is_error_code_enum<katana::error_code> : true_type {};
} // namespace std
