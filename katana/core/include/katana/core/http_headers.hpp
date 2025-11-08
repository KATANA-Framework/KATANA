#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <algorithm>
#include <cctype>

namespace katana::http {

inline bool ci_char_equal(char a, char b) noexcept {
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}

inline bool ci_equal(std::string_view a, std::string_view b) noexcept {
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(), ci_char_equal);
}

class headers_map {
public:
    headers_map() = default;
    headers_map(headers_map&&) noexcept = default;
    headers_map& operator=(headers_map&&) noexcept = default;
    headers_map(const headers_map&) = default;
    headers_map& operator=(const headers_map&) = default;

    void set(std::string name, std::string value) {
        for (auto& [n, v] : headers_) {
            if (ci_equal(n, name)) {
                v = std::move(value);
                return;
            }
        }
        headers_.emplace_back(std::move(name), std::move(value));
    }

    [[nodiscard]] std::optional<std::string_view> get(std::string_view name) const noexcept {
        for (const auto& [n, v] : headers_) {
            if (ci_equal(n, name)) {
                return v;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] bool contains(std::string_view name) const noexcept {
        return get(name).has_value();
    }

    void remove(std::string_view name) {
        headers_.erase(
            std::remove_if(headers_.begin(), headers_.end(),
                [name](const auto& pair) {
                    return ci_equal(pair.first, name);
                }),
            headers_.end()
        );
    }

    void clear() noexcept {
        headers_.clear();
    }

    auto begin() noexcept { return headers_.begin(); }
    auto end() noexcept { return headers_.end(); }
    [[nodiscard]] auto begin() const noexcept { return headers_.begin(); }
    [[nodiscard]] auto end() const noexcept { return headers_.end(); }

    [[nodiscard]] size_t size() const noexcept { return headers_.size(); }
    [[nodiscard]] bool empty() const noexcept { return headers_.empty(); }

private:
    std::vector<std::pair<std::string, std::string>> headers_;
};

} // namespace katana::http
