#include <iostream>
#include <vector>
#include <chrono>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

struct connection_state {
    int fd;
    size_t requests_sent = 0;
    size_t responses_received = 0;
    bool waiting_for_response = false;
    char buffer[4096];
};

int create_nonblocking_connection(const char* host, uint16_t port) {
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd < 0) return -1;

    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    connect(sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    return sockfd;
}

int main(int argc, char* argv[]) {
    const char* host = "127.0.0.1";
    uint16_t port = argc > 1 ? static_cast<uint16_t>(std::stoul(argv[1])) : 8080;
    size_t num_connections = argc > 2 ? std::stoul(argv[2]) : 100;
    size_t target_requests = argc > 3 ? std::stoul(argv[3]) : 100000;

    std::cout << "=== Async HTTP Benchmark ===\n";
    std::cout << "Target: " << host << ":" << port << "\n";
    std::cout << "Connections: " << num_connections << "\n";
    std::cout << "Target requests: " << target_requests << "\n\n";

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        std::cerr << "epoll_create1 failed\n";
        return 1;
    }

    std::vector<connection_state> connections;
    connections.reserve(num_connections);

    // Create connections
    for (size_t i = 0; i < num_connections; ++i) {
        int fd = create_nonblocking_connection(host, port);
        if (fd < 0) {
            std::cerr << "Failed to create connection " << i << "\n";
            continue;
        }

        connection_state state;
        state.fd = fd;
        connections.push_back(state);

        epoll_event ev{};
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
        ev.data.ptr = &connections.back();
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    }

    std::cout << "Created " << connections.size() << " connections\n";
    sleep(1);  // Let connections establish

    const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n";
    size_t request_len = strlen(request);

    size_t total_requests = 0;
    size_t total_responses = 0;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<epoll_event> events(1024);

    while (total_responses < target_requests) {
        int nfds = epoll_wait(epoll_fd, events.data(), static_cast<int>(events.size()), 100);

        for (int i = 0; i < nfds; ++i) {
            auto* conn = static_cast<connection_state*>(events[static_cast<size_t>(i)].data.ptr);

            if (events[static_cast<size_t>(i)].events & EPOLLOUT) {
                while (total_requests < target_requests && !conn->waiting_for_response) {
                    ssize_t sent = send(conn->fd, request, request_len, MSG_DONTWAIT);
                    if (sent > 0) {
                        conn->requests_sent++;
                        conn->waiting_for_response = true;
                        total_requests++;
                    } else {
                        break;
                    }
                }
            }

            if (events[static_cast<size_t>(i)].events & EPOLLIN) {
                ssize_t received = recv(conn->fd, conn->buffer, sizeof(conn->buffer), MSG_DONTWAIT);
                if (received > 0) {
                    conn->responses_received++;
                    conn->waiting_for_response = false;
                    total_responses++;
                }
            }
        }

        // Print progress
        if (total_responses % 10000 == 0 && total_responses > 0) {
            auto now = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(now - start).count();
            double rps = static_cast<double>(total_responses) / elapsed;
            std::cout << "\rProgress: " << total_responses << " / " << target_requests
                      << " (" << static_cast<int>(rps) << " req/s)" << std::flush;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();

    std::cout << "\n\n=== Results ===\n";
    std::cout << "Total requests: " << total_requests << "\n";
    std::cout << "Total responses: " << total_responses << "\n";
    std::cout << "Duration: " << duration << " seconds\n";
    std::cout << "Throughput: " << static_cast<size_t>(static_cast<double>(total_responses) / duration) << " req/s\n";
    std::cout << "Average latency: " << (duration * 1000.0 / static_cast<double>(total_responses)) << " ms\n";

    for (auto& conn : connections) {
        close(conn.fd);
    }
    close(epoll_fd);

    return 0;
}
