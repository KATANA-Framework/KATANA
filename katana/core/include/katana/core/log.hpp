#pragma once

// Structured JSON logging: one JSON object per line, with levels, a timestamp, a message and
// typed key/value fields. Cheap when a line is below the configured level (the event does no
// work). Thread-safe: each complete line is written under a lock.
//
//   katana::log::info("request handled").field("method", "GET").field("status", 200);
//   => {"level":"info","ts":1780839900123,"msg":"request handled","method":"GET","status":200}

#include "katana/core/serde.hpp" // escape_json_string_into

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <string>
#include <string_view>

namespace katana::log {

enum class level : int { trace = 0, debug = 1, info = 2, warn = 3, error = 4, off = 5 };

namespace detail {

inline std::atomic<int>& min_level_storage() {
    static std::atomic<int> value{static_cast<int>(level::info)};
    return value;
}

inline std::atomic<std::FILE*>& sink_storage() {
    static std::atomic<std::FILE*> value{stderr};
    return value;
}

inline std::mutex& write_mutex() {
    static std::mutex m;
    return m;
}

[[nodiscard]] constexpr std::string_view level_name(level l) noexcept {
    switch (l) {
    case level::trace: return "trace";
    case level::debug: return "debug";
    case level::info: return "info";
    case level::warn: return "warn";
    case level::error: return "error";
    case level::off: return "off";
    }
    return "info";
}

[[nodiscard]] inline int64_t epoch_millis() noexcept {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

} // namespace detail

// Set the minimum level that is emitted (anything below is dropped cheaply).
inline void set_min_level(level l) noexcept {
    detail::min_level_storage().store(static_cast<int>(l), std::memory_order_relaxed);
}

[[nodiscard]] inline level min_level() noexcept {
    return static_cast<level>(detail::min_level_storage().load(std::memory_order_relaxed));
}

// Redirect output (default stderr). Pass e.g. stdout or an open log file.
inline void set_sink(std::FILE* f) noexcept {
    detail::sink_storage().store(f, std::memory_order_relaxed);
}

// A single log line built fluently and flushed when it goes out of scope.
class event {
public:
    event(level lvl, std::string_view msg)
        : active_(static_cast<int>(lvl) >=
                  detail::min_level_storage().load(std::memory_order_relaxed)) {
        if (!active_) {
            return;
        }
        buffer_ = "{\"level\":\"";
        buffer_ += detail::level_name(lvl);
        buffer_ += "\",\"ts\":";
        buffer_ += std::to_string(detail::epoch_millis());
        buffer_ += ",\"msg\":\"";
        katana::serde::escape_json_string_into(msg, buffer_);
        buffer_ += '"';
    }

    event(const event&) = delete;
    event& operator=(const event&) = delete;

    // Movable so an event can be returned (e.g. from a context helper) and chained. The
    // moved-from event is deactivated so it never flushes.
    event(event&& other) noexcept
        : active_(other.active_), buffer_(std::move(other.buffer_)) {
        other.active_ = false; // deactivated: the moved-from event won't flush
    }
    event& operator=(event&&) = delete;

    ~event() { flush(); }

    event& field(std::string_view key, std::string_view value) {
        if (active_) {
            begin_key(key);
            buffer_ += '"';
            katana::serde::escape_json_string_into(value, buffer_);
            buffer_ += '"';
        }
        return *this;
    }

    event& field(std::string_view key, const char* value) {
        return field(key, std::string_view(value));
    }

    event& field(std::string_view key, int64_t value) {
        if (active_) {
            begin_key(key);
            buffer_ += std::to_string(value);
        }
        return *this;
    }

    event& field(std::string_view key, int value) {
        return field(key, static_cast<int64_t>(value));
    }

    event& field(std::string_view key, bool value) {
        if (active_) {
            begin_key(key);
            buffer_ += value ? "true" : "false";
        }
        return *this;
    }

private:
    void begin_key(std::string_view key) {
        buffer_ += ",\"";
        katana::serde::escape_json_string_into(key, buffer_);
        buffer_ += "\":";
    }

    void flush() noexcept {
        if (!active_) {
            return;
        }
        buffer_ += "}\n";
        std::FILE* sink = detail::sink_storage().load(std::memory_order_relaxed);
        if (sink != nullptr) {
            std::lock_guard<std::mutex> lock(detail::write_mutex());
            std::fwrite(buffer_.data(), 1, buffer_.size(), sink);
        }
    }

    bool active_;
    std::string buffer_;
};

inline event trace(std::string_view msg) { return event(level::trace, msg); }
inline event debug(std::string_view msg) { return event(level::debug, msg); }
inline event info(std::string_view msg) { return event(level::info, msg); }
inline event warn(std::string_view msg) { return event(level::warn, msg); }
inline event error(std::string_view msg) { return event(level::error, msg); }

} // namespace katana::log
