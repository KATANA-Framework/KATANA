#pragma once

#include "reactor.hpp"
#include "metrics.hpp"

#include <atomic>
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>

namespace katana {

class epoll_reactor : public reactor {
public:
    explicit epoll_reactor(int max_events = 128);
    ~epoll_reactor() override;

    epoll_reactor(const epoll_reactor&) = delete;
    epoll_reactor& operator=(const epoll_reactor&) = delete;

    result<void> run() override;
    void stop() override;

    result<void> register_fd(
        int fd,
        event_type events,
        event_callback callback
    ) override;

    result<void> modify_fd(
        int fd,
        event_type events
    ) override;

    result<void> unregister_fd(int fd) override;

    void schedule(task_fn task) override;

    void schedule_after(
        std::chrono::milliseconds delay,
        task_fn task
    ) override;

    void set_exception_handler(exception_handler handler) override;

    const reactor_metrics& metrics() const noexcept { return metrics_; }

private:
    struct fd_state {
        event_callback callback;
        event_type events;
    };

    struct timer_entry {
        std::chrono::steady_clock::time_point deadline;
        task_fn task;

        bool operator>(const timer_entry& other) const {
            return deadline > other.deadline;
        }
    };

    result<void> process_events(int timeout_ms);
    void process_tasks();
    void process_timers();
    int calculate_timeout() const;
    void handle_exception(std::string_view location, std::exception_ptr ex, int fd = -1) noexcept;

    int epoll_fd_;
    int max_events_;
    std::atomic<bool> running_;

    std::unordered_map<int, fd_state> fd_states_;
    std::vector<task_fn> pending_tasks_;
    std::priority_queue<timer_entry, std::vector<timer_entry>, std::greater<timer_entry>> timers_;

    mutable std::mutex tasks_mutex_;
    exception_handler exception_handler_;
    reactor_metrics metrics_;
};

} // namespace katana
