#include "katana/core/tls.hpp"

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <cerrno>
#include <cstring>
#include <sys/uio.h> // iovec
#include <utility>

namespace katana::tls {

namespace {

// Encode ALPN protocols into OpenSSL wire format: each protocol is a 1-byte length + bytes.
std::string encode_alpn(const std::vector<std::string>& protos) {
    std::string wire;
    for (const auto& p : protos) {
        if (p.empty() || p.size() > 255) {
            continue;
        }
        wire.push_back(static_cast<char>(p.size()));
        wire.append(p);
    }
    return wire;
}

// Server-side ALPN selection: pick the first of *our* protocols the client also offers.
int alpn_select_cb(SSL* /*ssl*/, const unsigned char** out, unsigned char* outlen,
                   const unsigned char* in, unsigned int inlen, void* arg) {
    const auto* wire = static_cast<const std::string*>(arg);
    if (wire == nullptr || wire->empty()) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    const auto* server = reinterpret_cast<const unsigned char*>(wire->data());
    // SSL_select_next_proto picks the server's preference among the client's list.
    unsigned char* selected = nullptr;
    if (SSL_select_next_proto(&selected, outlen, server, static_cast<unsigned int>(wire->size()), in,
                              inlen) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    *out = selected;
    return SSL_TLSEXT_ERR_OK;
}

std::error_code ssl_error(int ssl_err) {
    // Clean shutdown → the EOF sentinel the http_server already understands.
    if (ssl_err == SSL_ERROR_ZERO_RETURN) {
        return make_error_code(error_code::ok);
    }
    if (ssl_err == SSL_ERROR_SYSCALL && errno != 0) {
        return {errno, std::system_category()};
    }
    // Generic protocol failure — any non-ok code makes the server drop the connection.
    return {EPROTO, std::system_category()};
}

} // namespace

// ---------------------------------------------------------------------------- tls_context

result<std::shared_ptr<tls_context>> tls_context::create(const tls_config& cfg) {
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    if (ctx == nullptr) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }

    const long min_ver = cfg.min_version != 0 ? cfg.min_version : TLS1_2_VERSION;
    SSL_CTX_set_min_proto_version(ctx, min_ver);
    // Sensible hardening: no compression (CRIME), server cipher preference, no renegotiation churn.
    SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION | SSL_OP_CIPHER_SERVER_PREFERENCE |
                                 SSL_OP_NO_RENEGOTIATION);

    auto fail = [&](void) {
        SSL_CTX_free(ctx);
        return std::unexpected(make_error_code(error_code::invalid_fd));
    };

    if (SSL_CTX_use_certificate_chain_file(ctx, cfg.cert_file.c_str()) != 1) {
        return fail();
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, cfg.key_file.c_str(), SSL_FILETYPE_PEM) != 1) {
        return fail();
    }
    if (SSL_CTX_check_private_key(ctx) != 1) {
        return fail();
    }
    if (!cfg.cipher_list.empty()) {
        SSL_CTX_set_cipher_list(ctx, cfg.cipher_list.c_str());
    }
    if (!cfg.ca_file.empty()) {
        if (SSL_CTX_load_verify_locations(ctx, cfg.ca_file.c_str(), nullptr) != 1) {
            return fail();
        }
        int mode = SSL_VERIFY_PEER;
        if (cfg.require_client_cert) {
            mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
        }
        SSL_CTX_set_verify(ctx, mode, nullptr);
    }

    auto tc = std::shared_ptr<tls_context>(new tls_context(ctx));
    tc->alpn_wire_ = encode_alpn(cfg.alpn);
    if (!tc->alpn_wire_.empty()) {
        SSL_CTX_set_alpn_select_cb(ctx, &alpn_select_cb, &tc->alpn_wire_);
    }
    return tc;
}

tls_context::~tls_context() {
    if (ctx_ != nullptr) {
        SSL_CTX_free(ctx_);
    }
}

// ---------------------------------------------------------------------------- ssl_session

