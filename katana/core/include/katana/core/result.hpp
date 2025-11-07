#pragma once

#include <expected>
#include <system_error>
#include <string_view>

namespace katana {

template <typename T>
using result = std::expected<T, std::error_code>;

enum class error_code : int {
    ok = 0,
    epoll_create_failed = 1,
    epoll_ctl_failed = 2,
    epoll_wait_failed = 3,
    invalid_fd = 4,
    reactor_stopped = 5,
    timeout = 6,
};

class error_category : public std::error_category {
public:
    const char* name() const noexcept override { return "katana"; }

    std::string message(int ev) const override {
        switch (static_cast<error_code>(ev)) {
            case error_code::ok: return "success";
            case error_code::epoll_create_failed: return "epoll_create failed";
            case error_code::epoll_ctl_failed: return "epoll_ctl failed";
            case error_code::epoll_wait_failed: return "epoll_wait failed";
            case error_code::invalid_fd: return "invalid file descriptor";
            case error_code::reactor_stopped: return "reactor is stopped";
            case error_code::timeout: return "operation timed out";
            default: return "unknown error";
        }
    }
};

inline const error_category& get_error_category() {
    static error_category instance;
    return instance;
}

inline std::error_code make_error_code(error_code e) {
    return {static_cast<int>(e), get_error_category()};
}

} // namespace katana

namespace std {
template <>
struct is_error_code_enum<katana::error_code> : true_type {};
}
