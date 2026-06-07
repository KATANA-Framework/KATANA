#include "katana/core/arena.hpp"
#include "katana/core/cache.hpp"
#include "katana/core/contract_policies.hpp"
#include "katana/core/http.hpp"
#include "katana/core/http_server.hpp"
#include "katana/core/idempotency.hpp"
#include "katana/core/rate_limit.hpp"
#include "katana/core/reactor_pool.hpp"
#include "katana/core/router.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <csignal>
#include <cstring>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <optional>
#include <poll.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace katana;

constexpr uint16_t TEST_PORT = 9999;
constexpr std::string_view kSmallBody = "ok";
constexpr size_t kLargeBodySize = 128 * 1024;

namespace {

struct parsed_response {
    int status = 0;
    std::string body;
    std::vector<std::pair<std::string, std::string>> headers;

    [[nodiscard]] std::optional<std::string_view> header(std::string_view name) const {
        for (const auto& [header_name, value] : headers) {
            if (header_name == name) {
                return value;
            }
        }
        return std::nullopt;
    }
};

uint16_t reserve_ephemeral_port() {
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        return 0;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(fd);
        return 0;
    }

    socklen_t len = sizeof(addr);
    if (::getsockname(fd, reinterpret_cast<sockaddr*>(&addr), &len) < 0) {
        ::close(fd);
        return 0;
    }

    const uint16_t port = ntohs(addr.sin_port);
    ::close(fd);
    return port;
}

std::string make_get_request(std::string_view path) {
    std::string req;
    req.reserve(path.size() + 64);
    req.append("GET ");
    req.append(path);
    req.append(" HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: keep-alive\r\n\r\n");
    return req;
}

std::string make_post_request(std::string_view path,
                              std::string_view body,
                              std::vector<std::pair<std::string_view, std::string_view>> headers = {}) {
    std::string req;
    req.reserve(path.size() + body.size() + 128);
    req.append("POST ");
    req.append(path);
    req.append(" HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: keep-alive\r\n");
    req.append("Content-Length: ");
    req.append(std::to_string(body.size()));
    req.append("\r\n");
    for (const auto& [name, value] : headers) {
        req.append(name);
        req.append(": ");
        req.append(value);
        req.append("\r\n");
    }
    req.append("\r\n");
    req.append(body);
    return req;
}

bool try_extract_response(std::string& buffer, parsed_response& out) {
    const size_t header_end = buffer.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        return false;
    }

    const std::string_view headers(buffer.data(), header_end + 4);
    const size_t status_end = headers.find("\r\n");
    if (status_end == std::string::npos) {
        return false;
    }

    const std::string_view status_line(headers.data(), status_end);
    const size_t first_space = status_line.find(' ');
    const size_t second_space = status_line.find(' ', first_space + 1);
    if (first_space == std::string::npos || second_space == std::string::npos) {
        return false;
    }

    size_t content_length = 0;
    const std::string_view content_length_prefix = "Content-Length: ";
    size_t scan_pos = 0;
    bool found_content_length = false;
    out.headers.clear();
    while (scan_pos < headers.size()) {
        const size_t line_end = headers.find("\r\n", scan_pos);
        if (line_end == std::string::npos) {
            break;
        }
        const std::string_view line(headers.data() + scan_pos, line_end - scan_pos);
        const size_t colon = line.find(':');
        if (colon != std::string_view::npos) {
            size_t value_pos = colon + 1;
            if (value_pos < line.size() && line[value_pos] == ' ') {
                ++value_pos;
            }
            out.headers.emplace_back(std::string(line.substr(0, colon)),
                                     std::string(line.substr(value_pos)));
        }
        if (line.starts_with(content_length_prefix)) {
            content_length = static_cast<size_t>(
                std::stoul(std::string(line.substr(content_length_prefix.size()))));
            found_content_length = true;
            // keep scanning so headers after Content-Length (e.g. X-Request-Id) are captured
        }
        scan_pos = line_end + 2;
    }

    if (!found_content_length) {
        return false;
    }

    const size_t total_size = header_end + 4 + content_length;
    if (buffer.size() < total_size) {
        return false;
    }

    out.status =
        std::stoi(std::string(status_line.substr(first_space + 1, second_space - first_space - 1)));
    out.body.assign(buffer.data() + header_end + 4, content_length);
    buffer.erase(0, total_size);
    return true;
}

