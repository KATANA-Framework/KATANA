#include "katana/core/http_server.hpp"
#include "katana/core/detail/syscall_metrics.hpp"
#include "katana/core/problem.hpp"

#include <atomic>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sstream>
#include <sys/socket.h>

// Debug logging disabled for performance
#define DEBUG_LOG(fmt, ...)                                                                        \
    do {                                                                                           \
    } while (0)

namespace katana {
namespace http {

namespace {
constexpr size_t SMALL_CONTIGUOUS_RESPONSE_BODY_THRESHOLD = 256;
constexpr size_t PIPELINE_RESPONSE_BATCH_LIMIT = 64 * 1024;

struct deferred_response_state {
    server* owner = nullptr;
    reactor* owner_reactor = nullptr;
    std::weak_ptr<void> connection;
    std::atomic<bool> resolved{false};
};

struct deferred_response_delivery {
    std::shared_ptr<deferred_response_state> state;
    response resp;
};

bool try_resolve_deferred_response(const std::shared_ptr<deferred_response_state>& state) {
    bool expected = false;
    return state != nullptr &&
           state->resolved.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
}

bool schedule_request_context_task(void* user, http::request_context::scheduled_task task) {
    if (user == nullptr) {
        return false;
    }
    auto* owner_reactor = static_cast<reactor*>(user);
    return owner_reactor->schedule(std::move(task));
}

struct conn_close_counters {
    std::atomic<uint64_t> read_error{0};
    std::atomic<uint64_t> read_eof{0};
    std::atomic<uint64_t> parse_error{0};
    std::atomic<uint64_t> write_error{0};
    std::atomic<uint64_t> close_header{0};
};

conn_close_counters& close_counters() {
    static conn_close_counters counters;
    return counters;
}

bool conn_debug_enabled() {
    static bool enabled = std::getenv("KATANA_CONN_DEBUG") != nullptr;
    return enabled;
}

bool parser_debug_enabled() {
    static bool enabled = std::getenv("KATANA_HTTP_PARSER_DEBUG") != nullptr;
    return enabled;
}

const char* parser_state_name(parser::state state) noexcept {
    switch (state) {
    case parser::state::request_line:
        return "request_line";
    case parser::state::headers:
        return "headers";
    case parser::state::body:
        return "body";
    case parser::state::chunk_size:
        return "chunk_size";
    case parser::state::chunk_data:
        return "chunk_data";
    case parser::state::chunk_trailer:
        return "chunk_trailer";
    case parser::state::complete:
        return "complete";
    }
    return "unknown";
}

std::string escape_preview(std::string_view bytes) {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (char raw_ch : bytes) {
        const auto ch = static_cast<unsigned char>(raw_ch);
        switch (ch) {
        case '\r':
            out << "\\r";
            break;
        case '\n':
            out << "\\n";
            break;
        case '\t':
            out << "\\t";
            break;
        case '\\':
            out << "\\\\";
            break;
        default:
            if (std::isprint(ch)) {
                out << static_cast<char>(ch);
            } else {
                out << "\\x" << std::setw(2) << static_cast<unsigned int>(ch);
            }
            break;
        }
    }
    return out.str();
}

void maybe_log_close(const char* reason, uint64_t count) {
    if (!conn_debug_enabled()) {
        return;
    }
    if (count <= 20 || count % 1000 == 0) {
        std::cerr << "[conn_debug] close " << reason << " count=" << count << "\n";
    }
}

bool getenv_bool(const char* name, bool fallback) {
    const char* value = std::getenv(name);
    if (!value || !*value) {
        return fallback;
    }

    return std::strcmp(value, "1") == 0 || std::strcmp(value, "true") == 0 ||
           std::strcmp(value, "TRUE") == 0 || std::strcmp(value, "yes") == 0 ||
           std::strcmp(value, "YES") == 0;
}

void configure_client_socket(int fd) {
    if (fd < 0) {
        return;
    }

    int nodelay = getenv_bool("KATANA_TCP_NODELAY", true) ? 1 : 0;
    (void)::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

#ifdef TCP_QUICKACK
    int quickack = getenv_bool("KATANA_TCP_QUICKACK", false) ? 1 : 0;
    if (quickack != 0) {
        (void)::setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, &quickack, sizeof(quickack));
    }
#endif
}

void prepare_response_storage(std::string& head, std::string& body, response& resp) {
    head.clear();
    body.clear();

    if (resp.chunked || resp.body.size() <= SMALL_CONTIGUOUS_RESPONSE_BODY_THRESHOLD) {
        resp.serialize_into(head);
        return;
    }

    resp.serialize_head_into(head);
    body = std::move(resp.body);
}

} // namespace

server::flush_result server::flush_active_response(connection_state& state) {
    const size_t head_size = state.active_response.size();
    const size_t body_size = state.active_response_body.size();
    const size_t total_size = head_size + body_size;

    while (state.write_pos < total_size) {
        result<size_t> write_result = size_t{0};
        if (body_size == 0) {
            auto remaining = std::string_view(state.active_response).substr(state.write_pos);
            write_result = state.transport_write(as_bytes(remaining));
        } else {
            iovec iov[2];
            size_t iov_count = 0;
            if (state.write_pos < head_size) {
                iov[iov_count].iov_base = state.active_response.data() + state.write_pos;
                iov[iov_count].iov_len = head_size - state.write_pos;
                ++iov_count;
                iov[iov_count].iov_base = state.active_response_body.data();
                iov[iov_count].iov_len = body_size;
                ++iov_count;
            } else {
                const size_t body_offset = state.write_pos - head_size;
                iov[iov_count].iov_base = state.active_response_body.data() + body_offset;
                iov[iov_count].iov_len = body_size - body_offset;
                ++iov_count;
            }
            write_result = state.transport_writev(iov, iov_count);
        }

        if (!write_result) {
            auto err_val = write_result.error().value();
            auto count = ++close_counters().write_error;
            if (conn_debug_enabled() && (count <= 20 || count % 1000 == 0)) {
                std::cerr << "[conn_debug] close write_error count=" << count
                          << " errno=" << err_val << "\n";
            }
            return flush_result::error;
        }

        if (*write_result == 0) {
            return flush_result::blocked;
        }

        state.write_pos += *write_result;
    }

    state.active_response.clear();
    state.active_response_body.clear();
    state.write_pos = 0;
    return flush_result::complete;
}

void server::prepare_active_response(connection_state& state, response& resp) {
    prepare_response_storage(state.active_response, state.active_response_body, resp);
}

deferred_response_handle server::make_deferred_response_handle(void* user) {
    auto* state = static_cast<connection_state*>(user);
    if (state == nullptr || state->owner_server == nullptr || state->owner_reactor == nullptr) {
        return {};
    }

    auto deferred_state = std::make_shared<deferred_response_state>();
    deferred_state->owner = state->owner_server;
    deferred_state->owner_reactor = state->owner_reactor;
    deferred_state->connection = state->shared_from_this();
    return deferred_response_handle(std::move(deferred_state),
                                    &server::complete_deferred_response_opaque,
                                    &server::cancel_deferred_response_opaque);
}

bool server::complete_deferred_response_opaque(std::shared_ptr<void> opaque_state, response resp) {
    auto state = std::static_pointer_cast<deferred_response_state>(std::move(opaque_state));
    if (!try_resolve_deferred_response(state) || state->owner == nullptr ||
        state->owner_reactor == nullptr) {
        return false;
    }

    auto delivery = std::make_shared<deferred_response_delivery>();
    delivery->state = state;
    delivery->resp = std::move(resp);
    return state->owner_reactor->schedule([delivery]() {
        auto connection = delivery->state->connection.lock();
        if (!connection) {
            return;
        }

        auto typed_connection = std::static_pointer_cast<connection_state>(connection);
        delivery->state->owner->complete_deferred_response(
            *typed_connection, std::move(delivery->resp), *delivery->state->owner_reactor);
    });
}

void server::cancel_deferred_response_opaque(std::shared_ptr<void> opaque_state) {
    auto state = std::static_pointer_cast<deferred_response_state>(std::move(opaque_state));
    if (!try_resolve_deferred_response(state) || state->owner == nullptr ||
        state->owner_reactor == nullptr) {
        return;
    }

    auto delivery = std::make_shared<deferred_response_delivery>();
    delivery->state = state;
    delivery->resp.assign_error(
        problem_details::internal_server_error("Deferred response was abandoned"));
    (void)state->owner_reactor->schedule([delivery]() {
        auto connection = delivery->state->connection.lock();
        if (!connection) {
            return;
        }

        auto typed_connection = std::static_pointer_cast<connection_state>(connection);
        delivery->state->owner->complete_deferred_response(
            *typed_connection, std::move(delivery->resp), *delivery->state->owner_reactor);
    });
}

void server::complete_deferred_response(connection_state& state, response resp, reactor& r) {
    if (!state.watch || !state.deferred_response_active) {
        return;
    }

    state.deferred_ready_response.emplace(std::move(resp));
    handle_connection(state, r);
}

void server::handle_connection(connection_state& state, [[maybe_unused]] reactor& r) {
    state.owner_server = this;
    state.owner_reactor = &r;

    // TLS handshake must complete before any HTTP bytes flow. Re-armed per want-read/want-write
    // until it finishes; a failure drops the connection.
    if (state.tls && !state.handshake_done) {
        using hs = tls::ssl_session::handshake_state;
        switch (state.tls->handshake()) {
        case hs::want_read:
            state.set_watch_events(event_type::readable);
            return;
        case hs::want_write:
            state.set_watch_events(event_type::writable);
            return;
        case hs::failed:
            state.watch.reset();
            return;
        case hs::done:
            break;
        }
        state.handshake_done = true;
    }

    // DEBUG: Track iterations
    [[maybe_unused]] static thread_local int iter_count = 0;
    ++iter_count;
    DEBUG_LOG("[DEBUG] handle_connection iter=%d response_pending=%d read_buf_empty=%d\n",
              iter_count,
              state.has_pending_response() ? 1 : 0,
              state.http_parser.buffered_bytes() == 0 ? 1 : 0);

    auto arm_writable = [&]() {
        if (state.watch) {
            // Under TLS a blocked SSL_write may actually be waiting on a read (renegotiation);
            // re-arm the direction OpenSSL asked for.
            const event_type want = (state.tls && state.tls->wants_read()) ? event_type::readable
                                                                           : event_type::writable;
            state.set_watch_events(want);
        }
    };

    auto note_completed_requests = [&](size_t& completed_requests) {
        if (::katana::detail::g_syscall_metrics_active) [[unlikely]] {
            for (size_t i = 0; i < completed_requests; ++i) {
                ::katana::detail::syscall_metrics_registry::instance().note_completed_request();
            }
        }
        completed_requests = 0;
    };

    auto reset_for_next_request = [&]() {
        state.arena.reset();
        state.http_parser.prepare_for_next_request(&state.arena);
    };

    auto clear_queued_response = [&]() {
        state.queued_response.clear();
        state.queued_response_body.clear();
        state.queued_close_requested = false;
        state.queued_response_completed_requests = 0;
    };

    auto promote_queued_response = [&]() {
        if (conn_debug_enabled()) {
            std::cerr << "[conn_debug] promote queued head=" << state.queued_response.size()
                      << " body=" << state.queued_response_body.size()
                      << " close=" << state.queued_close_requested
                      << " completed=" << state.queued_response_completed_requests << "\n";
        }
        state.active_response = std::move(state.queued_response);
        state.active_response_body = std::move(state.queued_response_body);
        state.write_pos = 0;
        state.close_requested = state.queued_close_requested;
        state.active_response_completed_requests = state.queued_response_completed_requests;
        clear_queued_response();
    };

    auto queue_prepared_response = [&](bool close_requested, size_t completed_requests) {
        if (conn_debug_enabled()) {
            std::cerr << "[conn_debug] queue prepared scratch=" << state.response_scratch.size()
                      << " close=" << close_requested << " completed=" << completed_requests
                      << "\n";
        }
        state.queued_response = std::move(state.response_scratch);
        state.queued_response_body.clear();
        state.queued_close_requested = close_requested;
        state.queued_response_completed_requests = completed_requests;
        state.response_scratch.clear();
    };

    auto queue_response = [&](response& resp, bool close_requested, size_t completed_requests) {
        prepare_response_storage(state.queued_response, state.queued_response_body, resp);
        if (conn_debug_enabled()) {
            std::cerr << "[conn_debug] queue response head=" << state.queued_response.size()
                      << " body=" << state.queued_response_body.size()
                      << " close=" << close_requested << " completed=" << completed_requests
                      << "\n";
        }
        state.queued_close_requested = close_requested;
        state.queued_response_completed_requests = completed_requests;
    };

    auto flush_ready_responses = [&]() -> bool {
        while (state.has_pending_response()) {
            auto flush_state = flush_active_response(state);
            if (flush_state == flush_result::blocked) {
                if (conn_debug_enabled()) {
                    std::cerr << "[conn_debug] flush blocked write_pos=" << state.write_pos
                              << " pending=" << state.pending_response_bytes()
                              << " queued=" << state.queued_response_bytes() << "\n";
                }
                arm_writable();
                return false;
            }
            if (flush_state == flush_result::error) {
                state.watch.reset();
                return false;
            }

            if (conn_debug_enabled()) {
                std::cerr << "[conn_debug] flush complete completed="
                          << state.active_response_completed_requests
                          << " close=" << state.close_requested
                          << " queued=" << state.queued_response_bytes() << "\n";
            }

            note_completed_requests(state.active_response_completed_requests);
            if (state.close_requested) {
                auto count = ++close_counters().close_header;
                maybe_log_close("close_header", count);
                state.watch.reset();
                return false;
            }

            state.close_requested = false;
            if (state.has_queued_response()) {
                promote_queued_response();
            }
        }
        return true;
    };

    auto finalize_response = [&](const request& req, response& resp) -> bool {
        metrics_.record_status(resp.status);
        metrics_.in_flight.fetch_sub(1, std::memory_order_relaxed);

        // CORS headers for actual (non-preflight) responses from an allowed Origin.
        if (cors_) {
            if (auto origin = req.header("origin"); origin && cors_origin_allowed(*origin)) {
                add_cors_headers(*origin, resp);
            }
        }

#ifdef KATANA_HAS_COMPRESSION
        // Response compression: encode a compressible body per the client's Accept-Encoding.
        if (compression_ && resp.body.size() >= compression_->min_size &&
            !resp.headers.contains("Content-Encoding")) {
            const std::string_view ct = resp.headers.get("content-type").value_or("");
            if (is_compressible_type(ct)) {
                const content_encoding enc =
                    negotiate_encoding(req.header("accept-encoding").value_or(""), *compression_);
                if (enc != content_encoding::identity) {
                    if (auto encoded = compress(enc, resp.body, *compression_)) {
                        resp.body = std::move(*encoded);
                        resp.set_header("Content-Encoding", encoding_token(enc));
                        resp.set_header("Vary", "Accept-Encoding");
                    }
                }
            }
        }
#endif
        const int64_t duration_micros =
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - state.request_start)
                .count();
        metrics_.observe_duration_micros(duration_micros);
        if (const int ri = state.current_route_index;
            ri >= 0 && static_cast<size_t>(ri) < per_route_requests_.size()) {
            per_route_requests_[static_cast<size_t>(ri)].fetch_add(1, std::memory_order_relaxed);
            per_route_duration_micros_[static_cast<size_t>(ri)].fetch_add(
                static_cast<uint64_t>(duration_micros < 0 ? 0 : duration_micros),
                std::memory_order_relaxed);
        }

        // Correlation id: echo the client's X-Request-Id, or generate one. Reflect it on the
        // response (so the caller can tie its logs to ours) and into the access log.
        std::string generated_request_id;
        std::string_view request_id;
        if (auto inbound = req.header("x-request-id")) {
            request_id = *inbound;
        } else {
            generated_request_id = detail::generate_request_id();
            request_id = generated_request_id;
        }
        if (!resp.headers.contains("X-Request-Id")) {
            resp.headers.set_unknown("X-Request-Id", request_id); // copies; safe past this scope
        }

        const bool log_this_request =
            access_log_enabled_ &&
            (access_log_sample_every_ <= 1 ||
             access_log_counter_.fetch_add(1, std::memory_order_relaxed) % access_log_sample_every_ ==
                 0);
        const bool traced = tracing_enabled_ && state.current_trace.valid();
        if (log_this_request) {
            auto entry = katana::log::info("http_request");
            entry.field("method", method_to_string(req.http_method))
                .field("path", req.uri)
                .field("status", static_cast<int64_t>(resp.status))
                .field("bytes", static_cast<int64_t>(resp.body.size()))
                .field("duration_us", duration_micros)
                .field("request_id", request_id);
            if (traced) {
                entry.field("trace_id", state.current_trace.trace_id_hex());
            }
        }

        // Emit a span for sampled traced requests. A custom exporter (e.g. OTLP) replaces the
        // default `katana::log` span line.
        if (traced && state.current_trace.sampled) {
            std::string span_name(method_to_string(req.http_method));
            span_name += ' ';
            span_name.append(req.uri.data(), req.uri.size());
            if (span_exporter_) {
                span_exporter_(tracing::span_record{state.current_trace, span_name, resp.status,
                                                    duration_micros});
            } else {
                katana::log::info("span")
                    .field("trace_id", state.current_trace.trace_id_hex())
                    .field("span_id", state.current_trace.span_id_hex())
                    .field("parent_span_id", state.current_trace.parent_span_id_hex())
                    .field("name", span_name)
                    .field("status", static_cast<int64_t>(resp.status))
                    .field("duration_us", duration_micros);
            }
        }

        if (on_request_callback_) {
            on_request_callback_(req, resp);
        }

        auto connection_header = req.headers.get(http::field::connection);
        // During graceful shutdown, stop reusing connections: tell the client this is the last
        // response on the connection so kept-alive clients drain as their in-flight request
        // completes instead of holding the connection open to the deadline.
        bool close_connection =
            shutdown_manager::instance().is_shutdown_requested() ||
            (connection_header && (*connection_header == "close" || *connection_header == "Close"));

        if (!resp.headers.contains(http::field::connection)) {
            resp.headers.set_known_borrowed(http::field::connection,
                                            close_connection ? "close" : "keep-alive");
        }

        const bool can_batch_small_response =
            !close_connection && !resp.chunked &&
            resp.body.size() <= SMALL_CONTIGUOUS_RESPONSE_BODY_THRESHOLD;

        if (conn_debug_enabled()) {
            std::cerr << "[conn_debug] response uri=" << req.uri << " body=" << resp.body.size()
                      << " can_batch_small=" << can_batch_small_response
                      << " active_pending=" << state.pending_response_bytes()
                      << " queued=" << state.queued_response_bytes() << "\n";
        }

        if (can_batch_small_response) {
            state.response_scratch.clear();
            resp.serialize_into(state.response_scratch);

            if (state.pending_response_bytes() + state.response_scratch.size() <=
                    PIPELINE_RESPONSE_BATCH_LIMIT &&
                !state.has_queued_response()) {
                state.active_response.append(state.response_scratch);
                ++state.active_response_completed_requests;
                reset_for_next_request();

                if (state.http_parser.buffered_bytes() != 0) {
                    return true;
                }
            } else {
                if (state.has_pending_response()) {
                    queue_prepared_response(false, 1);
                    reset_for_next_request();
                    if (!flush_ready_responses()) {
                        return false;
                    }
                    if (state.http_parser.buffered_bytes() != 0) {
                        return true;
                    }
                    state.set_watch_events(event_type::readable);
                    return false;
                }

                state.active_response = std::move(state.response_scratch);
                state.active_response_body.clear();
                state.write_pos = 0;
                state.close_requested = false;
                state.active_response_completed_requests = 1;
                reset_for_next_request();
            }
        } else {
            if (state.has_pending_response()) {
                queue_response(resp, close_connection, 1);
                reset_for_next_request();
                if (!flush_ready_responses()) {
                    return false;
                }
                if (state.http_parser.buffered_bytes() != 0) {
                    return true;
                }
                state.set_watch_events(event_type::readable);
                return false;
            }

            state.close_requested = close_connection;
            prepare_active_response(state, resp);
            state.write_pos = 0;
            state.active_response_completed_requests = 1;
            reset_for_next_request();
        }

        if (!flush_ready_responses()) {
            DEBUG_LOG("[DEBUG] Write blocked/error with remaining=%zu\n",
                      state.pending_response_bytes() - state.write_pos);
            return false;
        }

        DEBUG_LOG("[DEBUG] Write complete\n");
        DEBUG_LOG("[DEBUG] Response sent, continuing keep-alive loop\n");

        if (state.http_parser.buffered_bytes() == 0) {
            state.set_watch_events(event_type::readable);
            return false;
        }

        return true;
    };

