#include "katana/sql/postgres.hpp"

#ifdef KATANA_HAS_LIBPQ

#include <libpq-fe.h>

#include <array>
#include <cerrno>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace katana::sql {

struct postgres_executor::impl {
    PGconn* connection = nullptr;
};

namespace {

std::error_code sql_error() {
    return std::make_error_code(std::errc::io_error);
}

class param_values_buffer {
public:
    explicit param_values_buffer(const parameters& params) {
        if (params.size() <= inline_values_.size()) {
            size_ = params.size();
            for (std::size_t index = 0; index < params.size(); ++index) {
                inline_values_[index] = params[index].data();
            }
            values_ = inline_values_.data();
            return;
        }

        heap_values_.reserve(params.size());
        for (const auto& param : params) {
            heap_values_.push_back(param.data());
        }
        size_ = heap_values_.size();
        values_ = heap_values_.data();
    }

    [[nodiscard]] const char* const* data() const noexcept { return values_; }
    [[nodiscard]] int count() const noexcept { return static_cast<int>(size_); }

private:
    std::array<const char*, 8> inline_values_{};
    std::vector<const char*> heap_values_;
    const char* const* values_ = nullptr;
    std::size_t size_ = 0;
};

class cell_view_buffer {
public:
    cell_view_buffer() noexcept = default;
    explicit cell_view_buffer(std::size_t field_count) {
        if (field_count <= inline_values_.size()) {
            values_ = inline_values_.data();
            size_ = field_count;
            return;
        }

        heap_values_.resize(field_count);
        values_ = heap_values_.data();
        size_ = heap_values_.size();
    }

    cell_view_buffer(const cell_view_buffer& other) { copy_from(other); }
    cell_view_buffer(cell_view_buffer&& other) noexcept { copy_from(other); }

    cell_view_buffer& operator=(const cell_view_buffer& other) {
        if (this != &other) {
            copy_from(other);
        }
        return *this;
    }

    cell_view_buffer& operator=(cell_view_buffer&& other) noexcept {
        if (this != &other) {
            copy_from(other);
        }
        return *this;
    }

    [[nodiscard]] cell_view* data() noexcept { return values_; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] row_view view() const noexcept { return row_view(values_, size_); }

private:
    void copy_from(const cell_view_buffer& other) {
        inline_values_ = other.inline_values_;
        heap_values_ = other.heap_values_;
        size_ = other.size_;
        if (!heap_values_.empty()) {
            values_ = heap_values_.data();
        } else {
            values_ = inline_values_.data();
        }
    }

    std::array<cell_view, 16> inline_values_{};
    std::vector<cell_view> heap_values_;
    cell_view* values_ = nullptr;
    std::size_t size_ = 0;
};

param_values_buffer build_param_values(const parameters& params) {
    return param_values_buffer(params);
}

cell_view_buffer build_row_values(std::size_t field_count) {
    return cell_view_buffer(field_count);
}

rows read_rows(PGresult* result) {
    rows out_rows;
    const int tuple_count = PQntuples(result);
    const int field_count = PQnfields(result);
    auto schema = std::make_shared<row_schema>();
    schema->column_names.reserve(static_cast<std::size_t>(field_count));
    for (int field_index = 0; field_index < field_count; ++field_index) {
        const char* name = PQfname(result, field_index);
        if (name != nullptr) {
            schema->column_names.emplace_back(name);
        } else {
            schema->column_names.emplace_back("column_" + std::to_string(field_index + 1));
        }
    }

    out_rows.reserve(static_cast<std::size_t>(tuple_count));
    for (int row_index = 0; row_index < tuple_count; ++row_index) {
        row out(schema);
        out.reserve(static_cast<std::size_t>(field_count));
        for (int field_index = 0; field_index < field_count; ++field_index) {
            if (PQgetisnull(result, row_index, field_index) != 0) {
                out.push(std::nullopt);
            } else {
                out.push(std::string(PQgetvalue(result, row_index, field_index)));
            }
        }
        out_rows.push_back(std::move(out));
    }
    return out_rows;
}

} // namespace

postgres_executor::postgres_executor(postgres_config config)
    : config_(std::move(config)), impl_(new impl()) {}

postgres_executor::~postgres_executor() {
    disconnect();
    delete impl_;
}

katana::result<void> postgres_executor::connect() {
    std::lock_guard<std::mutex> lock(mutex_);
    return ensure_connected();
}

katana::result<void> postgres_executor::ensure_connected() {
    if (is_connected()) {
        return {};
    }
    impl_->connection = PQconnectdb(config_.connection_string.c_str());
    if (impl_->connection == nullptr) {
        record_error("PQconnectdb returned null");
        return std::unexpected(sql_error());
    }
    if (PQstatus(impl_->connection) != CONNECTION_OK) {
        record_error(PQerrorMessage(impl_->connection));
        disconnect();
        return std::unexpected(sql_error());
    }
    last_error_.clear();
    return {};
}

