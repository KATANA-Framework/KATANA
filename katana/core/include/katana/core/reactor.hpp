#pragma once

#include "result.hpp"
#include "fd_event.hpp"
#include "inplace_function.hpp"

#include <chrono>
#include <functional>
#include <exception>
#include <string_view>

namespace katana {

using task_fn = inplace_function<void(), 128>;

struct exception_context {
    std::string_view location;
    std::exception_ptr exception;
    int32_t fd = -1;
};

using exception_handler = std::function<void(const exception_context&)>;

struct timeout_config {
    std::chrono::milliseconds read_timeout{30000};
    std::chrono::milliseconds write_timeout{30000};
    std::chrono::milliseconds idle_timeout{60000};
};

class reactor {
public:
    virtual ~reactor() = default;

    virtual result<void> run() = 0;
    virtual void stop() = 0;
    virtual void graceful_stop(std::chrono::milliseconds timeout = std::chrono::milliseconds(30000)) = 0;

    virtual result<void> register_fd(
        int32_t fd,
        event_type events,
        event_callback callback
    ) = 0;

    virtual result<void> register_fd_with_timeout(
        int32_t fd,
        event_type events,
        event_callback callback,
        const timeout_config& config
    ) = 0;

    virtual result<void> modify_fd(
        int32_t fd,
        event_type events
    ) = 0;

    virtual result<void> unregister_fd(int32_t fd) = 0;

    virtual void refresh_fd_timeout(int32_t fd) = 0;

    virtual bool schedule(task_fn task) = 0;

    virtual bool schedule_after(
        std::chrono::milliseconds delay,
        task_fn task
    ) = 0;

    virtual void set_exception_handler(exception_handler handler) = 0;
};

} // namespace katana
