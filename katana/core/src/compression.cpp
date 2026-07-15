#include "katana/core/compression.hpp"

#include <brotli/encode.h>
#include <zlib.h>
#include <zstd.h>

#include <cctype>

namespace katana::http {

namespace {

// Case-insensitive check that `haystack` contains `token` as a comma/space-delimited item, and that
// it isn't disabled with ;q=0. Simplified Accept-Encoding parsing (ignores fractional q-values).
bool accepts(std::string_view accept, std::string_view token) {
    size_t pos = 0;
    while (pos < accept.size()) {
        size_t comma = accept.find(',', pos);
        std::string_view item = accept.substr(pos, comma == std::string_view::npos ? std::string_view::npos
                                                                                    : comma - pos);
        pos = comma == std::string_view::npos ? accept.size() : comma + 1;

        // Trim spaces.
        while (!item.empty() && (item.front() == ' ' || item.front() == '\t')) item.remove_prefix(1);
        // Split coding;params.
        size_t semi = item.find(';');
        std::string_view coding = item.substr(0, semi);
        while (!coding.empty() && coding.back() == ' ') coding.remove_suffix(1);

        bool match = coding.size() == token.size();
        for (size_t i = 0; match && i < coding.size(); ++i) {
            match = std::tolower(static_cast<unsigned char>(coding[i])) == token[i];
        }
        if (!match) {
            continue;
        }
        // Reject explicit q=0.
        if (semi != std::string_view::npos && item.find("q=0", semi) != std::string_view::npos &&
            item.find("q=0.", semi) == std::string_view::npos) {
            return false;
        }
        return true;
    }
    return false;
}

std::optional<std::string> gzip_compress(std::string_view data, int level) {
    z_stream zs{};
    if (deflateInit2(&zs, level != 0 ? level : Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8,
                     Z_DEFAULT_STRATEGY) != Z_OK) {
        return std::nullopt;
    }
    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));
    zs.avail_in = static_cast<uInt>(data.size());
    std::string out;
    char buf[16384];
    int ret = Z_OK;
    do {
        zs.next_out = reinterpret_cast<Bytef*>(buf);
        zs.avail_out = sizeof(buf);
        ret = deflate(&zs, Z_FINISH);
        out.append(buf, sizeof(buf) - zs.avail_out);
    } while (ret == Z_OK);
    deflateEnd(&zs);
    if (ret != Z_STREAM_END) {
        return std::nullopt;
    }
    return out;
}

std::optional<std::string> brotli_compress(std::string_view data, int level) {
    size_t out_size = BrotliEncoderMaxCompressedSize(data.size());
    if (out_size == 0) {
        out_size = data.size() + 1024;
    }
    std::string out(out_size, '\0');
    const int quality = level != 0 ? level : BROTLI_DEFAULT_QUALITY;
    if (BrotliEncoderCompress(quality, BROTLI_DEFAULT_WINDOW, BROTLI_MODE_TEXT, data.size(),
                              reinterpret_cast<const uint8_t*>(data.data()), &out_size,
                              reinterpret_cast<uint8_t*>(out.data())) != BROTLI_TRUE) {
        return std::nullopt;
    }
    out.resize(out_size);
    return out;
}

std::optional<std::string> zstd_compress(std::string_view data, int level) {
    const size_t bound = ZSTD_compressBound(data.size());
    std::string out(bound, '\0');
    const size_t n =
        ZSTD_compress(out.data(), bound, data.data(), data.size(), level != 0 ? level : 3);
    if (ZSTD_isError(n) != 0u) {
        return std::nullopt;
    }
    out.resize(n);
    return out;
}

} // namespace

content_encoding negotiate_encoding(std::string_view accept, const compression_config& cfg) {
    if (accept.empty()) {
        return content_encoding::identity;
    }
    if (cfg.brotli && accepts(accept, "br")) {
        return content_encoding::brotli;
    }
    if (cfg.zstd && accepts(accept, "zstd")) {
        return content_encoding::zstd;
    }
    if (cfg.gzip && accepts(accept, "gzip")) {
        return content_encoding::gzip;
    }
    return content_encoding::identity;
}

std::optional<std::string> compress(content_encoding enc, std::string_view data,
                                    const compression_config& cfg) {
    switch (enc) {
    case content_encoding::gzip:
        return gzip_compress(data, cfg.level);
    case content_encoding::brotli:
        return brotli_compress(data, cfg.level);
    case content_encoding::zstd:
        return zstd_compress(data, cfg.level);
    case content_encoding::identity:
        break;
    }
    return std::nullopt;
}

std::string_view encoding_token(content_encoding enc) {
    switch (enc) {
    case content_encoding::gzip:
        return "gzip";
    case content_encoding::brotli:
        return "br";
    case content_encoding::zstd:
        return "zstd";
    case content_encoding::identity:
        break;
    }
    return {};
}

bool is_compressible_type(std::string_view ct) {
    // Match the media type prefix (ignore parameters after ';').
    const size_t semi = ct.find(';');
    std::string_view type = ct.substr(0, semi);
    static constexpr std::string_view kPrefixes[] = {
        "text/",
        "application/json",
        "application/xml",
        "application/javascript",
        "application/manifest+json",
        "application/xhtml+xml",
        "image/svg+xml",
    };
    for (auto p : kPrefixes) {
        if (type.size() >= p.size() && type.substr(0, p.size()) == p) {
            return true;
        }
    }
    return false;
}

} // namespace katana::http
