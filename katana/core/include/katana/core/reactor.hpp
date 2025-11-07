#pragma once

#include "result.hpp"
#include "fd_event.hpp"

#include <chrono>
#include <functional>

namespace katana {

using task_fn = std::function<void()>;

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

    virtual void schedule(task_fn task) = 0;

    virtual void schedule_after(
        std::chrono::milliseconds delay,
        task_fn task
    ) = 0;
};

} // namespace katana
