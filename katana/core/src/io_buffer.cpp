#include "katana/core/io_buffer.hpp"

#include <algorithm>
#include <cstring>
#include <cerrno>

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
        reinterpret_cast<const uint8_t*>(static_cast<const void*>(str.data())),
        str.size()
    ));
}

std::span<uint8_t> io_buffer::writable_span(size_t size) {
    ensure_writable(size);
    return std::span<uint8_t>(buffer_.data() + write_pos_, size);
}

void io_buffer::commit(size_t bytes) {
    write_pos_ += bytes;
}

std::span<const uint8_t> io_buffer::readable_span() const noexcept {
    return std::span<const uint8_t>(
        buffer_.data() + read_pos_,
        write_pos_ - read_pos_
    );
}

void io_buffer::consume(size_t bytes) {
    read_pos_ += std::min(bytes, size());

    if (read_pos_ == write_pos_) {
        read_pos_ = 0;
        write_pos_ = 0;
    }
}

void io_buffer::clear() noexcept {
    read_pos_ = 0;
    write_pos_ = 0;
}

void io_buffer::reserve(size_t new_capacity) {
    if (new_capacity > buffer_.size()) {
        buffer_.resize(new_capacity);
    }
}

void io_buffer::ensure_writable(size_t bytes) {
    size_t available = buffer_.size() - write_pos_;

    if (available < bytes) {
        if (read_pos_ > 0) {
            size_t data_size = write_pos_ - read_pos_;
            if (data_size > 0) {
                std::memmove(buffer_.data(), buffer_.data() + read_pos_, data_size);
            }
            read_pos_ = 0;
            write_pos_ = data_size;
            available = buffer_.size() - write_pos_;
        }

        if (available < bytes) {
            size_t current_size = buffer_.size();
            size_t doubled_size = current_size;

            if (current_size > 0 && current_size <= SIZE_MAX / 2) {
                doubled_size = current_size * 2;
            }

            size_t required_size = write_pos_ + bytes;
            if (required_size < write_pos_ || required_size < bytes) {
                throw std::bad_alloc();
            }

            size_t new_size = std::max(doubled_size, required_size);
            buffer_.resize(new_size);
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

result<size_t> read_vectored(int fd, scatter_gather_read& sg) {
#ifdef __linux__
    ssize_t result = readv(fd, const_cast<iovec*>(sg.iov()), static_cast<int>(sg.count()));

    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return std::unexpected(std::error_code(errno, std::system_category()));
    }

    return static_cast<size_t>(result);
#else
    (void)fd;
    (void)sg;
    return std::unexpected(make_error_code(error_code::ok));
#endif
}

result<size_t> write_vectored(int fd, scatter_gather_write& sg) {
#ifdef __linux__
    ssize_t result = writev(fd, sg.iov(), static_cast<int>(sg.count()));

    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
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