void postgres_executor::disconnect() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (impl_ != nullptr && impl_->connection != nullptr) {
        PQfinish(impl_->connection);
        impl_->connection = nullptr;
    }
    prepared_statements_.clear();
}

bool postgres_executor::is_connected() const noexcept {
    return impl_ != nullptr && impl_->connection != nullptr;
}

std::string_view postgres_executor::last_error_message() const noexcept {
    return last_error_;
}

katana::result<const char*> postgres_executor::ensure_prepared(std::string_view statement_name,
                                                                std::string_view sql) {
    if (const auto it = prepared_statements_.find(statement_name); it != prepared_statements_.end()) {
        return it->c_str();
    }
    auto connected = ensure_connected();
    if (!connected) {
        return std::unexpected(connected.error());
    }

    std::string owned_name(statement_name);
    std::string owned_sql(sql);
    PGresult* result = PQprepare(impl_->connection, owned_name.c_str(), owned_sql.c_str(), 0, nullptr);
    if (result == nullptr) {
        record_error("PQprepare returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        record_error(PQresultErrorMessage(result));
        PQclear(result);
        return std::unexpected(sql_error());
    }

    PQclear(result);
    auto [it, _] = prepared_statements_.insert(std::move(owned_name));
    return it->c_str();
}

katana::result<rows> postgres_executor::query(std::string_view statement_name,
                                              std::string_view sql,
                                              const parameters& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto prepared_name = ensure_prepared(statement_name, sql);
    if (!prepared_name) {
        return std::unexpected(prepared_name.error());
    }

    auto values = build_param_values(params);
    PGresult* result = PQexecPrepared(impl_->connection,
                                      *prepared_name,
                                      values.count(),
                                      values.data(),
                                      nullptr,
                                      nullptr,
                                      0);
    if (result == nullptr) {
        record_error("PQexecPrepared returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK) {
        record_error(PQresultErrorMessage(result));
        PQclear(result);
        return std::unexpected(sql_error());
    }

    auto out_rows = read_rows(result);
    PQclear(result);
    last_error_.clear();
    return out_rows;
}

katana::result<exec_result> postgres_executor::exec(std::string_view statement_name,
                                                    std::string_view sql,
                                                    const parameters& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto prepared_name = ensure_prepared(statement_name, sql);
    if (!prepared_name) {
        return std::unexpected(prepared_name.error());
    }

    auto values = build_param_values(params);
    PGresult* result = PQexecPrepared(impl_->connection,
                                      *prepared_name,
                                      values.count(),
                                      values.data(),
                                      nullptr,
                                      nullptr,
                                      0);
    if (result == nullptr) {
        record_error("PQexecPrepared returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        record_error(PQresultErrorMessage(result));
        PQclear(result);
        return std::unexpected(sql_error());
    }

    exec_result out;
    if (status == PGRES_TUPLES_OK) {
        out.affected_rows = static_cast<std::size_t>(PQntuples(result));
    } else {
        const char* tuples = PQcmdTuples(result);
        if (tuples != nullptr && *tuples != '\0') {
            out.affected_rows = static_cast<std::size_t>(std::strtoull(tuples, nullptr, 10));
        }
    }
    PQclear(result);
    last_error_.clear();
    return out;
}

katana::result<void> postgres_executor::query_each(std::string_view statement_name,
                                                   std::string_view sql,
                                                   const parameters& params,
                                                   row_handler handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto prepared_name = ensure_prepared(statement_name, sql);
    if (!prepared_name) {
        return std::unexpected(prepared_name.error());
    }

    auto values = build_param_values(params);
    PGresult* result = PQexecPrepared(impl_->connection,
                                      *prepared_name,
                                      values.count(),
                                      values.data(),
                                      nullptr,
                                      nullptr,
                                      0);
    if (result == nullptr) {
        record_error("PQexecPrepared returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK) {
        record_error(PQresultErrorMessage(result));
        PQclear(result);
        return std::unexpected(sql_error());
    }

    const int tuple_count = PQntuples(result);
    const int field_count = PQnfields(result);
    auto values_view = build_row_values(static_cast<std::size_t>(field_count));
    for (int row_index = 0; row_index < tuple_count; ++row_index) {
        for (int field_index = 0; field_index < field_count; ++field_index) {
            if (PQgetisnull(result, row_index, field_index) != 0) {
                values_view.data()[static_cast<std::size_t>(field_index)] = cell_view::null();
                continue;
            }
            const char* raw = PQgetvalue(result, row_index, field_index);
            const int raw_len = PQgetlength(result, row_index, field_index);
            values_view.data()[static_cast<std::size_t>(field_index)] =
                cell_view(raw, static_cast<std::size_t>(raw_len));
        }

        auto handled = handler(values_view.view());
        if (!handled) {
            PQclear(result);
            return std::unexpected(handled.error());
        }
    }

    PQclear(result);
    last_error_.clear();
    return {};
}

katana::result<void> postgres_executor::run_simple(const char* sql) {
    auto connected = ensure_connected();
    if (!connected) {
        return connected;
    }

    PGresult* result = PQexec(impl_->connection, sql);
    if (result == nullptr) {
        record_error("PQexec returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        record_error(PQresultErrorMessage(result));
        PQclear(result);
        return std::unexpected(sql_error());
    }

    PQclear(result);
    last_error_.clear();
    return {};
}

katana::result<void> postgres_executor::begin() {
    std::lock_guard<std::mutex> lock(mutex_);
    return run_simple("BEGIN");
}

katana::result<void> postgres_executor::commit() {
    std::lock_guard<std::mutex> lock(mutex_);
    return run_simple("COMMIT");
}

katana::result<void> postgres_executor::rollback() {
    std::lock_guard<std::mutex> lock(mutex_);
    return run_simple("ROLLBACK");
}

void postgres_executor::record_error(std::string message) {
    last_error_ = std::move(message);
    if (const char* debug = std::getenv("KATANA_SQL_DEBUG_ERRORS")) {
        if (*debug != '\0' && *debug != '0') {
            std::cerr << "[katana::sql::postgres] " << last_error_ << '\n';
        }
    }
}

postgres_transaction::postgres_transaction(postgres_executor& executor) noexcept
    : executor_(executor) {}

postgres_transaction::~postgres_transaction() {
    if (active_) {
        (void)executor_.rollback();
    }
}

katana::result<void> postgres_transaction::begin() {
    auto result = executor_.begin();
    if (result) {
        active_ = true;
    }
    return result;
}

katana::result<void> postgres_transaction::commit() {
    auto result = executor_.commit();
    if (result) {
        active_ = false;
    }
    return result;
}

katana::result<void> postgres_transaction::rollback() {
    auto result = executor_.rollback();
    if (result) {
        active_ = false;
    }
    return result;
}

bool postgres_transaction::active() const noexcept {
    return active_;
}

postgres_pool::postgres_pool(postgres_pool_config config) : config_(std::move(config)) {
    const std::size_t count = normalize_count(config_.executor_count);
    executors_.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        executors_.push_back(std::make_unique<postgres_executor>(config_.postgres));
    }
    if (config_.eager_connect) {
        (void)connect_all();
    }
}

std::size_t postgres_pool::size() const noexcept {
    return executors_.size();
}

postgres_executor& postgres_pool::for_reactor(std::size_t reactor_index) noexcept {
    return *executors_[reactor_index % executors_.size()];
}

const postgres_executor& postgres_pool::for_reactor(std::size_t reactor_index) const noexcept {
    return *executors_[reactor_index % executors_.size()];
}

postgres_executor& postgres_pool::current_executor() noexcept {
    static thread_local std::unordered_map<const postgres_pool*, std::size_t> executor_slots;
    const auto it = executor_slots.find(this);
    if (it != executor_slots.end()) {
        return *executors_[it->second];
    }

    const std::size_t slot = next_executor_.fetch_add(1, std::memory_order_relaxed) % executors_.size();
    executor_slots.emplace(this, slot);
    return *executors_[slot];
}

const postgres_executor& postgres_pool::current_executor() const noexcept {
    return const_cast<postgres_pool*>(this)->current_executor();
}

katana::result<void> postgres_pool::connect_all() {
    for (auto& executor : executors_) {
        auto result = executor->connect();
        if (!result) {
            return result;
        }
    }
    return {};
}

void postgres_pool::disconnect_all() noexcept {
    for (auto& executor : executors_) {
        executor->disconnect();
    }
}

std::size_t postgres_pool::normalize_count(std::size_t requested) noexcept {
    return requested == 0 ? 1 : requested;
}

katana::result<rows> postgres_pool_executor::query(std::string_view statement_name,
                                                   std::string_view sql,
                                                   const parameters& params) {
    return pool_.current_executor().query(statement_name, sql, params);
}

katana::result<exec_result> postgres_pool_executor::exec(std::string_view statement_name,
                                                         std::string_view sql,
                                                         const parameters& params) {
    return pool_.current_executor().exec(statement_name, sql, params);
}

katana::result<void> postgres_pool_executor::query_each(std::string_view statement_name,
                                                        std::string_view sql,
                                                        const parameters& params,
                                                        row_handler handler) {
    return pool_.current_executor().query_each(statement_name, sql, params, std::move(handler));
}

} // namespace katana::sql

#endif
