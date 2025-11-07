#pragma once

#include "reactor.hpp"
#include "epoll_reactor.hpp"
#include "metrics.hpp"

#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <functional>

namespace katana {

struct reactor_pool_config {
    uint32_t reactor_count = 0;
    int32_t max_events_per_reactor = 128;
    bool enable_adaptive_balancing = true;
};

class reactor_pool {
public:
    explicit reactor_pool(const reactor_pool_config& config = {});
    ~reactor_pool();

    reactor_pool(const reactor_pool&) = delete;
    reactor_pool& operator=(const reactor_pool&) = delete;

    void start();
    void stop();
    void graceful_stop(std::chrono::milliseconds timeout = std::chrono::milliseconds(30000));
    void wait();

    epoll_reactor& get_reactor(size_t index);
    size_t reactor_count() const noexcept { return reactors_.size(); }

    size_t select_reactor() noexcept;

    metrics_snapshot aggregate_metrics() const;

private:
    struct reactor_context {
        std::unique_ptr<epoll_reactor> reactor;
        std::thread thread;
        std::atomic<bool> running{false};
        std::atomic<uint64_t> load_score{0};
    };

    size_t select_least_loaded() noexcept;

    void worker_thread(reactor_context* ctx);

    std::vector<std::unique_ptr<reactor_context>> reactors_;
    reactor_pool_config config_;
    std::atomic<size_t> next_reactor_{0};
};

} // namespace katana
