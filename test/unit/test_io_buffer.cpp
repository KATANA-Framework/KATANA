#include "katana/core/io_buffer.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <unistd.h>

using namespace katana;

TEST(IOBuffer, DefaultConstruction) {
    io_buffer buf;
    EXPECT_EQ(buf.size(), 0);
    EXPECT_TRUE(buf.empty());
}

TEST(IOBuffer, ReserveCapacity) {
    io_buffer buf;
    buf.reserve(1024);
    EXPECT_GE(buf.capacity(), 1024);
    EXPECT_EQ(buf.size(), 0);
}

TEST(IOBuffer, AppendSpan) {
    io_buffer buf;
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    buf.append(std::span<const uint8_t>(data));

    EXPECT_EQ(buf.size(), 5);
    auto readable = buf.readable_span();
    EXPECT_EQ(readable.size(), 5);
    EXPECT_EQ(readable[0], 1);
    EXPECT_EQ(readable[4], 5);
}

TEST(IOBuffer, AppendString) {
    io_buffer buf;
    buf.append("hello");

    EXPECT_EQ(buf.size(), 5);
    auto readable = buf.readable_span();
    EXPECT_EQ(std::memcmp(readable.data(), "hello", 5), 0);
}

TEST(IOBuffer, WritableSpanAndCommit) {
    io_buffer buf;
    auto writable = buf.writable_span(10);
    EXPECT_GE(writable.size(), 10);

    std::memcpy(writable.data(), "test", 4);
    buf.commit(4);

    EXPECT_EQ(buf.size(), 4);
    auto readable = buf.readable_span();
    EXPECT_EQ(std::memcmp(readable.data(), "test", 4), 0);
}

TEST(IOBuffer, ConsumeBytes) {
    io_buffer buf;
    buf.append("hello world");
    EXPECT_EQ(buf.size(), 11);

    buf.consume(6);
    EXPECT_EQ(buf.size(), 5);

    auto readable = buf.readable_span();
    EXPECT_EQ(std::memcmp(readable.data(), "world", 5), 0);
}

TEST(IOBuffer, Clear) {
    io_buffer buf;
    buf.append("data");
    EXPECT_FALSE(buf.empty());

    buf.clear();
    EXPECT_TRUE(buf.empty());
    EXPECT_EQ(buf.size(), 0);
}

TEST(IOBuffer, MultipleAppends) {
    io_buffer buf;
    buf.append("hello");
    buf.append(" ");
    buf.append("world");

    EXPECT_EQ(buf.size(), 11);
    auto readable = buf.readable_span();
    EXPECT_EQ(std::memcmp(readable.data(), "hello world", 11), 0);
}

TEST(IOBuffer, ConsumeAndReappend) {
    io_buffer buf;
    buf.append("first");
    buf.consume(5);
    buf.append("second");

    EXPECT_EQ(buf.size(), 6);
    auto readable = buf.readable_span();
    EXPECT_EQ(std::memcmp(readable.data(), "second", 6), 0);
}

TEST(IOBuffer, BufferCompaction) {
    io_buffer buf(64);

    buf.append("aaaaaaaaaa");
    buf.consume(10);

    buf.append("bbbbbbbbbb");
    buf.consume(10);

    buf.append("cccccccccccccccccccccccccccccccccccccccccccccccccc");

    EXPECT_GT(buf.size(), 0);
    auto readable = buf.readable_span();
    EXPECT_EQ(readable[0], 'c');
}

TEST(IOBuffer, LargeBuffer) {
    io_buffer buf;
    std::vector<uint8_t> large_data(1024 * 1024, 0xAB);
    buf.append(std::span<const uint8_t>(large_data));

    EXPECT_EQ(buf.size(), 1024 * 1024);
    auto readable = buf.readable_span();
    EXPECT_EQ(readable[0], 0xAB);
    EXPECT_EQ(readable[1024 * 1024 - 1], 0xAB);
}

TEST(IOBuffer, Move) {
    io_buffer buf1;
    buf1.append("test data");

    io_buffer buf2 = std::move(buf1);
    EXPECT_EQ(buf2.size(), 9);
}

TEST(ScatterGatherRead, AddBuffer) {
    scatter_gather_read sg;

    std::vector<uint8_t> buf1(100);
    std::vector<uint8_t> buf2(200);

    sg.add_buffer(std::span<uint8_t>(buf1));
    sg.add_buffer(std::span<uint8_t>(buf2));

    EXPECT_EQ(sg.count(), 2);
    EXPECT_NE(sg.iov(), nullptr);
}

TEST(ScatterGatherRead, Clear) {
    scatter_gather_read sg;

    std::vector<uint8_t> buf(100);
    sg.add_buffer(std::span<uint8_t>(buf));
    EXPECT_EQ(sg.count(), 1);

    sg.clear();
    EXPECT_EQ(sg.count(), 0);
}

TEST(ScatterGatherWrite, AddBuffer) {
    scatter_gather_write sg;

    std::vector<uint8_t> buf1 = {1, 2, 3, 4, 5};
    std::vector<uint8_t> buf2 = {6, 7, 8, 9, 10};

    sg.add_buffer(std::span<const uint8_t>(buf1));
    sg.add_buffer(std::span<const uint8_t>(buf2));

    EXPECT_EQ(sg.count(), 2);
    EXPECT_NE(sg.iov(), nullptr);
}

