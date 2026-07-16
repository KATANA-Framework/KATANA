#pragma once

#include "generated_sql_models.hpp"
#include "katana/sql/gather.hpp"
#include "katana/sql/runtime.hpp"

#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace generated {

class generated_repository {
public:
    using create_note_async_handler = katana::inplace_function<void(katana::result<std::optional<CreateNoteRow>>), 256>;
    using get_note_async_handler = katana::inplace_function<void(katana::result<std::optional<GetNoteRow>>), 256>;
    using list_notes_async_handler = katana::inplace_function<void(katana::result<std::vector<ListNotesRow>>), 256>;

    explicit generated_repository(katana::sql::executor& executor) noexcept
        : executor_(executor),
          async_executor_(dynamic_cast<katana::sql::async_executor*>(&executor)) {}

    katana::result<std::optional<CreateNoteRow>> create_note(std::string_view p1, std::string_view p2, std::string_view p3, std::string_view p4, std::string_view p5) const {
        katana::sql::parameters params;
        params.reserve(5);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        params.push_back(katana::sql::encode_value(p3));
        params.push_back(katana::sql::encode_value(p4));
        params.push_back(katana::sql::encode_value(p5));
        std::optional<CreateNoteRow> out_row;
        std::size_t row_count = 0;
        auto status = executor_.query_each("create_note", create_note_sql, params,
            [&](const katana::sql::row_view& row) -> katana::result<void> {
                ++row_count;
                if (row_count != 1) {
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                }
                auto mapped = map_create_note(row);
                if (!mapped) {
                    return std::unexpected(mapped.error());
                }
                out_row = std::move(*mapped);
                return {};
            });
        if (!status) {
            return std::unexpected(status.error());
        }
        if (row_count == 0) {
            return std::optional<CreateNoteRow>{};
        }
        return out_row;
    }

    bool create_note_async(std::string_view p1, std::string_view p2, std::string_view p3, std::string_view p4, std::string_view p5, create_note_async_handler handler) const {
        if (!handler) {
            return false;
        }
        if (async_executor_ == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(create_note(p1, p2, p3, p4, p5));
            return true;
        }
        katana::sql::parameters params;
        params.reserve(5);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        params.push_back(katana::sql::encode_value(p3));
        params.push_back(katana::sql::encode_value(p4));
        params.push_back(katana::sql::encode_value(p5));
        return async_executor_->query_async("create_note", create_note_sql, std::move(params),
            [handler = std::move(handler)](katana::result<katana::sql::rows> rows_result) {
                if (!rows_result) {
                    handler(std::unexpected(rows_result.error()));
                    return;
                }
                handler(fold_create_note(std::move(*rows_result)));
            });
    }

    katana::sql::query_step<std::optional<CreateNoteRow>> create_note_step(std::string_view p1, std::string_view p2, std::string_view p3, std::string_view p4, std::string_view p5) const {
        katana::sql::parameters step_params;
        step_params.reserve(5);
        step_params.push_back(katana::sql::encode_value(p1));
        step_params.push_back(katana::sql::encode_value(p2));
        step_params.push_back(katana::sql::encode_value(p3));
        step_params.push_back(katana::sql::encode_value(p4));
        step_params.push_back(katana::sql::encode_value(p5));
        return katana::sql::query_step<std::optional<CreateNoteRow>>{"create_note", create_note_sql, std::move(step_params), &fold_create_note};
    }

    katana::result<std::optional<GetNoteRow>> get_note(int64_t p1) const {
        katana::sql::parameters params;
        params.reserve(1);
        params.push_back(katana::sql::encode_value(p1));
        std::optional<GetNoteRow> out_row;
        std::size_t row_count = 0;
        auto status = executor_.query_each("get_note", get_note_sql, params,
            [&](const katana::sql::row_view& row) -> katana::result<void> {
                ++row_count;
                if (row_count != 1) {
                    return std::unexpected(std::make_error_code(std::errc::invalid_argument));
                }
                auto mapped = map_get_note(row);
                if (!mapped) {
                    return std::unexpected(mapped.error());
                }
                out_row = std::move(*mapped);
                return {};
            });
        if (!status) {
            return std::unexpected(status.error());
        }
        if (row_count == 0) {
            return std::optional<GetNoteRow>{};
        }
        return out_row;
    }

