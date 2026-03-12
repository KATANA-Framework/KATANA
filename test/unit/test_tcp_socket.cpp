#include "katana/core/tcp_socket.hpp"

#include <array>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/uio.h>
#include <thread>
#include <unistd.h>

using namespace katana;

class TcpSocketTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a socketpair for testing
        int fds[2];
        ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, fds), 0);
        fd1_ = fds[0];
        fd2_ = fds[1];
    }

    void TearDown() override {
        if (fd1_ >= 0)
            close(fd1_);
        if (fd2_ >= 0)
            close(fd2_);
    }

    int fd1_{-1};
    int fd2_{-1};
};

TEST_F(TcpSocketTest, DefaultConstructor) {
    tcp_socket socket;
    EXPECT_FALSE(socket);
    EXPECT_EQ(socket.native_handle(), -1);
}

TEST_F(TcpSocketTest, ConstructorWithFd) {
    tcp_socket socket(fd1_);
    EXPECT_TRUE(socket);
    EXPECT_EQ(socket.native_handle(), fd1_);
    fd1_ = -1; // Ownership transferred
}

TEST_F(TcpSocketTest, MoveConstructor) {
    tcp_socket socket1(fd1_);
    fd1_ = -1;

    tcp_socket socket2(std::move(socket1));
    EXPECT_FALSE(socket1);
    EXPECT_TRUE(socket2);
    EXPECT_EQ(socket1.native_handle(), -1);
}

TEST_F(TcpSocketTest, MoveAssignment) {
    tcp_socket socket1(fd1_);
    tcp_socket socket2(fd2_);
    fd1_ = -1;
    fd2_ = -1;

    socket2 = std::move(socket1);
    EXPECT_FALSE(socket1);
    EXPECT_TRUE(socket2);
}

TEST_F(TcpSocketTest, MoveAssignmentSelf) {
    tcp_socket socket(fd1_);
    fd1_ = -1;

    tcp_socket& ref = socket;
    socket = std::move(ref);
    EXPECT_TRUE(socket); // Should still be valid after self-assignment
}

TEST_F(TcpSocketTest, Close) {
    tcp_socket socket(fd1_);
    fd1_ = -1;

    EXPECT_TRUE(socket);
    socket.close();
    EXPECT_FALSE(socket);
    EXPECT_EQ(socket.native_handle(), -1);
}

TEST_F(TcpSocketTest, DoubleClose) {
    tcp_socket socket(fd1_);
    fd1_ = -1;

    socket.close();
    socket.close(); // Should not crash
    EXPECT_FALSE(socket);
}

TEST_F(TcpSocketTest, Release) {
    tcp_socket socket(fd1_);
    int original_fd = fd1_;
    fd1_ = -1;

    int released_fd = socket.release();
    EXPECT_EQ(released_fd, original_fd);
    EXPECT_FALSE(socket);
    EXPECT_EQ(socket.native_handle(), -1);

    close(released_fd);
}

TEST_F(TcpSocketTest, ReadSuccess) {
    tcp_socket reader(fd1_);
    tcp_socket writer(fd2_);
    fd1_ = -1;
    fd2_ = -1;

    const char* msg = "Hello";
    size_t msg_len = strlen(msg);

    // Write data
    auto write_result =
        writer.write(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(msg), msg_len));
    ASSERT_TRUE(write_result.has_value());
    EXPECT_EQ(*write_result, msg_len);

    // Read data
    uint8_t buffer[100];
    auto read_result = reader.read(std::span<uint8_t>(buffer, sizeof(buffer)));
    ASSERT_TRUE(read_result.has_value());
    EXPECT_EQ(read_result->size(), msg_len);
    EXPECT_EQ(std::memcmp(read_result->data(), msg, msg_len), 0);
}

TEST_F(TcpSocketTest, ReadEmpty) {
    tcp_socket socket(fd1_);
    fd1_ = -1;

    // No data available, should return empty span (EAGAIN)
    uint8_t buffer[100];
    auto result = socket.read(std::span<uint8_t>(buffer, sizeof(buffer)));
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->empty());
}

TEST_F(TcpSocketTest, ReadInvalidFd) {
    tcp_socket socket; // Invalid fd

    uint8_t buffer[100];
    auto result = socket.read(std::span<uint8_t>(buffer, sizeof(buffer)));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), make_error_code(error_code::invalid_fd));
}

TEST_F(TcpSocketTest, ReadEOF) {
    tcp_socket reader(fd1_);
    tcp_socket writer(fd2_);
    fd1_ = -1;
    fd2_ = -1;

    // Close writer side
    writer.close();

    // Read should detect EOF
    uint8_t buffer[100];
    auto result = reader.read(std::span<uint8_t>(buffer, sizeof(buffer)));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), make_error_code(error_code::ok)); // EOF
}

TEST_F(TcpSocketTest, WriteSuccess) {
    tcp_socket socket(fd1_);
    fd1_ = -1;

    const char* msg = "Test message";
    auto result =
        socket.write(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(msg), strlen(msg)));
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(*result, 0);
}