TEST(ScatterGatherWrite, Clear) {
    scatter_gather_write sg;

    std::vector<uint8_t> buf = {1, 2, 3};
    sg.add_buffer(std::span<const uint8_t>(buf));
    EXPECT_EQ(sg.count(), 1);

    sg.clear();
    EXPECT_EQ(sg.count(), 0);
}

TEST(VectoredIO, ReadWriteLoopback) {
    int pipefd[2];
    ASSERT_EQ(pipe2(pipefd, O_NONBLOCK), 0);

    std::vector<uint8_t> write_buf1 = {1, 2, 3, 4, 5};
    std::vector<uint8_t> write_buf2 = {6, 7, 8, 9, 10};

    scatter_gather_write sg_write;
    sg_write.add_buffer(std::span<const uint8_t>(write_buf1));
    sg_write.add_buffer(std::span<const uint8_t>(write_buf2));

    auto write_result = write_vectored(pipefd[1], sg_write);
    ASSERT_TRUE(write_result.has_value());
    EXPECT_EQ(*write_result, 10);

    std::vector<uint8_t> read_buf1(5);
    std::vector<uint8_t> read_buf2(5);

    scatter_gather_read sg_read;
    sg_read.add_buffer(std::span<uint8_t>(read_buf1));
    sg_read.add_buffer(std::span<uint8_t>(read_buf2));

    auto read_result = read_vectored(pipefd[0], sg_read);
    ASSERT_TRUE(read_result.has_value());
    EXPECT_EQ(*read_result, 10);

    EXPECT_EQ(read_buf1, write_buf1);
    EXPECT_EQ(read_buf2, write_buf2);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST(VectoredIO, ReadFromInvalidFd) {
    scatter_gather_read sg;
    std::vector<uint8_t> buf(100);
    sg.add_buffer(std::span<uint8_t>(buf));

    auto result = read_vectored(-1, sg);
    EXPECT_FALSE(result.has_value());
}

TEST(VectoredIO, WriteToInvalidFd) {
    scatter_gather_write sg;
    std::vector<uint8_t> buf = {1, 2, 3};
    sg.add_buffer(std::span<const uint8_t>(buf));

    auto result = write_vectored(-1, sg);
    EXPECT_FALSE(result.has_value());
}

TEST(VectoredIO, PartialRead) {
    int pipefd[2];
    ASSERT_EQ(pipe2(pipefd, O_NONBLOCK), 0);

    std::vector<uint8_t> write_data = {1, 2, 3};
    [[maybe_unused]] auto written = write(pipefd[1], write_data.data(), 3);

    std::vector<uint8_t> read_buf(10);
    scatter_gather_read sg;
    sg.add_buffer(std::span<uint8_t>(read_buf));

    auto result = read_vectored(pipefd[0], sg);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 3);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST(VectoredIO, ReadWouldBlockReturnsError) {
    int pipefd[2];
    ASSERT_EQ(pipe2(pipefd, O_NONBLOCK), 0);

    scatter_gather_read sg;
    std::vector<uint8_t> buffer(16);
    sg.add_buffer(std::span<uint8_t>(buffer));

    auto result = read_vectored(pipefd[0], sg);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().value(), EAGAIN);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST(VectoredIO, WriteWouldBlockReturnsError) {
    int pipefd[2];
    ASSERT_EQ(pipe2(pipefd, O_NONBLOCK), 0);

    scatter_gather_write sg;
    std::vector<uint8_t> buffer(4096, 0x42);
    sg.add_buffer(std::span<const uint8_t>(buffer));

    size_t iterations = 0;
    while (true) {
        auto result = write_vectored(pipefd[1], sg);
        if (!result) {
            EXPECT_EQ(result.error().value(), EAGAIN);
            break;
        }

        EXPECT_GT(*result, 0U);
        ASSERT_LT(++iterations, 1024U);
    }

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST(VectoredIO, MultipleSmallBuffers) {
    int pipefd[2];
    ASSERT_EQ(pipe2(pipefd, O_NONBLOCK), 0);

    scatter_gather_write sg_write;
    std::vector<std::vector<uint8_t>> buffers;
    for (int i = 0; i < 10; ++i) {
        buffers.push_back({static_cast<uint8_t>(i)});
        sg_write.add_buffer(std::span<const uint8_t>(buffers.back()));
    }

    auto write_result = write_vectored(pipefd[1], sg_write);
    ASSERT_TRUE(write_result.has_value());
    EXPECT_EQ(*write_result, 10);

    std::vector<uint8_t> read_buf(10);
    scatter_gather_read sg_read;
    sg_read.add_buffer(std::span<uint8_t>(read_buf));

    auto read_result = read_vectored(pipefd[0], sg_read);
    ASSERT_TRUE(read_result.has_value());
    EXPECT_EQ(*read_result, 10);

    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(read_buf[i], i);
    }

    close(pipefd[0]);
    close(pipefd[1]);
}
