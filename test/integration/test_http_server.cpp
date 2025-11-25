#include "katana/core/arena.hpp"
#include "katana/core/http.hpp"
#include "katana/core/reactor_pool.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace katana;

constexpr uint16_t TEST_PORT = 9999;

class HTTPServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        listener_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        if (listener_fd < 0) {
            return; // environment may forbid sockets; tests below don't rely on bind/listen
        }

        int opt = 1;
        setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        setsockopt(listener_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(TEST_PORT);

        if (bind(listener_fd, static_cast<sockaddr*>(static_cast<void*>(&addr)), sizeof(addr)) <
            0) {
            close(listener_fd);
            listener_fd = -1;
            return;
        }
        if (listen(listener_fd, 10) < 0) {
            close(listener_fd);
            listener_fd = -1;
            return;
        }
    }

    void TearDown() override {
        if (listener_fd >= 0) {
            close(listener_fd);
        }
    }

    int listener_fd = -1;
};

TEST_F(HTTPServerTest, ChunkedEncoding) {
    http::response resp = http::response::ok("Hello, World!", "text/plain");
    resp.chunked = true;

    std::string serialized = resp.serialize();

    EXPECT_NE(serialized.find("Transfer-Encoding: chunked"), std::string::npos);
    EXPECT_EQ(serialized.find("Content-Length"), std::string::npos);
    EXPECT_NE(serialized.find("d\r\nHello, World!\r\n"), std::string::npos);
    EXPECT_NE(serialized.find("0\r\n\r\n"), std::string::npos);
}

TEST_F(HTTPServerTest, ChunkedParsing) {
    monotonic_arena arena;
    http::parser parser(&arena);

    std::string request_data = "POST /test HTTP/1.1\r\n"
                               "Host: localhost\r\n"
                               "Transfer-Encoding: chunked\r\n"
                               "\r\n"
                               "5\r\n"
                               "Hello\r\n"
                               "7\r\n"
                               ", World\r\n"
                               "0\r\n"
                               "\r\n";

    auto result = parser.parse(http::as_bytes(request_data));

    ASSERT_TRUE(result);
    EXPECT_TRUE(parser.is_complete());

    const auto& req = parser.get_request();
    EXPECT_EQ(req.body, "Hello, World");
}

TEST_F(HTTPServerTest, SizeLimits) {
    monotonic_arena arena;
    http::parser parser(&arena);

    std::string huge_uri(3000, 'a');
    std::string request_data = "GET /" + huge_uri + " HTTP/1.1\r\n\r\n";

    auto result = parser.parse(http::as_bytes(request_data));

    EXPECT_FALSE(result);
}

TEST_F(HTTPServerTest, ArenaAllocation) {
    monotonic_arena arena(4096);

    // Allocate using arena
    void* buffer = arena.allocate(1024, 1);
    EXPECT_NE(buffer, nullptr);
    EXPECT_GE(arena.bytes_allocated(), 1024);

    arena.reset();
    EXPECT_EQ(arena.bytes_allocated(), 0);
}
