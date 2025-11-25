#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <queue>
#include <unordered_set>
#include <vector>

namespace katana::test_support {

// Deterministic event loop with manual time control for async tests.
class VirtualEventLoop {
public:
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;
    using duration = clock::duration;

    VirtualEventLoop() = default;

    time_point now() const noexcept { return now_; }
    size_t pending() const noexcept { return tasks_.size(); }
    bool empty() const noexcept { return tasks_.empty(); }

    // Schedule a task at the current time.
    size_t post(std::function<void()> fn) { return post_at(now_, std::move(fn)); }

    // Schedule a task after the given delay.
    size_t post_after(duration delay, std::function<void()> fn) {
        return post_at(now_ + delay, std::move(fn));
    }

    // Schedule a task at an absolute virtual time.
    size_t post_at(time_point tp, std::function<void()> fn) {
        const size_t id = next_id_++;
        tasks_.push(Task{tp, id, std::move(fn)});
        return id;
    }

    // Cancel a scheduled task by id. Returns true if it was pending.
    bool cancel(size_t id) { return cancelled_.insert(id).second; }

    // Advance virtual time and run all tasks that are due.
    size_t advance(duration delta) {
        now_ += delta;
        return run_ready();
    }

    // Jump to the time of the next task (if any) and run it/them.
    size_t advance_to_next() {
        if (tasks_.empty()) {
            return 0;
        }
        const auto next_time = tasks_.top().when;
        if (next_time > now_) {
            now_ = next_time;
        }
        return run_ready();
    }

    // Run all tasks whose scheduled time is <= now().
    size_t run_ready() {
        size_t executed = 0;
        while (!tasks_.empty() && tasks_.top().when <= now_) {
            Task task = tasks_.top();
            tasks_.pop();
            if (cancelled_.erase(task.id) > 0) {
                continue;
            }
            task.fn();
            ++executed;
        }
        return executed;
    }

    // Drain the queue by jumping time to each task in order.
    size_t run_all() {
        size_t executed = 0;
        while (!tasks_.empty()) {
            executed += advance_to_next();
        }
        return executed;
    }

    // Run all ready tasks without advancing time further.
    size_t run_until_idle() {
        size_t total = 0;
        size_t ran = 0;
        do {
            ran = run_ready();
            total += ran;
        } while (ran > 0);
        return total;
    }

private:
    struct Task {
        time_point when;
        size_t id;
        std::function<void()> fn;
    };

    struct Compare {
        bool operator()(const Task& a, const Task& b) const noexcept {
            if (a.when == b.when) {
                return a.id > b.id;
            }
            return a.when > b.when; // min-heap by time
        }
    };

    time_point now_{time_point(duration::zero())};
    size_t next_id_{0};
    std::priority_queue<Task, std::vector<Task>, Compare> tasks_;
    std::unordered_set<size_t> cancelled_;
};

} // namespace katana::test_support
