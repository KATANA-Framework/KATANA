#pragma once

#include "katana/sql/runtime.hpp"

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace katana::sql {

#ifdef KATANA_HAS_LIBPQ

struct postgres_config {
    std::string connection_string;
    // When > 0, every connection runs with this server-side `statement_timeout` (ms), so a
    // runaway query is cancelled by Postgres (SQLSTATE 57014) instead of blocking a reactor.
    int statement_timeout_ms = 0;
};

struct postgres_pool_config {
    postgres_config postgres;
    std::size_t executor_count = 0;
    bool eager_connect = false;
};

struct transparent_string_hash {
    using is_transparent = void;

    std::size_t operator()(std::string_view value) const noexcept {
        return std::hash<std::string_view>{}(value);
    }

    std::size_t operator()(const std::string& value) const noexcept {
        return (*this)(std::string_view(value));
    }
};

struct transparent_string_equal {
    using is_transparent = void;

    bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
        return lhs == rhs;
    }
};

class postgres_executor final : public executor, public async_executor {
public:
    explicit postgres_executor(postgres_config config);
    ~postgres_executor() override;

    postgres_executor(const postgres_executor&) = delete;
    postgres_executor& operator=(const postgres_executor&) = delete;

    katana::result<void> connect();
    void disconnect() noexcept;
    [[nodiscard]] bool is_connected() const noexcept;
    [[nodiscard]] std::string_view last_error_message() const noexcept;

    katana::result<rows>
    query(std::string_view statement_name, std::string_view sql, const parameters& params) override;

    katana::result<exec_result>
    exec(std::string_view statement_name, std::string_view sql, const parameters& params) override;

    katana::result<void> query_each(std::string_view statement_name,
                                    std::string_view sql,
                                    const parameters& params,
                                    row_handler handler) override;

    bool query_async(std::string_view statement_name,
                     std::string_view sql,
                     parameters params,
                     async_query_handler handler) override;

    bool exec_async(std::string_view statement_name,
                    std::string_view sql,
                    parameters params,
                    async_exec_handler handler) override;

    katana::result<void> begin();
    katana::result<void> commit();
    katana::result<void> rollback();

private:
    struct impl;

    katana::result<const char*> ensure_prepared(std::string_view statement_name,
                                                std::string_view sql);
    katana::result<void> ensure_connected();
    katana::result<void> run_simple(const char* sql);
    void record_error(std::string message);

    postgres_config config_;
    impl* impl_ = nullptr;
    mutable std::mutex mutex_;
    std::unordered_set<std::string, transparent_string_hash, transparent_string_equal>
        prepared_statements_;
    std::string last_error_;
};

class postgres_transaction {
public:
    explicit postgres_transaction(postgres_executor& executor) noexcept;
    ~postgres_transaction();

    katana::result<void> begin();
    katana::result<void> commit();
    katana::result<void> rollback();

    [[nodiscard]] bool active() const noexcept;

private:
    postgres_executor& executor_;
    bool active_ = false;
};

class postgres_pool {
public:
    explicit postgres_pool(postgres_pool_config config);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] postgres_executor& for_reactor(std::size_t reactor_index) noexcept;
    [[nodiscard]] const postgres_executor& for_reactor(std::size_t reactor_index) const noexcept;
    [[nodiscard]] postgres_executor& current_executor() noexcept;
    [[nodiscard]] const postgres_executor& current_executor() const noexcept;

    katana::result<void> connect_all();
    void disconnect_all() noexcept;

private:
    static std::size_t normalize_count(std::size_t requested) noexcept;

    postgres_pool_config config_;
    std::vector<std::unique_ptr<postgres_executor>> executors_;
    mutable std::atomic<std::size_t> next_executor_{0};
};

class postgres_pool_executor final : public executor, public async_executor {
public:
    explicit postgres_pool_executor(postgres_pool& pool) noexcept : pool_(pool) {}

    katana::result<rows>
    query(std::string_view statement_name, std::string_view sql, const parameters& params) override;

    katana::result<exec_result>
    exec(std::string_view statement_name, std::string_view sql, const parameters& params) override;

    katana::result<void> query_each(std::string_view statement_name,
                                    std::string_view sql,
                                    const parameters& params,
                                    row_handler handler) override;

    bool query_async(std::string_view statement_name,
                     std::string_view sql,
                     parameters params,
                     async_query_handler handler) override;

    bool exec_async(std::string_view statement_name,
                    std::string_view sql,
                    parameters params,
                    async_exec_handler handler) override;

private:
    postgres_pool& pool_;
};

// Readiness probe for server.readiness_check(): runs a lightweight `SELECT 1` on the calling
// reactor's pooled connection. `server.readiness_check(katana::sql::pool_readiness(pool))`.
inline std::function<bool()> pool_readiness(postgres_pool& pool) {
    return [&pool]() {
        return pool.current_executor().query("readyz", "SELECT 1", {}).has_value();
    };
}

// Run `body` inside a transaction on `ex`: BEGIN, run body(), then COMMIT on success or ROLLBACK on
// failure. `body` is a callable returning katana::result<T> (any T, including void); its result is
// returned, and BEGIN/COMMIT failures surface as an error result. Removes the hand-written
// begin/commit/rollback boilerplate every write handler otherwise repeats:
//
//   auto r = katana::sql::transaction(ex, [&]() -> katana::result<void> {
//       KATANA_TRY(repo.update_thing(...));
//       KATANA_TRY(repo.audit(...));
//       return {};
//   });
template <typename Fn>
auto transaction(postgres_executor& ex, Fn&& body) -> std::invoke_result_t<Fn> {
    postgres_transaction txn(ex);
    if (auto begun = txn.begin(); !begun) {
        return std::unexpected(begun.error());
    }
    auto result = std::forward<Fn>(body)();
    if (!result) {
        (void)txn.rollback();
        return result;
    }
    if (auto committed = txn.commit(); !committed) {
        return std::unexpected(committed.error());
    }
    return result;
}

// Convenience overload: run the transaction on the calling reactor's pooled connection.
template <typename Fn>
auto transaction(postgres_pool& pool, Fn&& body) -> std::invoke_result_t<Fn> {
    return transaction(pool.current_executor(), std::forward<Fn>(body));
}

#endif

} // namespace katana::sql
