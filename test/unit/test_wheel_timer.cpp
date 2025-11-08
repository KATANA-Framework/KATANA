#include "katana/core/wheel_timer.hpp"

#include <gtest/gtest.h>
#include <thread>

using namespace katana;

TEST(WheelTimer, AddTimeout) {
    wheel_timer<> timer;

    bool called = false;
    auto id = timer.add(std::chrono::milliseconds(100), [&]() { called = true; });

    EXPECT_GT(id, 0);
    EXPECT_FALSE(called);

    timer.tick();
    EXPECT_FALSE(called);

    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    timer.tick();
    EXPECT_TRUE(called);
}

TEST(WheelTimer, CancelTimeout) {
    wheel_timer<> timer;

    bool called = false;
    auto id = timer.add(std::chrono::milliseconds(100), [&]() { called = true; });

    EXPECT_TRUE(timer.cancel(id));
    EXPECT_FALSE(called);

    timer.tick();
    EXPECT_FALSE(called);
}

TEST(WheelTimer, CancelInvalidId) {
    wheel_timer<> timer;

    EXPECT_FALSE(timer.cancel(999));
}

TEST(WheelTimer, MultipleTimeouts) {
    wheel_timer<> timer;

    int count = 0;
    timer.add(std::chrono::milliseconds(100), [&]() { count += 1; });
    timer.add(std::chrono::milliseconds(100), [&]() { count += 10; });
    timer.add(std::chrono::milliseconds(100), [&]() { count += 100; });

    timer.tick();
    EXPECT_EQ(count, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    timer.tick();
    EXPECT_EQ(count, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    timer.tick();
    EXPECT_EQ(count, 111);
}

TEST(WheelTimer, SimpleFlow) {
    wheel_timer<> timer;

    int counter = 0;
    timer.add(std::chrono::milliseconds(100), [&]() { counter++; });

    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        timer.tick();
    }

    EXPECT_EQ(counter, 1);
}
