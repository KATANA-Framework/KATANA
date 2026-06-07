#pragma once

// Distributed tracing primitives: W3C Trace Context (`traceparent`) parsing/formatting and a
// per-request span context. A 128-bit trace id flows end-to-end; each hop creates its own
// 64-bit span id and records the inbound span as its parent. This is transport-agnostic — the
// server records a span per request (default: a structured log line); an OTLP exporter can be
// layered on later by consuming the same span_context.
//
//   auto sc = katana::tracing::start_server_span(req.header("traceparent"));
//   // sc.trace_id flows to downstream calls via sc.to_traceparent()

#include <cstdint>
#include <optional>
#include <random>
#include <string>
#include <string_view>

namespace katana::tracing {

namespace detail {

// A cheap thread-local PRNG for id generation (seeded once from random_device). Trace/span ids
// only need to be unique, not cryptographically strong.
inline uint64_t next_random_u64() {
    thread_local uint64_t state = [] {
        std::random_device rd;
        uint64_t s = (static_cast<uint64_t>(rd()) << 32) ^ static_cast<uint64_t>(rd());
        return s != 0 ? s : 0x9E3779B97F4A7C15ULL;
    }();
    // xorshift64*
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 0x2545F4914F6CDD1DULL;
}

constexpr char hex_digit(uint8_t v) {
    return static_cast<char>(v < 10 ? '0' + v : 'a' + (v - 10));
}

inline void append_hex_u64(std::string& out, uint64_t v) {
    for (int shift = 60; shift >= 0; shift -= 4) {
        out.push_back(hex_digit(static_cast<uint8_t>((v >> shift) & 0xF)));
    }
}

// Parse exactly `len` hex chars starting at s into a uint64 accumulator chain. Returns false on
// any non-hex char.
inline bool parse_hex(std::string_view s, uint64_t& out) {
    out = 0;
    for (char c : s) {
        uint64_t d = 0;
        if (c >= '0' && c <= '9') {
            d = static_cast<uint64_t>(c - '0');
        } else if (c >= 'a' && c <= 'f') {
            d = static_cast<uint64_t>(c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            d = static_cast<uint64_t>(c - 'A' + 10);
        } else {
            return false;
        }
        out = (out << 4) | d;
    }
    return true;
}

} // namespace detail

// A 128-bit trace id is carried as two 64-bit halves (high then low).
struct span_context {
    uint64_t trace_id_hi = 0;
    uint64_t trace_id_lo = 0;
    uint64_t span_id = 0;        // this hop's span
    uint64_t parent_span_id = 0; // inbound span (0 = root)
    bool sampled = false;

    [[nodiscard]] bool valid() const noexcept {
        return (trace_id_hi != 0 || trace_id_lo != 0) && span_id != 0;
    }

    [[nodiscard]] std::string trace_id_hex() const {
        std::string out;
        out.reserve(32);
        detail::append_hex_u64(out, trace_id_hi);
        detail::append_hex_u64(out, trace_id_lo);
        return out;
    }

    [[nodiscard]] std::string span_id_hex() const {
        std::string out;
        out.reserve(16);
        detail::append_hex_u64(out, span_id);
        return out;
    }

    [[nodiscard]] std::string parent_span_id_hex() const {
        std::string out;
        out.reserve(16);
        detail::append_hex_u64(out, parent_span_id);
        return out;
    }

    // W3C `traceparent` for propagating THIS span to a downstream service.
    [[nodiscard]] std::string to_traceparent() const {
        std::string out = "00-";
        out += trace_id_hex();
        out += '-';
        out += span_id_hex();
        out += sampled ? "-01" : "-00";
        return out;
    }
};

// The fields decoded from an inbound `traceparent` header.
struct parsed_traceparent {
    uint64_t trace_id_hi = 0;
    uint64_t trace_id_lo = 0;
    uint64_t parent_span_id = 0;
    bool sampled = false;
};

// Parse a W3C `traceparent`: version(2)-traceid(32)-spanid(16)-flags(2), all lowercase hex.
// Rejects all-zero trace/span ids. Tolerates future versions (>00) per spec as long as the
// shape matches.
[[nodiscard]] inline std::optional<parsed_traceparent> parse_traceparent(std::string_view h) {
    if (h.size() < 55) { // "00-" + 32 + "-" + 16 + "-" + 2
        return std::nullopt;
    }
    if (h[2] != '-' || h[35] != '-' || h[52] != '-') {
        return std::nullopt;
    }
    uint64_t version = 0;
    if (!detail::parse_hex(h.substr(0, 2), version) || version == 0xFF) {
        return std::nullopt;
    }
    parsed_traceparent out;
    uint64_t flags = 0;
    if (!detail::parse_hex(h.substr(3, 16), out.trace_id_hi) ||
        !detail::parse_hex(h.substr(19, 16), out.trace_id_lo) ||
        !detail::parse_hex(h.substr(36, 16), out.parent_span_id) ||
        !detail::parse_hex(h.substr(53, 2), flags)) {
        return std::nullopt;
    }
    if ((out.trace_id_hi == 0 && out.trace_id_lo == 0) || out.parent_span_id == 0) {
        return std::nullopt; // invalid per spec
    }
    out.sampled = (flags & 0x01) != 0;
    return out;
}

// Begin a server span: continue the inbound trace if a valid `traceparent` is present, otherwise
// start a fresh trace. Always allocates a new span id for this hop.
[[nodiscard]] inline span_context start_server_span(std::optional<std::string_view> traceparent) {
    span_context sc;
    sc.span_id = detail::next_random_u64();
    if (sc.span_id == 0) {
        sc.span_id = 1;
    }
    if (traceparent) {
        if (auto parsed = parse_traceparent(*traceparent)) {
            sc.trace_id_hi = parsed->trace_id_hi;
            sc.trace_id_lo = parsed->trace_id_lo;
            sc.parent_span_id = parsed->parent_span_id;
            sc.sampled = parsed->sampled;
            return sc;
        }
    }
    // New root trace.
    sc.trace_id_hi = detail::next_random_u64();
    sc.trace_id_lo = detail::next_random_u64();
    if (sc.trace_id_hi == 0 && sc.trace_id_lo == 0) {
        sc.trace_id_lo = 1;
    }
    sc.parent_span_id = 0;
    sc.sampled = true; // default sampler: always-on
    return sc;
}

} // namespace katana::tracing