    bool get_note_async(int64_t p1, get_note_async_handler handler) const {
        if (!handler) {
            return false;
        }
        if (async_executor_ == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(get_note(p1));
            return true;
        }
        katana::sql::parameters params;
        params.reserve(1);
        params.push_back(katana::sql::encode_value(p1));
        return async_executor_->query_async("get_note", get_note_sql, std::move(params),
            [handler = std::move(handler)](katana::result<katana::sql::rows> rows_result) {
                if (!rows_result) {
                    handler(std::unexpected(rows_result.error()));
                    return;
                }
                handler(fold_get_note(std::move(*rows_result)));
            });
    }

    katana::sql::query_step<std::optional<GetNoteRow>> get_note_step(int64_t p1) const {
        katana::sql::parameters step_params;
        step_params.reserve(1);
        step_params.push_back(katana::sql::encode_value(p1));
        return katana::sql::query_step<std::optional<GetNoteRow>>{"get_note", get_note_sql, std::move(step_params), &fold_get_note};
    }

    katana::result<std::vector<ListNotesRow>> list_notes(std::string_view p1, int64_t p2, int64_t p3) const {
        katana::sql::parameters params;
        params.reserve(3);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        params.push_back(katana::sql::encode_value(p3));
        std::vector<ListNotesRow> out_rows;
        auto status = executor_.query_each("list_notes", list_notes_sql, params,
            [&](const katana::sql::row_view& row) -> katana::result<void> {
                auto mapped = map_list_notes(row);
                if (!mapped) {
                    return std::unexpected(mapped.error());
                }
                out_rows.push_back(std::move(*mapped));
                return {};
            });
        if (!status) {
            return std::unexpected(status.error());
        }
        return out_rows;
    }

    bool list_notes_async(std::string_view p1, int64_t p2, int64_t p3, list_notes_async_handler handler) const {
        if (!handler) {
            return false;
        }
        if (async_executor_ == nullptr) {
            // No async executor: run synchronously and deliver the result inline
            // so the completion always fires (returning false here would hang a
            // deferred response).
            handler(list_notes(p1, p2, p3));
            return true;
        }
        katana::sql::parameters params;
        params.reserve(3);
        params.push_back(katana::sql::encode_value(p1));
        params.push_back(katana::sql::encode_value(p2));
        params.push_back(katana::sql::encode_value(p3));
        return async_executor_->query_async("list_notes", list_notes_sql, std::move(params),
            [handler = std::move(handler)](katana::result<katana::sql::rows> rows_result) {
                if (!rows_result) {
                    handler(std::unexpected(rows_result.error()));
                    return;
                }
                handler(fold_list_notes(std::move(*rows_result)));
            });
    }

    katana::sql::query_step<std::vector<ListNotesRow>> list_notes_step(std::string_view p1, int64_t p2, int64_t p3) const {
        katana::sql::parameters step_params;
        step_params.reserve(3);
        step_params.push_back(katana::sql::encode_value(p1));
        step_params.push_back(katana::sql::encode_value(p2));
        step_params.push_back(katana::sql::encode_value(p3));
        return katana::sql::query_step<std::vector<ListNotesRow>>{"list_notes", list_notes_sql, std::move(step_params), &fold_list_notes};
    }

private:
    katana::sql::executor& executor_;
    katana::sql::async_executor* const async_executor_;

