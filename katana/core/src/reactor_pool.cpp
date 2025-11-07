#include "katana/core/reactor_pool.hpp"
#include "katana/core/cpu_info.hpp"

#include <iostream>

namespace katana {

reactor_pool::reactor_pool(const reactor_pool_config& config)
    : config_(config)
{
    if (config_.reactor_count == 0) {
        config_.reactor_count = cpu_info::core_count();
    }

    reactors_.reserve(config_.reactor_count);

    for (uint32_t i = 0; i < config_.reactor_count; ++i) {
        auto ctx = std::make_unique<reactor_context>();
        ctx->reactor = std::make_unique<epoll_reactor>(config_.max_events_per_reactor);
        ctx->core_id = i;
        reactors_.push_back(std::move(ctx));
    }
}

reactor_pool::~reactor_pool() {
    stop();
    wait();
}

void reactor_pool::start() {
    for (auto& ctx : reactors_) {
        ctx->running.store(true, std::memory_order_release);
        ctx->thread = std::thread(&reactor_pool::worker_thread, this, ctx.get());
    }
}

void reactor_pool::stop() {
    for (auto& ctx : reactors_) {
        ctx->running.store(false, std::memory_order_release);
        ctx->reactor->stop();
    }
}

void reactor_pool::graceful_stop(std::chrono::milliseconds timeout) {
    for (auto& ctx : reactors_) {
        ctx->running.store(false, std::memory_order_release);
        ctx->reactor->graceful_stop(timeout);
    }
}

void reactor_pool::wait() {
    for (auto& ctx : reactors_) {
        if (ctx->thread.joinable()) {
            ctx->thread.join();
        }
    }
}

epoll_reactor& reactor_pool::get_reactor(size_t index) {
    return *reactors_[index % reactors_.size()]->reactor;
}

size_t reactor_pool::select_reactor() noexcept {
    if (config_.enable_adaptive_balancing) {
        return select_least_loaded();
    }
    return next_reactor_.fetch_add(1, std::memory_order_relaxed) % reactors_.size();
}

size_t reactor_pool::select_least_loaded() noexcept {
    if (reactors_.empty()) {
        return 0;
    }

    size_t min_load_idx = 0;
    uint64_t min_load = UINT64_MAX;

    for (size_t i = 0; i < reactors_.size(); ++i) {
        auto metrics = reactors_[i]->reactor->metrics().snapshot();

        // Load score: pending tasks + active events + rejected tasks * 10
        uint64_t load = (metrics.tasks_scheduled - metrics.tasks_executed) +
                       metrics.fd_events_processed +
                       (metrics.tasks_rejected * 10);

        reactors_[i]->load_score.store(load, std::memory_order_relaxed);

        if (load < min_load) {
            min_load = load;
            min_load_idx = i;
        }
    }

    return min_load_idx;
}

metrics_snapshot reactor_pool::aggregate_metrics() const {
    metrics_snapshot total;
    for (const auto& ctx : reactors_) {
        total += ctx->reactor->metrics().snapshot();
    }
    return total;
}

void reactor_pool::worker_thread(reactor_context* ctx) {
    if (config_.enable_thread_pinning) {
        if (!cpu_info::pin_thread_to_core(ctx->core_id)) {
            std::cerr << "[reactor_pool] Warning: Failed to pin thread to core "
                      << ctx->core_id << "\n";
        }
    }

    auto result = ctx->reactor->run();
    if (!result) {
        std::cerr << "[reactor_pool] Reactor error: " << result.error().message() << "\n";
    }
}

} // namespace katana
