#ifdef KATANA_USE_IO_URING
#include "katana/core/io_uring_reactor.hpp"
using reactor_impl = katana::io_uring_reactor;
#else
#include "katana/core/epoll_reactor.hpp"
using reactor_impl = katana::epoll_reactor;
#endif

#include <chrono>
#include <gtest/gtest.h>
#include <thread>
#include <unistd.h>

using namespace std::chrono_literals;

class ReactorTest : public ::testing::Test {
protected:
    void SetUp() override { reactor_ = std::make_unique<reactor_impl>(); }

    void TearDown() override { reactor_.reset(); }

    std::unique_ptr<reactor_impl> reactor_;
};

TEST_F(ReactorTest, CreateReactor) {
    ASSERT_NE(reactor_, nullptr);
}

TEST_F(ReactorTest, StopReactor) {
    reactor_->schedule([this]() { reactor_->stop(); });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
}

TEST_F(ReactorTest, ScheduleTask) {
    bool executed = false;

    reactor_->schedule([&executed, this]() {
        executed = true;
        reactor_->stop();
    });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(executed);
}

TEST_F(ReactorTest, ScheduleAfter) {
    bool executed = false;
    auto start = std::chrono::steady_clock::now();

    reactor_->schedule_after(100ms, [&executed, this]() {
        executed = true;
        reactor_->stop();
    });

    auto result = reactor_->run();
    auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(executed);
    EXPECT_GE(elapsed, 100ms);
    EXPECT_LT(elapsed, 250ms);
}

TEST_F(ReactorTest, RegisterFd) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    bool readable = false;

    auto result = reactor_->register_fd(
        pipefd[0], katana::event_type::readable, [&readable, this](katana::event_type events) {
            if (katana::has_flag(events, katana::event_type::readable)) {
                readable = true;
                reactor_->stop();
            }
        });

    ASSERT_TRUE(result.has_value());

    std::thread writer([pipefd]() {
        std::this_thread::sleep_for(50ms);
        char buf = 'x';
        [[maybe_unused]] auto _ = write(pipefd[1], &buf, 1);
    });

    reactor_->run();
    writer.join();

    EXPECT_TRUE(readable);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, UnregisterFd) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    auto result =
        reactor_->register_fd(pipefd[0], katana::event_type::readable, [](katana::event_type) {});

    ASSERT_TRUE(result.has_value());

    result = reactor_->unregister_fd(pipefd[0]);
    EXPECT_TRUE(result.has_value());

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, ModifyFd) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    auto result =
        reactor_->register_fd(pipefd[0], katana::event_type::readable, [](katana::event_type) {});

    ASSERT_TRUE(result.has_value());

    result = reactor_->modify_fd(pipefd[0],
                                 katana::event_type::readable | katana::event_type::edge_triggered);

    EXPECT_TRUE(result.has_value());

    reactor_->unregister_fd(pipefd[0]);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, InvalidFd) {
    auto result =
        reactor_->register_fd(-1, katana::event_type::readable, [](katana::event_type) {});

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), katana::make_error_code(katana::error_code::invalid_fd));
}

TEST_F(ReactorTest, MultipleScheduledTasks) {
    int counter = 0;

    for (int i = 0; i < 10; ++i) {
        reactor_->schedule([&counter]() { ++counter; });
    }

    reactor_->schedule([this]() { reactor_->stop(); });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(counter, 10);
}

TEST_F(ReactorTest, ExceptionInScheduledTask) {
    bool exception_handled = false;
    bool task_after_exception = false;

    reactor_->set_exception_handler([&exception_handled](const katana::exception_context& ctx) {
        exception_handled = true;
        EXPECT_EQ(ctx.location, "scheduled_task");
        EXPECT_NE(ctx.exception, nullptr);
        EXPECT_EQ(ctx.fd, -1);
    });

    reactor_->schedule([]() { throw std::runtime_error("test exception"); });

    reactor_->schedule([&task_after_exception, this]() {
        task_after_exception = true;
        reactor_->stop();
    });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(exception_handled);
    EXPECT_TRUE(task_after_exception);
}

TEST_F(ReactorTest, ExceptionInDelayedTask) {
    bool exception_handled = false;

    reactor_->set_exception_handler([&exception_handled](const katana::exception_context& ctx) {
        exception_handled = true;
        EXPECT_EQ(ctx.location, "delayed_task");
    });

    reactor_->schedule_after(50ms, []() { throw std::logic_error("delayed task exception"); });

    reactor_->schedule_after(100ms, [this]() { reactor_->stop(); });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(exception_handled);
}

