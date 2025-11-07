#pragma once

#include <atomic>
#include <memory>
#include <optional>

namespace katana {

template <typename T>
class mpsc_queue {
public:
    mpsc_queue() {
        auto n = new node();
        head_.store(n, std::memory_order_relaxed);
        tail_ = n;
    }

    ~mpsc_queue() {
        while (auto item = pop()) {}
        delete tail_;
    }

    mpsc_queue(const mpsc_queue&) = delete;
    mpsc_queue& operator=(const mpsc_queue&) = delete;

    void push(T value) {
        auto new_node = new node();
        new_node->data = std::move(value);

        node* prev = head_.exchange(new_node, std::memory_order_acq_rel);
        prev->next.store(new_node, std::memory_order_release);
    }

    std::optional<T> pop() {
        node* next = tail_->next.load(std::memory_order_acquire);
        if (!next) {
            return std::nullopt;
        }

        T value = std::move(next->data.value());
        delete tail_;
        tail_ = next;
        return value;
    }

    bool empty() const {
        return tail_->next.load(std::memory_order_acquire) == nullptr;
    }

private:
    struct node {
        std::atomic<node*> next{nullptr};
        std::optional<T> data;
    };

    alignas(64) std::atomic<node*> head_;
    alignas(64) node* tail_;
};

} // namespace katana
