#pragma once

#include "result.hpp"
#include "fd_event.hpp"

#include <chrono>
#include <functional>
#include <exception>
#include <string_view>

namespace katana {

using task_fn = std::function<void()>;

struct exception_context {
    std::string_view location;
    std::exception_ptr exception;
    int fd = -1;
};

using exception_handler = std::function<void(const exception_context&)>;

class reactor {
public:
    virtual ~reactor() = default;

    virtual result<void> run() = 0;
    virtual void stop() = 0;

    virtual result<void> register_fd(
        int fd,
        event_type events,
        event_callback callback
    ) = 0;

    virtual result<void> modify_fd(
        int fd,
        event_type events
    ) = 0;

    virtual result<void> unregister_fd(int fd) = 0;

    virtual bool schedule(task_fn task) = 0;

    virtual bool schedule_after(
        std::chrono::milliseconds delay,
        task_fn task
    ) = 0;

    virtual void set_exception_handler(exception_handler handler) = 0;
};

} // namespace katana
