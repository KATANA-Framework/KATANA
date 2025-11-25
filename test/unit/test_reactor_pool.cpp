#include "katana/core/cpu_info.hpp"
#include "katana/core/reactor_pool.hpp"

#include <atomic>
#include <gtest/gtest.h>

TEST(ReactorPoolTest, CreatePool) {
    katana::reactor_pool_config config;
    config.reactor_count = 4;

    katana::reactor_pool pool(config);
    EXPECT_EQ(pool.reactor_count(), 4);
}

TEST(ReactorPoolTest, DefaultCoreCount) {
    katana::reactor_pool_config config;

    katana::reactor_pool pool(config);
    EXPECT_EQ(pool.reactor_count(), katana::cpu_info::core_count());
}

TEST(ReactorPoolTest, StartStop) {
    katana::reactor_pool_config config;
    config.reactor_count = 2;

    katana::reactor_pool pool(config);

    std::atomic<int> counter{0};

    for (size_t i = 0; i < pool.reactor_count(); ++i) {
        auto& reactor = pool.get_reactor(i);
        reactor.schedule([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
    }

    pool.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pool.stop();
    pool.wait();

    EXPECT_EQ(counter.load(), 2);
}

TEST(ReactorPoolTest, RoundRobinSelection) {
    katana::reactor_pool_config config;
    config.reactor_count = 4;

    katana::reactor_pool pool(config);

    auto idx1 = pool.select_reactor();
    auto idx2 = pool.select_reactor();
    auto idx3 = pool.select_reactor();
    auto idx4 = pool.select_reactor();
    auto idx5 = pool.select_reactor();

    EXPECT_LT(idx1, pool.reactor_count());
    EXPECT_LT(idx2, pool.reactor_count());
    EXPECT_LT(idx3, pool.reactor_count());
    EXPECT_LT(idx4, pool.reactor_count());
    EXPECT_EQ(idx5 % pool.reactor_count(), idx1 % pool.reactor_count());
}

TEST(ReactorPoolTest, MetricsAggregation) {
    katana::reactor_pool_config config;
    config.reactor_count = 2;

    katana::reactor_pool pool(config);

    for (size_t i = 0; i < pool.reactor_count(); ++i) {
        auto& reactor = pool.get_reactor(i);
        for (int j = 0; j < 5; ++j) {
            reactor.schedule([]() {});
        }
    }

    pool.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pool.stop();
    pool.wait();

    auto metrics = pool.aggregate_metrics();
    EXPECT_EQ(metrics.tasks_scheduled, 10);
    EXPECT_EQ(metrics.tasks_executed, 10);
}

TEST(ReactorPoolTest, IsolatedState) {
    katana::reactor_pool_config config;
    config.reactor_count = 2;

    katana::reactor_pool pool(config);

    std::atomic<int> counter0{0};
    std::atomic<int> counter1{0};

    auto& reactor0 = pool.get_reactor(0);
    auto& reactor1 = pool.get_reactor(1);

    for (int i = 0; i < 10; ++i) {
        reactor0.schedule([&counter0]() { counter0.fetch_add(1, std::memory_order_relaxed); });
        reactor1.schedule([&counter1]() { counter1.fetch_add(1, std::memory_order_relaxed); });
    }

    pool.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pool.stop();
    pool.wait();

    EXPECT_EQ(counter0.load(), 10);
    EXPECT_EQ(counter1.load(), 10);
}
