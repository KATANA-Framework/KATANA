#pragma once
// CBOR + MessagePack → JSON transcoders. The media_type_registry classifies application/cbor and
// application/x-msgpack; this provides the matching decoders so a request body in a binary format can
// be transcoded to JSON and fed to the existing generated `parse_<Dto>(json, arena)` — one parse
// path, full validation, no hand-built DTOs. Self-contained, no external deps.
//
// Scope: the JSON-equivalent subset — unsigned/negative ints, float16/32/64, text strings, bools,
// null, arrays, and string-keyed maps. Byte strings are treated as UTF-8 text; non-string map keys
// are stringified. Malformed input returns std::nullopt (never throws), so callers answer 400.

#include "media_type_registry.hpp"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

namespace katana::serde {

namespace detail {

inline void json_escape(std::string& out, std::string_view s) {
    out.push_back('"');
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b";  break;
            case '\f': out += "\\f";  break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (c < 0x20) {
                    static const char* hex = "0123456789abcdef";
                    out += "\\u00";
                    out.push_back(hex[c >> 4]);
                    out.push_back(hex[c & 0xF]);
                } else {
                    out.push_back(static_cast<char>(c));
                }
        }
    }
    out.push_back('"');
}

inline void append_double(std::string& out, double d) {
    // JSON has no NaN/Inf; coerce to 0 to keep the document valid.
    if (!(d == d) || d == std::numeric_limits<double>::infinity() ||
        d == -std::numeric_limits<double>::infinity()) {
        out += "0";
        return;
    }
    char buf[32];
    int n = std::snprintf(buf, sizeof(buf), "%.17g", d);
    if (n > 0) out.append(buf, static_cast<size_t>(n));
    else out += "0";
}

// ── Cursor over the input bytes ───────────────────────────────────────────────
struct cursor {
    const unsigned char* p;
    const unsigned char* end;
    bool ok = true;

    bool avail(size_t n) const { return static_cast<size_t>(end - p) >= n; }
    unsigned char u8() {
        if (!avail(1)) { ok = false; return 0; }
        return *p++;
    }
    uint64_t be(size_t n) {
        uint64_t v = 0;
        if (!avail(n)) { ok = false; return 0; }
        for (size_t i = 0; i < n; ++i) v = (v << 8) | *p++;
        return v;
    }
};

// ── CBOR ──────────────────────────────────────────────────────────────────────
inline bool cbor_value(cursor& c, std::string& out, int depth);

inline bool cbor_uint_arg(cursor& c, uint8_t info, uint64_t& val) {
    if (info < 24) { val = info; return true; }
    switch (info) {
        case 24: val = c.be(1); return c.ok;
        case 25: val = c.be(2); return c.ok;
        case 26: val = c.be(4); return c.ok;
        case 27: val = c.be(8); return c.ok;
        default: return false; // indefinite / reserved not supported
    }
}

inline bool cbor_string(cursor& c, uint64_t len, std::string& out) {
    if (!c.avail(len)) return false;
    std::string_view s(reinterpret_cast<const char*>(c.p), len);
    json_escape(out, s);
    c.p += len;
    return true;
}

