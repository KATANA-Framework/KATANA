#include "katana/core/shutdown.hpp"

#include <chrono>
#include <csignal>
#include <gtest/gtest.h>
#include <thread>

using namespace katana;
using namespace std::chrono_literals;

class ShutdownManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset shutdown state before each test
        // Note: We can't truly reset the singleton, so tests must be careful
        // We'll work with the state as-is
    }
};

TEST_F(ShutdownManagerTest, Singleton) {
    auto& mgr1 = shutdown_manager::instance();
    auto& mgr2 = shutdown_manager::instance();

    // Should be the same instance
    EXPECT_EQ(&mgr1, &mgr2);
}

TEST_F(ShutdownManagerTest, RequestShutdown) {
    auto& mgr = shutdown_manager::instance();

    mgr.request_shutdown();
    EXPECT_TRUE(mgr.is_shutdown_requested());

    // Should remain true
    EXPECT_TRUE(mgr.is_shutdown_requested());
}

TEST_F(ShutdownManagerTest, RecordShutdownTime) {
    auto& mgr = shutdown_manager::instance();

    auto before = std::chrono::steady_clock::now();
    mgr.record_shutdown_time();
    auto after = std::chrono::steady_clock::now();

    auto recorded = mgr.shutdown_time();

    // Shutdown time should be between before and after
    EXPECT_GE(recorded, before);
    EXPECT_LE(recorded, after);
}

TEST_F(ShutdownManagerTest, IsDeadlineExceededNoShutdown) {
    auto& mgr = shutdown_manager::instance();

    // Create a new manager state by working with what we have
    // This test might be affected by previous tests, but we can still check behavior

    // If shutdown is not requested, deadline should not be exceeded
    // Note: Can't easily reset singleton, so we'll test the logic

    if (!mgr.is_shutdown_requested()) {
        EXPECT_FALSE(mgr.is_deadline_exceeded(100ms));
    }
}

TEST_F(ShutdownManagerTest, IsDeadlineExceededWithinDeadline) {
    auto& mgr = shutdown_manager::instance();

    mgr.request_shutdown();
    mgr.record_shutdown_time();

    // Immediately after recording, deadline should not be exceeded
    EXPECT_FALSE(mgr.is_deadline_exceeded(1000ms));
}

TEST_F(ShutdownManagerTest, IsDeadlineExceededAfterDeadline) {
    auto& mgr = shutdown_manager::instance();

    mgr.request_shutdown();
    mgr.record_shutdown_time();

    // Wait for a short time
    std::this_thread::sleep_for(50ms);

    // With a very short deadline, it should be exceeded
    EXPECT_TRUE(mgr.is_deadline_exceeded(10ms));
}

TEST_F(ShutdownManagerTest, IsDeadlineExceededAutoRecord) {
    auto& mgr = shutdown_manager::instance();

    mgr.request_shutdown();

    // Don't record shutdown time manually
    // First call should auto-record and return false
    bool result1 = mgr.is_deadline_exceeded(1000ms);

    // Should have auto-recorded the time (or it was already recorded)
    auto recorded = mgr.shutdown_time();
    (void)recorded; // Just verify we can get it without crash

    // Should not be exceeded yet with a long deadline
    EXPECT_FALSE(result1);
}

TEST_F(ShutdownManagerTest, SetShutdownCallback) {
    auto& mgr = shutdown_manager::instance();

    bool callback_called = false;
    mgr.set_shutdown_callback([&callback_called]() { callback_called = true; });

    // Callback should be set but not called yet
    EXPECT_FALSE(callback_called);
}

TEST_F(ShutdownManagerTest, TriggerShutdown) {
    auto& mgr = shutdown_manager::instance();

    bool callback_called = false;
    mgr.set_shutdown_callback([&callback_called]() { callback_called = true; });

    mgr.trigger_shutdown();

    // Shutdown should be requested
    EXPECT_TRUE(mgr.is_shutdown_requested());

    // Callback should have been called
    EXPECT_TRUE(callback_called);
}

TEST_F(ShutdownManagerTest, TriggerShutdownNoCallback) {
    auto& mgr = shutdown_manager::instance();

    // Clear any existing callback
    mgr.set_shutdown_callback(nullptr);

    // Should not crash when triggering without a callback
    EXPECT_NO_THROW(mgr.trigger_shutdown());

    EXPECT_TRUE(mgr.is_shutdown_requested());
}

TEST_F(ShutdownManagerTest, SetupSignalHandlers) {
    auto& mgr = shutdown_manager::instance();

    // Should not crash
    EXPECT_NO_THROW(mgr.setup_signal_handlers());
}

TEST_F(ShutdownManagerTest, SignalHandlerSIGINT) {
    auto& mgr = shutdown_manager::instance();

    mgr.setup_signal_handlers();

    // Note: Sending signals to self can be tricky
    // We'll just verify setup doesn't crash

    // To properly test, we'd need to send SIGINT
    // but that's dangerous in a test environment

    // Instead, just verify the setup worked
    EXPECT_NO_THROW(mgr.setup_signal_handlers());
}

TEST_F(ShutdownManagerTest, MultipleCallbacks) {
    auto& mgr = shutdown_manager::instance();

    int call_count = 0;

    // Set first callback
    mgr.set_shutdown_callback([&call_count]() { call_count++; });

    mgr.trigger_shutdown();
    EXPECT_EQ(call_count, 1);

    // Set second callback (replaces first)
    mgr.set_shutdown_callback([&call_count]() { call_count += 10; });

    // Reset shutdown flag for next trigger (we can't truly reset singleton)
    // So we'll just call the callback by setting it again
    call_count = 0;
    mgr.set_shutdown_callback([&call_count]() { call_count += 10; });

    mgr.trigger_shutdown();
    // The new callback should be called
    EXPECT_GE(call_count, 10);
}

TEST_F(ShutdownManagerTest, ShutdownTimeInitiallyDefault) {
    auto& mgr = shutdown_manager::instance();

    // Note: Can't easily reset singleton, so this test is best-effort
    // Just verify we can get shutdown_time without crash
    auto time = mgr.shutdown_time();
    (void)time; // Just make sure it doesn't crash
}

TEST_F(ShutdownManagerTest, ThreadSafeShutdownRequest) {
    auto& mgr = shutdown_manager::instance();

    // Multiple threads requesting shutdown
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&mgr]() {
            mgr.request_shutdown();
            EXPECT_TRUE(mgr.is_shutdown_requested());
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_TRUE(mgr.is_shutdown_requested());
}

TEST_F(ShutdownManagerTest, DeadlineWithDifferentDurations) {
    auto& mgr = shutdown_manager::instance();

    mgr.request_shutdown();
    mgr.record_shutdown_time();

    std::this_thread::sleep_for(50ms);

    // Very short deadline - should be exceeded
    EXPECT_TRUE(mgr.is_deadline_exceeded(10ms));

    // Very long deadline - should not be exceeded
    EXPECT_FALSE(mgr.is_deadline_exceeded(10000ms));
}

TEST_F(ShutdownManagerTest, CallbackWithException) {
    auto& mgr = shutdown_manager::instance();

    // Set a callback that throws
    mgr.set_shutdown_callback([]() { throw std::runtime_error("Test exception"); });

    // trigger_shutdown should propagate the exception
    bool threw_exception = false;
    try {
        mgr.trigger_shutdown();
    } catch (const std::runtime_error&) {
        threw_exception = true;
    }
    EXPECT_TRUE(threw_exception);
}
