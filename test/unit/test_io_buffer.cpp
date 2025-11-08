#include "katana/core/io_buffer.hpp"

#include <gtest/gtest.h>
#include <cstring>

using namespace katana;

TEST(IOBuffer, InitialState) {
    io_buffer buffer(1024);

    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
}

TEST(IOBuffer, AppendString) {
    io_buffer buffer(1024);
    std::string_view data = "Hello, World!";

    buffer.append(data);

    EXPECT_FALSE(buffer.empty());
    EXPECT_EQ(buffer.size(), data.size());

    auto readable = buffer.readable_span();
    EXPECT_EQ(readable.size(), data.size());
    EXPECT_EQ(std::memcmp(readable.data(), data.data(), data.size()), 0);
}

TEST(IOBuffer, AppendMultiple) {
    io_buffer buffer(1024);

    buffer.append("Hello");
    buffer.append(" ");
    buffer.append("World");

    EXPECT_EQ(buffer.size(), 11);

    auto readable = buffer.readable_span();
    std::string result(reinterpret_cast<const char*>(readable.data()), readable.size());
    EXPECT_EQ(result, "Hello World");
}

TEST(IOBuffer, Consume) {
    io_buffer buffer(1024);
    buffer.append("Hello, World!");

    buffer.consume(7);

    EXPECT_EQ(buffer.size(), 6);

    auto readable = buffer.readable_span();
    std::string result(reinterpret_cast<const char*>(readable.data()), readable.size());
    EXPECT_EQ(result, "World!");
}

TEST(IOBuffer, ConsumeAll) {
    io_buffer buffer(1024);
    buffer.append("Test");

    buffer.consume(4);

    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
}

TEST(IOBuffer, WritableSpan) {
    io_buffer buffer(1024);

    auto writable = buffer.writable_span(10);
    EXPECT_GE(writable.size(), 10);

    std::memcpy(writable.data(), "TestData", 8);
    buffer.commit(8);

    EXPECT_EQ(buffer.size(), 8);

    auto readable = buffer.readable_span();
    std::string result(reinterpret_cast<const char*>(readable.data()), readable.size());
    EXPECT_EQ(result, "TestData");
}

TEST(IOBuffer, CommitAfterWrite) {
    io_buffer buffer(1024);

    auto writable = buffer.writable_span(100);
    size_t written = 50;
    std::memset(writable.data(), 'A', written);
    buffer.commit(written);

    EXPECT_EQ(buffer.size(), written);
}

TEST(IOBuffer, Clear) {
    io_buffer buffer(1024);
    buffer.append("Some data");

    buffer.clear();

    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
}

TEST(IOBuffer, AppendBytes) {
    io_buffer buffer(1024);
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05};

    buffer.append(std::span<const uint8_t>(data));

    EXPECT_EQ(buffer.size(), 5);

    auto readable = buffer.readable_span();
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(readable[i], data[i]);
    }
}

TEST(IOBuffer, Reserve) {
    io_buffer buffer(16);
    size_t initial_capacity = buffer.capacity();

    buffer.reserve(1024);

    EXPECT_GE(buffer.capacity(), 1024);
    EXPECT_GT(buffer.capacity(), initial_capacity);
}

TEST(IOBuffer, LargeAppend) {
    io_buffer buffer(64);
    std::string large_data(10000, 'X');

    buffer.append(large_data);

    EXPECT_EQ(buffer.size(), large_data.size());

    auto readable = buffer.readable_span();
    std::string result(reinterpret_cast<const char*>(readable.data()), readable.size());
    EXPECT_EQ(result, large_data);
}

TEST(IOBuffer, ConsumePartialReadAgain) {
    io_buffer buffer(1024);
    buffer.append("0123456789");

    buffer.consume(5);
    EXPECT_EQ(buffer.size(), 5);

    auto readable = buffer.readable_span();
    std::string result(reinterpret_cast<const char*>(readable.data()), readable.size());
    EXPECT_EQ(result, "56789");

    buffer.consume(3);
    EXPECT_EQ(buffer.size(), 2);

    readable = buffer.readable_span();
    result = std::string(reinterpret_cast<const char*>(readable.data()), readable.size());
    EXPECT_EQ(result, "89");
}
