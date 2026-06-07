#include "katana/core/redis_client.hpp"
#include "katana/core/redis_policy_stores.hpp"

#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <charconv>
#include <chrono>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace katana::http;

namespace {

uint16_t reserve_port() {
    const int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
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

    const auto port = ntohs(addr.sin_port);
    ::close(fd);
    return port;
}

struct fake_redis_server {
    explicit fake_redis_server(uint16_t listen_port) : port(listen_port) {}

    ~fake_redis_server() { stop(); }

    void start() {
        // Bind + listen synchronously so the listener is ready before start() returns.
        // Otherwise the client could connect before the worker thread reached listen(),
        // a startup race that shows up under `ctest -j`. The worker only accepts + serves.
        listener_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
        if (listener_fd < 0) {
            return;
        }
        int opt = 1;
        ::setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(port);
        if (::bind(listener_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0 ||
            ::listen(listener_fd, 4) < 0) {
            ::close(listener_fd);
            listener_fd = -1;
            return;
        }
        running.store(true);
        worker = std::thread([this]() { run(); });
    }

    void stop() {
        if (!running.exchange(false)) {
            return;
        }
        if (listener_fd >= 0) {
            ::shutdown(listener_fd, SHUT_RDWR);
            ::close(listener_fd);
            listener_fd = -1;
        }
        if (client_fd >= 0) {
            ::shutdown(client_fd, SHUT_RDWR);
            ::close(client_fd);
            client_fd = -1;
        }
        if (worker.joinable()) {
            worker.join();
        }
    }

    uint16_t port = 0;
    std::thread worker;
    std::atomic<bool> running{false};
    int listener_fd = -1;
    int client_fd = -1;
    std::unordered_map<std::string, std::string> values;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expiries;

private:
    static std::optional<std::vector<std::string>> parse_command(std::string& buffer) {
        if (buffer.empty() || buffer.front() != '*') {
            return std::nullopt;
        }

        auto eol = buffer.find("\r\n");
        if (eol == std::string::npos) {
            return std::nullopt;
        }

        int argc = 0;
        auto [argc_end, argc_ec] =
            std::from_chars(buffer.data() + 1, buffer.data() + eol, argc);
        if (argc_ec != std::errc() || argc_end != buffer.data() + eol) {
            return std::nullopt;
        }

        size_t cursor = eol + 2;
        std::vector<std::string> args;
        args.reserve(static_cast<size_t>(argc));
        for (int i = 0; i < argc; ++i) {
            if (cursor >= buffer.size() || buffer[cursor] != '$') {
                return std::nullopt;
            }
            const auto len_eol = buffer.find("\r\n", cursor);
            if (len_eol == std::string::npos) {
                return std::nullopt;
            }
            size_t len = 0;
            auto [len_end, len_ec] = std::from_chars(buffer.data() + cursor + 1,
                                                     buffer.data() + len_eol,
                                                     len);
            if (len_ec != std::errc() || len_end != buffer.data() + len_eol) {
                return std::nullopt;
            }
            cursor = len_eol + 2;
            if (buffer.size() < cursor + len + 2) {
                return std::nullopt;
            }
            args.emplace_back(buffer.data() + cursor, len);
            cursor += len;
            if (buffer.compare(cursor, 2, "\r\n") != 0) {
                return std::nullopt;
            }
            cursor += 2;
        }

        buffer.erase(0, cursor);
        return args;
    }

    static void send_all(int fd, std::string_view bytes) {
        size_t sent = 0;
        while (sent < bytes.size()) {
            const auto n = ::send(fd, bytes.data() + sent, bytes.size() - sent, 0);
            if (n <= 0) {
                return;
            }
            sent += static_cast<size_t>(n);
        }
    }

    void expire_if_needed(const std::string& key) {
        const auto it = expiries.find(key);
        if (it == expiries.end()) {
            return;
        }
        if (it->second <= std::chrono::steady_clock::now()) {
            expiries.erase(it);
            values.erase(key);
        }
    }

    void handle_command(const std::vector<std::string>& args) {
        if (args.empty()) {
            send_all(client_fd, "-ERR empty\r\n");
            return;
        }

        const auto& cmd = args[0];
        if (cmd == "GET" && args.size() == 2) {
            expire_if_needed(args[1]);
            auto it = values.find(args[1]);
            if (it == values.end()) {
                send_all(client_fd, "$-1\r\n");
                return;
            }
            std::string resp = "$" + std::to_string(it->second.size()) + "\r\n" + it->second +
                               "\r\n";
            send_all(client_fd, resp);
            return;
        }

        if (cmd == "SET" && (args.size() == 5 || args.size() == 6)) {
            bool nx = false;
            size_t ttl_index = 3;
            if (args.size() == 6) {
                nx = args[3] == "NX";
                ttl_index = 4;
            }
            expire_if_needed(args[1]);
            if (nx && values.contains(args[1])) {
                send_all(client_fd, "$-1\r\n");
                return;
            }
            values[args[1]] = args[2];
            int64_t ttl_ms = 0;
            std::from_chars(args[ttl_index + 1].data(),
                            args[ttl_index + 1].data() + args[ttl_index + 1].size(),
                            ttl_ms);
            expiries[args[1]] = std::chrono::steady_clock::now() + std::chrono::milliseconds(ttl_ms);
            send_all(client_fd, "+OK\r\n");
            return;
        }

        if (cmd == "DEL" && args.size() == 2) {
            expire_if_needed(args[1]);
            const auto erased = values.erase(args[1]);
            expiries.erase(args[1]);
            send_all(client_fd, ":" + std::to_string(erased) + "\r\n");
            return;
        }

        if (cmd == "INCR" && args.size() == 2) {
            expire_if_needed(args[1]);
            int64_t value = 0;
            if (auto it = values.find(args[1]); it != values.end()) {
                std::from_chars(it->second.data(), it->second.data() + it->second.size(), value);
            }
            ++value;
            values[args[1]] = std::to_string(value);
            send_all(client_fd, ":" + std::to_string(value) + "\r\n");
            return;
        }

        if (cmd == "PEXPIRE" && args.size() == 3) {
            expire_if_needed(args[1]);
            if (!values.contains(args[1])) {
                send_all(client_fd, ":0\r\n");
                return;
            }
            int64_t ttl_ms = 0;
            std::from_chars(args[2].data(), args[2].data() + args[2].size(), ttl_ms);
            expiries[args[1]] = std::chrono::steady_clock::now() + std::chrono::milliseconds(ttl_ms);
            send_all(client_fd, ":1\r\n");
            return;
        }

        if (cmd == "PTTL" && args.size() == 2) {
            expire_if_needed(args[1]);
            if (!values.contains(args[1])) {
                send_all(client_fd, ":-2\r\n");
                return;
            }
            const auto it = expiries.find(args[1]);
            if (it == expiries.end()) {
                send_all(client_fd, ":-1\r\n");
                return;
            }
            const auto remaining =
                std::chrono::duration_cast<std::chrono::milliseconds>(it->second -
                                                                      std::chrono::steady_clock::now())
                    .count();
            send_all(client_fd, ":" + std::to_string(std::max<int64_t>(remaining, 0)) + "\r\n");
            return;
        }

        send_all(client_fd, "-ERR unsupported\r\n");
    }

    void run() {
        // listener_fd is already bound + listening (set up synchronously in start()).
        client_fd = ::accept(listener_fd, nullptr, nullptr);
        if (client_fd < 0) {
            return;
        }

        std::string buffer;
        std::array<char, 4096> chunk{};
        while (running.load()) {
            const auto n = ::recv(client_fd, chunk.data(), chunk.size(), 0);
            if (n <= 0) {
                return;
            }
            buffer.append(chunk.data(), static_cast<size_t>(n));
            for (;;) {
                auto command = parse_command(buffer);
                if (!command.has_value()) {
                    break;
                }
                handle_command(*command);
            }
        }
    }
};

} // namespace

TEST(RedisClient, RoundTripsBasicCommandsOverTcp) {
    const uint16_t port = reserve_port();
    ASSERT_NE(port, 0);

    fake_redis_server server(port);
    server.start();

    redis_sync_client::options opts;
    opts.host = "127.0.0.1";
    opts.port = port;
    opts.connect_timeout = std::chrono::milliseconds(500);
    opts.io_timeout = std::chrono::milliseconds(500);
    redis_sync_client client(std::move(opts));

    ASSERT_TRUE(client.connect());
    EXPECT_TRUE(client.connected());

    client.set("alpha", "one", std::chrono::seconds(5));
    auto loaded = client.get("alpha");
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(*loaded, "one");

    EXPECT_FALSE(client.set_if_absent("alpha", "two", std::chrono::seconds(5)));
    EXPECT_TRUE(client.set_if_absent("beta", "two", std::chrono::seconds(5)));
    EXPECT_EQ(client.increment("counter"), 1);
    EXPECT_EQ(client.increment("counter"), 2);

    auto ttl = client.ttl("beta");
    ASSERT_TRUE(ttl.has_value());
    EXPECT_GT(*ttl, std::chrono::milliseconds::zero());

    client.expire("beta", std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    EXPECT_FALSE(client.get("beta").has_value());

    client.erase("alpha");
    EXPECT_FALSE(client.get("alpha").has_value());

    server.stop();
}

TEST(RedisClient, WorksAsBackendForRedisPolicyStore) {
    const uint16_t port = reserve_port();
    ASSERT_NE(port, 0);

    fake_redis_server server(port);
    server.start();

    redis_sync_client::options opts;
    opts.host = "127.0.0.1";
    opts.port = port;
    redis_sync_client client(std::move(opts));
    ASSERT_TRUE(client.connect());

    redis_response_cache_store store(client);
    const route_policy_view policy{
        route_cache_policy_view{route_cache_policy_kind::enabled, "true"},
        {},
        {},
        {},
        "cachedJobs",
    };

    store.store(&policy,
                "GET /jobs",
                response_snapshot{200, "OK", {{"Content-Type", "text/plain"}}, "body", false},
                std::chrono::seconds(5),
                policy_clock::now());

    auto loaded = store.lookup(&policy, "GET /jobs", policy_clock::now());
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->body, "body");

    server.stop();
}
