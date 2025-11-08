#pragma once

#include "reactor.hpp"
#include "metrics.hpp"
#include "mpsc_queue.hpp"
#include "wheel_timer.hpp"

#include <sys/epoll.h>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <queue>

namespace katana {

class epoll_reactor : public reactor {
public:
    static constexpr size_t DEFAULT_MAX_PENDING_TASKS = 10000;

    explicit epoll_reactor(int32_t max_events = 128, size_t max_pending_tasks = DEFAULT_MAX_PENDING_TASKS);
    ~epoll_reactor() override;

    epoll_reactor(const epoll_reactor&) = delete;
    epoll_reactor& operator=(const epoll_reactor&) = delete;

    result<void> run() override;
    void stop() override;
    void graceful_stop(std::chrono::milliseconds timeout) override;

    result<void> register_fd(
        int32_t fd,
        event_type events,
        event_callback callback
    ) override;

    result<void> register_fd_with_timeout(
        int32_t fd,
        event_type events,
        event_callback callback,
        const timeout_config& config
    ) override;

    result<void> modify_fd(
        int32_t fd,
        event_type events
    ) override;

    result<void> unregister_fd(int32_t fd) override;

    void refresh_fd_timeout(int32_t fd) override;

    bool schedule(task_fn task) override;

    bool schedule_after(
        std::chrono::milliseconds delay,
        task_fn task
    ) override;

    void set_exception_handler(exception_handler handler) override;

    const reactor_metrics& metrics() const noexcept { return metrics_; }

private:
    struct fd_state {
        event_callback callback;
        event_type events;
        timeout_config timeouts;
        wheel_timer<>::timeout_id timeout_id = 0;
        std::chrono::steady_clock::time_point last_activity;
        bool has_timeout = false;
    };

    struct timer_entry {
        std::chrono::steady_clock::time_point deadline;
        task_fn task;

        bool operator>(const timer_entry& other) const {
            return deadline > other.deadline;
        }
    };

    result<void> process_events(int32_t timeout_ms);
    void process_tasks();
    void process_timers();
    void process_wheel_timer();
    int32_t calculate_timeout() const;
    void handle_exception(std::string_view location, std::exception_ptr ex, int32_t fd = -1) noexcept;
    void setup_fd_timeout(int32_t fd, fd_state& state);
    void cancel_fd_timeout(fd_state& state);

    int32_t epoll_fd_;
    int32_t wakeup_fd_;
    int32_t max_events_;
    std::atomic<bool> running_;
    std::atomic<bool> graceful_shutdown_;
    std::chrono::steady_clock::time_point graceful_shutdown_deadline_;

    std::vector<fd_state> fd_states_;
    mpsc_queue<task_fn> pending_tasks_;
    std::priority_queue<timer_entry, std::vector<timer_entry>, std::greater<timer_entry>> timers_;
    mpsc_queue<timer_entry> pending_timers_;

    exception_handler exception_handler_;
    reactor_metrics metrics_;

    wheel_timer<> wheel_timer_;
    std::chrono::steady_clock::time_point last_wheel_tick_;

    std::vector<epoll_event> events_buffer_;
};

} // namespace katana
