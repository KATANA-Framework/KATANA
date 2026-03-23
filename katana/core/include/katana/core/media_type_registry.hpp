#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>
#include <string_view>

namespace katana::http {

enum class media_format : std::uint8_t {
    unknown = 0,
    json,
    cbor,
    msgpack,
};

struct media_type_entry {
    std::string_view mime_type;
    media_format format = media_format::unknown;
};

namespace detail {

[[nodiscard]] constexpr char ascii_tolower(char ch) noexcept {
    return (ch >= 'A' && ch <= 'Z') ? static_cast<char>(ch - 'A' + 'a') : ch;
}

[[nodiscard]] constexpr bool ascii_iequals(std::string_view lhs, std::string_view rhs) noexcept {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (ascii_tolower(lhs[i]) != ascii_tolower(rhs[i])) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] constexpr std::string_view trim_ascii_ws(std::string_view value) noexcept {
    while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) {
        value.remove_prefix(1);
    }
    while (!value.empty() && (value.back() == ' ' || value.back() == '\t')) {
        value.remove_suffix(1);
    }
    return value;
}

[[nodiscard]] constexpr std::string_view media_type_token(std::string_view value) noexcept {
    value = trim_ascii_ws(value);
    const auto semicolon = value.find(';');
    if (semicolon != std::string_view::npos) {
        value = value.substr(0, semicolon);
    }
    return trim_ascii_ws(value);
}

[[nodiscard]] constexpr bool media_range_matches(std::string_view media_range,
                                                 std::string_view mime_type) noexcept {
    media_range = media_type_token(media_range);
    mime_type = media_type_token(mime_type);
    if (media_range.empty() || mime_type.empty()) {
        return false;
    }
    if (media_range == "*/*") {
        return true;
    }
    if (media_range.size() > 2 && media_range.substr(media_range.size() - 2) == "/*") {
        const auto slash = mime_type.find('/');
        if (slash == std::string_view::npos) {
            return false;
        }
        return ascii_iequals(media_range.substr(0, media_range.size() - 1),
                             mime_type.substr(0, slash + 1));
    }
    return ascii_iequals(media_range, mime_type);
}

[[nodiscard]] constexpr int media_range_specificity(std::string_view media_range) noexcept {
    media_range = media_type_token(media_range);
    if (media_range.empty()) {
        return -1;
    }
    if (media_range == "*/*") {
        return 0;
    }
    if (media_range.size() > 2 && media_range.substr(media_range.size() - 2) == "/*") {
        return 1;
    }
    return 2;
}

[[nodiscard]] constexpr std::uint16_t parse_qvalue_millis(std::string_view value) noexcept {
    value = trim_ascii_ws(value);
    if (value.empty()) {
        return 0;
    }
    if (value == "1") {
        return 1000;
    }
    if (value.front() == '1') {
        value.remove_prefix(1);
        if (value.empty()) {
            return 1000;
        }
        if (value.front() != '.') {
            return 0;
        }
        value.remove_prefix(1);
        if (value.size() > 3) {
            return 0;
        }
        for (char ch : value) {
            if (ch != '0') {
                return 0;
            }
        }
        return 1000;
    }
    if (value.front() != '0') {
        return 0;
    }
    value.remove_prefix(1);
    if (value.empty()) {
        return 0;
    }
    if (value.front() != '.') {
        return 0;
    }
    value.remove_prefix(1);
    if (value.size() > 3) {
        return 0;
    }

    std::uint16_t millis = 0;
    std::uint16_t multiplier = 100;
    for (char ch : value) {
        if (ch < '0' || ch > '9') {
            return 0;
        }
        millis =
            static_cast<std::uint16_t>(millis + static_cast<std::uint16_t>(ch - '0') * multiplier);
        multiplier = static_cast<std::uint16_t>(multiplier / 10);
    }
    return millis;
}

struct accept_score {
    std::uint16_t q_millis = 0;
    int specificity = -1;
    bool matched = false;
};

[[nodiscard]] inline accept_score score_accept_for_mime(std::string_view accept_header,
                                                        std::string_view mime_type) noexcept {
    accept_header = trim_ascii_ws(accept_header);
    mime_type = media_type_token(mime_type);
    if (mime_type.empty()) {
        return {};
    }
    if (accept_header.empty() || accept_header == "*/*") {
        return {.q_millis = 1000, .specificity = 0, .matched = true};
    }

    accept_score best{};
    std::string_view remaining = accept_header;
    while (!remaining.empty()) {
        const auto comma = remaining.find(',');
        auto item = comma == std::string_view::npos ? remaining : remaining.substr(0, comma);
        remaining =
            comma == std::string_view::npos ? std::string_view{} : remaining.substr(comma + 1);
        item = trim_ascii_ws(item);
        if (item.empty()) {
            continue;
        }

        auto media_range = item;
        std::uint16_t q_millis = 1000;
        const auto semicolon = item.find(';');
        if (semicolon != std::string_view::npos) {
            media_range = trim_ascii_ws(item.substr(0, semicolon));
            auto params = item.substr(semicolon + 1);
            while (!params.empty()) {
                const auto next_semicolon = params.find(';');
                auto param = next_semicolon == std::string_view::npos
                                 ? params
                                 : params.substr(0, next_semicolon);
                params = next_semicolon == std::string_view::npos
                             ? std::string_view{}
                             : params.substr(next_semicolon + 1);

                param = trim_ascii_ws(param);
                const auto equals = param.find('=');
                if (equals == std::string_view::npos) {
                    continue;
                }
                const auto name = trim_ascii_ws(param.substr(0, equals));
                if (!ascii_iequals(name, "q")) {
                    continue;
                }
                q_millis = parse_qvalue_millis(param.substr(equals + 1));
            }
        }

        const int specificity = media_range_specificity(media_range);
        if (specificity < 0 || q_millis == 0 || !media_range_matches(media_range, mime_type)) {
            continue;
        }

        if (!best.matched || q_millis > best.q_millis ||
            (q_millis == best.q_millis && specificity > best.specificity)) {
            best = {.q_millis = q_millis, .specificity = specificity, .matched = true};
        }
    }

    return best;
}

template <typename Candidate, typename GetMime>
[[nodiscard]] inline const Candidate* negotiate_accept(std::string_view accept_header,
                                                       std::span<const Candidate> candidates,
                                                       GetMime get_mime) noexcept {
    if (candidates.empty()) {
        return nullptr;
    }

    const Candidate* best_candidate = nullptr;
    accept_score best_score{};
    for (const auto& candidate : candidates) {
        const auto score = score_accept_for_mime(accept_header, get_mime(candidate));
        if (!score.matched) {
            continue;
        }
        if (!best_candidate || score.q_millis > best_score.q_millis ||
            (score.q_millis == best_score.q_millis && score.specificity > best_score.specificity)) {
            best_candidate = &candidate;
            best_score = score;
        }
    }
    return best_candidate;
}

} // namespace detail