std::vector<parsed_response> read_responses_slowly(int fd,
                                                   size_t expected_count,
                                                   std::chrono::milliseconds timeout,
                                                   size_t chunk_size = 4096) {
    std::vector<parsed_response> responses;
    responses.reserve(expected_count);

    std::string buffer;
    buffer.reserve(expected_count * 64);
    const auto deadline = std::chrono::steady_clock::now() + timeout;

    while (responses.size() < expected_count && std::chrono::steady_clock::now() < deadline) {
        std::string chunk(chunk_size, '\0');
        ssize_t n = ::recv(fd, chunk.data(), chunk.size(), 0);
        if (n > 0) {
            buffer.append(chunk.data(), static_cast<size_t>(n));
            parsed_response resp;
            while (try_extract_response(buffer, resp)) {
                responses.push_back(resp);
                if (responses.size() == expected_count) {
                    break;
                }
                resp = parsed_response{};
            }
            continue;
        }
        if (n == 0) {
            break;
        }
        if (errno == EINTR) {
            continue;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        break;
    }

    return responses;
}

pid_t spawn_pipeline_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    const std::string large_body(kLargeBodySize, 'L');
    const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/small">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             out.assign_text(std::string(kSmallBody));
             return result<void>{};
         })},
        {http::method::get,
         http::path_pattern::from_literal<"/large">(),
         http::handler_fn(
             [&large_body](const http::request&, http::request_context&, http::response& out) {
                 out.assign_text(large_body);
                 return result<void>{};
             })},
    };

    const http::router r(routes);
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(1)
                   .graceful_shutdown(std::chrono::milliseconds(100))
                   .run());
}

pid_t spawn_deferred_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/small">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             out.assign_text(std::string(kSmallBody));
             return result<void>{};
         })},
        {http::method::get,
         http::path_pattern::from_literal<"/deferred">(),
         http::handler_fn(
             [](const http::request&, http::request_context& ctx, http::response&) -> result<void> {
                 auto deferred = ctx.share_deferred_response();
                 if (!deferred) {
                     return std::unexpected(make_error_code(error_code::reactor_stopped));
                 }

                 if (!ctx.schedule([deferred]() {
                         http::response out;
                         out.assign_text("later");
                         (void)deferred.complete(std::move(out));
                     })) {
                     return std::unexpected(make_error_code(error_code::reactor_stopped));
                 }

                 return result<void>{};
             })},
        {http::method::get,
         http::path_pattern::from_literal<"/abandon">(),
         http::handler_fn(
             [](const http::request&, http::request_context& ctx, http::response&) -> result<void> {
                 auto deferred = ctx.share_deferred_response();
                 if (!deferred) {
                     return std::unexpected(make_error_code(error_code::reactor_stopped));
                 }
                 return result<void>{};
             })},
    };

    const http::router r(routes);
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(1)
                   .graceful_shutdown(std::chrono::milliseconds(100))
                   .run());
}

pid_t spawn_rate_limited_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    const http::route_policy_view policy{
        {},
        {},
        http::route_rate_limit_policy_view{
            true, "1/s", std::optional<size_t>{1U}, http::route_rate_limit_unit::second},
    };

    const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/limited">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             out.assign_text("limited-ok");
             return result<void>{};
         }),
         {},
         &policy},
    };

    http::in_memory_rate_limit_executor executor;
    const http::router r(routes);
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(1)
                   .policy_executor(executor)
                   .graceful_shutdown(std::chrono::milliseconds(100))
                   .run());
}

pid_t spawn_idempotent_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    const http::route_policy_view policy{
        {},
        {},
        {},
        http::route_idempotency_policy_view{http::route_idempotency_policy_kind::enabled, "true"},
    };

    static int call_count = 0;
    const http::route_entry routes[] = {
        {http::method::post,
         http::path_pattern::from_literal<"/idempotent">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             ++call_count;
             out.assign_text("created-" + std::to_string(call_count), "text/plain", 201, "Created");
             return result<void>{};
         }),
         {},
         &policy},
    };

    http::in_memory_idempotency_executor executor;
    const http::router r(routes);
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(1)
                   .policy_executor(executor)
                   .graceful_shutdown(std::chrono::milliseconds(100))
                   .run());
}

