#include "katana/core/epoll_reactor.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace katana {

namespace {

constexpr uint32_t to_epoll_events(event_type events) noexcept {
    uint32_t result = 0;

    if (has_flag(events, event_type::readable)) {
        result |= EPOLLIN;
    }
    if (has_flag(events, event_type::writable)) {
        result |= EPOLLOUT;
    }
    if (has_flag(events, event_type::edge_triggered)) {
        result |= EPOLLET;
    }

    return result;
}

constexpr event_type from_epoll_events(uint32_t events) noexcept {
    event_type result = event_type::none;

    if (events & EPOLLIN) {
        result = result | event_type::readable;
    }
    if (events & EPOLLOUT) {
        result = result | event_type::writable;
    }
    if (events & EPOLLERR) {
        result = result | event_type::error;
    }
    if (events & EPOLLHUP) {
        result = result | event_type::hup;
    }

    return result;
}

} // namespace

epoll_reactor::epoll_reactor(int max_events)
    : epoll_fd_(-1)
    , max_events_(max_events)
    , running_(false)
{
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ < 0) {
        throw std::system_error(
            errno,
            std::system_category(),
            "epoll_create1 failed"
        );
    }
}

epoll_reactor::~epoll_reactor() {
    if (epoll_fd_ >= 0) {
        close(epoll_fd_);
    }
}

result<void> epoll_reactor::run() {
    if (running_.exchange(true)) {
        return std::unexpected(make_error_code(error_code::reactor_stopped));
    }

    while (running_.load(std::memory_order_relaxed)) {
        process_timers();
        process_tasks();

        int timeout_ms = calculate_timeout();
        auto res = process_events(timeout_ms);
        if (!res) {
            running_ = false;
            return res;
        }
    }

    return {};
}

void epoll_reactor::stop() {
    running_.store(false, std::memory_order_relaxed);
}

result<void> epoll_reactor::register_fd(
    int fd,
    event_type events,
    event_callback callback
) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    epoll_event ev{};
    ev.events = to_epoll_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    fd_states_[fd] = fd_state{std::move(callback), events};
    return {};
}

result<void> epoll_reactor::modify_fd(int fd, event_type events) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto it = fd_states_.find(fd);
    if (it == fd_states_.end()) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    epoll_event ev{};
    ev.events = to_epoll_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    it->second.events = events;
    return {};
}

result<void> epoll_reactor::unregister_fd(int fd) {
    if (fd < 0) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    auto it = fd_states_.find(fd);
    if (it == fd_states_.end()) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    fd_states_.erase(it);
    return {};
}

void epoll_reactor::schedule(task_fn task) {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    pending_tasks_.push_back(std::move(task));
}

void epoll_reactor::schedule_after(
    std::chrono::milliseconds delay,
    task_fn task
) {
    auto deadline = std::chrono::steady_clock::now() + delay;
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    timers_.push(timer_entry{deadline, std::move(task)});
}

result<void> epoll_reactor::process_events(int timeout_ms) {
    std::vector<epoll_event> events(max_events_);

    int nfds = epoll_wait(epoll_fd_, events.data(), max_events_, timeout_ms);

    if (nfds < 0) {
        if (errno == EINTR) {
            return {};
        }
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    for (int i = 0; i < nfds; ++i) {
        int fd = events[i].data.fd;
        auto it = fd_states_.find(fd);
        if (it != fd_states_.end()) {
            event_type ev = from_epoll_events(events[i].events);
            it->second.callback(ev);
        }
    }

    return {};
}

void epoll_reactor::process_tasks() {
    std::vector<task_fn> tasks;
    {
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        tasks.swap(pending_tasks_);
    }

    for (auto& task : tasks) {
        task();
    }
}

void epoll_reactor::process_timers() {
    auto now = std::chrono::steady_clock::now();
    std::vector<task_fn> ready_tasks;

    {
        std::lock_guard<std::mutex> lock(tasks_mutex_);

        while (!timers_.empty() && timers_.top().deadline <= now) {
            ready_tasks.push_back(std::move(timers_.top().task));
            timers_.pop();
        }
    }

    for (auto& task : ready_tasks) {
        task();
    }
}

int epoll_reactor::calculate_timeout() const {
    std::lock_guard<std::mutex> lock(tasks_mutex_);

    if (!pending_tasks_.empty()) {
        return 0;
    }

    if (timers_.empty()) {
        return 100;
    }

    auto now = std::chrono::steady_clock::now();
    auto deadline = timers_.top().deadline;

    if (deadline <= now) {
        return 0;
    }

    auto timeout = std::chrono::duration_cast<std::chrono::milliseconds>(
        deadline - now
    );

    return static_cast<int>(std::min<int64_t>(timeout.count(), 100));
}

} // namespace katana
