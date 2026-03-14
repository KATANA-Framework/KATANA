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
            write_result = state.socket.write(as_bytes(remaining));
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
            write_result = state.socket.writev(iov, iov_count);
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

void server::handle_connection(connection_state& state, [[maybe_unused]] reactor& r) {
    // DEBUG: Track iterations
    [[maybe_unused]] static thread_local int iter_count = 0;
    ++iter_count;
    DEBUG_LOG("[DEBUG] handle_connection iter=%d response_pending=%d read_buf_empty=%d\n",
              iter_count,
              state.has_pending_response() ? 1 : 0,
              state.http_parser.buffered_bytes() == 0 ? 1 : 0);

    auto arm_writable = [&]() {
        if (state.watch) {
            state.set_watch_events(event_type::writable);
        }
    };

    auto note_completed_requests = [&](size_t& completed_requests) {
        for (size_t i = 0; i < completed_requests; ++i) {
            ::katana::detail::syscall_metrics_registry::instance().note_completed_request();
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

    auto close_with_parse_error = [&]() -> void {
        if (parser_debug_enabled()) {
            std::cerr << "[parser_debug] state="
                      << parser_state_name(state.http_parser.current_state())
                      << " parse_pos=" << state.http_parser.parse_pos()
                      << " buffer_size=" << state.http_parser.buffer_size()
                      << " buffered=" << state.http_parser.buffered_bytes() << " preview=\""
                      << escape_preview(state.http_parser.unparsed_view(128)) << "\"\n";
        }
        response resp{&state.arena};
        resp.assign_error(problem_details::bad_request("Invalid HTTP request"));
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
            close_with_parse_error();
            return;
        }

        if (!state.http_parser.is_complete()) {
            auto writable = state.http_parser.writable_input_span(4096);
            if (!writable) {
                state.watch.reset();
                return;
            }

            auto read_result = state.socket.read(*writable);
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
                state.set_watch_events(event_type::readable);
                return;
            }

            parse_result = state.http_parser.commit_input(read_result->size());
            if (!parse_result) {
                close_with_parse_error();
                return;
            }

            continue;
        }

        const auto& req = state.http_parser.get_request();
        request_context ctx{state.arena};
        response resp{&state.arena};
        dispatch_request(req, ctx, resp);

        if (on_request_callback_) {
            on_request_callback_(req, resp);
        }

        auto connection_header = req.headers.get(http::field::connection);
        bool close_connection =
            connection_header && (*connection_header == "close" || *connection_header == "Close");

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
                    continue;
                }
            } else {
                if (state.has_pending_response()) {
                    queue_prepared_response(false, 1);
                    reset_for_next_request();
                    if (!flush_ready_responses()) {
                        return;
                    }
                    if (state.http_parser.buffered_bytes() != 0) {
                        continue;
                    }
                    state.set_watch_events(event_type::readable);
                    return;
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
                    return;
                }
                if (state.http_parser.buffered_bytes() != 0) {
                    continue;
                }
                state.set_watch_events(event_type::readable);
                return;
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
            return;
        }

        DEBUG_LOG("[DEBUG] Write complete\n");

        DEBUG_LOG("[DEBUG] Response sent, continuing keep-alive loop\n");

        if (state.http_parser.buffered_bytes() == 0) {
            state.set_watch_events(event_type::readable);
            return;
        }
    }
    DEBUG_LOG("[DEBUG] Exiting handle_connection (while loop ended)\n");
}

int server::run() {
    reactor_pool_config config;
    config.reactor_count = static_cast<uint32_t>(worker_count_);
    config.enable_adaptive_balancing = true;
    reactor_pool pool(config);
    ::katana::detail::scoped_syscall_metrics_reporter syscall_metrics_reporter;

    std::vector<std::shared_ptr<fd_watch>> accept_watches;

    auto accept_handler = [this](reactor& r, int listener_fd) {
        while (true) {
            int fd = ::accept4(listener_fd, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
            if (fd < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                }
                return;
            }

            configure_client_socket(fd);

            auto state = std::make_shared<connection_state>(tcp_socket(fd));
            auto state_ptr = state.get();

            state->watch = std::make_unique<fd_watch>(
                r, fd, state->watch_events, [this, state, state_ptr, &r](event_type) {
                    handle_connection(*state_ptr, r);
                });
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
    return 0;
}

} // namespace http
} // namespace katana
