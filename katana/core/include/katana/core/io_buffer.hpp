#pragma once

#include "result.hpp"

#include <vector>
#include <span>
#include <cstdint>
#include <string_view>

#ifdef __linux__
#include <sys/uio.h>
#else
struct iovec {
    void* iov_base;
    size_t iov_len;
};
#endif

namespace katana {

class io_buffer {
public:
    io_buffer() = default;
    io_buffer(io_buffer&&) noexcept = default;
    io_buffer& operator=(io_buffer&&) noexcept = default;
    io_buffer(const io_buffer&) = default;
    io_buffer& operator=(const io_buffer&) = default;
    explicit io_buffer(size_t capacity);

    void append(std::span<const uint8_t> data);
    void append(std::string_view str);

    std::span<uint8_t> writable_span(size_t size);
    void commit(size_t bytes);

    [[nodiscard]] std::span<const uint8_t> readable_span() const noexcept;
    void consume(size_t bytes);

    [[nodiscard]] size_t size() const noexcept { return write_pos_ - read_pos_; }
    [[nodiscard]] size_t capacity() const noexcept { return buffer_.capacity(); }
    [[nodiscard]] bool empty() const noexcept { return read_pos_ == write_pos_; }

    void clear() noexcept;
    void reserve(size_t new_capacity);

private:
    void ensure_writable(size_t bytes);

    std::vector<uint8_t> buffer_;
    size_t read_pos_ = 0;
    size_t write_pos_ = 0;
};

class scatter_gather_read {
public:
    scatter_gather_read() = default;
    scatter_gather_read(scatter_gather_read&&) noexcept = default;
    scatter_gather_read& operator=(scatter_gather_read&&) noexcept = default;
    scatter_gather_read(const scatter_gather_read&) = default;
    scatter_gather_read& operator=(const scatter_gather_read&) = default;

    void add_buffer(std::span<uint8_t> buf);

    [[nodiscard]] const iovec* iov() const noexcept { return iovecs_.data(); }
    [[nodiscard]] size_t count() const noexcept { return iovecs_.size(); }

    void clear() noexcept;

private:
    std::vector<iovec> iovecs_;
};

class scatter_gather_write {
public:
    scatter_gather_write() = default;
    scatter_gather_write(scatter_gather_write&&) noexcept = default;
    scatter_gather_write& operator=(scatter_gather_write&&) noexcept = default;
    scatter_gather_write(const scatter_gather_write&) = default;
    scatter_gather_write& operator=(const scatter_gather_write&) = default;

    void add_buffer(std::span<const uint8_t> buf);

    [[nodiscard]] const iovec* iov() const noexcept { return iovecs_.data(); }
    [[nodiscard]] size_t count() const noexcept { return iovecs_.size(); }

    void clear() noexcept;

private:
    std::vector<iovec> iovecs_;
};

result<size_t> read_vectored(int32_t fd, scatter_gather_read& sg);
result<size_t> write_vectored(int32_t fd, scatter_gather_write& sg);

} // namespace katana