inline bool cbor_value(cursor& c, std::string& out, int depth) {
    if (depth > 64) return false;
    const uint8_t initial = c.u8();
    if (!c.ok) return false;
    const uint8_t major = initial >> 5;
    const uint8_t info = initial & 0x1F;
    switch (major) {
        case 0: { uint64_t v; if (!cbor_uint_arg(c, info, v)) return false; out += std::to_string(v); return true; }
        case 1: { uint64_t v; if (!cbor_uint_arg(c, info, v)) return false; out += "-"; out += std::to_string(v + 1); return true; }
        case 2:
        case 3: { uint64_t len; if (!cbor_uint_arg(c, info, len)) return false; return cbor_string(c, len, out); }
        case 4: {
            uint64_t n; if (!cbor_uint_arg(c, info, n)) return false;
            out.push_back('[');
            for (uint64_t i = 0; i < n; ++i) {
                if (i) out.push_back(',');
                if (!cbor_value(c, out, depth + 1)) return false;
            }
            out.push_back(']');
            return true;
        }
        case 5: {
            uint64_t n; if (!cbor_uint_arg(c, info, n)) return false;
            out.push_back('{');
            for (uint64_t i = 0; i < n; ++i) {
                if (i) out.push_back(',');
                std::string key;
                if (!cbor_value(c, key, depth + 1)) return false;
                if (!key.empty() && key.front() == '"') out += key;          // already a JSON string
                else { out.push_back('"'); out += key; out.push_back('"'); } // stringify non-string key
                out.push_back(':');
                if (!cbor_value(c, out, depth + 1)) return false;
            }
            out.push_back('}');
            return true;
        }
        case 6: { uint64_t tag; if (!cbor_uint_arg(c, info, tag)) return false; return cbor_value(c, out, depth + 1); } // ignore tag
        case 7: {
            switch (info) {
                case 20: out += "false"; return true;
                case 21: out += "true";  return true;
                case 22: case 23: out += "null"; return true;
                case 25: { uint64_t h = c.be(2); if (!c.ok) return false; // half-float
                           uint32_t sign = (h & 0x8000) << 16; uint32_t exp = (h >> 10) & 0x1F; uint32_t man = h & 0x3FF;
                           double d; uint32_t bits;
                           if (exp == 0) { d = std::ldexp(man, -24); if (sign) d = -d; append_double(out, d); return true; }
                           else if (exp == 31) { out += "0"; return true; }
                           else { bits = sign | ((exp + 112) << 23) | (man << 13); float f; std::memcpy(&f, &bits, 4); append_double(out, f); return true; } }
                case 26: { uint32_t b = static_cast<uint32_t>(c.be(4)); if (!c.ok) return false; float f; std::memcpy(&f, &b, 4); append_double(out, f); return true; }
                case 27: { uint64_t b = c.be(8); if (!c.ok) return false; double d; std::memcpy(&d, &b, 8); append_double(out, d); return true; }
                default: return false;
            }
        }
        default: return false;
    }
}

// ── MessagePack ─────────────────────────────────────────────────────────────
inline bool mp_value(cursor& c, std::string& out, int depth);

