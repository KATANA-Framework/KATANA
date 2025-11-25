#include "support/virtual_event_loop.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <vector>

using namespace std::chrono_literals;
using katana::test_support::VirtualEventLoop;

TEST(VirtualEventLoop, RunsReadyTasksWithoutAdvancing) {
    VirtualEventLoop loop;
    std::vector<int> order;

    loop.post([&] { order.push_back(1); });
    loop.post([&] { order.push_back(2); });

    EXPECT_EQ(loop.run_ready(), 2u);
    EXPECT_EQ(order, (std::vector<int>{1, 2}));
    EXPECT_EQ(loop.pending(), 0u);
}

TEST(VirtualEventLoop, AdvancesTimeAndRunsScheduled) {
    VirtualEventLoop loop;
    std::vector<int> order;

    loop.post_after(5ms, [&] { order.push_back(5); });
    loop.post_after(1ms, [&] { order.push_back(1); });
    loop.post_after(3ms, [&] { order.push_back(3); });

    EXPECT_EQ(order.size(), 0u);

    loop.advance(2ms);
    EXPECT_EQ(order, (std::vector<int>{1}));

    loop.advance(3ms);
    EXPECT_EQ(order, (std::vector<int>{1, 3, 5}));
    EXPECT_TRUE(loop.empty());
}

TEST(VirtualEventLoop, AdvanceToNextJumpsTime) {
    VirtualEventLoop loop;
    std::vector<int> order;

    loop.post_after(10ms, [&] { order.push_back(10); });
    loop.post_after(2ms, [&] { order.push_back(2); });

    EXPECT_EQ(loop.advance_to_next(), 1u);
    EXPECT_EQ(order, (std::vector<int>{2}));

    EXPECT_EQ(loop.advance_to_next(), 1u);
    EXPECT_EQ(order, (std::vector<int>{2, 10}));
}

TEST(VirtualEventLoop, CancelSkipsTask) {
    VirtualEventLoop loop;
    std::vector<int> order;

    auto id = loop.post_after(1ms, [&] { order.push_back(1); });
    loop.post_after(2ms, [&] { order.push_back(2); });
    ASSERT_TRUE(loop.cancel(id));

    loop.run_all();
    EXPECT_EQ(order, (std::vector<int>{2}));
}