pid_t spawn_cached_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    const http::route_policy_view policy{
        http::route_cache_policy_view{http::route_cache_policy_kind::enabled, "true"},
        {},
        {},
    };

    static int call_count = 0;
    const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/cached">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             ++call_count;
             out.assign_text("cached-" + std::to_string(call_count));
             return result<void>{};
         }),
         {},
         &policy},
    };

    http::in_memory_response_cache_executor executor;
    const http::router r(routes);
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(1)
                   .policy_executor(executor)
                   .graceful_shutdown(std::chrono::milliseconds(100))
                   .run());
}

pid_t spawn_contract_policy_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    const http::route_policy_view cache_policy{
        http::route_cache_policy_view{http::route_cache_policy_kind::enabled, "true"},
        {},
        http::route_rate_limit_policy_view{
            true, "2/s", std::optional<size_t>{2U}, http::route_rate_limit_unit::second},
    };
    const http::route_policy_view idempotency_policy{
        {},
        {},
        http::route_rate_limit_policy_view{
            true, "1/s", std::optional<size_t>{1U}, http::route_rate_limit_unit::second},
        http::route_idempotency_policy_view{http::route_idempotency_policy_kind::enabled, "true"},
    };

    static int cache_call_count = 0;
    static int idempotent_call_count = 0;
    const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/contract-cached">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             ++cache_call_count;
             out.assign_text("contract-cached-" + std::to_string(cache_call_count));
             return result<void>{};
         }),
         {},
         &cache_policy},
        {http::method::post,
         http::path_pattern::from_literal<"/contract-idempotent">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             ++idempotent_call_count;
             out.assign_text("contract-created-" + std::to_string(idempotent_call_count),
                             "text/plain",
                             201,
                             "Created");
             return result<void>{};
         }),
         {},
         &idempotency_policy},
    };

    http::in_memory_contract_policy_executor executor;
    const http::router r(routes);
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(1)
                   .policy_executor(executor)
                   .graceful_shutdown(std::chrono::milliseconds(100))
                   .run());
}

pid_t spawn_access_log_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    // The handler echoes the inbound correlation id it sees in the request context, so the
    // test can confirm handlers observe the client's X-Request-Id.
    const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/ping">(),
         http::handler_fn(
             [](const http::request&, http::request_context& ctx, http::response& out) {
                 out.assign_text(std::string("ctx-id=") + std::string(ctx.request_id));
                 return result<void>{};
             }),
         {},
         nullptr},
    };

    const http::router r(routes);
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(1)
                   .access_log()
                   .graceful_shutdown(std::chrono::milliseconds(100))
                   .run());
}

pid_t spawn_graceful_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/ping">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             out.assign_text("pong");
             return result<void>{};
         })},
    };

    const http::router r(routes);
    // Deliberately large drain deadline: a correct graceful shutdown that stops accepting and
    // drains idle connections should exit almost immediately, NOT wait this long.
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(2)
                   .graceful_shutdown(std::chrono::seconds(8))
                   .run());
}

pid_t spawn_timeout_server(uint16_t port) {
    pid_t pid = ::fork();
    if (pid != 0) {
        return pid;
    }

    const http::route_entry routes[] = {
        {http::method::get,
         http::path_pattern::from_literal<"/ping">(),
         http::handler_fn([](const http::request&, http::request_context&, http::response& out) {
             out.assign_text("pong");
             return result<void>{};
         })},
    };

    const http::router r(routes);
    std::_Exit(http::server(r)
                   .listen(port)
                   .workers(1)
                   // Short read/write windows so a stalled (slowloris-style) client is dropped.
                   .connection_timeout(std::chrono::milliseconds(300),
                                       std::chrono::milliseconds(300),
                                       std::chrono::seconds(2))
                   .graceful_shutdown(std::chrono::milliseconds(100))
                   .run());
}