    auto close_with_parse_error = [&](std::error_code parse_ec) -> void {
        if (parser_debug_enabled()) {
            std::cerr << "[parser_debug] state="
                      << parser_state_name(state.http_parser.current_state())
                      << " parse_pos=" << state.http_parser.parse_pos()
                      << " buffer_size=" << state.http_parser.buffer_size()
                      << " buffered=" << state.http_parser.buffered_bytes() << " preview=\""
                      << escape_preview(state.http_parser.unparsed_view(128)) << "\"\n";
        }
        response resp{&state.arena};
        // Map request-size violations to their specific status (414/431/413); everything else
        // is a generic malformed request (400).
        if (parse_ec == make_error_code(error_code::uri_too_long)) {
            resp.assign_error(problem_details::uri_too_long("Request URI too long"));
        } else if (parse_ec == make_error_code(error_code::header_fields_too_large)) {
            resp.assign_error(
                problem_details::request_header_fields_too_large("Request header fields too large"));
        } else if (parse_ec == make_error_code(error_code::payload_too_large)) {
            resp.assign_error(problem_details::content_too_large("Request body too large"));
        } else {
            resp.assign_error(problem_details::bad_request("Invalid HTTP request"));
        }
        resp.headers.set_known_borrowed(http::field::connection, "close");
        auto count = ++close_counters().parse_error;
        maybe_log_close("parse_error", count);

        if (state.has_pending_response() || state.has_queued_response()) {
            queue_response(resp, true, 0);
            if (!flush_ready_responses()) {
                return;
            }
            state.watch.reset();
            return;
        }

        prepare_active_response(state, resp);
        state.write_pos = 0;
        state.close_requested = true;
        state.active_response_completed_requests = 0;
        (void)flush_ready_responses();
    };

