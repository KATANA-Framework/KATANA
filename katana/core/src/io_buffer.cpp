#include "katana/core/io_buffer.hpp"

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <limits.h>
#include <new>

#ifdef __linux__
#include <sys/uio.h>
#include <unistd.h>
#endif

namespace katana {

namespace {
std::unique_ptr<uint8_t[], io_buffer::aligned_delete> allocate_raw(size_t n) {
    // Use default-initialized storage to avoid zero-fill overhead on hot path.
    if (n == 0) {
        return nullptr;
    }
    // Align to 64 bytes to keep memcpy in the fast path for AVX loads/stores.
    return std::unique_ptr<uint8_t[], io_buffer::aligned_delete>(
        static_cast<uint8_t*>(::operator new[](n, std::align_val_t(64))));
}
} // namespace

io_buffer::io_buffer() {
    capacity_ = INITIAL_CAPACITY;
    owner_ = allocate_raw(capacity_);
    data_ = owner_.get();
}

io_buffer::io_buffer(size_t capacity) : capacity_(capacity) {
    if (capacity_ == 0) {
        capacity_ = INITIAL_CAPACITY;
    }
    owner_ = allocate_raw(capacity_);
    data_ = owner_.get();
}

void io_buffer::append(std::span<const uint8_t> data) {
    const size_t data_size = data.size();
    const size_t new_write_pos = write_pos_ + data_size;

    // Fast path: enough space already available.
    if (new_write_pos <= capacity_) {
        std::memcpy(data_ + write_pos_, data.data(), data_size);
        write_pos_ = new_write_pos;
        return;
    }

    // Slow path: grow/compact then copy.
    ensure_writable(data_size);
    std::memcpy(data_ + write_pos_, data.data(), data_size);
    write_pos_ += data_size;
}

void io_buffer::append(std::string_view str) {
    append(std::span<const uint8_t>(
        reinterpret_cast<const uint8_t*>(static_cast<const void*>(str.data())), str.size()));
}

std::span<uint8_t> io_buffer::writable_span(size_t size) {
    if (write_pos_ + size > capacity_) {
        ensure_writable(size);
    }
    return std::span<uint8_t>(data_ + write_pos_, size);
}

void io_buffer::commit(size_t bytes) {
    write_pos_ += bytes;
}

std::span<const uint8_t> io_buffer::readable_span() const noexcept {
    return std::span<const uint8_t>(data_ + read_pos_, write_pos_ - read_pos_);
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
    if (new_capacity > capacity_) {
        // Allocate new storage and preserve unread data at the front.
        auto new_buffer = allocate_raw(new_capacity);
        size_t data_size = write_pos_ - read_pos_;
        if (data_size > 0) {
            std::memcpy(new_buffer.get(), data_ + read_pos_, data_size);
        }
        owner_ = std::move(new_buffer);
        data_ = owner_.get();
        capacity_ = new_capacity;
        write_pos_ = data_size;
        read_pos_ = 0;
    }
}

void io_buffer::compact_if_needed() {
    // Only compact if we've consumed a significant amount and fragmentation is high
    if (read_pos_ >= COMPACT_THRESHOLD && read_pos_ > size()) {
        // Validate invariant: write_pos_ must be >= read_pos_ to prevent integer underflow
        assert(write_pos_ >= read_pos_ && "Buffer invariant violated: write_pos_ < read_pos_");

        size_t data_size = write_pos_ - read_pos_;
        if (data_size > 0) {
            std::memmove(data_, data_ + read_pos_, data_size);
        }
        read_pos_ = 0;
        write_pos_ = data_size;
    }
}

void io_buffer::ensure_writable(size_t bytes) {
    size_t available = capacity_ > write_pos_ ? capacity_ - write_pos_ : 0;

    if (available < bytes) {
        compact_if_needed();
        available = capacity_ > write_pos_ ? capacity_ - write_pos_ : 0;

        if (available < bytes) {
            size_t current_cap = capacity_;
            size_t doubled_cap = current_cap > 0 ? current_cap * 2 : INITIAL_CAPACITY;

            if (current_cap > SIZE_MAX / 2) {
                doubled_cap = SIZE_MAX;
            }

            size_t required_cap = write_pos_ + bytes;
            if (required_cap < write_pos_ || required_cap < bytes) {
                throw std::bad_alloc();
            }

            size_t new_cap = std::max(doubled_cap, required_cap);

            auto new_buffer = allocate_raw(new_cap);
            size_t data_size = write_pos_ - read_pos_;
            if (data_size > 0) {
                std::memcpy(new_buffer.get(), data_ + read_pos_, data_size);
            }
            owner_ = std::move(new_buffer);
            data_ = owner_.get();
            capacity_ = new_cap;
            write_pos_ = data_size;
            read_pos_ = 0;
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