int connect_with_retry(uint16_t port) {
    for (int attempt = 0; attempt < 200; ++attempt) {
        int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
        if (fd < 0) {
            return -1;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(port);

        if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
            return fd;
        }

        ::close(fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return -1;
}

void send_all(int fd, std::string_view bytes) {
    size_t offset = 0;
    while (offset < bytes.size()) {
        ssize_t n = ::send(fd, bytes.data() + offset, bytes.size() - offset, 0);
        ASSERT_GE(n, 0);
        offset += static_cast<size_t>(n);
    }
}

} // namespace

class HTTPServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        listener_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
        if (listener_fd < 0) {
            return; // environment may forbid sockets; tests below don't rely on bind/listen
        }

        int opt = 1;
        setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        setsockopt(listener_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(TEST_PORT);

        if (bind(listener_fd, static_cast<sockaddr*>(static_cast<void*>(&addr)), sizeof(addr)) <
            0) {
            close(listener_fd);
            listener_fd = -1;
            return;
        }
        if (listen(listener_fd, 10) < 0) {
            close(listener_fd);
            listener_fd = -1;
            return;
        }
    }

    void TearDown() override {
        if (listener_fd >= 0) {
            close(listener_fd);
        }
    }

    int listener_fd = -1;
};

TEST_F(HTTPServerTest, ChunkedEncoding) {
    http::response resp = http::response::ok("Hello, World!", "text/plain");
    resp.chunked = true;

    std::string serialized = resp.serialize();

    EXPECT_NE(serialized.find("Transfer-Encoding: chunked"), std::string::npos);
    EXPECT_EQ(serialized.find("Content-Length"), std::string::npos);
    EXPECT_NE(serialized.find("d\r\nHello, World!\r\n"), std::string::npos);
    EXPECT_NE(serialized.find("0\r\n\r\n"), std::string::npos);
}

TEST_F(HTTPServerTest, ChunkedParsing) {
    monotonic_arena arena;
    http::parser parser(&arena);

    std::string request_data = "POST /test HTTP/1.1\r\n"
                               "Host: localhost\r\n"
                               "Transfer-Encoding: chunked\r\n"
                               "\r\n"
                               "5\r\n"
                               "Hello\r\n"
                               "7\r\n"
                               ", World\r\n"
                               "0\r\n"
                               "\r\n";

    auto result = parser.parse(http::as_bytes(request_data));

    ASSERT_TRUE(result);
    EXPECT_TRUE(parser.is_complete());

    const auto& req = parser.get_request();
    EXPECT_EQ(req.body, "Hello, World");
}

TEST_F(HTTPServerTest, SizeLimits) {
    monotonic_arena arena;
    http::parser parser(&arena);

    std::string huge_uri(3000, 'a');
    std::string request_data = "GET /" + huge_uri + " HTTP/1.1\r\n\r\n";

    auto result = parser.parse(http::as_bytes(request_data));

    EXPECT_FALSE(result);
}

TEST_F(HTTPServerTest, ArenaAllocation) {
    monotonic_arena arena(4096);

    // Allocate using arena
    void* buffer = arena.allocate(1024, 1);
    EXPECT_NE(buffer, nullptr);
    EXPECT_GE(arena.bytes_allocated(), 1024);

    arena.reset();
    EXPECT_EQ(arena.bytes_allocated(), 0);
}

TEST(HTTPServerPipeline, PreservesOrderingAcrossMixedSmallAndLargeResponses) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_pipeline_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;

        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    std::string pipeline;
    std::vector<std::string> expected_bodies;
    expected_bodies.reserve(25);
    for (int i = 0; i < 12; ++i) {
        pipeline += make_get_request("/small");
        expected_bodies.emplace_back(kSmallBody);
    }
    pipeline += make_get_request("/large");
    expected_bodies.emplace_back(kLargeBodySize, 'L');
    for (int i = 0; i < 12; ++i) {
        pipeline += make_get_request("/small");
        expected_bodies.emplace_back(kSmallBody);
    }

    send_all(client_fd, pipeline);

    auto responses =
        read_responses_slowly(client_fd, expected_bodies.size(), std::chrono::seconds(5));
    ASSERT_EQ(responses.size(), expected_bodies.size());
    for (size_t i = 0; i < responses.size(); ++i) {
        EXPECT_EQ(responses[i].status, 200);
        EXPECT_EQ(responses[i].body, expected_bodies[i]);
    }
}