    if (state.deferred_response_active && state.deferred_ready_response.has_value()) {
        auto ready_response = std::move(*state.deferred_ready_response);
        state.deferred_ready_response.reset();
        state.deferred_response_active = false;
        if (!finalize_response(state.http_parser.get_request(), ready_response)) {
            return;
        }
    }

    if (state.deferred_response_active) {
        if (state.has_pending_response() && !flush_ready_responses()) {
            return;
        }
        state.set_watch_events(event_type::none);
        return;
    }

    if (state.has_pending_response()) {
        if (!flush_ready_responses()) {
            return;
        }
        if (state.http_parser.buffered_bytes() == 0) {
            state.set_watch_events(event_type::readable);
            return;
        }
    }

    while (true) {
        auto parse_result = state.http_parser.parse_available();
        if (!parse_result) {
            close_with_parse_error(parse_result.error());
            return;
        }

        if (!state.http_parser.is_complete()) {
            auto writable = state.http_parser.writable_input_span(4096);
            if (!writable) {
                state.watch.reset();
                return;
            }

            auto read_result = state.transport_read(*writable);
            if (!read_result) {
                if (read_result.error().value() == EAGAIN ||
                    read_result.error().value() == EWOULDBLOCK) {
                    state.set_watch_events(event_type::readable);
                    return;
                }
                if (read_result.error().value() == static_cast<int>(error_code::ok)) {
                    auto count = ++close_counters().read_eof;
                    maybe_log_close("read_eof", count);
                } else {
                    auto count = ++close_counters().read_error;
                    maybe_log_close("read_error", count);
                }
                state.watch.reset();
                return;
            }
            if (read_result->empty()) {
                // TLS may block a read on writability (renegotiation); honor its requested direction.
                const event_type want = (state.tls && state.tls->wants_write())
                                            ? event_type::writable
                                            : event_type::readable;
                state.set_watch_events(want);
                return;
            }

            // Progress was made: push the idle/read timeout forward so an actively-fed
            // connection isn't closed mid-request (no-op when timeouts are disabled).
            if (state.watch) {
                state.watch->refresh_timeout();
            }

            parse_result = state.http_parser.commit_input(read_result->size());
            if (!parse_result) {
                close_with_parse_error(parse_result.error());
                return;
            }

            continue;
        }

        const auto& req = state.http_parser.get_request();
        request_context ctx{state.arena};
        ctx.client_ip = state.client_ip; // stable for the connection's lifetime
        ctx.policy_executor = policy_executor_;
        ctx.task_scheduler_user = &r;
        ctx.task_scheduler = &schedule_request_context_task;
        ctx.reactor_user = &r;
        ctx.deferred_response_user = &state;
        ctx.deferred_response_factory = &server::make_deferred_response_handle;
        response resp{&state.arena};
        state.request_start = std::chrono::steady_clock::now();
        if (tracing_enabled_) {
            state.current_trace = tracing::start_server_span(req.header("traceparent"));
            if (auto ts = req.header("tracestate")) {
                state.current_trace.tracestate = *ts; // carried verbatim for propagation
            }
            ctx.trace = state.current_trace; // visible to the handler for downstream propagation
        }
        dispatch_request(req, ctx, resp);
        state.current_route_index = ctx.route_index; // for per-route metrics in finalize

        if (ctx.is_response_deferred()) {
            state.deferred_response_active = true;
            if (state.has_pending_response() && !flush_ready_responses()) {
                return;
            }
            if (!state.has_pending_response()) {
                state.set_watch_events(event_type::none);
            }
            return;
        }

        if (!finalize_response(req, resp)) {
            return;
        }
    }
    DEBUG_LOG("[DEBUG] Exiting handle_connection (while loop ended)\n");
}

