#include "katana/core/problem.hpp"

#include <charconv>

namespace {

void append_json_escaped(std::string& out, std::string_view value) {
    for (char raw_ch : value) {
        const auto ch = static_cast<unsigned char>(raw_ch);
        switch (ch) {
        case '"':
            out.append("\\\"");
            break;
        case '\\':
            out.append("\\\\");
            break;
        case '\b':
            out.append("\\b");
            break;
        case '\f':
            out.append("\\f");
            break;
        case '\n':
            out.append("\\n");
            break;
        case '\r':
            out.append("\\r");
            break;
        case '\t':
            out.append("\\t");
            break;
        default:
            if (ch < 0x20) {
                char buf[6] = {'\\', 'u', '0', '0', '0', '0'};
                constexpr char hex[] = "0123456789abcdef";
                buf[4] = hex[(ch >> 4) & 0x0F];
                buf[5] = hex[ch & 0x0F];
                out.append(buf, sizeof(buf));
            } else {
                out.push_back(static_cast<char>(ch));
            }
            break;
        }
    }
}

void append_json_string_field(std::string& out,
                              std::string_view key,
                              std::string_view value,
                              bool& first) {
    if (!first) {
        out.push_back(',');
    }
    first = false;
    out.push_back('"');
    out.append(key);
    out.append("\":\"");
    append_json_escaped(out, value);
    out.push_back('"');
}

void append_json_int_field(std::string& out, std::string_view key, int value, bool& first) {
    if (!first) {
        out.push_back(',');
    }
    first = false;
    out.push_back('"');
    out.append(key);
    out.append("\":");
    char buf[16];
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value);
    if (ec == std::errc()) {
        out.append(buf, static_cast<size_t>(ptr - buf));
    } else {
        out.push_back('0');
    }
}

} // namespace

namespace katana {

std::string problem_details::to_json() const {
    size_t reserve = 48 + type.size() + title.size() + extensions.size() * 24;
    if (detail) {
        reserve += detail->size();
    }
    if (instance) {
        reserve += instance->size();
    }
    for (const auto& [key, value] : extensions) {
        reserve += key.size() + value.size();
    }

    std::string out;
    out.reserve(reserve);
    out.push_back('{');

    bool first = true;
    append_json_string_field(out, "type", type, first);
    append_json_string_field(out, "title", title, first);
    append_json_int_field(out, "status", status, first);

    if (detail) {
        append_json_string_field(out, "detail", *detail, first);
    }

    if (instance) {
        append_json_string_field(out, "instance", *instance, first);
    }

    for (const auto& [key, value] : extensions) {
        append_json_string_field(out, key, value, first);
    }

    out.push_back('}');
    return out;
}

problem_details problem_details::bad_request(std::string_view detail) {
    problem_details p;
    p.status = 400;
    p.title = "Bad Request";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::unauthorized(std::string_view detail) {
    problem_details p;
    p.status = 401;
    p.title = "Unauthorized";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::forbidden(std::string_view detail) {
    problem_details p;
    p.status = 403;
    p.title = "Forbidden";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::not_found(std::string_view detail) {
    problem_details p;
    p.status = 404;
    p.title = "Not Found";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::method_not_allowed(std::string_view detail) {
    problem_details p;
    p.status = 405;
    p.title = "Method Not Allowed";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::not_acceptable(std::string_view detail) {
    problem_details p;
    p.status = 406;
    p.title = "Not Acceptable";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::unsupported_media_type(std::string_view detail) {
    problem_details p;
    p.status = 415;
    p.title = "Unsupported Media Type";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::conflict(std::string_view detail) {
    problem_details p;
    p.status = 409;
    p.title = "Conflict";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::unprocessable_entity(std::string_view detail) {
    problem_details p;
    p.status = 422;
    p.title = "Unprocessable Entity";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::internal_server_error(std::string_view detail) {
    problem_details p;
    p.status = 500;
    p.title = "Internal Server Error";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

problem_details problem_details::service_unavailable(std::string_view detail) {
    problem_details p;
    p.status = 503;
    p.title = "Service Unavailable";
    if (!detail.empty()) {
        p.detail = std::string(detail);
    }
    return p;
}

} // namespace katana