TEST(HTTPServerPipeline, FlushesSingleSmallResponseWithoutWaitingForBatch) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_pipeline_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;

        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    send_all(client_fd, make_get_request("/small"));

    pollfd pfd{};
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    const int ready = ::poll(&pfd, 1, 500);
    ASSERT_EQ(ready, 1);
    ASSERT_NE((pfd.revents & POLLIN), 0);

    auto first = read_responses_slowly(client_fd, 1, std::chrono::milliseconds(250));
    ASSERT_EQ(first.size(), 1u);
    EXPECT_EQ(first[0].status, 200);
    EXPECT_EQ(first[0].body, kSmallBody);

    send_all(client_fd, make_get_request("/small"));
    auto second = read_responses_slowly(client_fd, 1, std::chrono::milliseconds(250));
    ASSERT_EQ(second.size(), 1u);
    EXPECT_EQ(second[0].status, 200);
    EXPECT_EQ(second[0].body, kSmallBody);
}

TEST(HTTPServerDeferredResponse, CompletesOnReactorAndPreservesPipelineOrdering) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_deferred_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;

        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    std::string pipeline = make_get_request("/deferred");
    pipeline += make_get_request("/small");
    send_all(client_fd, pipeline);

    auto responses = read_responses_slowly(client_fd, 2, std::chrono::seconds(2));
    ASSERT_EQ(responses.size(), 2u);
    EXPECT_EQ(responses[0].status, 200);
    EXPECT_EQ(responses[0].body, "later");
    EXPECT_EQ(responses[1].status, 200);
    EXPECT_EQ(responses[1].body, kSmallBody);
}

TEST(HTTPServerDeferredResponse, AbandonedHandleReturnsInternalServerError) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_deferred_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;

        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    send_all(client_fd, make_get_request("/abandon"));

    auto responses = read_responses_slowly(client_fd, 1, std::chrono::seconds(2));
    ASSERT_EQ(responses.size(), 1u);
    EXPECT_EQ(responses[0].status, 500);
    EXPECT_FALSE(responses[0].body.empty());
}

TEST(HTTPServerPolicies, RateLimitExecutorReturns429OverLiveSocketPath) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_rate_limited_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;

        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    const std::string pipeline = make_get_request("/limited") + make_get_request("/limited");
    send_all(client_fd, pipeline);

    auto responses = read_responses_slowly(client_fd, 2, std::chrono::seconds(2));
    ASSERT_EQ(responses.size(), 2u);
    EXPECT_EQ(responses[0].status, 200);
    EXPECT_EQ(responses[0].body, "limited-ok");
    EXPECT_EQ(responses[1].status, 429);
    EXPECT_NE(responses[1].body.find("\"title\":\"Too Many Requests\""), std::string::npos);
}

TEST(HTTPServerPolicies, IdempotencyExecutorReplaysStoredResponseOverLiveSocketPath) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_idempotent_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;

        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    const std::vector<std::pair<std::string_view, std::string_view>> headers = {
        {"Idempotency-Key", "live-key-1"}};
    const std::string pipeline = make_post_request("/idempotent", "{}", headers) +
                                 make_post_request("/idempotent", "{}", headers);
    send_all(client_fd, pipeline);

    auto responses = read_responses_slowly(client_fd, 2, std::chrono::seconds(2));
    ASSERT_EQ(responses.size(), 2u);
    EXPECT_EQ(responses[0].status, 201);
    EXPECT_EQ(responses[0].body, "created-1");
    EXPECT_EQ(responses[1].status, 201);
    EXPECT_EQ(responses[1].body, "created-1");
}

TEST(HTTPServerPolicies, ResponseCacheExecutorReplaysCachedResponseOverLiveSocketPath) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_cached_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;

        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    const std::string pipeline = make_get_request("/cached") + make_get_request("/cached");
    send_all(client_fd, pipeline);

    auto responses = read_responses_slowly(client_fd, 2, std::chrono::seconds(2));
    ASSERT_EQ(responses.size(), 2u);
    EXPECT_EQ(responses[0].status, 200);
    EXPECT_EQ(responses[0].body, "cached-1");
    EXPECT_FALSE(responses[0].header("X-Katana-Cache").has_value());
    EXPECT_EQ(responses[1].status, 200);
    EXPECT_EQ(responses[1].body, "cached-1");
    EXPECT_EQ(responses[1].header("X-Katana-Cache"), std::optional<std::string_view>{"HIT"});
}

