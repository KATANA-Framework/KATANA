#pragma once

// Minimal blocking HTTP/1.1 client for outbound calls the framework needs (fetching a JWKS document,
// POSTing OTLP spans). Not a high-throughput client — one request per call, `Connection: close`,
// synchronous. Supports http:// and (when built with OpenSSL) https:// with optional cert
// verification. For heavy outbound traffic use a dedicated client; this covers control-plane calls.

#include "katana/core/result.hpp"

#include <chrono>
#include <string>
#include <string_view>

namespace katana::http {

struct http_client_response {
    int status = 0;
    std::string body;
};

struct http_client_options {
    std::chrono::milliseconds timeout{5000};
    bool verify_tls = true; // set false to accept a self-signed cert (dev only)
};

// GET a URL (http:// or https://). Returns the status + body, or an error on connect/transport failure.
result<http_client_response> http_get(std::string_view url, const http_client_options& opts = {});

// POST a body with the given Content-Type.
result<http_client_response> http_post(std::string_view url, std::string_view content_type,
                                       std::string_view body, const http_client_options& opts = {});

} // namespace katana::http
