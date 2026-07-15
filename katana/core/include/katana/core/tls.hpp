#pragma once

// TLS/HTTPS termination via OpenSSL, integrated with the non-blocking reactor. ssl_session mirrors
// tcp_socket's read/write/writev contract exactly (empty span = EAGAIN, 0 = blocked, error_code::ok
// = EOF) so the existing http_server flush/parse loop drives it unchanged, plus a handshake() phase
// and want-read/want-write hints for interest re-arming.

#include "result.hpp"

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

struct iovec;
struct ssl_st;    // OpenSSL SSL
struct ssl_ctx_st; // OpenSSL SSL_CTX

namespace katana::tls {

struct tls_config {
    std::string cert_file;             // PEM certificate (may include the chain)
    std::string key_file;              // PEM private key
    std::string ca_file;               // optional: verify client certs against this CA (mTLS)
    std::vector<std::string> alpn{"http/1.1"}; // ALPN protocols offered, in preference order
    std::string cipher_list;           // optional TLS<=1.2 cipher override (empty = modern default)
    int min_version = 0;               // 0 = TLS 1.2 (set in create())
    bool require_client_cert = false;  // mTLS
};

// Owns an SSL_CTX (cert/key/ciphers/ALPN/SNI). Immutable once created; hot-reload swaps the whole
// shared_ptr<tls_context> on the server so in-flight connections keep their old context.
class tls_context {
public:
    static result<std::shared_ptr<tls_context>> create(const tls_config& cfg);
    ~tls_context();

    tls_context(const tls_context&) = delete;
    tls_context& operator=(const tls_context&) = delete;

    [[nodiscard]] ssl_ctx_st* native() const noexcept { return ctx_; }

private:
    explicit tls_context(ssl_ctx_st* ctx) noexcept : ctx_(ctx) {}
    ssl_ctx_st* ctx_ = nullptr;
    std::string alpn_wire_; // length-prefixed ALPN list; app-data for the select callback
};

// A per-connection TLS session bound to a non-blocking fd. Move-only; owns the SSL*.
class ssl_session {
public:
    enum class handshake_state : uint8_t { want_read, want_write, done, failed };

    ssl_session() = default;
    ssl_session(const tls_context& ctx, int fd);
    ~ssl_session();
    ssl_session(ssl_session&& other) noexcept;
    ssl_session& operator=(ssl_session&& other) noexcept;
    ssl_session(const ssl_session&) = delete;
    ssl_session& operator=(const ssl_session&) = delete;

    [[nodiscard]] explicit operator bool() const noexcept { return ssl_ != nullptr; }

    // Drive the TLS handshake one step; call again on the requested readiness until done/failed.
    handshake_state handshake();

    // Same conventions as tcp_socket: read() returns the filled prefix (empty span == would-block),
    // unexpected(error_code::ok) on clean peer close (SSL_ERROR_ZERO_RETURN); write()/writev() return
    // bytes written, 0 == would-block. writev coalesces (no vectored TLS write).
    result<std::span<uint8_t>> read(std::span<uint8_t> buf);
    result<size_t> write(std::span<const uint8_t> data);
    result<size_t> writev(const iovec* iov, size_t count);

    // After a would-block, which readiness the SSL layer is waiting on (for interest re-arming).
    [[nodiscard]] bool wants_read() const noexcept { return want_read_; }
    [[nodiscard]] bool wants_write() const noexcept { return want_write_; }

    // Negotiated ALPN protocol (e.g. "http/1.1"), empty if none.
    [[nodiscard]] std::string alpn_protocol() const;

private:
    ssl_st* ssl_ = nullptr;
    bool want_read_ = false;
    bool want_write_ = false;
};

} // namespace katana::tls