TEST(HTTPServerPolicies, InMemoryContractPolicyExecutorAppliesCanonicalPoliciesOverLiveSocketPath) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_contract_policy_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;

        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    const std::vector<std::pair<std::string_view, std::string_view>> idempotency_headers = {
        {"Idempotency-Key", "contract-live-key"}};
    std::string pipeline;
    pipeline += make_get_request("/contract-cached");
    pipeline += make_get_request("/contract-cached");
    pipeline += make_post_request("/contract-idempotent", "{}", idempotency_headers);
    pipeline += make_post_request("/contract-idempotent", "{}", idempotency_headers);
    pipeline += make_post_request(
        "/contract-idempotent",
        "{}",
        {{"Idempotency-Key", "contract-live-key-2"}});
    send_all(client_fd, pipeline);

    auto responses = read_responses_slowly(client_fd, 5, std::chrono::seconds(2));
    ASSERT_EQ(responses.size(), 5u);
    EXPECT_EQ(responses[0].status, 200);
    EXPECT_EQ(responses[0].body, "contract-cached-1");
    EXPECT_EQ(responses[1].status, 200);
    EXPECT_EQ(responses[1].body, "contract-cached-1");
    EXPECT_EQ(responses[1].header("X-Katana-Cache"), std::optional<std::string_view>{"HIT"});
    EXPECT_EQ(responses[2].status, 201);
    EXPECT_EQ(responses[2].body, "contract-created-1");
    EXPECT_EQ(responses[3].status, 201);
    EXPECT_EQ(responses[3].body, "contract-created-1");
    EXPECT_EQ(responses[3].header("Idempotency-Replayed"),
              std::optional<std::string_view>{"true"});
    EXPECT_EQ(responses[4].status, 429);
}

TEST(HTTPServerAccessLog, EchoesClientRequestIdToResponseAndHandler) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_access_log_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;
        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    send_all(client_fd,
             "GET /ping HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: keep-alive\r\n"
             "X-Request-Id: client-xyz-42\r\n\r\n");

    auto responses = read_responses_slowly(client_fd, 1, std::chrono::seconds(2));
    ASSERT_EQ(responses.size(), 1u);
    EXPECT_EQ(responses[0].status, 200);
    // Response carries the client's correlation id back...
    EXPECT_EQ(responses[0].header("X-Request-Id"),
              std::optional<std::string_view>{"client-xyz-42"});
    // ...and the handler observed the same id via request_context.
    EXPECT_EQ(responses[0].body, "ctx-id=client-xyz-42");
}

TEST(HTTPServerAccessLog, GeneratesRequestIdWhenClientOmitsIt) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_access_log_server(port);
    ASSERT_GT(server_pid, 0);

    struct guard {
        pid_t pid;
        int* client_fd;
        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    };

    int client_fd = connect_with_retry(port);
    guard cleanup{server_pid, &client_fd};
    ASSERT_GE(client_fd, 0);

    send_all(client_fd, make_get_request("/ping"));

    auto responses = read_responses_slowly(client_fd, 1, std::chrono::seconds(2));
    ASSERT_EQ(responses.size(), 1u);
    EXPECT_EQ(responses[0].status, 200);
    // The server synthesizes a 16-hex correlation id and reflects it on the response.
    auto rid = responses[0].header("X-Request-Id");
    ASSERT_TRUE(rid.has_value());
    EXPECT_EQ(rid->size(), 16u);
    EXPECT_EQ(rid->find_first_not_of("0123456789abcdef"), std::string_view::npos);
}

