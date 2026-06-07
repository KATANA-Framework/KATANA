#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <vector>

namespace katana::sql::generated {

struct CreateNoteRow {
    std::optional<int64_t> id;
    std::optional<std::string> created_at;
};

struct GetNoteRow {
    std::optional<int64_t> id;
    std::optional<std::string> title;
    std::optional<std::string> body;
    std::optional<std::string> priority;
    std::optional<std::string> metadata;
    std::optional<std::string> due_date;
    std::optional<std::string> created_at;
};

struct ListNotesRow {
    std::optional<int64_t> id;
    std::optional<std::string> title;
    std::optional<std::string> body;
    std::optional<std::string> priority;
    std::optional<std::string> metadata;
    std::optional<std::string> due_date;
    std::optional<std::string> created_at;
};

} // namespace katana::sql::generated
