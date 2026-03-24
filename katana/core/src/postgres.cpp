#include "katana/sql/postgres.hpp"

#ifdef KATANA_HAS_LIBPQ

#include "katana/core/fd_watch.hpp"
#include "katana/core/handler_context.hpp"
#include "katana/core/reactor.hpp"

#include <libpq-fe.h>

#include <array>
#include <cerrno>
#include <condition_variable>
#include <cstdlib>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace katana::sql {

namespace {

constexpr std::size_t async_queue_limit = 1024;
using prepared_statement_set =
    std::unordered_set<std::string, transparent_string_hash, transparent_string_equal>;

struct async_request {
    enum class kind { query, exec };

    kind operation = kind::query;
    std::string statement_name;
    std::string sql;
    parameters params;
    async_query_handler query_handler;
    async_exec_handler exec_handler;
};

struct reactor_async_state : std::enable_shared_from_this<reactor_async_state> {
    enum class phase { idle, connecting, preparing, executing };

    PGconn* connection = nullptr;
    reactor* owner_reactor = nullptr;
    std::thread::id owner_thread_id{};
    std::string connection_string;
    std::mutex mutex;
    std::optional<fd_watch> watch;
    event_type watch_events = event_type::none;
    std::deque<async_request> queue;
    std::optional<async_request> active_request;
    phase current_phase = phase::idle;
    bool flush_pending = false;
    bool stopping = false;
    prepared_statement_set prepared_statements;
    std::string last_error;
};

void maybe_debug_log(std::string_view message) {
    if (const char* debug = std::getenv("KATANA_SQL_DEBUG_ERRORS")) {
        if (*debug != '\0' && *debug != '0') {
            std::cerr << "[katana::sql::postgres] " << message << '\n';
        }
    }
}

std::error_code sql_error() {
    return std::make_error_code(std::errc::io_error);
}

void record_error_message(std::string& target, std::string message) {
    target = std::move(message);
    maybe_debug_log(target);
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

katana::result<void>
ensure_connected_impl(PGconn*& connection, std::string_view connection_string, std::string& error) {
    if (connection != nullptr) {
        return {};
    }

    connection = PQconnectdb(std::string(connection_string).c_str());
    if (connection == nullptr) {
        record_error_message(error, "PQconnectdb returned null");
        return std::unexpected(sql_error());
    }
    if (PQstatus(connection) != CONNECTION_OK) {
        record_error_message(error, PQerrorMessage(connection));
        PQfinish(connection);
        connection = nullptr;
        return std::unexpected(sql_error());
    }

    error.clear();
    return {};
}

katana::result<const char*> ensure_prepared_impl(PGconn*& connection,
                                                 prepared_statement_set& prepared_statements,
                                                 std::string_view connection_string,
                                                 std::string_view statement_name,
                                                 std::string_view sql,
                                                 std::string& error) {
    if (const auto it = prepared_statements.find(statement_name); it != prepared_statements.end()) {
        return it->c_str();
    }

    auto connected = ensure_connected_impl(connection, connection_string, error);
    if (!connected) {
        return std::unexpected(connected.error());
    }

    std::string owned_name(statement_name);
    std::string owned_sql(sql);
    PGresult* result = PQprepare(connection, owned_name.c_str(), owned_sql.c_str(), 0, nullptr);
    if (result == nullptr) {
        record_error_message(error, "PQprepare returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        record_error_message(error, PQresultErrorMessage(result));
        PQclear(result);
        return std::unexpected(sql_error());
    }

    PQclear(result);
    auto [it, _] = prepared_statements.insert(std::move(owned_name));
    error.clear();
    return it->c_str();
}

katana::result<rows>
execute_query_impl(PGconn* connection, const char* prepared_name, const parameters& params, std::string& error) {
    auto values = build_param_values(params);
    PGresult* result =
        PQexecPrepared(connection, prepared_name, values.count(), values.data(), nullptr, nullptr, 0);
    if (result == nullptr) {
        record_error_message(error, "PQexecPrepared returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK) {
        record_error_message(error, PQresultErrorMessage(result));
        PQclear(result);
        return std::unexpected(sql_error());
    }

    auto out_rows = read_rows(result);
    PQclear(result);
    error.clear();
    return out_rows;
}

katana::result<exec_result>
execute_exec_impl(PGconn* connection, const char* prepared_name, const parameters& params, std::string& error) {
    auto values = build_param_values(params);
    PGresult* result =
        PQexecPrepared(connection, prepared_name, values.count(), values.data(), nullptr, nullptr, 0);
    if (result == nullptr) {
        record_error_message(error, "PQexecPrepared returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        record_error_message(error, PQresultErrorMessage(result));
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
    error.clear();
    return out;
}

katana::result<void> execute_query_each_impl(PGconn* connection,
                                             const char* prepared_name,
                                             const parameters& params,
                                             row_handler handler,
                                             std::string& error) {
    auto values = build_param_values(params);
    PGresult* result =
        PQexecPrepared(connection, prepared_name, values.count(), values.data(), nullptr, nullptr, 0);
    if (result == nullptr) {
        record_error_message(error, "PQexecPrepared returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK) {
        record_error_message(error, PQresultErrorMessage(result));
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
    error.clear();
    return {};
}

katana::result<void>
run_simple_impl(PGconn*& connection, std::string_view connection_string, const char* sql, std::string& error) {
    auto connected = ensure_connected_impl(connection, connection_string, error);
    if (!connected) {
        return connected;
    }

    PGresult* result = PQexec(connection, sql);
    if (result == nullptr) {
        record_error_message(error, "PQexec returned null");
        return std::unexpected(sql_error());
    }

    const auto status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        record_error_message(error, PQresultErrorMessage(result));
        PQclear(result);
        return std::unexpected(sql_error());
    }

    PQclear(result);
    error.clear();
    return {};
}

reactor* current_handler_reactor() noexcept {
    auto* ctx = http::try_ctx();
    if (ctx == nullptr || ctx->reactor_user == nullptr) {
        return nullptr;
    }
    return static_cast<reactor*>(ctx->reactor_user);
}

void close_reactor_async_connection(reactor_async_state& state) {
    state.watch.reset();
    state.watch_events = event_type::none;
    state.current_phase = reactor_async_state::phase::idle;
    state.flush_pending = false;
    state.prepared_statements.clear();
    if (state.connection != nullptr) {
        PQfinish(state.connection);
        state.connection = nullptr;
    }
}

struct reactor_request_completion {
    async_request request;
    katana::result<rows> query_result = std::unexpected(sql_error());
    katana::result<exec_result> exec_result_value = std::unexpected(sql_error());
};

enum class reactor_phase_poll_result { waiting, continue_now, failed };

struct reactor_command_poll_result {
    enum class status { waiting, ready, failed };

    status state = status::waiting;
    PGresult* result = nullptr;
};

void drive_reactor_async_queue(const std::shared_ptr<reactor_async_state>& state,
                               event_type ready_events = event_type::none);

bool ensure_reactor_watch(reactor_async_state& state, event_type events) {
    if (state.connection == nullptr || state.owner_reactor == nullptr) {
        return false;
    }

    const auto fd = PQsocket(state.connection);
    if (fd < 0) {
        record_error_message(state.last_error, "PQsocket returned invalid fd");
        return false;
    }

    if (state.watch && state.watch->is_registered()) {
        if (state.watch_events == events) {
            return true;
        }
        const auto modified = state.watch->modify(events);
        if (!modified) {
            record_error_message(state.last_error, modified.error().message());
            state.watch.reset();
            state.watch_events = event_type::none;
            return false;
        }
        state.watch_events = events;
        return true;
    }

    auto weak_state = std::weak_ptr<reactor_async_state>(state.shared_from_this());
    fd_watch watch(*state.owner_reactor,
                   fd,
                   events,
                   [weak_state](event_type ready_events) {
                       if (auto locked = weak_state.lock()) {
                           drive_reactor_async_queue(locked, ready_events);
                       }
                   });
    if (!watch.is_registered()) {
        record_error_message(state.last_error, "failed to register PostgreSQL socket with reactor");
        return false;
    }
    state.watch.emplace(std::move(watch));
    state.watch_events = events;
    return true;
}

bool start_reactor_connect_locked(reactor_async_state& state) {
    if (state.connection != nullptr) {
        return true;
    }

    state.connection = PQconnectStart(state.connection_string.c_str());
    if (state.connection == nullptr) {
        record_error_message(state.last_error, "PQconnectStart returned null");
        return false;
    }
    if (PQstatus(state.connection) == CONNECTION_BAD) {
        record_error_message(state.last_error, PQerrorMessage(state.connection));
        PQfinish(state.connection);
        state.connection = nullptr;
        return false;
    }
    if (PQsetnonblocking(state.connection, 1) != 0) {
        record_error_message(state.last_error, PQerrorMessage(state.connection));
        PQfinish(state.connection);
        state.connection = nullptr;
        return false;
    }

    state.current_phase = reactor_async_state::phase::connecting;
    state.flush_pending = false;
    return true;
}

bool start_reactor_prepare_locked(reactor_async_state& state) {
    if (!state.active_request.has_value() || state.connection == nullptr) {
        record_error_message(state.last_error, "reactor prepare requested without active connection");
        return false;
    }

    if (PQsendPrepare(state.connection,
                      state.active_request->statement_name.c_str(),
                      state.active_request->sql.c_str(),
                      0,
                      nullptr) == 0) {
        record_error_message(state.last_error, PQerrorMessage(state.connection));
        return false;
    }

    state.current_phase = reactor_async_state::phase::preparing;
    const int flush_status = PQflush(state.connection);
    if (flush_status < 0) {
        record_error_message(state.last_error, PQerrorMessage(state.connection));
        return false;
    }

    state.flush_pending = flush_status == 1;
    const auto watch_events =
        state.flush_pending ? (event_type::readable | event_type::writable) : event_type::readable;
    return ensure_reactor_watch(state, watch_events);
}

bool start_reactor_execute_locked(reactor_async_state& state) {
    if (!state.active_request.has_value() || state.connection == nullptr) {
        record_error_message(state.last_error, "reactor exec requested without active connection");
        return false;
    }

    auto values = build_param_values(state.active_request->params);
    if (PQsendQueryPrepared(state.connection,
                            state.active_request->statement_name.c_str(),
                            values.count(),
                            values.data(),
                            nullptr,
                            nullptr,
                            0) == 0) {
        record_error_message(state.last_error, PQerrorMessage(state.connection));
        return false;
    }

    state.current_phase = reactor_async_state::phase::executing;
    const int flush_status = PQflush(state.connection);
    if (flush_status < 0) {
        record_error_message(state.last_error, PQerrorMessage(state.connection));
        return false;
    }

    state.flush_pending = flush_status == 1;
    const auto watch_events =
        state.flush_pending ? (event_type::readable | event_type::writable) : event_type::readable;
    return ensure_reactor_watch(state, watch_events);
}

reactor_phase_poll_result
poll_connect_phase_locked(reactor_async_state& state, event_type ready_events) {
    if (state.connection == nullptr) {
        record_error_message(state.last_error, "reactor connect poll without connection");
        return reactor_phase_poll_result::failed;
    }

    if (ready_events != event_type::none && !has_flag(ready_events, event_type::readable) &&
        !has_flag(ready_events, event_type::writable)) {
        return reactor_phase_poll_result::waiting;
    }

    for (;;) {
        switch (PQconnectPoll(state.connection)) {
        case PGRES_POLLING_READING:
            if (!ensure_reactor_watch(state, event_type::readable)) {
                return reactor_phase_poll_result::failed;
            }
            return reactor_phase_poll_result::waiting;
        case PGRES_POLLING_WRITING:
            if (!ensure_reactor_watch(state, event_type::writable)) {
                return reactor_phase_poll_result::failed;
            }
            return reactor_phase_poll_result::waiting;
        case PGRES_POLLING_OK:
            state.current_phase = reactor_async_state::phase::idle;
            state.last_error.clear();
            return reactor_phase_poll_result::continue_now;
        case PGRES_POLLING_ACTIVE:
            continue;
        case PGRES_POLLING_FAILED:
            record_error_message(state.last_error, PQerrorMessage(state.connection));
            return reactor_phase_poll_result::failed;
        }
    }
}

reactor_command_poll_result
poll_command_phase_locked(reactor_async_state& state, event_type ready_events) {
    if (state.connection == nullptr) {
        record_error_message(state.last_error, "reactor command poll without connection");
        return {.state = reactor_command_poll_result::status::failed};
    }

    if (state.flush_pending) {
        if (ready_events != event_type::none && !has_flag(ready_events, event_type::writable)) {
            return {.state = reactor_command_poll_result::status::waiting};
        }

        const int flush_status = PQflush(state.connection);
        if (flush_status < 0) {
            record_error_message(state.last_error, PQerrorMessage(state.connection));
            return {.state = reactor_command_poll_result::status::failed};
        }

        state.flush_pending = flush_status == 1;
        const auto watch_events =
            state.flush_pending ? (event_type::readable | event_type::writable)
                                : event_type::readable;
        if (!ensure_reactor_watch(state, watch_events)) {
            return {.state = reactor_command_poll_result::status::failed};
        }
        if (state.flush_pending) {
            return {.state = reactor_command_poll_result::status::waiting};
        }
    }

    if (PQconsumeInput(state.connection) == 0) {
        record_error_message(state.last_error, PQerrorMessage(state.connection));
        return {.state = reactor_command_poll_result::status::failed};
    }

    if (PQisBusy(state.connection) != 0) {
        if (!ensure_reactor_watch(state, event_type::readable)) {
            return {.state = reactor_command_poll_result::status::failed};
        }
        return {.state = reactor_command_poll_result::status::waiting};
    }

    PGresult* last_result = nullptr;
    while (PGresult* next_result = PQgetResult(state.connection)) {
        if (last_result != nullptr) {
            PQclear(last_result);
        }
        last_result = next_result;
    }

    if (last_result == nullptr) {
        record_error_message(state.last_error, "PQgetResult returned no result");
        return {.state = reactor_command_poll_result::status::failed};
    }

    return {.state = reactor_command_poll_result::status::ready, .result = last_result};
}

bool start_reactor_request_locked(reactor_async_state& state) {
    if (state.active_request.has_value()) {
        return true;
    }

    if (state.queue.empty()) {
        state.watch.reset();
        state.watch_events = event_type::none;
        return true;
    }

    state.active_request = std::move(state.queue.front());
    state.queue.pop_front();
    state.current_phase = reactor_async_state::phase::idle;
    state.flush_pending = false;
    return true;
}

std::optional<reactor_request_completion>
poll_reactor_request_locked(reactor_async_state& state, event_type ready_events) {
    if (!state.active_request.has_value() || state.connection == nullptr) {
        if (!state.active_request.has_value()) {
            return std::nullopt;
        }
    }

    auto fail_active_request = [&]() -> std::optional<reactor_request_completion> {
        reactor_request_completion completion;
        completion.request = std::move(*state.active_request);
        state.active_request.reset();
        close_reactor_async_connection(state);
        if (completion.request.operation == async_request::kind::query) {
            completion.query_result = std::unexpected(sql_error());
        } else {
            completion.exec_result_value = std::unexpected(sql_error());
        }
        return completion;
    };

    if (has_flag(ready_events, event_type::error) || has_flag(ready_events, event_type::hup)) {
        if (state.connection != nullptr) {
            record_error_message(state.last_error, PQerrorMessage(state.connection));
        } else {
            record_error_message(state.last_error, "reactor PostgreSQL socket closed");
        }
        return fail_active_request();
    }

    for (;;) {
        switch (state.current_phase) {
        case reactor_async_state::phase::idle:
            if (state.connection == nullptr) {
                if (!start_reactor_connect_locked(state)) {
                    return fail_active_request();
                }
                continue;
            }

            if (state.prepared_statements.find(state.active_request->statement_name) ==
                state.prepared_statements.end()) {
                if (!start_reactor_prepare_locked(state)) {
                    return fail_active_request();
                }
            } else {
                if (!start_reactor_execute_locked(state)) {
                    return fail_active_request();
                }
            }
            continue;

        case reactor_async_state::phase::connecting: {
            const auto connect_result = poll_connect_phase_locked(state, ready_events);
            if (connect_result == reactor_phase_poll_result::waiting) {
                return std::nullopt;
            }
            if (connect_result == reactor_phase_poll_result::failed) {
                return fail_active_request();
            }
            ready_events = event_type::none;
            continue;
        }

        case reactor_async_state::phase::preparing: {
            auto poll_result = poll_command_phase_locked(state, ready_events);
            if (poll_result.state == reactor_command_poll_result::status::waiting) {
                return std::nullopt;
            }
            if (poll_result.state == reactor_command_poll_result::status::failed) {
                return fail_active_request();
            }

            std::unique_ptr<PGresult, decltype(&PQclear)> result_holder(poll_result.result, &PQclear);
            if (PQresultStatus(result_holder.get()) != PGRES_COMMAND_OK) {
                record_error_message(state.last_error, PQresultErrorMessage(result_holder.get()));
                return fail_active_request();
            }

            state.prepared_statements.insert(state.active_request->statement_name);
            state.current_phase = reactor_async_state::phase::idle;
            state.last_error.clear();
            ready_events = event_type::none;
            continue;
        }

        case reactor_async_state::phase::executing: {
            auto poll_result = poll_command_phase_locked(state, ready_events);
            if (poll_result.state == reactor_command_poll_result::status::waiting) {
                return std::nullopt;
            }
            if (poll_result.state == reactor_command_poll_result::status::failed) {
                return fail_active_request();
            }

            std::unique_ptr<PGresult, decltype(&PQclear)> result_holder(poll_result.result, &PQclear);
            reactor_request_completion completion;
            completion.request = std::move(*state.active_request);
            state.active_request.reset();
            state.current_phase = reactor_async_state::phase::idle;

            const auto status = PQresultStatus(result_holder.get());
            if (completion.request.operation == async_request::kind::query) {
                if (status != PGRES_TUPLES_OK) {
                    record_error_message(state.last_error, PQresultErrorMessage(result_holder.get()));
                    completion.query_result = std::unexpected(sql_error());
                    close_reactor_async_connection(state);
                } else {
                    completion.query_result = read_rows(result_holder.get());
                    state.last_error.clear();
                }
            } else {
                if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
                    record_error_message(state.last_error, PQresultErrorMessage(result_holder.get()));
                    completion.exec_result_value = std::unexpected(sql_error());
                    close_reactor_async_connection(state);
                } else {
                    exec_result out;
                    if (status == PGRES_TUPLES_OK) {
                        out.affected_rows = static_cast<std::size_t>(PQntuples(result_holder.get()));
                    } else {
                        const char* tuples = PQcmdTuples(result_holder.get());
                        if (tuples != nullptr && *tuples != '\0') {
                            out.affected_rows =
                                static_cast<std::size_t>(std::strtoull(tuples, nullptr, 10));
                        }
                    }
                    completion.exec_result_value = out;
                    state.last_error.clear();
                }
            }

            if (!state.active_request.has_value() && state.queue.empty()) {
                state.watch.reset();
                state.watch_events = event_type::none;
            }
            return completion;
        }
        }
    }
}

void invoke_reactor_completion(reactor_request_completion completion) {
    if (completion.request.operation == async_request::kind::query) {
        completion.request.query_handler(std::move(completion.query_result));
        return;
    }
    completion.request.exec_handler(std::move(completion.exec_result_value));
}

void drive_reactor_async_queue(const std::shared_ptr<reactor_async_state>& state,
                               event_type ready_events) {
    event_type pending_events = ready_events;
    for (;;) {
        std::optional<reactor_request_completion> completion;
        bool waiting_for_reactor = false;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (state->stopping) {
                return;
            }

            if (!state->active_request.has_value()) {
                if (!start_reactor_request_locked(*state)) {
                    reactor_request_completion failed;
                    if (state->active_request.has_value()) {
                        failed.request = std::move(*state->active_request);
                        state->active_request.reset();
                    } else if (!state->queue.empty()) {
                        failed.request = std::move(state->queue.front());
                        state->queue.pop_front();
                    } else {
                        return;
                    }
                    close_reactor_async_connection(*state);
                    if (failed.request.operation == async_request::kind::query) {
                        failed.query_result = std::unexpected(sql_error());
                    } else {
                        failed.exec_result_value = std::unexpected(sql_error());
                    }
                    completion = std::move(failed);
                } else if (!state->active_request.has_value()) {
                    return;
                }
            }

            if (!completion.has_value()) {
                completion = poll_reactor_request_locked(*state, pending_events);
            }

            waiting_for_reactor = !completion.has_value() && state->active_request.has_value();
        }

        if (!completion.has_value()) {
            if (waiting_for_reactor) {
                return;
            }
            pending_events = event_type::none;
            continue;
        }

        invoke_reactor_completion(std::move(*completion));
        pending_events = event_type::none;
    }
}

} // namespace

struct postgres_executor::impl {
    struct async_state {
        PGconn* connection = nullptr;
        std::mutex mutex;
        std::condition_variable cv;
        std::deque<async_request> queue;
        std::thread worker;
        bool stopping = false;
        bool worker_started = false;
        prepared_statement_set prepared_statements;
        std::string last_error;
    };

    static bool enqueue_reactor_async(const std::shared_ptr<reactor_async_state>& state,
                                      async_request request,
                                      std::string_view connection_string,
                                      reactor& owner_reactor) {
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (state->stopping) {
                return false;
            }
            if (state->owner_reactor == nullptr) {
                state->owner_reactor = &owner_reactor;
                state->owner_thread_id = std::this_thread::get_id();
                state->connection_string = std::string(connection_string);
            } else if (state->owner_reactor != &owner_reactor) {
                return false;
            }

            const std::size_t pending_count =
                state->queue.size() + (state->active_request.has_value() ? 1U : 0U);
            if (pending_count >= async_queue_limit) {
                return false;
            }
            state->queue.push_back(std::move(request));
        }

        drive_reactor_async_queue(state);
        return true;
    }

    static void shutdown_reactor_async(const std::shared_ptr<reactor_async_state>& state) noexcept {
        reactor* owner_reactor = nullptr;
        bool on_owner_thread = false;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->stopping = true;
            owner_reactor = state->owner_reactor;
            on_owner_thread = owner_reactor != nullptr &&
                              state->owner_thread_id == std::this_thread::get_id();
            if (owner_reactor == nullptr || on_owner_thread) {
                close_reactor_async_connection(*state);
                state->active_request.reset();
                state->queue.clear();
                state->owner_reactor = nullptr;
                state->owner_thread_id = {};
                state->connection_string.clear();
                state->stopping = false;
                return;
            }
        }

        auto done = std::make_shared<std::promise<void>>();
        auto done_future = done->get_future();
        if (!owner_reactor->schedule([state, done]() {
                {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    close_reactor_async_connection(*state);
                    state->active_request.reset();
                    state->queue.clear();
                    state->owner_reactor = nullptr;
                    state->owner_thread_id = {};
                    state->connection_string.clear();
                    state->stopping = false;
                }
                done->set_value();
            })) {
            std::lock_guard<std::mutex> lock(state->mutex);
            close_reactor_async_connection(*state);
            state->active_request.reset();
            state->queue.clear();
            state->owner_reactor = nullptr;
            state->owner_thread_id = {};
            state->connection_string.clear();
            state->stopping = false;
            return;
        }

        done_future.wait();
    }

    static void run_async_worker(async_state& state, std::string connection_string) {
        for (;;) {
            async_request next_request;
            {
                std::unique_lock<std::mutex> queue_lock(state.mutex);
                state.cv.wait(queue_lock, [&] { return state.stopping || !state.queue.empty(); });
                if (state.stopping && state.queue.empty()) {
                    break;
                }
                next_request = std::move(state.queue.front());
                state.queue.pop_front();
            }

            auto prepared_name = ensure_prepared_impl(state.connection,
                                                      state.prepared_statements,
                                                      connection_string,
                                                      next_request.statement_name,
                                                      next_request.sql,
                                                      state.last_error);
            if (!prepared_name) {
                if (next_request.operation == async_request::kind::query) {
                    next_request.query_handler(std::unexpected(prepared_name.error()));
                } else {
                    next_request.exec_handler(std::unexpected(prepared_name.error()));
                }
                continue;
            }

            if (next_request.operation == async_request::kind::query) {
                next_request.query_handler(execute_query_impl(
                    state.connection, *prepared_name, next_request.params, state.last_error));
            } else {
                next_request.exec_handler(execute_exec_impl(
                    state.connection, *prepared_name, next_request.params, state.last_error));
            }
        }

        state.prepared_statements.clear();
        if (state.connection != nullptr) {
            PQfinish(state.connection);
            state.connection = nullptr;
        }
    }

    PGconn* connection = nullptr;
    async_state async;
    std::shared_ptr<reactor_async_state> reactor_async = std::make_shared<reactor_async_state>();
};

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
    return ensure_connected_impl(impl_->connection, config_.connection_string, last_error_);
}

void postgres_executor::disconnect() noexcept {
    if (impl_ != nullptr && impl_->reactor_async != nullptr) {
        impl::shutdown_reactor_async(impl_->reactor_async);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (impl_ != nullptr && impl_->connection != nullptr) {
            PQfinish(impl_->connection);
            impl_->connection = nullptr;
        }
        prepared_statements_.clear();
    }

    if (impl_ == nullptr) {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(impl_->async.mutex);
        impl_->async.stopping = true;
        lock.unlock();
        impl_->async.cv.notify_one();
    }

    if (impl_->async.worker.joinable()) {
        impl_->async.worker.join();
    }

    std::lock_guard<std::mutex> lock(impl_->async.mutex);
    impl_->async.worker_started = false;
    impl_->async.stopping = false;
    impl_->async.queue.clear();
    impl_->async.prepared_statements.clear();
    if (impl_->async.connection != nullptr) {
        PQfinish(impl_->async.connection);
        impl_->async.connection = nullptr;
    }
}

bool postgres_executor::is_connected() const noexcept {
    return impl_ != nullptr && impl_->connection != nullptr;
}

std::string_view postgres_executor::last_error_message() const noexcept {
    return last_error_;
}

katana::result<const char*> postgres_executor::ensure_prepared(std::string_view statement_name,
                                                               std::string_view sql) {
    return ensure_prepared_impl(impl_->connection,
                                prepared_statements_,
                                config_.connection_string,
                                statement_name,
                                sql,
                                last_error_);
}

katana::result<rows> postgres_executor::query(std::string_view statement_name,
                                              std::string_view sql,
                                              const parameters& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto prepared_name = ensure_prepared(statement_name, sql);
    if (!prepared_name) {
        return std::unexpected(prepared_name.error());
    }
    return execute_query_impl(impl_->connection, *prepared_name, params, last_error_);
}

katana::result<exec_result> postgres_executor::exec(std::string_view statement_name,
                                                    std::string_view sql,
                                                    const parameters& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto prepared_name = ensure_prepared(statement_name, sql);
    if (!prepared_name) {
        return std::unexpected(prepared_name.error());
    }
    return execute_exec_impl(impl_->connection, *prepared_name, params, last_error_);
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
    return execute_query_each_impl(impl_->connection, *prepared_name, params, std::move(handler), last_error_);
}

katana::result<void> postgres_executor::run_simple(const char* sql) {
    return run_simple_impl(impl_->connection, config_.connection_string, sql, last_error_);
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

bool postgres_executor::query_async(std::string_view statement_name,
                                    std::string_view sql,
                                    parameters params,
                                    async_query_handler handler) {
    if (!handler) {
        return false;
    }

    async_request request;
    request.operation = async_request::kind::query;
    request.statement_name = std::string(statement_name);
    request.sql = std::string(sql);
    request.params = std::move(params);
    request.query_handler = std::move(handler);

    if (auto* owner_reactor = current_handler_reactor(); owner_reactor != nullptr &&
                                                      impl_ != nullptr &&
                                                      impl_->reactor_async != nullptr) {
        if (impl::enqueue_reactor_async(
                impl_->reactor_async, std::move(request), config_.connection_string, *owner_reactor)) {
            return true;
        }
    }

    auto& async = impl_->async;
    {
        std::lock_guard<std::mutex> lock(async.mutex);
        if (!async.worker_started) {
            async.stopping = false;
            async.worker = std::thread(
                [state = &async, connection_string = config_.connection_string] {
                    impl::run_async_worker(*state, std::move(connection_string));
                });
            async.worker_started = true;
        }

        if (async.stopping || async.queue.size() >= async_queue_limit) {
            return false;
        }
        async.queue.push_back(std::move(request));
    }

    async.cv.notify_one();
    return true;
}

bool postgres_executor::exec_async(std::string_view statement_name,
                                   std::string_view sql,
                                   parameters params,
                                   async_exec_handler handler) {
    if (!handler) {
        return false;
    }

    async_request request;
    request.operation = async_request::kind::exec;
    request.statement_name = std::string(statement_name);
    request.sql = std::string(sql);
    request.params = std::move(params);
    request.exec_handler = std::move(handler);

    if (auto* owner_reactor = current_handler_reactor(); owner_reactor != nullptr &&
                                                      impl_ != nullptr &&
                                                      impl_->reactor_async != nullptr) {
        if (impl::enqueue_reactor_async(
                impl_->reactor_async, std::move(request), config_.connection_string, *owner_reactor)) {
            return true;
        }
    }

    auto& async = impl_->async;
    {
        std::lock_guard<std::mutex> lock(async.mutex);
        if (!async.worker_started) {
            async.stopping = false;
            async.worker = std::thread(
                [state = &async, connection_string = config_.connection_string] {
                    impl::run_async_worker(*state, std::move(connection_string));
                });
            async.worker_started = true;
        }

        if (async.stopping || async.queue.size() >= async_queue_limit) {
            return false;
        }
        async.queue.push_back(std::move(request));
    }

    async.cv.notify_one();
    return true;
}

void postgres_executor::record_error(std::string message) {
    record_error_message(last_error_, std::move(message));
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

    const std::size_t slot =
        next_executor_.fetch_add(1, std::memory_order_relaxed) % executors_.size();
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

bool postgres_pool_executor::query_async(std::string_view statement_name,
                                         std::string_view sql,
                                         parameters params,
                                         async_query_handler handler) {
    return pool_.current_executor().query_async(
        statement_name, sql, std::move(params), std::move(handler));
}

bool postgres_pool_executor::exec_async(std::string_view statement_name,
                                        std::string_view sql,
                                        parameters params,
                                        async_exec_handler handler) {
    return pool_.current_executor().exec_async(
        statement_name, sql, std::move(params), std::move(handler));
}

} // namespace katana::sql

#endif
