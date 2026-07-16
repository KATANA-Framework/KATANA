#include "katana/core/http_client.hpp"

#include <cstdlib>
#include <optional>
#include <system_error>

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef KATANA_HAS_OPENSSL
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

namespace katana::http {

namespace {

struct parsed_url {
    bool https = false;
    std::string host;
    std::string port;
    std::string target; // path + query
};

std::optional<parsed_url> parse_url(std::string_view url) {
    parsed_url out;
    if (url.starts_with("https://")) {
        out.https = true;
        url.remove_prefix(8);
    } else if (url.starts_with("http://")) {
        url.remove_prefix(7);
    } else {
        return std::nullopt;
    }
    const auto slash = url.find('/');
    std::string_view authority = slash == std::string_view::npos ? url : url.substr(0, slash);
    out.target = slash == std::string_view::npos ? "/" : std::string(url.substr(slash));
    const auto colon = authority.find(':');
    if (colon == std::string_view::npos) {
        out.host = std::string(authority);
        out.port = out.https ? "443" : "80";
    } else {
        out.host = std::string(authority.substr(0, colon));
        out.port = std::string(authority.substr(colon + 1));
    }
    if (out.host.empty()) {
        return std::nullopt;
    }
    return out;
}

// A byte stream over either a plain fd or a TLS session.
struct stream {
    int fd = -1;
#ifdef KATANA_HAS_OPENSSL
    SSL* ssl = nullptr;
    SSL_CTX* ctx = nullptr;
#endif
    ~stream() {
#ifdef KATANA_HAS_OPENSSL
        if (ssl != nullptr) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
        if (ctx != nullptr) {
            SSL_CTX_free(ctx);
        }
#endif
        if (fd >= 0) {
            ::close(fd);
        }
    }

    bool write_all(std::string_view data) {
        size_t sent = 0;
        while (sent < data.size()) {
            ssize_t n;
#ifdef KATANA_HAS_OPENSSL
            if (ssl != nullptr) {
                n = SSL_write(ssl, data.data() + sent, static_cast<int>(data.size() - sent));
            } else
#endif
            {
                n = ::send(fd, data.data() + sent, data.size() - sent, 0);
            }
            if (n <= 0) {
                return false;
            }
            sent += static_cast<size_t>(n);
        }
        return true;
    }

    // Read until EOF (peer closes on `Connection: close`).
    std::string read_all() {
        std::string out;
        char buf[8192];
        for (;;) {
            ssize_t n;
#ifdef KATANA_HAS_OPENSSL
            if (ssl != nullptr) {
                n = SSL_read(ssl, buf, sizeof(buf));
            } else
#endif
            {
                n = ::recv(fd, buf, sizeof(buf), 0);
            }
            if (n <= 0) {
                break;
            }
            out.append(buf, static_cast<size_t>(n));
        }
        return out;
    }
};

result<http_client_response> request(std::string_view method, std::string_view url,
                                     std::string_view content_type, std::string_view body,
                                     const http_client_options& opts) {
    auto parsed = parse_url(url);
    if (!parsed) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* res = nullptr;
    if (::getaddrinfo(parsed->host.c_str(), parsed->port.c_str(), &hints, &res) != 0 ||
        res == nullptr) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }

    int fd = -1;
    for (addrinfo* ai = res; ai != nullptr; ai = ai->ai_next) {
        fd = ::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd < 0) {
            continue;
        }
        timeval tv{static_cast<time_t>(opts.timeout.count() / 1000),
                   static_cast<suseconds_t>((opts.timeout.count() % 1000) * 1000)};
        ::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
        ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        if (::connect(fd, ai->ai_addr, ai->ai_addrlen) == 0) {
            break;
        }
        ::close(fd);
        fd = -1;
    }
    ::freeaddrinfo(res);
    if (fd < 0) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }

    stream s;
    s.fd = fd;

    if (parsed->https) {
#ifdef KATANA_HAS_OPENSSL
        s.ctx = SSL_CTX_new(TLS_client_method());
        if (s.ctx == nullptr) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }
        if (opts.verify_tls) {
            SSL_CTX_set_verify(s.ctx, SSL_VERIFY_PEER, nullptr);
            SSL_CTX_set_default_verify_paths(s.ctx);
        }
        s.ssl = SSL_new(s.ctx);
        SSL_set_fd(s.ssl, fd);
        SSL_set_tlsext_host_name(s.ssl, parsed->host.c_str()); // SNI
        if (SSL_connect(s.ssl) != 1) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }
        if (opts.verify_tls && SSL_get_verify_result(s.ssl) != X509_V_OK) {
            return std::unexpected(std::make_error_code(std::errc::io_error));
        }
#else
        return std::unexpected(std::make_error_code(std::errc::not_supported));
#endif
    }

    std::string req;
    req += method;
    req += ' ';
    req += parsed->target;
    req += " HTTP/1.1\r\nHost: ";
    req += parsed->host;
    req += "\r\nConnection: close\r\nAccept: */*\r\n";
    if (!body.empty()) {
        req += "Content-Type: ";
        req += content_type;
        req += "\r\nContent-Length: ";
        req += std::to_string(body.size());
        req += "\r\n";
    }
    req += "\r\n";
    req += body;

    if (!s.write_all(req)) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }

    const std::string raw = s.read_all();
    if (raw.empty()) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }

    // Parse status line + split headers/body.
    http_client_response resp;
    const auto line_end = raw.find("\r\n");
    if (line_end == std::string::npos) {
        return std::unexpected(std::make_error_code(std::errc::io_error));
    }
    {
        const std::string status_line = raw.substr(0, line_end);
        const auto sp = status_line.find(' ');
        if (sp != std::string::npos) {
            resp.status = std::atoi(status_line.c_str() + sp + 1);
        }
    }
    const auto header_end = raw.find("\r\n\r\n");
    std::string body_part =
        header_end == std::string::npos ? std::string{} : raw.substr(header_end + 4);

    // Handle chunked transfer-encoding minimally (concatenate chunk bodies).
    const std::string headers = raw.substr(0, header_end == std::string::npos ? raw.size() : header_end);
    if (headers.find("Transfer-Encoding: chunked") != std::string::npos ||
        headers.find("transfer-encoding: chunked") != std::string::npos) {
        std::string decoded;
        size_t pos = 0;
        while (pos < body_part.size()) {
            const auto nl = body_part.find("\r\n", pos);
            if (nl == std::string::npos) {
                break;
            }
            const size_t chunk_size = std::strtoul(body_part.c_str() + pos, nullptr, 16);
            if (chunk_size == 0) {
                break;
            }
            const size_t data_start = nl + 2;
            if (data_start + chunk_size > body_part.size()) {
                break;
            }
            decoded.append(body_part, data_start, chunk_size);
            pos = data_start + chunk_size + 2; // skip trailing \r\n
        }
        resp.body = std::move(decoded);
    } else {
        resp.body = std::move(body_part);
    }
    return resp;
}

} // namespace

result<http_client_response> http_get(std::string_view url, const http_client_options& opts) {
    return request("GET", url, {}, {}, opts);
}

result<http_client_response> http_post(std::string_view url, std::string_view content_type,
                                       std::string_view body, const http_client_options& opts) {
    return request("POST", url, content_type, body, opts);
}

} // namespace katana::http