inline bool mp_string(cursor& c, uint64_t len, std::string& out) {
    if (!c.avail(len)) return false;
    std::string_view s(reinterpret_cast<const char*>(c.p), len);
    json_escape(out, s);
    c.p += len;
    return true;
}
inline bool mp_array(cursor& c, uint64_t n, std::string& out, int depth) {
    out.push_back('[');
    for (uint64_t i = 0; i < n; ++i) { if (i) out.push_back(','); if (!mp_value(c, out, depth + 1)) return false; }
    out.push_back(']');
    return true;
}
inline bool mp_map(cursor& c, uint64_t n, std::string& out, int depth) {
    out.push_back('{');
    for (uint64_t i = 0; i < n; ++i) {
        if (i) out.push_back(',');
        std::string key;
        if (!mp_value(c, key, depth + 1)) return false;
        if (!key.empty() && key.front() == '"') out += key;
        else { out.push_back('"'); out += key; out.push_back('"'); }
        out.push_back(':');
        if (!mp_value(c, out, depth + 1)) return false;
    }
    out.push_back('}');
    return true;
}
inline bool mp_value(cursor& c, std::string& out, int depth) {
    if (depth > 64) return false;
    const uint8_t b = c.u8();
    if (!c.ok) return false;
    if (b <= 0x7F) { out += std::to_string(b); return true; }               // positive fixint
    if (b >= 0xE0) { out += std::to_string(static_cast<int8_t>(b)); return true; } // negative fixint
    if ((b & 0xF0) == 0x80) return mp_map(c, b & 0x0F, out, depth);          // fixmap
    if ((b & 0xF0) == 0x90) return mp_array(c, b & 0x0F, out, depth);        // fixarray
    if ((b & 0xE0) == 0xA0) return mp_string(c, b & 0x1F, out);              // fixstr
    switch (b) {
        case 0xC0: out += "null";  return true;
        case 0xC2: out += "false"; return true;
        case 0xC3: out += "true";  return true;
        case 0xCA: { uint32_t v = static_cast<uint32_t>(c.be(4)); if (!c.ok) return false; float f; std::memcpy(&f, &v, 4); append_double(out, f); return true; }
        case 0xCB: { uint64_t v = c.be(8); if (!c.ok) return false; double d; std::memcpy(&d, &v, 8); append_double(out, d); return true; }
        case 0xCC: out += std::to_string(c.be(1)); return c.ok;
        case 0xCD: out += std::to_string(c.be(2)); return c.ok;
        case 0xCE: out += std::to_string(c.be(4)); return c.ok;
        case 0xCF: out += std::to_string(c.be(8)); return c.ok;
        case 0xD0: out += std::to_string(static_cast<int8_t>(c.be(1))); return c.ok;
        case 0xD1: out += std::to_string(static_cast<int16_t>(c.be(2))); return c.ok;
        case 0xD2: out += std::to_string(static_cast<int32_t>(c.be(4))); return c.ok;
        case 0xD3: out += std::to_string(static_cast<int64_t>(c.be(8))); return c.ok;
        case 0xD9: { uint64_t n = c.be(1); return c.ok && mp_string(c, n, out); }
        case 0xDA: { uint64_t n = c.be(2); return c.ok && mp_string(c, n, out); }
        case 0xDB: { uint64_t n = c.be(4); return c.ok && mp_string(c, n, out); }
        case 0xDC: { uint64_t n = c.be(2); return c.ok && mp_array(c, n, out, depth); }
        case 0xDD: { uint64_t n = c.be(4); return c.ok && mp_array(c, n, out, depth); }
        case 0xDE: { uint64_t n = c.be(2); return c.ok && mp_map(c, n, out, depth); }
        case 0xDF: { uint64_t n = c.be(4); return c.ok && mp_map(c, n, out, depth); }
        case 0xC4: { uint64_t n = c.be(1); return c.ok && mp_string(c, n, out); } // bin as text
        case 0xC5: { uint64_t n = c.be(2); return c.ok && mp_string(c, n, out); }
        case 0xC6: { uint64_t n = c.be(4); return c.ok && mp_string(c, n, out); }
        default: return false;
    }
}

} // namespace detail

inline std::optional<std::string> cbor_to_json(std::string_view bytes) {
    detail::cursor c{reinterpret_cast<const unsigned char*>(bytes.data()),
                     reinterpret_cast<const unsigned char*>(bytes.data()) + bytes.size()};
    std::string out;
    out.reserve(bytes.size() * 2);
    if (!detail::cbor_value(c, out, 0) || !c.ok) return std::nullopt;
    return out;
}

inline std::optional<std::string> msgpack_to_json(std::string_view bytes) {
    detail::cursor c{reinterpret_cast<const unsigned char*>(bytes.data()),
                     reinterpret_cast<const unsigned char*>(bytes.data()) + bytes.size()};
    std::string out;
    out.reserve(bytes.size() * 2);
    if (!detail::mp_value(c, out, 0) || !c.ok) return std::nullopt;
    return out;
}

// Transcode a request body in `fmt` to JSON: json → copy through; cbor/msgpack → decode; unknown →
// nullopt. The result is fed to the generated `parse_<Dto>`. Used by the generated router bindings
// when an operation declares a binary request media type.
inline std::optional<std::string> transcode_to_json(std::string_view body, http::media_format fmt) {
    switch (fmt) {
        case http::media_format::json:
            return std::string(body);
        case http::media_format::cbor:
            return cbor_to_json(body);
        case http::media_format::msgpack:
            return msgpack_to_json(body);
        default:
            return std::nullopt;
    }
}

} // namespace katana::serde