TEST_F(TcpSocketTest, WriteInvalidFd) {
    tcp_socket socket; // Invalid fd

    const char* msg = "Test";
    auto result =
        socket.write(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(msg), strlen(msg)));
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), make_error_code(error_code::invalid_fd));
}

TEST_F(TcpSocketTest, WriteLargeData) {
    tcp_socket writer(fd1_);
    tcp_socket reader(fd2_);
    fd1_ = -1;
    fd2_ = -1;

    // Create large buffer
    std::vector<uint8_t> large_buffer(64 * 1024, 'A');

    // Write in background thread
    std::thread write_thread([&]() {
        auto result = writer.write(std::span<const uint8_t>(large_buffer));
        EXPECT_TRUE(result.has_value());
    });

    // Read all data
    std::vector<uint8_t> read_buffer(64 * 1024);
    size_t total_read = 0;
    while (total_read < large_buffer.size()) {
        auto result = reader.read(
            std::span<uint8_t>(read_buffer.data() + total_read, read_buffer.size() - total_read));
        if (result.has_value() && !result->empty()) {
            total_read += result->size();
        } else if (!result.has_value()) {
            break;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    write_thread.join();
    EXPECT_GT(total_read, 0);
}

TEST_F(TcpSocketTest, WritevHandlesPartialWritesAndPreservesOrdering) {
    tcp_socket writer(fd1_);
    tcp_socket reader(fd2_);
    fd1_ = -1;
    fd2_ = -1;

    const int sndbuf = 4096;
    ASSERT_EQ(::setsockopt(writer.native_handle(),
                           SOL_SOCKET,
                           SO_SNDBUF,
                           &sndbuf,
                           static_cast<socklen_t>(sizeof(sndbuf))),
              0);

    std::string head(8192, 'H');
    std::string body(65536, 'B');
    std::string expected = head + body;
    std::string received;
    received.reserve(expected.size());

    size_t offset = 0;
    bool saw_blocked_write = false;

    auto make_iov = [&](size_t start_offset) {
        std::array<iovec, 2> iov{};
        size_t count = 0;
        if (start_offset < head.size()) {
            iov[count].iov_base = head.data() + start_offset;
            iov[count].iov_len = head.size() - start_offset;
            ++count;
            iov[count].iov_base = body.data();
            iov[count].iov_len = body.size();
            ++count;
        } else {
            const size_t body_offset = start_offset - head.size();
            iov[count].iov_base = body.data() + body_offset;
            iov[count].iov_len = body.size() - body_offset;
            ++count;
        }
        return std::pair{iov, count};
    };

    while (offset < expected.size()) {
        auto [iov, count] = make_iov(offset);
        auto write_result = writer.writev(iov.data(), count);
        ASSERT_TRUE(write_result.has_value());

        if (*write_result == 0) {
            saw_blocked_write = true;
            break;
        }

        offset += *write_result;
    }

    while (offset < expected.size()) {
        char buffer[16384];
        while (true) {
            auto read_result =
                reader.read(std::span<uint8_t>(reinterpret_cast<uint8_t*>(buffer), sizeof(buffer)));
            ASSERT_TRUE(read_result.has_value());
            if (read_result->empty()) {
                break;
            }
            received.append(buffer, buffer + read_result->size());
        }

        auto [iov, count] = make_iov(offset);
        auto write_result = writer.writev(iov.data(), count);
        ASSERT_TRUE(write_result.has_value());

        if (*write_result == 0) {
            saw_blocked_write = true;
        } else {
            offset += *write_result;
        }
    }

    auto [iov, count] = make_iov(offset);
    auto final_write = writer.writev(iov.data(), count);
    ASSERT_TRUE(final_write.has_value());
    EXPECT_EQ(*final_write, 0U);
    EXPECT_TRUE(saw_blocked_write);

    char drain[16384];
    while (received.size() < expected.size()) {
        auto read_result =
            reader.read(std::span<uint8_t>(reinterpret_cast<uint8_t*>(drain), sizeof(drain)));
        ASSERT_TRUE(read_result.has_value());
        if (read_result->empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        received.append(drain, drain + read_result->size());
    }

    EXPECT_EQ(received, expected);
}

TEST_F(TcpSocketTest, DestructorClosesSocket) {
    int original_fd = fd1_;
    {
        tcp_socket socket(fd1_);
        fd1_ = -1;
        EXPECT_TRUE(socket);
    }
    // Socket should be closed now

    // Verify fd is closed by trying to use it
    char buf;
    ssize_t result = ::read(original_fd, &buf, 1);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(errno, EBADF);
}

TEST_F(TcpSocketTest, BoolOperator) {
    tcp_socket socket1;
    EXPECT_FALSE(socket1);

    tcp_socket socket2(fd1_);
    fd1_ = -1;
    EXPECT_TRUE(socket2);

    socket2.close();
    EXPECT_FALSE(socket2);
}

TEST_F(TcpSocketTest, NativeHandle) {
    tcp_socket socket(42);
    EXPECT_EQ(socket.native_handle(), 42);

    socket.close();
    EXPECT_EQ(socket.native_handle(), -1);
}
