#pragma once

#include "katana/core/inplace_function.hpp"
#include "katana/core/result.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

namespace katana::sql {

struct exec_result {
    std::size_t affected_rows = 0;
};

using cell = std::optional<std::string>;

class cell_view {
public:
    cell_view() noexcept = default;
    cell_view(const char* data, std::size_t size) noexcept : data_(data), size_(size) {}
    explicit cell_view(std::string_view value) noexcept
        : data_(value.data()), size_(value.size()) {}

    static cell_view null() noexcept {
        cell_view out;
        out.is_null_ = true;
        return out;
    }

    static cell_view borrowed(std::string_view value) noexcept { return cell_view(value); }

    [[nodiscard]] bool has_value() const noexcept { return !is_null_; }
    [[nodiscard]] explicit operator bool() const noexcept { return has_value(); }
    [[nodiscard]] const char* data() const noexcept { return data_; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] std::string_view value() const noexcept { return std::string_view(data_, size_); }
    [[nodiscard]] std::string_view operator*() const noexcept { return value(); }

private:
    const char* data_ = nullptr;
    std::size_t size_ = 0;
    bool is_null_ = false;
};

class parameter {
public:
    parameter() noexcept = default;
    parameter(const parameter& other) noexcept { copy_from(other); }
    parameter(parameter&& other) noexcept { copy_from(other); }

    parameter& operator=(const parameter& other) noexcept {
        if (this != &other) {
            copy_from(other);
        }
        return *this;
    }

    parameter& operator=(parameter&& other) noexcept {
        if (this != &other) {
            copy_from(other);
        }
        return *this;
    }

    static parameter null() noexcept {
        parameter out;
        out.is_null_ = true;
        return out;
    }

    static parameter borrowed(std::string_view value) noexcept {
        parameter out;
        out.view_ = value;
        return out;
    }

    static parameter from_text(std::string_view value) {
        parameter out;
        if (value.size() < out.storage_.size()) {
            std::copy(value.begin(), value.end(), out.storage_.begin());
            out.view_ = std::string_view(out.storage_.data(), value.size());
            return out;
        }

        out.owned_ = std::string(value);
        out.view_ = std::string_view(out.owned_);
        return out;
    }

    [[nodiscard]] bool has_value() const noexcept { return !is_null_; }
    [[nodiscard]] explicit operator bool() const noexcept { return has_value(); }
    [[nodiscard]] std::string_view value() const noexcept { return view_; }
    [[nodiscard]] const std::string_view& operator*() const noexcept { return view_; }
    [[nodiscard]] const char* data() const noexcept { return is_null_ ? nullptr : view_.data(); }

    template <typename Integer> static parameter from_integer(Integer value) {
        parameter out;
        const auto [ptr, ec] =
            std::to_chars(out.storage_.data(), out.storage_.data() + out.storage_.size(), value);
        if (ec != std::errc{}) {
            return null();
        }
        out.view_ = std::string_view(out.storage_.data(),
                                     static_cast<std::size_t>(ptr - out.storage_.data()));
        return out;
    }

    static parameter from_bool(bool value) noexcept {
        parameter out;
        if (value) {
            out.storage_[0] = 't';
            out.storage_[1] = 'r';
            out.storage_[2] = 'u';
            out.storage_[3] = 'e';
            out.view_ = std::string_view(out.storage_.data(), 4);
        } else {
            out.storage_[0] = 'f';
            out.storage_[1] = 'a';
            out.storage_[2] = 'l';
            out.storage_[3] = 's';
            out.storage_[4] = 'e';
            out.view_ = std::string_view(out.storage_.data(), 5);
        }
        return out;
    }

