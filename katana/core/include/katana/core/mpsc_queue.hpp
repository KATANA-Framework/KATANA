#pragma once

#include <atomic>
#include <memory>
#include <optional>

namespace katana {

template <typename T> class mpsc_queue {
public:
    explicit mpsc_queue(size_t max_size = 0) : max_size_(max_size) {
        auto n = new node();
        head_.store(n, std::memory_order_relaxed);
        tail_ = n;
    }

    ~mpsc_queue() {
        while (auto item = pop()) {
        }
        delete tail_;
    }

    mpsc_queue(const mpsc_queue&) = delete;
    mpsc_queue& operator=(const mpsc_queue&) = delete;

    void push(T value) {
        auto new_node = new node();
        new_node->data = std::move(value);

        node* prev = head_.exchange(new_node, std::memory_order_acq_rel);
        prev->next.store(new_node, std::memory_order_release);

        if (max_size_ > 0) {
            size_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    bool try_push(T value) {
        if (max_size_ > 0) {
            size_t old_size = size_.load(std::memory_order_acquire);
            do {
                if (old_size >= max_size_) {
                    return false;
                }
            } while (!size_.compare_exchange_weak(
                old_size, old_size + 1, std::memory_order_acq_rel, std::memory_order_acquire));
            push_impl(std::move(value));
        } else {
            push(std::move(value));
        }
        return true;
    }

    std::optional<T> pop() {
        node* next = tail_->next.load(std::memory_order_acquire);
        if (!next) {
            return std::nullopt;
        }

        T value = std::move(next->data.value());
        delete tail_;
        tail_ = next;

        if (max_size_ > 0) {
            size_.fetch_sub(1, std::memory_order_relaxed);
        }

        return value;
    }

    [[nodiscard]] bool empty() const {
        return tail_->next.load(std::memory_order_acquire) == nullptr;
    }

    [[nodiscard]] size_t size() const { return size_.load(std::memory_order_relaxed); }

private:
    void push_impl(T value) {
        auto new_node = new node();
        new_node->data = std::move(value);

        node* prev = head_.exchange(new_node, std::memory_order_acq_rel);
        prev->next.store(new_node, std::memory_order_release);
    }

    struct node {
        std::atomic<node*> next{nullptr};
        std::optional<T> data;
    };

    alignas(64) std::atomic<node*> head_;
    alignas(64) node* tail_;
    alignas(64) std::atomic<size_t> size_{0};
    alignas(64) const size_t max_size_; // 0 means unlimited
};

} // namespace katana
