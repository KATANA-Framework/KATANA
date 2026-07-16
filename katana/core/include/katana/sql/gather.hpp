#pragma once

// katana::sql::gather — fan several async queries out on one reactor connection and join their typed
// results into a single continuation, replacing hand-nested `_async` callback pyramids (F14 / DX-16).
//
//   gather(executor, [](katana::result<std::tuple<A, B, C>> joined) { ... },
//          repo.a_step(id), repo.b_step(id), repo.c_step(id));
//
// If the executor implements `pipelined_executor` (Postgres does), the whole batch is sent in ONE
// libpq pipeline — the queries still run on the single per-reactor connection, but without a network
// round-trip per query. Otherwise gather falls back to chaining `query_async` sequentially. Either
// way the continuation fires once, off the reactor, with a tuple of decoded results — or the first
// query/decode error. It is a latency optimization layered on the existing model, never a change to
// concurrency semantics (the connection still executes one statement at a time).

#include "katana/core/inplace_function.hpp"
#include "katana/core/result.hpp"
#include "katana/sql/error.hpp"
#include "katana/sql/runtime.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>

namespace katana::sql {

// One query in a gather: a prepared statement plus a decoder turning its raw rows into a typed T.
// `statement_name`/`sql` are borrowed (typically generated string_view constants) and must outlive
// the gather; `params` and `decode` are owned.
template <typename T>
struct query_step {
    std::string_view statement_name;
    std::string_view sql;
    parameters params;
    katana::inplace_function<katana::result<T>(rows), 128> decode;
};

// Build a query_step deducing T from the decoder's `result<T>` return type.
template <typename Fn>
auto make_step(std::string_view statement_name, std::string_view sql, parameters params, Fn&& decode) {
    using result_type = std::invoke_result_t<Fn&, rows>;
    using value_type = typename result_type::value_type;
    return query_step<value_type>{statement_name, sql, std::move(params),
                                  std::forward<Fn>(decode)};
}

namespace detail {

// The name/sql/params of a step, type-erased for the sequential fallback which indexes by runtime i.
struct gather_plan_entry {
    std::string_view statement_name;
    std::string_view sql;
    parameters params;
};

template <typename Cont, typename... Ts>
struct gather_state : std::enable_shared_from_this<gather_state<Cont, Ts...>> {
    static constexpr std::size_t count = sizeof...(Ts);

    Cont continuation;
    std::tuple<query_step<Ts>...> steps; // kept for the typed decoders
    std::array<gather_plan_entry, count> plan;
    async_executor& executor;
    std::vector<katana::result<rows>> raw;
    bool finished = false;

    gather_state(Cont cont, async_executor& ex, query_step<Ts>... s)
        : continuation(std::move(cont)), steps(std::move(s)...), executor(ex) {
        raw.assign(count, katana::result<rows>(std::unexpected(make_error_code(errc::query_failed))));
        fill_plan(std::index_sequence_for<Ts...>{});
    }

    template <std::size_t... I>
    void fill_plan(std::index_sequence<I...>) {
        // Move params out of the steps into the plan — the decoders don't need them.
        ((plan[I] = gather_plan_entry{std::get<I>(steps).statement_name, std::get<I>(steps).sql,
                                      std::move(std::get<I>(steps).params)}),
         ...);
    }

    void start() {
        if constexpr (count == 0) {
            finish();
            return;
        }
        if (auto* pex = dynamic_cast<pipelined_executor*>(&executor)) {
            std::vector<pipeline_query> queries;
            queries.reserve(count);
            for (auto& entry : plan) {
                queries.push_back({entry.statement_name, entry.sql, entry.params});
            }
            auto self = this->shared_from_this();
            const bool accepted = pex->query_pipeline(
                std::move(queries), [self](std::vector<katana::result<rows>> results) {
                    if (results.size() == self->raw.size()) {
                        self->raw = std::move(results);
                    }
                    self->finish();
                });
            if (accepted) {
                return; // pipelined; params in `plan` stay untouched as backup were it not accepted
            }
        }
        run_sequential(0);
    }

    // Chain query_async one at a time — correct on any async_executor, same wall-clock as the
    // hand-nested callback chain it replaces.
    void run_sequential(std::size_t index) {
        if (index >= count) {
            finish();
            return;
        }
        auto self = this->shared_from_this();
        const bool dispatched = executor.query_async(
            plan[index].statement_name, plan[index].sql, plan[index].params,
            [self, index](katana::result<rows> r) {
                self->raw[index] = std::move(r);
                self->run_sequential(index + 1);
            });
        if (!dispatched) {
            raw[index] = std::unexpected(make_error_code(errc::query_failed));
            run_sequential(index + 1);
        }
    }

    void finish() {
        if (finished) {
            return;
        }
        finished = true;
        deliver(std::index_sequence_for<Ts...>{});
    }

    template <std::size_t I>
    katana::result<std::tuple_element_t<I, std::tuple<Ts...>>> decode_one() {
        using value_type = std::tuple_element_t<I, std::tuple<Ts...>>;
        if (!raw[I]) {
            return katana::result<value_type>(std::unexpected(raw[I].error()));
        }
        return std::get<I>(steps).decode(std::move(*raw[I]));
    }

    template <std::size_t... I>
    void deliver(std::index_sequence<I...>) {
        std::tuple<katana::result<Ts>...> decoded{decode_one<I>()...};
        // First failure (in query order) short-circuits the whole join.
        std::optional<std::error_code> first_error;
        ((first_error = first_error ? first_error
                                    : (std::get<I>(decoded) ? std::optional<std::error_code>{}
                                                            : std::optional<std::error_code>{
                                                                  std::get<I>(decoded).error()})),
         ...);
        if (first_error) {
            continuation(katana::result<std::tuple<Ts...>>(std::unexpected(*first_error)));
            return;
        }
        continuation(katana::result<std::tuple<Ts...>>(
            std::tuple<Ts...>{std::move(*std::get<I>(decoded))...}));
    }
};

} // namespace detail

// Run all steps and invoke `continuation` once with result<tuple<Ts...>> (or the first error).
// Returns immediately; the continuation fires later on the reactor that owns the connection.
template <typename Cont, typename... Ts>
void gather(async_executor& executor, Cont continuation, query_step<Ts>... steps) {
    auto state = std::make_shared<detail::gather_state<Cont, Ts...>>(
        std::move(continuation), executor, std::move(steps)...);
    state->start();
}

} // namespace katana::sql