    static constexpr std::string_view create_note_sql = R"__KATANA_SQL__(
INSERT INTO demo_notes (title, body, priority, metadata, due_date)
VALUES ($1::text, $2::text, $3::text, $4::jsonb, $5::timestamptz)
RETURNING id::bigint AS id, created_at::text AS created_at;
)__KATANA_SQL__";

    template <typename Row>
    static katana::result<CreateNoteRow> map_create_note(const Row& row) {
        CreateNoteRow out;
        {
            if (row.size() <= 0) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(0);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.id = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 1) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(1);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.created_at = std::move(*parsed);
            }
        }
        return out;
    }

    static katana::result<std::optional<CreateNoteRow>> fold_create_note(katana::sql::rows rows_result) {
        if (rows_result.size() > 1) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        if (rows_result.empty()) {
            return std::optional<CreateNoteRow>{};
        }
        auto mapped = map_create_note(rows_result.front());
        if (!mapped) {
            return std::unexpected(mapped.error());
        }
        return std::optional<CreateNoteRow>(std::move(*mapped));
    }

    static constexpr std::string_view get_note_sql = R"__KATANA_SQL__(
SELECT id::bigint AS id, title::text AS title, body::text AS body,
       priority::text AS priority, metadata::text AS metadata,
       due_date::text AS due_date, created_at::text AS created_at
FROM demo_notes WHERE id = $1::bigint;
)__KATANA_SQL__";

    template <typename Row>
    static katana::result<GetNoteRow> map_get_note(const Row& row) {
        GetNoteRow out;
        {
            if (row.size() <= 0) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(0);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.id = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 1) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(1);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.title = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 2) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(2);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.body = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 3) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(3);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.priority = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 4) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(4);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.metadata = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 5) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(5);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.due_date = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 6) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(6);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.created_at = std::move(*parsed);
            }
        }
        return out;
    }

    static katana::result<std::optional<GetNoteRow>> fold_get_note(katana::sql::rows rows_result) {
        if (rows_result.size() > 1) {
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        if (rows_result.empty()) {
            return std::optional<GetNoteRow>{};
        }
        auto mapped = map_get_note(rows_result.front());
        if (!mapped) {
            return std::unexpected(mapped.error());
        }
        return std::optional<GetNoteRow>(std::move(*mapped));
    }

    static constexpr std::string_view list_notes_sql = R"__KATANA_SQL__(
SELECT id::bigint AS id, title::text AS title, body::text AS body,
       priority::text AS priority, metadata::text AS metadata,
       due_date::text AS due_date, created_at::text AS created_at
FROM demo_notes
WHERE ($1::text IS NULL OR priority = $1::text)
ORDER BY id DESC
LIMIT $2::bigint OFFSET $3::bigint;
)__KATANA_SQL__";

    template <typename Row>
    static katana::result<ListNotesRow> map_list_notes(const Row& row) {
        ListNotesRow out;
        {
            if (row.size() <= 0) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(0);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<int64_t>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.id = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 1) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(1);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.title = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 2) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(2);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.body = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 3) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(3);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.priority = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 4) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(4);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.metadata = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 5) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(5);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.due_date = std::move(*parsed);
            }
        }
        {
            if (row.size() <= 6) {
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            const auto& cell = row.at(6);
            if (cell.has_value()) {
                auto parsed = katana::sql::parse_value<std::string>(cell);
                if (!parsed) {
                    return std::unexpected(parsed.error());
                }
                out.created_at = std::move(*parsed);
            }
        }
        return out;
    }

    static katana::result<std::vector<ListNotesRow>> fold_list_notes(katana::sql::rows rows_result) {
        std::vector<ListNotesRow> out_rows;
        out_rows.reserve(rows_result.size());
        for (const auto& row : rows_result) {
            auto mapped = map_list_notes(row);
            if (!mapped) {
                return std::unexpected(mapped.error());
            }
            out_rows.push_back(std::move(*mapped));
        }
        return out_rows;
    }

};

} // namespace generated