ssl_session::ssl_session(const tls_context& ctx, int fd) {
    ssl_ = SSL_new(ctx.native());
    if (ssl_ != nullptr) {
        SSL_set_fd(ssl_, fd); // BIO_NOCLOSE: the fd stays owned by tcp_socket
        SSL_set_accept_state(ssl_);
    }
}

ssl_session::~ssl_session() {
    if (ssl_ != nullptr) {
        SSL_free(ssl_);
    }
}

ssl_session::ssl_session(ssl_session&& other) noexcept
    : ssl_(std::exchange(other.ssl_, nullptr)), want_read_(other.want_read_),
      want_write_(other.want_write_) {}

ssl_session& ssl_session::operator=(ssl_session&& other) noexcept {
    if (this != &other) {
        if (ssl_ != nullptr) {
            SSL_free(ssl_);
        }
        ssl_ = std::exchange(other.ssl_, nullptr);
        want_read_ = other.want_read_;
        want_write_ = other.want_write_;
    }
    return *this;
}

ssl_session::handshake_state ssl_session::handshake() {
    want_read_ = want_write_ = false;
    if (ssl_ == nullptr) {
        return handshake_state::failed;
    }
    const int ret = SSL_accept(ssl_);
    if (ret == 1) {
        return handshake_state::done;
    }
    switch (SSL_get_error(ssl_, ret)) {
    case SSL_ERROR_WANT_READ:
        want_read_ = true;
        return handshake_state::want_read;
    case SSL_ERROR_WANT_WRITE:
        want_write_ = true;
        return handshake_state::want_write;
    default:
        return handshake_state::failed;
    }
}

result<std::span<uint8_t>> ssl_session::read(std::span<uint8_t> buf) {
    want_read_ = want_write_ = false;
    if (ssl_ == nullptr) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }
    const int n = SSL_read(ssl_, buf.data(), static_cast<int>(buf.size()));
    if (n > 0) {
        return buf.subspan(0, static_cast<size_t>(n));
    }
    switch (SSL_get_error(ssl_, n)) {
    case SSL_ERROR_WANT_READ:
        want_read_ = true;
        return std::span<uint8_t>{};
    case SSL_ERROR_WANT_WRITE:
        want_write_ = true; // renegotiation: read is blocked on a write
        return std::span<uint8_t>{};
    default:
        return std::unexpected(ssl_error(SSL_get_error(ssl_, n)));
    }
}

result<size_t> ssl_session::write(std::span<const uint8_t> data) {
    want_read_ = want_write_ = false;
    if (ssl_ == nullptr) {
        return std::unexpected(make_error_code(error_code::invalid_fd));
    }
    if (data.empty()) {
        return size_t{0};
    }
    const int n = SSL_write(ssl_, data.data(), static_cast<int>(data.size()));
    if (n > 0) {
        return static_cast<size_t>(n);
    }
    switch (SSL_get_error(ssl_, n)) {
    case SSL_ERROR_WANT_WRITE:
        want_write_ = true;
        return size_t{0}; // blocked
    case SSL_ERROR_WANT_READ:
        want_read_ = true; // renegotiation: write is blocked on a read
        return size_t{0};
    default:
        return std::unexpected(ssl_error(SSL_get_error(ssl_, n)));
    }
}

result<size_t> ssl_session::writev(const iovec* iov, size_t count) {
    // No vectored TLS write: send the first non-empty segment; the caller's write_pos loop advances
    // through the rest across calls (same partial-write contract as tcp_socket).
    for (size_t i = 0; i < count; ++i) {
        if (iov[i].iov_len == 0) {
            continue;
        }
        return write({static_cast<const uint8_t*>(iov[i].iov_base), iov[i].iov_len});
    }
    return size_t{0};
}

std::string ssl_session::alpn_protocol() const {
    if (ssl_ == nullptr) {
        return {};
    }
    const unsigned char* proto = nullptr;
    unsigned int len = 0;
    SSL_get0_alpn_selected(ssl_, &proto, &len);
    if (proto == nullptr || len == 0) {
        return {};
    }
    return std::string(reinterpret_cast<const char*>(proto), len);
}

} // namespace katana::tls
