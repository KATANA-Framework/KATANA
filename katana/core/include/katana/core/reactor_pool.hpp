#pragma once

#include "reactor.hpp"
#include "reactor_impl.hpp"
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
    bool enable_thread_pinning = false;
};

class reactor_pool {
private:
    struct reactor_context {
        std::unique_ptr<reactor_impl> reactor;
        std::thread thread;
        std::atomic<bool> running{false};
        std::atomic<uint64_t> load_score{0};
        uint32_t core_id{0};
        int32_t listener_fd{-1};
    };

public:
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = reactor_impl;
        using difference_type = std::ptrdiff_t;
        using pointer = reactor_impl*;
        using reference = reactor_impl&;

        iterator() = default;
        explicit iterator(std::vector<std::unique_ptr<reactor_context>>::iterator it) : it_(it) {}

        reference operator*() const { return *(*it_)->reactor; }
        pointer operator->() const { return (*it_)->reactor.get(); }

        iterator& operator++() { ++it_; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        iterator& operator--() { --it_; return *this; }
        iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

        iterator& operator+=(difference_type n) { it_ += n; return *this; }
        iterator& operator-=(difference_type n) { it_ -= n; return *this; }

        iterator operator+(difference_type n) const { return iterator(it_ + n); }
        iterator operator-(difference_type n) const { return iterator(it_ - n); }

        difference_type operator-(const iterator& other) const { return it_ - other.it_; }

        reference operator[](difference_type n) const { return *it_[n]->reactor; }

        bool operator==(const iterator& other) const { return it_ == other.it_; }
        bool operator!=(const iterator& other) const { return it_ != other.it_; }
        bool operator<(const iterator& other) const { return it_ < other.it_; }
        bool operator>(const iterator& other) const { return it_ > other.it_; }
        bool operator<=(const iterator& other) const { return it_ <= other.it_; }
        bool operator>=(const iterator& other) const { return it_ >= other.it_; }

    private:
        std::vector<std::unique_ptr<reactor_context>>::iterator it_;
    };

    using const_iterator = iterator;

    explicit reactor_pool(const reactor_pool_config& config = {});
    ~reactor_pool();

    reactor_pool(const reactor_pool&) = delete;
    reactor_pool& operator=(const reactor_pool&) = delete;

    void start();
    void stop();
    void graceful_stop(std::chrono::milliseconds timeout = std::chrono::milliseconds(30000));
    void wait();

    reactor_impl& get_reactor(size_t index);
    [[nodiscard]] size_t reactor_count() const noexcept { return reactors_.size(); }
    [[nodiscard]] size_t size() const noexcept { return reactors_.size(); }

    reactor_impl& operator[](size_t index) { return get_reactor(index); }
    const reactor_impl& operator[](size_t index) const {
        return const_cast<reactor_pool*>(this)->get_reactor(index);
    }

    iterator begin() { return iterator(reactors_.begin()); }
    iterator end() { return iterator(reactors_.end()); }
    [[nodiscard]] const_iterator begin() const {
        return const_iterator(const_cast<reactor_pool*>(this)->reactors_.begin());
    }
    [[nodiscard]] const_iterator end() const {
        return const_iterator(const_cast<reactor_pool*>(this)->reactors_.end());
    }

    size_t select_reactor() noexcept;

    [[nodiscard]] metrics_snapshot aggregate_metrics() const;

    template<typename AcceptHandler>
    result<void> start_listening(uint16_t port, AcceptHandler&& handler) {
        for (auto& ctx : reactors_) {
            auto listener_fd = create_listener_socket_reuseport(port);
            if (listener_fd < 0) {
                return std::unexpected(std::error_code(errno, std::system_category()));
            }

            ctx->listener_fd = listener_fd;

            auto& reactor = *ctx->reactor;
            auto res = reactor.register_fd(
                listener_fd,
                event_type::readable | event_type::edge_triggered,
                [handler, listener_fd](event_type events) {
                    if (has_flag(events, event_type::readable)) {
                        handler(listener_fd);
                    }
                }
            );

            if (!res) {
                close(listener_fd);
                return res;
            }
        }
        return {};
    }

private:
    size_t select_least_loaded() noexcept;

    void worker_thread(reactor_context* ctx);

    static int32_t create_listener_socket_reuseport(uint16_t port);

    std::vector<std::unique_ptr<reactor_context>> reactors_;
    reactor_pool_config config_;
    std::atomic<size_t> next_reactor_{0};
};

} // namespace katana
