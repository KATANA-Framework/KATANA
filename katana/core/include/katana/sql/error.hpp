#pragma once

// Typed SQL error codes derived from PostgreSQL SQLSTATE values, so callers can tell a
// retryable transient failure (serialization failure, deadlock) from a permanent one
// (unique/foreign-key violation, syntax error) instead of seeing a single generic io_error.

#include <string>
#include <string_view>
#include <system_error>

namespace katana::sql {

enum class errc {
    ok = 0,
    query_failed = 1,      // generic failure with no SQLSTATE available (send/connect)
    connection_failure,    // 08xxx / lost connection
    serialization_failure, // 40001 — retryable
    deadlock_detected,     // 40P01 — retryable
    unique_violation,      // 23505
    foreign_key_violation, // 23503
    not_null_violation,    // 23502
    check_violation,       // 23514
    integrity_constraint,  // other 23xxx
    syntax_or_access,      // 42xxx
    data_exception,        // 22xxx
};

namespace detail {

class sql_category_impl final : public std::error_category {
public:
    [[nodiscard]] const char* name() const noexcept override { return "katana.sql"; }

    [[nodiscard]] std::string message(int ev) const override {
        switch (static_cast<errc>(ev)) {
        case errc::ok:
            return "ok";
        case errc::query_failed:
            return "query failed";
        case errc::connection_failure:
            return "connection failure";
        case errc::serialization_failure:
            return "serialization failure (retryable)";
        case errc::deadlock_detected:
            return "deadlock detected (retryable)";
        case errc::unique_violation:
            return "unique violation";
        case errc::foreign_key_violation:
            return "foreign key violation";
        case errc::not_null_violation:
            return "not-null violation";
        case errc::check_violation:
            return "check violation";
        case errc::integrity_constraint:
            return "integrity constraint violation";
        case errc::syntax_or_access:
            return "syntax error or access rule violation";
        case errc::data_exception:
            return "data exception";
        }
        return "unknown sql error";
    }
};

} // namespace detail

[[nodiscard]] inline const std::error_category& sql_category() noexcept {
    static const detail::sql_category_impl instance;
    return instance;
}

[[nodiscard]] inline std::error_code make_error_code(errc e) noexcept {
    return {static_cast<int>(e), sql_category()};
}

// Transient failures that are safe to retry (the whole transaction).
[[nodiscard]] inline bool is_retryable(std::error_code ec) noexcept {
    return ec.category() == sql_category() &&
           (ec.value() == static_cast<int>(errc::serialization_failure) ||
            ec.value() == static_cast<int>(errc::deadlock_detected));
}

// Map a 5-character PostgreSQL SQLSTATE (e.g. "40001") to an errc.
// Empty / unknown → query_failed.
[[nodiscard]] inline errc errc_from_sqlstate(std::string_view s) noexcept {
    if (s.size() != 5) {
        return errc::query_failed;
    }
    if (s == "40001") {
        return errc::serialization_failure;
    }
    if (s == "40P01") {
        return errc::deadlock_detected;
    }
    if (s == "23505") {
        return errc::unique_violation;
    }
    if (s == "23503") {
        return errc::foreign_key_violation;
    }
    if (s == "23502") {
        return errc::not_null_violation;
    }
    if (s == "23514") {
        return errc::check_violation;
    }
    // Class-level fallbacks (first two characters).
    const auto cls = s.substr(0, 2);
    if (cls == "23") {
        return errc::integrity_constraint;
    }
    if (cls == "42") {
        return errc::syntax_or_access;
    }
    if (cls == "22") {
        return errc::data_exception;
    }
    if (cls == "08") {
        return errc::connection_failure;
    }
    return errc::query_failed;
}

} // namespace katana::sql

template <>
struct std::is_error_code_enum<katana::sql::errc> : std::true_type {};
