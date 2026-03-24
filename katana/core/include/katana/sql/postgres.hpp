#pragma once

#include "katana/sql/runtime.hpp"

#include <cstddef>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace katana::sql {

#ifdef KATANA_HAS_LIBPQ

struct postgres_config {
    std::string connection_string;
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

    std::size_t operator()(const std::string& value) const noexcept { return (*this)(std::string_view(value)); }
};

struct transparent_string_equal {
    using is_transparent = void;

    bool operator()(std::string_view lhs, std::string_view rhs) const noexcept { return lhs == rhs; }
};

class postgres_executor final : public executor {
public:
    explicit postgres_executor(postgres_config config);
    ~postgres_executor() override;

    postgres_executor(const postgres_executor&) = delete;
    postgres_executor& operator=(const postgres_executor&) = delete;

    katana::result<void> connect();
    void disconnect() noexcept;
    [[nodiscard]] bool is_connected() const noexcept;
    [[nodiscard]] std::string_view last_error_message() const noexcept;

    katana::result<rows> query(std::string_view statement_name,
                               std::string_view sql,
                               const parameters& params) override;

    katana::result<exec_result> exec(std::string_view statement_name,
                                     std::string_view sql,
                                     const parameters& params) override;

    katana::result<void> query_each(std::string_view statement_name,
                                    std::string_view sql,
                                    const parameters& params,
                                    row_handler handler) override;

    katana::result<void> begin();
    katana::result<void> commit();
    katana::result<void> rollback();

private:
    struct impl;

    katana::result<const char*> ensure_prepared(std::string_view statement_name, std::string_view sql);
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

class postgres_pool_executor final : public executor {
public:
    explicit postgres_pool_executor(postgres_pool& pool) noexcept : pool_(pool) {}

    katana::result<rows> query(std::string_view statement_name,
                               std::string_view sql,
                               const parameters& params) override;

    katana::result<exec_result> exec(std::string_view statement_name,
                                     std::string_view sql,
                                     const parameters& params) override;

    katana::result<void> query_each(std::string_view statement_name,
                                    std::string_view sql,
                                    const parameters& params,
                                    row_handler handler) override;

private:
    postgres_pool& pool_;
};

#endif

} // namespace katana::sql