    template <typename Float> static parameter from_float(Float value) {
        parameter out;
#if defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
        const auto [ptr, ec] = std::to_chars(out.storage_.data(),
                                             out.storage_.data() + out.storage_.size(),
                                             value,
                                             std::chars_format::general);
        if (ec == std::errc{}) {
            out.view_ = std::string_view(out.storage_.data(),
                                         static_cast<std::size_t>(ptr - out.storage_.data()));
            return out;
        }
#endif
        const int written = std::snprintf(
            out.storage_.data(), out.storage_.size(), "%.17g", static_cast<double>(value));
        if (written <= 0 || static_cast<std::size_t>(written) >= out.storage_.size()) {
            return null();
        }
        out.view_ = std::string_view(out.storage_.data(), static_cast<std::size_t>(written));
        return out;
    }

private:
    void copy_from(const parameter& other) noexcept {
        is_null_ = other.is_null_;
        storage_ = other.storage_;
        owned_ = other.owned_;
        if (other.references_inline_storage()) {
            const auto offset =
                static_cast<std::size_t>(other.view_.data() - other.storage_.data());
            view_ = std::string_view(storage_.data() + offset, other.view_.size());
        } else if (other.references_owned_storage()) {
            view_ = std::string_view(owned_);
        } else {
            view_ = other.view_;
        }
    }

    [[nodiscard]] bool references_inline_storage() const noexcept {
        const auto* begin = storage_.data();
        const auto* end = storage_.data() + storage_.size();
        return view_.data() >= begin && view_.data() < end;
    }

    [[nodiscard]] bool references_owned_storage() const noexcept {
        const auto* begin = owned_.data();
        const auto* end = owned_.data() + owned_.size();
        return !owned_.empty() && view_.data() >= begin && view_.data() < end;
    }

    bool is_null_ = false;
    std::array<char, 64> storage_{};
    std::string owned_;
    std::string_view view_{};
};

using parameters = std::vector<parameter>;

struct row_schema {
    std::vector<std::string> column_names;
};

class row {
public:
    row() = default;
    explicit row(std::shared_ptr<row_schema> row_schema_ptr) : schema_(std::move(row_schema_ptr)) {}

    row(std::initializer_list<std::pair<std::string, cell>> fields) {
        schema_ = std::make_shared<row_schema>();
        schema_->column_names.reserve(fields.size());
        values_.reserve(fields.size());
        for (const auto& field : fields) {
            emplace(field.first, field.second);
        }
    }

    void reserve(std::size_t count) { values_.reserve(count); }

    void emplace(std::string name, cell value) {
        ensure_schema();
        schema_->column_names.push_back(std::move(name));
        values_.push_back(std::move(value));
    }

    void push(cell value) { values_.push_back(std::move(value)); }

    [[nodiscard]] const cell& at(std::size_t index) const { return values_.at(index); }

    [[nodiscard]] const std::string* column_name(std::size_t index) const noexcept {
        if (!schema_ || index >= schema_->column_names.size()) {
            return nullptr;
        }
        return &schema_->column_names[index];
    }

    [[nodiscard]] const cell* find(std::string_view key) const noexcept {
        if (!schema_) {
            return nullptr;
        }
        const auto limit = std::min(schema_->column_names.size(), values_.size());
        for (std::size_t index = 0; index < limit; ++index) {
            if (schema_->column_names[index] == key) {
                return &values_[index];
            }
        }
        return nullptr;
    }

    [[nodiscard]] const cell& at(std::string_view key) const {
        if (const auto* value = find(key); value != nullptr) {
            return *value;
        }
        throw std::out_of_range("katana::sql::row::at missing key");
    }

    [[nodiscard]] std::size_t size() const noexcept { return values_.size(); }

private:
    void ensure_schema() {
        if (!schema_) {
            schema_ = std::make_shared<row_schema>();
        }
    }

    std::shared_ptr<row_schema> schema_;
    std::vector<cell> values_;
};

using rows = std::vector<row>;

class row_view {
public:
    row_view() = default;
    row_view(const cell_view* values, std::size_t size) : values_(values), size_(size) {}

    [[nodiscard]] const cell_view& at(std::size_t index) const { return values_[index]; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

private:
    const cell_view* values_ = nullptr;
    std::size_t size_ = 0;
};

using row_handler = katana::inplace_function<katana::result<void>(const row_view&), 128>;

template <typename T> struct is_std_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T> inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

class executor {
public:
    virtual ~executor() = default;