TEST_F(ReactorTest, ExceptionInFdCallback) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    bool exception_handled = false;

    reactor_->set_exception_handler(
        [&exception_handled, pipefd](const katana::exception_context& ctx) {
            exception_handled = true;
            EXPECT_EQ(ctx.location, "fd_callback");
            EXPECT_EQ(ctx.fd, pipefd[0]);
        });

    auto result =
        reactor_->register_fd(pipefd[0], katana::event_type::readable, [](katana::event_type) {
            throw std::runtime_error("fd callback exception");
        });

    ASSERT_TRUE(result.has_value());

    std::thread writer([pipefd, this]() {
        std::this_thread::sleep_for(50ms);
        char buf = 'x';
        [[maybe_unused]] auto _ = write(pipefd[1], &buf, 1);

        std::this_thread::sleep_for(50ms);
        reactor_->schedule([this]() { reactor_->stop(); });
    });

    reactor_->run();
    writer.join();

    EXPECT_TRUE(exception_handled);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, GracefulStop) {
    auto start = std::chrono::steady_clock::now();

    reactor_->schedule([this]() { reactor_->graceful_stop(std::chrono::milliseconds(1000)); });

    auto result = reactor_->run();
    auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_TRUE(result.has_value());
    EXPECT_LT(elapsed, std::chrono::milliseconds(100));
}

TEST_F(ReactorTest, GracefulStopWithPendingTasks) {
    int counter = 0;

    for (int i = 0; i < 5; ++i) {
        reactor_->schedule([&counter]() { ++counter; });
    }

    reactor_->schedule([this]() { reactor_->graceful_stop(std::chrono::milliseconds(1000)); });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(counter, 5);
}

TEST_F(ReactorTest, RegisterFdWithTimeout) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    bool timed_out = false;

    katana::timeout_config config;
    config.idle_timeout = std::chrono::milliseconds(100);

    auto result = reactor_->register_fd_with_timeout(
        pipefd[0],
        katana::event_type::readable,
        [&timed_out, this](katana::event_type events) {
            if (katana::has_flag(events, katana::event_type::timeout)) {
                timed_out = true;
                reactor_->stop();
            }
        },
        config);

    ASSERT_TRUE(result.has_value());

    auto start = std::chrono::steady_clock::now();
    reactor_->run();
    auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_TRUE(timed_out);
    EXPECT_GE(elapsed, std::chrono::milliseconds(100));
    EXPECT_LT(elapsed, std::chrono::milliseconds(300));

    auto snapshot = reactor_->metrics().snapshot();
    EXPECT_EQ(snapshot.fd_timeouts, 1);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, RefreshFdTimeout) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    bool timed_out = false;
    int refresh_count = 0;

    katana::timeout_config config;
    config.idle_timeout = std::chrono::milliseconds(1000);

    auto result = reactor_->register_fd_with_timeout(
        pipefd[0],
        katana::event_type::readable,
        [&timed_out, &refresh_count, this, pipefd](katana::event_type events) {
            if (katana::has_flag(events, katana::event_type::timeout)) {
                timed_out = true;
                reactor_->stop();
            } else if (refresh_count < 3) {
                char buf;
                [[maybe_unused]] auto n = ::read(pipefd[0], &buf, 1);
                ++refresh_count;
                reactor_->refresh_fd_timeout(pipefd[0]);
            }
        },
        config);

    ASSERT_TRUE(result.has_value());

    reactor_->schedule_after(std::chrono::milliseconds(50), [pipefd]() {
        char buf = 'x';
        [[maybe_unused]] auto _ = write(pipefd[1], &buf, 1);
    });

    reactor_->schedule_after(std::chrono::milliseconds(150), [pipefd]() {
        char buf = 'y';
        [[maybe_unused]] auto _ = write(pipefd[1], &buf, 1);
    });

    reactor_->schedule_after(std::chrono::milliseconds(250), [pipefd]() {
        char buf = 'z';
        [[maybe_unused]] auto _ = write(pipefd[1], &buf, 1);
    });

    auto start = std::chrono::steady_clock::now();
    reactor_->run();
    auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_TRUE(timed_out);
    EXPECT_GE(elapsed, std::chrono::milliseconds(350));

    close(pipefd[0]);
    close(pipefd[1]);
}

#ifdef __linux__
#include <sys/eventfd.h>