bool server::reload_tls() {
    if (!tls_config_) {
        return false;
    }
    auto ctx = tls::tls_context::create(*tls_config_);
    if (!ctx) {
        std::cerr << "[server] TLS reload failed; keeping the current certificate\n";
        return false;
    }
    std::lock_guard<std::mutex> lock(tls_ctx_mutex_);
    tls_ctx_ = std::move(*ctx);
    return true;
}

int server::run() {
    // Build the TLS context once before accepting; a bad cert/key fails fast here.
    if (tls_config_) {
        auto ctx = tls::tls_context::create(*tls_config_);
        if (!ctx) {
            std::cerr << "[server] TLS setup failed (check cert/key: " << tls_config_->cert_file
                      << " / " << tls_config_->key_file << ")\n";
            return 1;
        }
        {
            std::lock_guard<std::mutex> lock(tls_ctx_mutex_);
            tls_ctx_ = std::move(*ctx);
        }
    }

    // Compose the effective policy executor: when auth is enabled, run the auth executor first
    // (so an unauthenticated request is rejected before cache/rate-limit/idempotency), then the
    // app's policy executor if any.
    if (authenticator_ && authenticator_->enabled()) {
        auth_executor_.emplace(authenticator_);
        if (app_policy_executor_ != nullptr) {
            policy_chain_.emplace(std::initializer_list<route_policy_executor*>{
                &*auth_executor_, app_policy_executor_});
            policy_executor_ = &*policy_chain_;
        } else {
            policy_executor_ = &*auth_executor_;
        }
    } else {
        policy_executor_ = app_policy_executor_;
    }

    if (load_shedding_ && load_shedding_->per_client_rps != 0) {
        edge_limiter_ = std::make_unique<edge_limiter>(load_shedding_->per_client_rps);
    }

    init_per_route_metrics(); // size per-route counters from the router before workers start
    reactor_pool_config config;
    config.reactor_count = static_cast<uint32_t>(worker_count_);
    config.enable_adaptive_balancing = true;
    reactor_pool pool(config); // reactor metrics are collected by default; exposed opt-in below
    pool_ = &pool; // valid while run() blocks; cleared before returning
    ::katana::detail::scoped_syscall_metrics_reporter syscall_metrics_reporter;

    std::vector<std::shared_ptr<fd_watch>> accept_watches;

    auto accept_handler = [this](reactor& r, int listener_fd) {
        while (true) {
            sockaddr_storage peer{};
            socklen_t peer_len = sizeof(peer);
            int fd = ::accept4(listener_fd, reinterpret_cast<sockaddr*>(&peer), &peer_len,
                               SOCK_NONBLOCK | SOCK_CLOEXEC);
            if (fd < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                }
                return;
            }

            // Enforce the max-connections cap (global across workers): refuse over-cap accepts
            // so an overloaded server sheds load instead of exhausting fds/memory.
            if (max_connections_ != 0 &&
                metrics_.active_connections.load(std::memory_order_relaxed) >=
                    static_cast<int64_t>(max_connections_)) {
                metrics_.connections_rejected.fetch_add(1, std::memory_order_relaxed);
                ::close(fd);
                continue; // keep draining the accept queue, closing further over-cap sockets
            }

            configure_client_socket(fd);

            metrics_.active_connections.fetch_add(1, std::memory_order_relaxed);
            auto state = std::make_shared<connection_state>(tcp_socket(fd));
            state->active_conn_counter = &metrics_.active_connections;
            // Capture the peer IP for edge rate-limiting / access logs.
            {
                char ipbuf[INET6_ADDRSTRLEN] = {0};
                if (peer.ss_family == AF_INET) {
                    ::inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in*>(&peer)->sin_addr, ipbuf,
                                sizeof(ipbuf));
                } else if (peer.ss_family == AF_INET6) {
                    ::inet_ntop(AF_INET6, &reinterpret_cast<sockaddr_in6*>(&peer)->sin6_addr, ipbuf,
                                sizeof(ipbuf));
                }
                state->client_ip = ipbuf;
            }
            if (auto ctx = current_tls_context()) {
                state->tls.emplace(*ctx, fd); // TLS handshake runs on first handle_connection
            }
            auto state_ptr = state.get();

            auto on_event = [this, state, state_ptr, &r](event_type ev) {
                if (has_flag(ev, event_type::timeout)) {
                    // The reactor closes the fd right after this returns; detach it from the
                    // connection's tcp_socket so it isn't closed twice.
                    (void)state_ptr->socket.release();
                    metrics_.connection_timeouts.fetch_add(1, std::memory_order_relaxed);
                    return;
                }
                handle_connection(*state_ptr, r);
            };

            if (connection_timeout_) {
                state->watch = std::make_unique<fd_watch>(r, fd, state->watch_events,
                                                          std::move(on_event), *connection_timeout_);
            } else {
                state->watch =
                    std::make_unique<fd_watch>(r, fd, state->watch_events, std::move(on_event));
            }
        }
    };

    if (reuseport_) {
        auto res = pool.start_listening(port_, accept_handler);
        if (!res) {
            std::cerr << "Failed to start listeners on port " << port_ << ": "
                      << res.error().message() << "\n";
            return 1;
        }
    } else {
        // Fallback: single listener on reactor 0
        tcp_listener listener(port_);
        if (!listener) {
            std::cerr << "Failed to create listener on port " << port_ << "\n";
            return 1;
        }
        listener.set_reuseport(false).set_backlog(backlog_);

        auto& r = pool.get_reactor(0);
        auto listen_fd = listener.native_handle();
        auto listen_watch = std::make_shared<fd_watch>(
            r, listen_fd, event_type::readable, [&r, &listener, accept_handler](event_type) {
                accept_handler(r, listener.native_handle());
            });
        accept_watches.push_back(std::move(listen_watch));
    }

    // Setup signal handlers for graceful shutdown
    shutdown_manager::instance().setup_signal_handlers();
    shutdown_manager::instance().set_shutdown_callback([&pool, this]() {
        if (on_stop_callback_) {
            on_stop_callback_();
        }
        pool.graceful_stop(shutdown_timeout_);
    });

    // Call on_start callback
    if (on_start_callback_) {
        on_start_callback_();
    } else {
        std::cout << "HTTP server listening on http://" << host_ << ":" << port_ << "\n";
        std::cout << "Workers: " << worker_count_ << "\n";
        std::cout << "Press Ctrl+C to stop\n\n";
    }

    pool.start();
    pool.wait();
    pool_ = nullptr; // pool is about to be destroyed
    return 0;
}

} // namespace http
} // namespace katana