[[nodiscard]] constexpr media_format infer_media_format(std::string_view mime_type) noexcept {
    mime_type = detail::media_type_token(mime_type);
    if (detail::ascii_iequals(mime_type, "application/json")) {
        return media_format::json;
    }
    if (detail::ascii_iequals(mime_type, "application/cbor")) {
        return media_format::cbor;
    }
    if (detail::ascii_iequals(mime_type, "application/msgpack") ||
        detail::ascii_iequals(mime_type, "application/x-msgpack")) {
        return media_format::msgpack;
    }
    return media_format::unknown;
}

class media_type_registry {
public:
    static constexpr size_t max_entries = 16;
    static constexpr size_t max_mime_length = 96;

    constexpr media_type_registry() = default;

    [[nodiscard]] constexpr bool register_type(media_type_entry entry) noexcept {
        entry.mime_type = detail::media_type_token(entry.mime_type);
        if (entry.mime_type.empty()) {
            return false;
        }
        if (entry.format == media_format::unknown) {
            entry.format = infer_media_format(entry.mime_type);
        }
        if (find(entry.mime_type) != nullptr || size_ >= entries_.size() ||
            entry.mime_type.size() > max_mime_length) {
            return false;
        }
        for (size_t i = 0; i < entry.mime_type.size(); ++i) {
            mime_storage_[size_][i] = entry.mime_type[i];
        }
        mime_storage_[size_][entry.mime_type.size()] = '\0';
        entry.mime_type = std::string_view(mime_storage_[size_].data(), entry.mime_type.size());
        entries_[size_] = entry;
        ++size_;
        return true;
    }

    [[nodiscard]] constexpr const media_type_entry*
    find(std::string_view mime_type) const noexcept {
        mime_type = detail::media_type_token(mime_type);
        if (mime_type.empty()) {
            return nullptr;
        }
        for (size_t i = 0; i < size_; ++i) {
            if (detail::ascii_iequals(entries_[i].mime_type, mime_type)) {
                return &entries_[i];
            }
        }
        return nullptr;
    }

    [[nodiscard]] inline const media_type_entry*
    negotiate(std::string_view accept_header) const noexcept {
        return detail::negotiate_accept(
            accept_header, all(), [](const media_type_entry& entry) { return entry.mime_type; });
    }

    [[nodiscard]] constexpr std::span<const media_type_entry> all() const noexcept {
        return {entries_.data(), size_};
    }

    [[nodiscard]] static const media_type_registry& default_instance() noexcept {
        static const media_type_registry registry = [] {
            media_type_registry instance;
            (void)instance.register_type({"application/json", media_format::json});
            (void)instance.register_type({"application/cbor", media_format::cbor});
            (void)instance.register_type({"application/msgpack", media_format::msgpack});
            (void)instance.register_type({"application/x-msgpack", media_format::msgpack});
            return instance;
        }();
        return registry;
    }

private:
    std::array<media_type_entry, max_entries> entries_{};
    std::array<std::array<char, max_mime_length + 1>, max_entries> mime_storage_{};
    size_t size_ = 0;
};

} // namespace katana::http