TEST_F(ReactorTest, EventFdIntegration) {
    int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    ASSERT_GE(efd, 0);

    bool event_received = false;

    auto result = reactor_->register_fd(
        efd, katana::event_type::readable, [&event_received, this](katana::event_type events) {
            if (katana::has_flag(events, katana::event_type::readable)) {
                event_received = true;
                reactor_->stop();
            }
        });

    ASSERT_TRUE(result.has_value());

    std::thread writer([efd]() {
        std::this_thread::sleep_for(50ms);
        uint64_t value = 1;
        [[maybe_unused]] auto _ = write(efd, &value, sizeof(value));
    });

    reactor_->run();
    writer.join();

    EXPECT_TRUE(event_received);

    close(efd);
}
#endif

TEST_F(ReactorTest, EdgeTriggeredMode) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    int callback_count = 0;

    auto result =
        reactor_->register_fd(pipefd[0],
                              katana::event_type::readable | katana::event_type::edge_triggered,
                              [&callback_count, this](katana::event_type events) {
                                  if (katana::has_flag(events, katana::event_type::readable)) {
                                      ++callback_count;
                                      if (callback_count >= 2) {
                                          reactor_->stop();
                                      }
                                  }
                              });

    ASSERT_TRUE(result.has_value());

    std::thread writer([pipefd]() {
        std::this_thread::sleep_for(50ms);
        char buf1 = 'x';
        [[maybe_unused]] auto _ = write(pipefd[1], &buf1, 1);

        std::this_thread::sleep_for(50ms);
        char buf2 = 'y';
        _ = write(pipefd[1], &buf2, 1);
    });

    reactor_->run();
    writer.join();

    EXPECT_EQ(callback_count, 2);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, WritableEvent) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    bool writable = false;

    auto result = reactor_->register_fd(
        pipefd[1], katana::event_type::writable, [&writable, this](katana::event_type events) {
            if (katana::has_flag(events, katana::event_type::writable)) {
                writable = true;
                reactor_->stop();
            }
        });

    ASSERT_TRUE(result.has_value());

    reactor_->run();

    EXPECT_TRUE(writable);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, ConcurrentScheduling) {
    std::atomic<int> counter{0};
    constexpr int NUM_TASKS = 100;

    std::thread scheduler1([this, &counter]() {
        for (int i = 0; i < NUM_TASKS / 2; ++i) {
            reactor_->schedule([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
        }
    });

    std::thread scheduler2([this, &counter]() {
        for (int i = 0; i < NUM_TASKS / 2; ++i) {
            reactor_->schedule([&counter]() { counter.fetch_add(1, std::memory_order_relaxed); });
        }
    });

    reactor_->schedule_after(200ms, [this]() { reactor_->stop(); });

    reactor_->run();

    scheduler1.join();
    scheduler2.join();

    EXPECT_EQ(counter.load(), NUM_TASKS);
}

TEST_F(ReactorTest, UnregisterFdDuringCallback) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    bool callback_executed = false;

    auto result =
        reactor_->register_fd(pipefd[0],
                              katana::event_type::readable,
                              [&callback_executed, this, pipefd](katana::event_type events) {
                                  if (katana::has_flag(events, katana::event_type::readable)) {
                                      callback_executed = true;
                                      reactor_->unregister_fd(pipefd[0]);
                                      reactor_->schedule([this]() { reactor_->stop(); });
                                  }
                              });

    ASSERT_TRUE(result.has_value());

    std::thread writer([pipefd]() {
        std::this_thread::sleep_for(50ms);
        char buf = 'x';
        [[maybe_unused]] auto _ = write(pipefd[1], &buf, 1);
    });

    reactor_->run();
    writer.join();

    EXPECT_TRUE(callback_executed);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, ScheduleTaskFromCallback) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    bool secondary_task_executed = false;

    auto result =
        reactor_->register_fd(pipefd[0],
                              katana::event_type::readable,
                              [&secondary_task_executed, this](katana::event_type events) {
                                  if (katana::has_flag(events, katana::event_type::readable)) {
                                      reactor_->schedule([&secondary_task_executed, this]() {
                                          secondary_task_executed = true;
                                          reactor_->stop();
                                      });
                                  }
                              });

    ASSERT_TRUE(result.has_value());

    std::thread writer([pipefd]() {
        std::this_thread::sleep_for(50ms);
        char buf = 'x';
        [[maybe_unused]] auto _ = write(pipefd[1], &buf, 1);
    });

    reactor_->run();
    writer.join();

    EXPECT_TRUE(secondary_task_executed);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, RunWithoutTasks) {
    // Reactor should handle empty run gracefully
    reactor_->schedule([this]() { reactor_->stop(); });
    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
}

