#pragma once

// Minimal OTLP/HTTP (JSON) span exporter. Wire it into the tracing seam:
//
//   server.span_exporter(katana::tracing::otlp_http_exporter("http://localhost:4318/v1/traces"));
//
// It buffers spans and flushes them on a background thread to an OTLP/HTTP collector as OTLP/JSON.
// Not a high-throughput exporter (blocking POST per flush) — good enough to ship spans to a local
// collector without pulling in gRPC/protobuf. For heavy trace volume use a dedicated pipeline.

#include "http_client.hpp"
#include "tracing.hpp"

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace katana::tracing {

struct otlp_options {
    std::chrono::milliseconds flush_interval{2000};
    std::size_t max_batch = 512;
    bool verify_tls = true;
    std::string service_name = "katana";
};

namespace detail {

inline void otlp_json_escape(std::string& out, std::string_view s) {
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out.push_back(c);
        }
    }
}

struct otlp_span {
    std::string trace_id, span_id, parent_span_id, name;
    int status = 0;
    std::int64_t start_ns = 0, end_ns = 0;
};

struct otlp_state {
    std::string endpoint;
    otlp_options opts;
    std::mutex mu;
    std::vector<otlp_span> buffer;

    void flush() {
        std::vector<otlp_span> batch;
        {
            std::lock_guard<std::mutex> lock(mu);
            if (buffer.empty()) {
                return;
            }
            batch.swap(buffer);
        }
        std::string body;
        body += R"({"resourceSpans":[{"resource":{"attributes":[{"key":"service.name","value":{"stringValue":")";
        otlp_json_escape(body, opts.service_name);
        body += R"("}}]},"scopeSpans":[{"spans":[)";
        for (std::size_t i = 0; i < batch.size(); ++i) {
            const auto& s = batch[i];
            if (i != 0) {
                body += ',';
            }
            // OTLP status: 2 = Error (5xx), else 1 = Ok.
            const int code = s.status >= 500 ? 2 : 1;
            body += R"({"traceId":")" + s.trace_id + R"(","spanId":")" + s.span_id + R"(",)";
            if (!s.parent_span_id.empty() && s.parent_span_id != "0000000000000000") {
                body += R"("parentSpanId":")" + s.parent_span_id + R"(",)";
            }
            body += R"("name":")";
            otlp_json_escape(body, s.name);
            body += R"(","kind":2,"startTimeUnixNano":")" + std::to_string(s.start_ns) +
                    R"(","endTimeUnixNano":")" + std::to_string(s.end_ns) +
                    R"(","status":{"code":)" + std::to_string(code) + "}}";
        }
        body += "]}]}]}";
        (void)http::http_post(endpoint, "application/json", body, {.verify_tls = opts.verify_tls});
    }
};

} // namespace detail

inline std::function<void(const span_record&)> otlp_http_exporter(std::string endpoint,
                                                                  otlp_options opts = {}) {
    auto state = std::make_shared<detail::otlp_state>();
    state->endpoint = std::move(endpoint);
    state->opts = std::move(opts);

    // Background flusher: exits once the exporter (and its state) is dropped.
    std::weak_ptr<detail::otlp_state> weak = state;
    const auto interval = state->opts.flush_interval;
    std::thread([weak, interval]() {
        for (;;) {
            std::this_thread::sleep_for(interval);
            auto s = weak.lock();
            if (!s) {
                return;
            }
            s->flush();
        }
    }).detach();

    return [state](const span_record& record) {
        const auto now = std::chrono::system_clock::now().time_since_epoch();
        const std::int64_t end_ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
        detail::otlp_span span;
        span.trace_id = record.span.trace_id_hex();
        span.span_id = record.span.span_id_hex();
        span.parent_span_id = record.span.parent_span_id_hex();
        span.name.assign(record.name.begin(), record.name.end());
        span.status = record.status;
        span.end_ns = end_ns;
        span.start_ns = end_ns - record.duration_micros * 1000;
        std::lock_guard<std::mutex> lock(state->mu);
        if (state->buffer.size() < state->opts.max_batch) {
            state->buffer.push_back(std::move(span));
        }
    };
}

} // namespace katana::tracing