TEST(HTTPServerGracefulShutdown, StopsAcceptingAndDrainsPromptlyOnSigterm) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_graceful_server(port);
    ASSERT_GT(server_pid, 0);

    bool reaped = false;
    struct guard {
        pid_t pid;
        bool* reaped;
        ~guard() {
            if (!*reaped && pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    } cleanup{server_pid, &reaped};

    // Confirm the server is serving, then close our connection (Connection: close) so no
    // in-flight or idle keep-alive connection remains at shutdown time.
    {
        int fd = connect_with_retry(port);
        ASSERT_GE(fd, 0);
        send_all(fd,
                 "GET /ping HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n");
        auto responses = read_responses_slowly(fd, 1, std::chrono::seconds(2));
        ASSERT_EQ(responses.size(), 1u);
        EXPECT_EQ(responses[0].body, "pong");
        ::close(fd);
    }

    // Ask for graceful shutdown. The drain deadline is 8s; a correct shutdown closes the
    // listeners and exits within a small fraction of that.
    const auto sent_at = std::chrono::steady_clock::now();
    ASSERT_EQ(::kill(server_pid, SIGTERM), 0);

    int status = 0;
    for (int i = 0; i < 300; ++i) { // up to ~3s
        pid_t r = ::waitpid(server_pid, &status, WNOHANG);
        if (r == server_pid) {
            reaped = true;
            break;
        }
        ASSERT_EQ(r, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    const auto elapsed = std::chrono::steady_clock::now() - sent_at;

    ASSERT_TRUE(reaped); // server must exit within 3s of SIGTERM (else drain is stuck)
    EXPECT_TRUE(WIFEXITED(status)); // clean exit, not killed
    // Must be well under the 8s deadline — proves prompt drain, not deadline force-close.
    EXPECT_LT(elapsed, std::chrono::seconds(3));
}

TEST(HTTPServerConnectionTimeout, ClosesStalledPartialRequestAndCountsIt) {
    const uint16_t port = reserve_ephemeral_port();
    ASSERT_NE(port, 0);

    const pid_t server_pid = spawn_timeout_server(port);
    ASSERT_GT(server_pid, 0);

    int client_fd = -1;
    struct guard {
        pid_t pid;
        int* client_fd;
        ~guard() {
            if (client_fd && *client_fd >= 0) {
                ::close(*client_fd);
                *client_fd = -1;
            }
            if (pid > 0) {
                ::kill(pid, SIGKILL);
                int status = 0;
                (void)::waitpid(pid, &status, 0);
            }
        }
    } cleanup{server_pid, &client_fd};

    client_fd = connect_with_retry(port);
    ASSERT_GE(client_fd, 0);

    // Send a partial request (no terminating blank line) and then stall.
    send_all(client_fd, "GET /ping HTTP/1.1\r\nHost: 127.0.0.1\r\n");

    // The server should close the connection once the 300ms read timeout elapses. Detect the
    // peer close (recv returns 0) within a generous bound.
    const auto start = std::chrono::steady_clock::now();
    bool closed = false;
    char buf[256];
    for (int i = 0; i < 200; ++i) { // up to ~2s
        pollfd pfd{client_fd, POLLIN, 0};
        if (::poll(&pfd, 1, 20) > 0) {
            ssize_t n = ::recv(client_fd, buf, sizeof(buf), 0);
            if (n == 0) { // orderly close by the server
                closed = true;
                break;
            }
            if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
                closed = true; // RST etc. also counts as dropped
                break;
            }
        }
    }
    const auto elapsed = std::chrono::steady_clock::now() - start;
    ASSERT_TRUE(closed); // stalled connection must be dropped, not held open
    EXPECT_GE(elapsed, std::chrono::milliseconds(150)); // it was the timeout, not an instant reject
    EXPECT_LT(elapsed, std::chrono::milliseconds(1800));

    // The drop is reflected in the connection-timeout metric (queried over a fresh connection).
    int metrics_fd = connect_with_retry(port);
    ASSERT_GE(metrics_fd, 0);
    send_all(metrics_fd, make_get_request("/metrics"));
    auto responses = read_responses_slowly(metrics_fd, 1, std::chrono::seconds(2));
    ::close(metrics_fd);
    ASSERT_EQ(responses.size(), 1u);
    const std::string& body = responses[0].body;
    // Match the data line (newline-anchored), not the "# HELP/# TYPE" comment lines that also
    // contain the metric name.
    const std::string needle = "\nkatana_http_connection_timeouts_total ";
    const size_t pos = body.find(needle);
    ASSERT_NE(pos, std::string::npos);
    const int count = std::stoi(body.substr(pos + needle.size()));
    EXPECT_GE(count, 1);
}