TEST_F(ReactorTest, MultipleRuns) {
    // Test multiple consecutive runs
    for (int i = 0; i < 3; ++i) {
        bool executed = false;
        reactor_->schedule([&executed, this]() {
            executed = true;
            reactor_->stop();
        });
        auto result = reactor_->run();
        EXPECT_TRUE(result.has_value());
        EXPECT_TRUE(executed);
    }
}

TEST_F(ReactorTest, ModifyNonExistentFd) {
    auto result = reactor_->modify_fd(999, katana::event_type::readable);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ReactorTest, UnregisterNonExistentFd) {
    auto result = reactor_->unregister_fd(999);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ReactorTest, RegisterDuplicateFd) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    auto result1 =
        reactor_->register_fd(pipefd[0], katana::event_type::readable, [](katana::event_type) {});
    ASSERT_TRUE(result1.has_value());

    // Try to register same fd again - should fail
    auto result2 =
        reactor_->register_fd(pipefd[0], katana::event_type::readable, [](katana::event_type) {});
    EXPECT_FALSE(result2.has_value());

    reactor_->unregister_fd(pipefd[0]);
    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, RefreshTimeoutNonExistentFd) {
    // refresh_fd_timeout returns void, so just verify it doesn't crash
    EXPECT_NO_THROW(reactor_->refresh_fd_timeout(999));
}

TEST_F(ReactorTest, RefreshTimeoutWithoutTimeout) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    auto result =
        reactor_->register_fd(pipefd[0], katana::event_type::readable, [](katana::event_type) {});
    ASSERT_TRUE(result.has_value());

    // Refresh timeout on fd without timeout configured - should not crash
    EXPECT_NO_THROW(reactor_->refresh_fd_timeout(pipefd[0]));

    reactor_->unregister_fd(pipefd[0]);
    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, ZeroTimeout) {
    reactor_->schedule_after(std::chrono::milliseconds(0), [this]() { reactor_->stop(); });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
}

TEST_F(ReactorTest, VeryShortTimeout) {
    bool executed = false;

    reactor_->schedule_after(std::chrono::milliseconds(1), [&executed, this]() {
        executed = true;
        reactor_->stop();
    });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(executed);
}

TEST_F(ReactorTest, MetricsTracking) {
    auto initial_metrics = reactor_->metrics().snapshot();

    reactor_->schedule([]() {});
    reactor_->schedule([]() {});
    reactor_->schedule([this]() { reactor_->stop(); });

    reactor_->run();

    auto final_metrics = reactor_->metrics().snapshot();
    EXPECT_GT(final_metrics.tasks_executed, initial_metrics.tasks_executed);
}

TEST_F(ReactorTest, BothReadableAndWritable) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    bool readable = false;
    bool writable = false;

    auto result =
        reactor_->register_fd(pipefd[1],
                              katana::event_type::readable | katana::event_type::writable,
                              [&readable, &writable, this](katana::event_type events) {
                                  if (katana::has_flag(events, katana::event_type::readable)) {
                                      readable = true;
                                  }
                                  if (katana::has_flag(events, katana::event_type::writable)) {
                                      writable = true;
                                      reactor_->stop();
                                  }
                              });

    ASSERT_TRUE(result.has_value());

    reactor_->run();

    EXPECT_TRUE(writable);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, CancelScheduledAfter) {
    bool executed = false;

    // Schedule a task but stop before it executes
    reactor_->schedule_after(std::chrono::seconds(10), [&executed]() { executed = true; });

    reactor_->schedule([this]() { reactor_->stop(); });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(executed);
}

TEST_F(ReactorTest, EmptyCallback) {
    reactor_->schedule([]() {});
    reactor_->schedule([this]() { reactor_->stop(); });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
}

TEST_F(ReactorTest, LargeFdNumber) {
    // Test with a larger fd number
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    auto result =
        reactor_->register_fd(pipefd[0], katana::event_type::readable, [](katana::event_type) {});

    if (result.has_value()) {
        reactor_->unregister_fd(pipefd[0]);
    }

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, ScheduleAfterMultiple) {
    std::vector<bool> executed(5, false);

    for (size_t i = 0; i < 5; ++i) {
        reactor_->schedule_after(std::chrono::milliseconds(50 + i * 10),
                                 [&executed, i]() { executed[i] = true; });
    }

    reactor_->schedule_after(std::chrono::milliseconds(200), [this]() { reactor_->stop(); });

    reactor_->run();

    for (bool e : executed) {
        EXPECT_TRUE(e);
    }
}
