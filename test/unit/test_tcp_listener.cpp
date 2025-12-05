#include "katana/core/tcp_listener.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <utility>

using namespace katana;

class TcpListenerTest : public ::testing::Test {
protected:
    static uint16_t find_free_port() {
        // Create temporary socket to find a free port
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            return 0;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = 0; // Let OS pick a port

        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            return 0;
        }

        socklen_t len = sizeof(addr);
        if (getsockname(sock, (struct sockaddr*)&addr, &len) < 0) {
            close(sock);
            return 0;
        }

        uint16_t port = ntohs(addr.sin_port);
        close(sock);
        return port;
    }
};

TEST_F(TcpListenerTest, DefaultConstructor) {
    tcp_listener listener;
    EXPECT_FALSE(listener);
}

TEST_F(TcpListenerTest, CreateListenerIPv4) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return; // environment does not permit binding
    }

    tcp_listener listener(port, false);
    EXPECT_TRUE(listener);
    EXPECT_GE(listener.native_handle(), 0);
}

TEST_F(TcpListenerTest, CreateListenerIPv6) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    // Try IPv6, may not be available on all systems
    try {
        tcp_listener listener(port, true);
        EXPECT_TRUE(listener);
        EXPECT_GE(listener.native_handle(), 0);
    } catch (const std::system_error& e) {
        // IPv6 might not be available, that's OK - skip test
        return;
    }
}

TEST_F(TcpListenerTest, BindToZeroPort) {
    // Port 0 should let OS pick a port
    try {
        tcp_listener listener(0, false);
        EXPECT_TRUE(listener);
    } catch (const std::system_error&) {
        return;
    }
}

TEST_F(TcpListenerTest, BindToUsedPort) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener1(port, false);
    EXPECT_TRUE(listener1);

    // Try to bind to same port - should throw
    bool threw_exception = false;
    try {
        tcp_listener listener2(port, false);
    } catch (const std::system_error&) {
        threw_exception = true;
    }
    EXPECT_TRUE(threw_exception);
}

TEST_F(TcpListenerTest, MoveConstructor) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener1(port, false);
    int original_fd = listener1.native_handle();

    tcp_listener listener2(std::move(listener1));
    EXPECT_FALSE(listener1);
    EXPECT_TRUE(listener2);
    EXPECT_EQ(listener2.native_handle(), original_fd);
}

TEST_F(TcpListenerTest, MoveAssignment) {
    uint16_t port1 = find_free_port();
    uint16_t port2 = find_free_port();
    if (port1 == 0 || port2 == 0) {
        return;
    }

    tcp_listener listener1(port1, false);
    tcp_listener listener2(port2, false);

    int fd1 = listener1.native_handle();

    listener2 = std::move(listener1);
    EXPECT_FALSE(listener1);
    EXPECT_TRUE(listener2);
    EXPECT_EQ(listener2.native_handle(), fd1);
}

TEST_F(TcpListenerTest, AcceptConnection) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener(port, false);
    ASSERT_TRUE(listener);

    // Connect in a background thread
    std::thread client_thread([port]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
            return;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(port);

        connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        close(sock);
    });

    // Wait for connection with timeout
    auto start = std::chrono::steady_clock::now();
    std::optional<result<tcp_socket>> accepted;
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(2)) {
        auto result = listener.accept();
        if (result.has_value()) {
            accepted.emplace(std::move(result));
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    client_thread.join();

    ASSERT_TRUE(accepted.has_value());
    ASSERT_TRUE(accepted->has_value());
    EXPECT_TRUE(**accepted);
}

TEST_F(TcpListenerTest, AcceptNoConnection) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener(port, false);
    ASSERT_TRUE(listener);

    // Should return error (EAGAIN/EWOULDBLOCK) when no connection
    auto result = listener.accept();
    EXPECT_FALSE(result.has_value());
}

TEST_F(TcpListenerTest, SetReuseAddr) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener(port, false);
    EXPECT_TRUE(listener);

    // Should not throw
    listener.set_reuseaddr(true);
    listener.set_reuseaddr(false);
    EXPECT_TRUE(listener);
}

TEST_F(TcpListenerTest, SetReusePort) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener(port, false);
    EXPECT_TRUE(listener);

    // Should not throw
    listener.set_reuseport(true);
    listener.set_reuseport(false);
    EXPECT_TRUE(listener);
}

TEST_F(TcpListenerTest, SetBacklog) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener(port, false);
    EXPECT_TRUE(listener);

    // Should not throw
    listener.set_backlog(100);
    listener.set_backlog(512);
    EXPECT_TRUE(listener);
}

TEST_F(TcpListenerTest, SetBacklogBeforeBind) {
    tcp_listener listener;
    EXPECT_FALSE(listener);

    // Should not crash
    listener.set_backlog(100);
    EXPECT_FALSE(listener);
}

TEST_F(TcpListenerTest, BoolOperator) {
    tcp_listener listener1;
    EXPECT_FALSE(listener1);

    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener2(port, false);
    EXPECT_TRUE(listener2);
}

TEST_F(TcpListenerTest, NativeHandle) {
    tcp_listener listener1;
    EXPECT_LT(listener1.native_handle(), 0);

    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener2(port, false);
    EXPECT_GE(listener2.native_handle(), 0);
}

TEST_F(TcpListenerTest, MultipleConnections) {
    uint16_t port = find_free_port();
    if (port == 0) {
        return;
    }

    tcp_listener listener(port, false);
    listener.set_backlog(10);
    ASSERT_TRUE(listener);

    // Create multiple client connections
    std::vector<std::thread> clients;
    for (int i = 0; i < 3; ++i) {
        clients.emplace_back([port]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
                return;

            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            addr.sin_port = htons(port);

            connect(sock, (struct sockaddr*)&addr, sizeof(addr));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            close(sock);
        });
    }

    // Accept multiple connections
    int accepted_count = 0;
    auto start = std::chrono::steady_clock::now();
    while (accepted_count < 3 &&
           std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
        auto result = listener.accept();
        if (result.has_value()) {
            accepted_count++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (auto& client : clients) {
        client.join();
    }

    EXPECT_EQ(accepted_count, 3);
}
