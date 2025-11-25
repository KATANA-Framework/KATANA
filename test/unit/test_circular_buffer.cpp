#include "katana/core/circular_buffer.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <vector>

using namespace katana;

TEST_CASE("circular_buffer basic operations", "[circular_buffer]") {
    circular_buffer buf(64);

    SECTION("write and read") {
        REQUIRE(buf.empty());
        REQUIRE(buf.size() == 0);

        const char* data = "hello";
        size_t written = buf.write(std::span(reinterpret_cast<const uint8_t*>(data), 5));
        REQUIRE(written == 5);
        REQUIRE(buf.size() == 5);
        REQUIRE(!buf.empty());

        uint8_t read_buf[5];
        size_t read = buf.read(std::span(read_buf, 5));
        REQUIRE(read == 5);
        REQUIRE(std::memcmp(read_buf, data, 5) == 0);
        REQUIRE(buf.empty());
    }

    SECTION("wrap around") {
        std::vector<uint8_t> data(40, 'A');
        buf.write(std::span(data));
        REQUIRE(buf.size() == 40);

        buf.consume(30);
        REQUIRE(buf.size() == 10);

        std::vector<uint8_t> more_data(40, 'B');
        size_t written = buf.write(std::span(more_data));
        REQUIRE(written == 40);

        auto peek = buf.peek();
        REQUIRE(peek.size() >= 10);
    }

    SECTION("capacity and reserve") {
        REQUIRE(buf.capacity() >= 64);

        buf.reserve(256);
        REQUIRE(buf.capacity() >= 256);
    }
}

TEST_CASE("circular_buffer stress test", "[circular_buffer]") {
    circular_buffer buf(128);

    std::vector<uint8_t> pattern;
    for (int i = 0; i < 256; ++i) {
        pattern.push_back(static_cast<uint8_t>(i));
    }

    size_t total_written = 0;
    size_t total_read = 0;

    for (int round = 0; round < 100; ++round) {
        size_t chunk_size = (round % 50) + 1;
        size_t offset = total_written % pattern.size();
        size_t to_write = std::min(chunk_size, pattern.size() - offset);

        std::span write_span(pattern.data() + offset, to_write);
        size_t written = buf.write(write_span);
        total_written += written;

        if (buf.size() > 64) {
            std::vector<uint8_t> read_buf(32);
            size_t read = buf.read(std::span(read_buf));
            total_read += read;
        }
    }

    while (!buf.empty()) {
        std::vector<uint8_t> read_buf(64);
        size_t read = buf.read(std::span(read_buf));
        total_read += read;
    }

    REQUIRE(total_written == total_read);
}
