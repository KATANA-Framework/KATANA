#include "katana/core/epoll_reactor.hpp"

#include <gtest/gtest.h>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

class ReactorTest : public ::testing::Test {
protected:
    void SetUp() override {
        reactor_ = std::make_unique<katana::epoll_reactor>();
    }

    void TearDown() override {
        reactor_.reset();
    }

    std::unique_ptr<katana::epoll_reactor> reactor_;
};

TEST_F(ReactorTest, CreateReactor) {
    ASSERT_NE(reactor_, nullptr);
}

TEST_F(ReactorTest, StopReactor) {
    reactor_->schedule([this]() {
        reactor_->stop();
    });

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
        pipefd[0],
        katana::event_type::readable,
        [&readable, this](katana::event_type events) {
            if (katana::has_flag(events, katana::event_type::readable)) {
                readable = true;
                reactor_->stop();
            }
        }
    );

    ASSERT_TRUE(result.has_value());

    std::thread writer([pipefd]() {
        std::this_thread::sleep_for(50ms);
        char buf = 'x';
        write(pipefd[1], &buf, 1);
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

    auto result = reactor_->register_fd(
        pipefd[0],
        katana::event_type::readable,
        [](katana::event_type) {}
    );

    ASSERT_TRUE(result.has_value());

    result = reactor_->unregister_fd(pipefd[0]);
    EXPECT_TRUE(result.has_value());

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, ModifyFd) {
    int pipefd[2];
    ASSERT_EQ(pipe(pipefd), 0);

    auto result = reactor_->register_fd(
        pipefd[0],
        katana::event_type::readable,
        [](katana::event_type) {}
    );

    ASSERT_TRUE(result.has_value());

    result = reactor_->modify_fd(
        pipefd[0],
        katana::event_type::readable | katana::event_type::edge_triggered
    );

    EXPECT_TRUE(result.has_value());

    reactor_->unregister_fd(pipefd[0]);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST_F(ReactorTest, InvalidFd) {
    auto result = reactor_->register_fd(
        -1,
        katana::event_type::readable,
        [](katana::event_type) {}
    );

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(
        result.error(),
        katana::make_error_code(katana::error_code::invalid_fd)
    );
}

TEST_F(ReactorTest, MultipleScheduledTasks) {
    int counter = 0;

    for (int i = 0; i < 10; ++i) {
        reactor_->schedule([&counter]() {
            ++counter;
        });
    }

    reactor_->schedule([this]() {
        reactor_->stop();
    });

    auto result = reactor_->run();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(counter, 10);
}
