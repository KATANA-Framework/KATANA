#include "katana/core/ring_buffer_queue.hpp"

#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>

using namespace katana;

TEST_CASE("ring_buffer_queue basic operations", "[ring_buffer_queue]") {
    ring_buffer_queue<int> queue(16);

    SECTION("push and pop") {
        REQUIRE(queue.empty());
        REQUIRE(queue.try_push(42));
        REQUIRE(!queue.empty());
        REQUIRE(queue.size() == 1);

        auto val = queue.pop();
        REQUIRE(val.has_value());
        REQUIRE(*val == 42);
        REQUIRE(queue.empty());
    }

    SECTION("multiple push and pop") {
        for (int i = 0; i < 10; ++i) {
            REQUIRE(queue.try_push(i));
        }
        REQUIRE(queue.size() == 10);

        for (int i = 0; i < 10; ++i) {
            auto val = queue.pop();
            REQUIRE(val.has_value());
            REQUIRE(*val == i);
        }
        REQUIRE(queue.empty());
    }

    SECTION("capacity limit") {
        size_t cap = queue.capacity();
        for (size_t i = 0; i < cap; ++i) {
            REQUIRE(queue.try_push(static_cast<int>(i)));
        }
        REQUIRE(!queue.try_push(999));
        REQUIRE(queue.size() == cap);

        queue.pop();
        REQUIRE(queue.try_push(999));
    }
}

TEST_CASE("ring_buffer_queue concurrent operations", "[ring_buffer_queue]") {
    ring_buffer_queue<int> queue(1024);
    const int num_ops = 10000;
    std::atomic<int> sum_produced{0};
    std::atomic<int> sum_consumed{0};

    SECTION("single producer single consumer") {
        std::thread producer([&] {
            for (int i = 0; i < num_ops; ++i) {
                while (!queue.try_push(i)) {
                    std::this_thread::yield();
                }
                sum_produced.fetch_add(i, std::memory_order_relaxed);
            }
        });

        std::thread consumer([&] {
            int consumed = 0;
            while (consumed < num_ops) {
                int val;
                if (queue.try_pop(val)) {
                    sum_consumed.fetch_add(val, std::memory_order_relaxed);
                    ++consumed;
                } else {
                    std::this_thread::yield();
                }
            }
        });

        producer.join();
        consumer.join();

        REQUIRE(queue.empty());
        REQUIRE(sum_produced.load() == sum_consumed.load());
    }

    SECTION("multiple producers multiple consumers") {
        const int num_threads = 4;
        std::vector<std::thread> producers;
        std::vector<std::thread> consumers;
        std::atomic<int> consumed_count{0};

        for (int t = 0; t < num_threads; ++t) {
            producers.emplace_back([&, t] {
                for (int i = 0; i < num_ops / num_threads; ++i) {
                    int val = t * 10000 + i;
                    while (!queue.try_push(val)) {
                        std::this_thread::yield();
                    }
                    sum_produced.fetch_add(val, std::memory_order_relaxed);
                }
            });
        }

        for (int t = 0; t < num_threads; ++t) {
            consumers.emplace_back([&] {
                while (consumed_count.fetch_add(1, std::memory_order_relaxed) < num_ops) {
                    int val;
                    if (queue.try_pop(val)) {
                        sum_consumed.fetch_add(val, std::memory_order_relaxed);
                    } else {
                        consumed_count.fetch_sub(1, std::memory_order_relaxed);
                        std::this_thread::yield();
                    }
                }
            });
        }

        for (auto& t : producers)
            t.join();
        for (auto& t : consumers)
            t.join();

        REQUIRE(sum_produced.load() == sum_consumed.load());
    }
}

TEST_CASE("ring_buffer_queue no allocations", "[ring_buffer_queue]") {
    ring_buffer_queue<int> queue(64);

    for (int i = 0; i < 100; ++i) {
        if (queue.try_push(i)) {
            queue.pop();
        }
    }

    REQUIRE(queue.empty());
}
