#include "katana/core/io_buffer.hpp"

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <limits.h>

#ifdef __linux__
#include <sys/uio.h>
#include <unistd.h>
#endif

namespace katana {

io_buffer::io_buffer(size_t capacity) {
    buffer_.reserve(capacity);
}

void io_buffer::append(std::span<const uint8_t> data) {
    ensure_writable(data.size());
    std::memcpy(buffer_.data() + write_pos_, data.data(), data.size());
    write_pos_ += data.size();
}

void io_buffer::append(std::string_view str) {
    append(std::span<const uint8_t>(
        reinterpret_cast<const uint8_t*>(static_cast<const void*>(str.data())), str.size()));
}

std::span<uint8_t> io_buffer::writable_span(size_t size) {
    ensure_writable(size);
    return std::span<uint8_t>(buffer_.data() + write_pos_, size);
}

void io_buffer::commit(size_t bytes) {
    write_pos_ += bytes;
}

std::span<const uint8_t> io_buffer::readable_span() const noexcept {
    return std::span<const uint8_t>(buffer_.data() + read_pos_, write_pos_ - read_pos_);
}

void io_buffer::consume(size_t bytes) {
    read_pos_ += std::min(bytes, size());

    if (read_pos_ == write_pos_) {
        read_pos_ = 0;
        write_pos_ = 0;
    } else {
        compact_if_needed();
    }
}

void io_buffer::clear() noexcept {
    read_pos_ = 0;
    write_pos_ = 0;
}

void io_buffer::reserve(size_t new_capacity) {
    if (new_capacity > buffer_.capacity()) {
        buffer_.reserve(new_capacity);
    }
}

void io_buffer::compact_if_needed() {
    // Only compact if we've consumed a significant amount and fragmentation is high
    if (read_pos_ >= COMPACT_THRESHOLD && read_pos_ > size()) {
        // Validate invariant: write_pos_ must be >= read_pos_ to prevent integer underflow
        assert(write_pos_ >= read_pos_ && "Buffer invariant violated: write_pos_ < read_pos_");

        size_t data_size = write_pos_ - read_pos_;
        if (data_size > 0) {
            std::memmove(buffer_.data(), buffer_.data() + read_pos_, data_size);
        }
        read_pos_ = 0;
        write_pos_ = data_size;
    }
}

void io_buffer::ensure_writable(size_t bytes) {
    size_t current_size = buffer_.size();
    size_t available = current_size > write_pos_ ? current_size - write_pos_ : 0;

    if (available < bytes) {
        compact_if_needed();
        available = current_size > write_pos_ ? current_size - write_pos_ : 0;

        if (available < bytes) {
            size_t current_cap = buffer_.capacity();
            size_t doubled_cap = current_cap > 0 ? current_cap * 2 : 64;

            if (current_cap > SIZE_MAX / 2) {
                doubled_cap = SIZE_MAX;
            }

            size_t required_cap = write_pos_ + bytes;
            if (required_cap < write_pos_ || required_cap < bytes) {
                throw std::bad_alloc();
            }

            size_t new_cap = std::max(doubled_cap, required_cap);
            buffer_.reserve(new_cap);
            buffer_.resize(write_pos_ + bytes);
        }
    }
}

void scatter_gather_read::add_buffer(std::span<uint8_t> buf) {
    if (!buf.empty()) {
        iovecs_.push_back(iovec{buf.data(), buf.size()});
    }
}

void scatter_gather_read::clear() noexcept {
    iovecs_.clear();
}

void scatter_gather_write::add_buffer(std::span<const uint8_t> buf) {
    if (!buf.empty()) {
        iovecs_.push_back(iovec{const_cast<uint8_t*>(buf.data()), buf.size()});
    }
}

void scatter_gather_write::clear() noexcept {
    iovecs_.clear();
}

result<size_t> read_vectored(int32_t fd, scatter_gather_read& sg) {
#ifdef __linux__
    int iov_count = static_cast<int>(std::min<size_t>(sg.count(), IOV_MAX));
    ssize_t result = readv(fd, const_cast<iovec*>(sg.iov()), iov_count);

    if (result < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return static_cast<size_t>(result);
#else
    (void)fd;
    (void)sg;
    return std::unexpected(make_error_code(error_code::ok));
#endif
}

result<size_t> write_vectored(int32_t fd, scatter_gather_write& sg) {
#ifdef __linux__
    int iov_count = static_cast<int>(std::min<size_t>(sg.count(), IOV_MAX));
    ssize_t result = writev(fd, sg.iov(), iov_count);

    if (result < 0) {
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return static_cast<size_t>(result);
#else
    (void)fd;
    (void)sg;
    return std::unexpected(make_error_code(error_code::ok));
#endif
}

} // namespace katana