    virtual katana::result<rows>
    query(std::string_view statement_name, std::string_view sql, const parameters& params) = 0;

    virtual katana::result<exec_result>
    exec(std::string_view statement_name, std::string_view sql, const parameters& params) = 0;

    virtual katana::result<void> query_each(std::string_view statement_name,
                                            std::string_view sql,
                                            const parameters& params,
                                            row_handler handler) = 0;
};

inline const cell* find_cell(const row& input, std::string_view key) {
    return input.find(key);
}

inline void append_pg_escaped_string(std::string& out, std::string_view value) {
    out.push_back('"');
    for (char c : value) {
        if (c == '"' || c == '\\') {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    out.push_back('"');
}

template <typename T> inline katana::result<std::string> format_pg_array_element(const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        std::string out;
        out.reserve(value.size() + 2);
        append_pg_escaped_string(out, value);
        return out;
    } else if constexpr (std::is_same_v<T, std::string_view>) {
        std::string out;
        out.reserve(value.size() + 2);
        append_pg_escaped_string(out, value);
        return out;
    } else if constexpr (std::is_same_v<T, const char*>) {
        if (value == nullptr) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        std::string out;
        const std::string_view view(value);
        out.reserve(view.size() + 2);
        append_pg_escaped_string(out, view);
        return out;
    } else if constexpr (std::is_same_v<T, bool>) {
        return value ? std::string("true") : std::string("false");
    } else if constexpr (std::is_integral_v<T>) {
        std::array<char, 64> buffer{};
        const auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        if (ec != std::errc{}) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        return std::string(buffer.data(), static_cast<std::size_t>(ptr - buffer.data()));
    } else if constexpr (std::is_floating_point_v<T>) {
        std::array<char, 128> buffer{};
#if defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
        const auto [ptr, ec] = std::to_chars(buffer.data(),
                                             buffer.data() + buffer.size(),
                                             value,
                                             std::chars_format::general);
        if (ec == std::errc{}) {
            return std::string(buffer.data(), static_cast<std::size_t>(ptr - buffer.data()));
        }
#endif
        const int written =
            std::snprintf(buffer.data(), buffer.size(), "%.17g", static_cast<double>(value));
        if (written <= 0 || static_cast<std::size_t>(written) >= buffer.size()) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        return std::string(buffer.data(), static_cast<std::size_t>(written));
    } else {
        static_assert(sizeof(T) == 0, "format_pg_array_element does not support this type");
    }
}

template <typename T>
inline katana::result<std::string> format_pg_array(const std::vector<T>& values) {
    std::string out;
    out.push_back('{');
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            out.push_back(',');
        }
        const T element = static_cast<T>(values[index]);
        auto encoded = format_pg_array_element<T>(element);
        if (!encoded) {
            return std::unexpected(encoded.error());
        }
        out += *encoded;
    }
    out.push_back('}');
    return out;
}

template <typename Fn>
inline katana::result<void> for_each_pg_array_token(std::string_view raw, Fn&& fn) {
    if (raw.size() < 2 || raw.front() != '{' || raw.back() != '}') {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }

    const std::size_t end = raw.size() - 1;
    std::size_t pos = 1;
    if (pos == end) {
        return {};
    }

    while (pos < end) {
        std::string token;
        if (raw[pos] == '"') {
            ++pos;
            bool closed = false;
            while (pos < end) {
                const char c = raw[pos];
                if (c == '\\') {
                    ++pos;
                    if (pos >= end) {
                        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                    }
                    token.push_back(raw[pos]);
                    ++pos;
                    continue;
                }
                if (c == '"') {
                    ++pos;
                    closed = true;
                    break;
                }
                token.push_back(c);
                ++pos;
            }
            if (!closed) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
        } else {
            const std::size_t token_start = pos;
            while (pos < end && raw[pos] != ',') {
                if (raw[pos] == '{' || raw[pos] == '}' || raw[pos] == '"') {
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                }
                ++pos;
            }
            token.assign(raw.substr(token_start, pos - token_start));
            while (!token.empty() && std::isspace(static_cast<unsigned char>(token.front()))) {
                token.erase(token.begin());
            }
            while (!token.empty() && std::isspace(static_cast<unsigned char>(token.back()))) {
                token.pop_back();
            }
            if (token == "NULL") {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
        }

        auto status = fn(std::string_view(token));
        if (!status) {
            return std::unexpected(status.error());
        }

        if (pos == end) {
            break;
        }
        if (raw[pos] != ',') {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        ++pos;
        while (pos < end && std::isspace(static_cast<unsigned char>(raw[pos]))) {
            ++pos;
        }
    }

    return {};
}

template <typename T> inline parameter encode_value(const T& value) {
    if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
        return parameter::from_text(value);
    } else if constexpr (std::is_same_v<std::decay_t<T>, std::string_view>) {
        return parameter::from_text(value);
    } else if constexpr (std::is_same_v<std::decay_t<T>, const char*>) {
        return value == nullptr ? parameter::null() : parameter::from_text(value);
    } else if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
        return parameter::from_bool(value);
    } else if constexpr (std::is_integral_v<std::decay_t<T>>) {
        return parameter::from_integer(value);
    } else if constexpr (std::is_floating_point_v<std::decay_t<T>>) {
        return parameter::from_float(value);
    } else {
        static_assert(sizeof(T) == 0, "encode_value does not support this type");
    }
}

template <typename T> inline parameter encode_value(const std::vector<T>& value) {
    auto encoded = format_pg_array(value);
    if (!encoded) {
        return parameter::null();
    }
    return parameter::from_text(*encoded);
}

template <typename T> inline parameter encode_value(const std::optional<T>& value) {
    if (!value.has_value()) {
        return parameter::null();
    }
    return encode_value(*value);
}

template <typename T> inline katana::result<T> parse_value(std::string_view raw) {
    if constexpr (std::is_same_v<T, std::string>) {
        return std::string(raw);
    } else if constexpr (is_std_vector_v<T>) {
        T out;
        using element_type = typename T::value_type;
        auto status = for_each_pg_array_token(raw, [&](std::string_view token) -> katana::result<void> {
            auto parsed = parse_value<element_type>(token);
            if (!parsed) {
                return std::unexpected(parsed.error());
            }
            out.push_back(std::move(*parsed));
            return {};
        });
        if (!status) {
            return std::unexpected(status.error());
        }
        return out;
    } else if constexpr (std::is_same_v<T, bool>) {
        if (raw == "true" || raw == "t" || raw == "1") {
            return true;
        }
        if (raw == "false" || raw == "f" || raw == "0") {
            return false;
        }
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    } else if constexpr (std::is_integral_v<T>) {
        T value{};
        const auto* begin = raw.data();
        const auto* end = raw.data() + raw.size();
        auto [ptr, ec] = std::from_chars(begin, end, value);
        if (ec != std::errc{} || ptr != end) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        return value;
    } else if constexpr (std::is_floating_point_v<T>) {
        T value{};
#if defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
        const auto* begin = raw.data();
        const auto* end = raw.data() + raw.size();
        auto [ptr, ec] = std::from_chars(begin, end, value, std::chars_format::general);
        if (ec == std::errc{} && ptr == end && std::isfinite(value)) {
            return value;
        }
#endif
        std::array<char, 128> local_buffer{};
        const char* buffer_begin = nullptr;
        if (raw.size() < local_buffer.size()) {
            std::copy(raw.begin(), raw.end(), local_buffer.begin());
            local_buffer[raw.size()] = '\0';
            buffer_begin = local_buffer.data();
        } else {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        char* parse_end = nullptr;
        errno = 0;
        const double parsed = std::strtod(buffer_begin, &parse_end);
        if (parse_end == nullptr || *parse_end != '\0' || errno == ERANGE ||
            !std::isfinite(parsed)) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        return static_cast<T>(parsed);
    } else {
        static_assert(sizeof(T) == 0, "parse_value does not support this type");
    }
}

template <typename T> inline katana::result<T> parse_value(const cell_view& raw) {
    if (!raw.has_value()) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
    return parse_value<T>(raw.value());
}

} // namespace katana::sql
