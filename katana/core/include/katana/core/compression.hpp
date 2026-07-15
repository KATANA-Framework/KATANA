#pragma once

// Response compression: negotiate an encoding from Accept-Encoding and compress a response body with
// gzip / brotli / zstd. Opt-in via server.compression(); the server applies it in finalize_response.

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace katana::http {

enum class content_encoding : uint8_t { identity, gzip, brotli, zstd };

struct compression_config {
    bool gzip = true;
    bool brotli = true;
    bool zstd = true;
    size_t min_size = 256; // do not compress bodies smaller than this
    int level = 0;         // 0 = per-codec default
};

// Best encoding the client accepts, in server preference order (brotli > zstd > gzip), honoring the
// config's enabled codecs and skipping any `;q=0`. `identity` when nothing matches.
[[nodiscard]] content_encoding negotiate_encoding(std::string_view accept_encoding,
                                                  const compression_config& cfg);

// Compress `data`; nullopt on failure (caller then sends it uncompressed).
[[nodiscard]] std::optional<std::string> compress(content_encoding enc, std::string_view data,
                                                  const compression_config& cfg);

// Content-Encoding token: "gzip" / "br" / "zstd" (empty for identity).
[[nodiscard]] std::string_view encoding_token(content_encoding enc);

// Whether a Content-Type is worth compressing (text/*, application/json|xml|javascript, image/svg).
[[nodiscard]] bool is_compressible_type(std::string_view content_type);

} // namespace katana::http
