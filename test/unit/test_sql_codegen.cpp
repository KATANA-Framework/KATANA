#include <gtest/gtest.h>

#include <atomic>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {

std::string shell_quote(const fs::path& path) {
    std::string value = path.string();
    std::string quoted;
    quoted.reserve(value.size() + 2);
    quoted.push_back('\'');
    for (char ch : value) {
        if (ch == '\'') {
            quoted += "'\\''";
        } else {
            quoted.push_back(ch);
        }
    }
    quoted.push_back('\'');
    return quoted;
}

std::string read_file(const fs::path& path) {
    std::ifstream in(path);
    if (!in) {
        return {};
    }
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

class SqlCodegenTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Unique per process AND per test so `ctest -j` (multiple test binaries
        // shelling out to katana_gen) can't have one process's TearDown remove_all
        // delete another's working files. PID isolates processes; the counter
        // isolates successive tests within a process.
        static std::atomic<unsigned> seq{0};
        std::string unique = "katana_sql_codegen_test_" + std::to_string(::getpid()) + "_" +
                             std::to_string(seq.fetch_add(1));
        temp_dir = fs::temp_directory_path() / unique;
        std::error_code ec;
        fs::remove_all(temp_dir, ec);
        fs::create_directories(temp_dir);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(temp_dir, ec);
    }

    bool run_codegen(const std::string& extra_flags = "") {
        auto katana_gen = fs::path(KATANA_GEN_BIN);
        if (!fs::exists(katana_gen)) {
            katana_gen = fs::path("./katana_gen");
        }
        if (!fs::exists(katana_gen)) {
            return false;
        }

        std::string cmd = shell_quote(katana_gen) + " sql -i " + shell_quote(temp_dir) + " -o " +
                          shell_quote(temp_dir);
        if (!extra_flags.empty()) {
            cmd += " " + extra_flags;
        }
        return std::system(cmd.c_str()) == 0;
    }

    void write_sql(const std::string& file_name, const std::string& content) {
        std::ofstream out(temp_dir / file_name);
        out << content;
    }

    fs::path temp_dir;
};

TEST_F(SqlCodegenTest, GeneratesModelsAndRepository) {
    write_sql("get_user.sql",
              R"(-- name: get_user :one
SELECT id::bigint AS id, name::text AS name
FROM users
WHERE id = $1::bigint;
)");
    write_sql("touch_user.sql",
              R"(-- name: touch_user :exec
UPDATE users SET touched_at = NOW() WHERE id = $1::bigint;
)");

    ASSERT_TRUE(run_codegen());

    const auto models = read_file(temp_dir / "generated_sql_models.hpp");
    const auto repo = read_file(temp_dir / "generated_sql_repository.hpp");

    ASSERT_FALSE(models.empty());
    ASSERT_FALSE(repo.empty());
    EXPECT_NE(models.find("struct GetUserRow"), std::string::npos);
    EXPECT_NE(models.find("std::optional<int64_t> id;"), std::string::npos);
    EXPECT_NE(repo.find("get_user(int64_t p1) const"), std::string::npos);
    EXPECT_NE(repo.find("using get_user_async_handler"), std::string::npos);
    EXPECT_NE(repo.find("bool get_user_async(int64_t p1, get_user_async_handler handler) const"),
              std::string::npos);
    EXPECT_NE(repo.find("touch_user(int64_t p1) const"), std::string::npos);
    EXPECT_NE(
        repo.find("bool touch_user_async(int64_t p1, touch_user_async_handler handler) const"),
        std::string::npos);
}

TEST_F(SqlCodegenTest, NamedParametersProduceNamedArgsAndPositionalSql) {
    write_sql("get_user.sql",
              R"(-- name: get_user :one
SELECT id::bigint AS id, status::text AS status
FROM users
WHERE id = @user_id::bigint AND status = @status::text;
)");

    ASSERT_TRUE(run_codegen());
    const auto repo = read_file(temp_dir / "generated_sql_repository.hpp");
    ASSERT_FALSE(repo.empty());

    // @name → readable, collision-safe `arg_` C++ parameters (first-appearance order).
    EXPECT_NE(repo.find("get_user(int64_t arg_user_id, std::string_view arg_status) const"),
              std::string::npos);
    EXPECT_NE(repo.find("encode_value(arg_user_id)"), std::string::npos);
    EXPECT_NE(repo.find("encode_value(arg_status)"), std::string::npos);
    // The SQL string sent to Postgres must use positional $N, not @name.
    EXPECT_NE(repo.find("id = $1::bigint AND status = $2::text"), std::string::npos);
    EXPECT_EQ(repo.find("@user_id"), std::string::npos);
}

TEST_F(SqlCodegenTest, NamedParameterInCommentIsNotTreatedAsPlaceholder) {
    // An `@word` inside a `--` comment or a string literal must NOT mint a phantom parameter.
    write_sql("assign.sql",
              R"(-- name: assign_ticket :one
-- Reassign a ticket. Note: @assignee must exist (this @word is only a comment).
UPDATE tickets SET assignee_id = @assignee_id::bigint, label = 'sent to @nobody'
WHERE id = @ticket_id::bigint
RETURNING id::bigint AS id, assignee_id::bigint AS assignee_id;
)");

    ASSERT_TRUE(run_codegen());
    const auto repo = read_file(temp_dir / "generated_sql_repository.hpp");
    ASSERT_FALSE(repo.empty());

    // Exactly two real params, in first-appearance order; no phantom arg_assignee/arg_nobody.
    EXPECT_NE(repo.find("assign_ticket(int64_t arg_assignee_id, int64_t arg_ticket_id) const"),
              std::string::npos);
    EXPECT_EQ(repo.find("arg_assignee,"), std::string::npos);
    EXPECT_EQ(repo.find("arg_nobody"), std::string::npos);
    EXPECT_EQ(repo.find("$3"), std::string::npos); // only $1/$2 exist
    // The string literal with @nobody survives verbatim in the emitted SQL.
    EXPECT_NE(repo.find("'sent to @nobody'"), std::string::npos);
}

TEST_F(SqlCodegenTest, NamedParametersEmitParamsStructAndOverload) {
    write_sql("assign.sql",
              R"(-- name: assign_ticket :one
UPDATE tickets SET assignee_id = @assignee_id::bigint WHERE id = @ticket_id::bigint
RETURNING id::bigint AS id;
)");
    // A raw positional query must NOT get a params struct (back-compat).
    write_sql("legacy.sql",
              R"(-- name: legacy_lookup :one
SELECT id::bigint AS id FROM t WHERE id = $1::bigint;
)");

    ASSERT_TRUE(run_codegen());
    const auto models = read_file(temp_dir / "generated_sql_models.hpp");
    const auto repo = read_file(temp_dir / "generated_sql_repository.hpp");

    // Named-args struct (members in first-appearance order) + a forwarding overload.
    EXPECT_NE(models.find("struct AssignTicketParams"), std::string::npos);
    EXPECT_NE(models.find("int64_t assignee_id;"), std::string::npos);
    EXPECT_NE(models.find("int64_t ticket_id;"), std::string::npos);
    EXPECT_NE(repo.find("assign_ticket(const AssignTicketParams& args) const"), std::string::npos);
    EXPECT_NE(repo.find("assign_ticket_async(const AssignTicketParams& args"), std::string::npos);
    // The positional method stays as the API of record.
    EXPECT_NE(repo.find("assign_ticket(int64_t arg_assignee_id, int64_t arg_ticket_id) const"),
              std::string::npos);
    // Raw $N queries get no struct.
    EXPECT_EQ(models.find("struct LegacyLookupParams"), std::string::npos);
}

TEST_F(SqlCodegenTest, PositionalParametersKeepLegacyPNames) {
    write_sql("legacy.sql",
              R"(-- name: legacy_lookup :one
SELECT id::bigint AS id FROM users WHERE id = $1::bigint;
)");

    ASSERT_TRUE(run_codegen());
    const auto repo = read_file(temp_dir / "generated_sql_repository.hpp");
    ASSERT_FALSE(repo.empty());

    // Raw $N queries keep their historical pN names (back-compat).
    EXPECT_NE(repo.find("legacy_lookup(int64_t p1) const"), std::string::npos);
    EXPECT_NE(repo.find("encode_value(p1)"), std::string::npos);
}

TEST_F(SqlCodegenTest, GeneratesRowDtoBridgeForExactMatchesOnly) {
    // A DTO whose fields exactly match a query row, plus a query whose row is only a subset.
    std::ofstream(temp_dir / "api.yaml") << R"(openapi: 3.0.0
info: { title: Bridge API, version: "1.0" }
paths: {}
components:
  schemas:
    Widget:
      type: object
      required: [id, label]
      properties:
        id: { type: integer }
        label: { type: string }
)";
    write_sql("get_widget.sql", R"(-- name: get_widget :one
SELECT id::bigint AS id, label::text AS label FROM widgets WHERE id = $1::bigint;
)");
    write_sql("count_widget.sql", R"(-- name: count_widget :one
SELECT id::bigint AS id FROM widgets WHERE id = $1::bigint;
)");

    const std::string flags =
        "--namespace wid --openapi " + shell_quote(temp_dir / "api.yaml");
    ASSERT_TRUE(run_codegen(flags));

    const auto bridge = read_file(temp_dir / "generated_bridge.hpp");
    ASSERT_FALSE(bridge.empty());
    // Exact match → both converters, in the shared namespace.
    EXPECT_NE(bridge.find("namespace wid"), std::string::npos);
    EXPECT_NE(bridge.find("Widget to_Widget(const GetWidgetRow& row, katana::monotonic_arena*"),
              std::string::npos);
    EXPECT_NE(bridge.find("GetWidgetRow to_GetWidgetRow(const Widget& dto)"), std::string::npos);
    // Subset row (only id) must NOT be bridged to Widget.
    EXPECT_EQ(bridge.find("CountWidgetRow to_"), std::string::npos);
}

TEST_F(SqlCodegenTest, SqlAstDumpIsStable) {
    write_sql("users.sql",
              R"(-- name: list_users :many
SELECT id::bigint AS id, name::text AS name
FROM users
WHERE active = $1::bool
ORDER BY id;
)");

    ASSERT_TRUE(run_codegen("--dump-ast --json"));

    const auto ast_json = read_file(temp_dir / "sql_ast.json");
    ASSERT_FALSE(ast_json.empty());
    const std::string expected =
        R"({"queries":[{"name":"list_users","mode":"many","path":"users.sql","params":[{"index":1,"name":"","type":"bool","cpp":"bool"}],"columns":[{"name":"id","type":"bigint","cpp":"int64_t"},{"name":"name","type":"text","cpp":"std::string"}]}]})";
    EXPECT_EQ(ast_json, expected);
}

TEST_F(SqlCodegenTest, RejectsExecReturningWithoutResultMode) {
    write_sql("touch_user.sql",
              R"(-- name: touch_user :exec
UPDATE users
SET last_seen_at = NOW()
WHERE id = $1::bigint
RETURNING id::bigint AS id;
)");

    EXPECT_FALSE(run_codegen());
}

TEST_F(SqlCodegenTest, SupportsUpsertAndBulkArrayParameters) {
    write_sql("upsert_user.sql",
              R"(-- name: upsert_user :one
INSERT INTO users (id, name, active)
VALUES ($1::bigint, $2::text, $3::bool)
ON CONFLICT (id) DO UPDATE
SET
  name = EXCLUDED.name,
  active = EXCLUDED.active
RETURNING
  id::bigint AS id,
  name::text AS name,
  active::bool AS active;
)");
    write_sql("bulk_upsert_users.sql",
              R"(-- name: bulk_upsert_users :many
INSERT INTO users (id, name, active)
SELECT batch.id, batch.name, batch.active
FROM UNNEST($1::bigint[], $2::text[], $3::bool[]) AS batch(id, name, active)
ON CONFLICT (id) DO UPDATE
SET
  name = EXCLUDED.name,
  active = EXCLUDED.active
RETURNING
  id::bigint AS id,
  name::text AS name,
  active::bool AS active;
)");

    ASSERT_TRUE(run_codegen("--dump-ast --json"));

    const auto models = read_file(temp_dir / "generated_sql_models.hpp");
    const auto repo = read_file(temp_dir / "generated_sql_repository.hpp");
    const auto ast_json = read_file(temp_dir / "sql_ast.json");

    ASSERT_FALSE(models.empty());
    ASSERT_FALSE(repo.empty());
    ASSERT_FALSE(ast_json.empty());
    EXPECT_NE(repo.find("upsert_user(int64_t p1, std::string_view p2, bool p3) const"),
              std::string::npos);
    EXPECT_NE(repo.find("bulk_upsert_users(const std::vector<int64_t>& p1, const "
                        "std::vector<std::string>& p2, const std::vector<bool>& p3) const"),
              std::string::npos);
    EXPECT_NE(models.find("struct UpsertUserRow"), std::string::npos);
    EXPECT_NE(models.find("struct BulkUpsertUsersRow"), std::string::npos);
    EXPECT_NE(ast_json.find("\"type\":\"bigint[]\",\"cpp\":\"std::vector<int64_t>\""),
              std::string::npos);
    EXPECT_NE(ast_json.find("\"type\":\"text[]\",\"cpp\":\"std::vector<std::string>\""),
              std::string::npos);
    EXPECT_NE(ast_json.find("\"type\":\"bool[]\",\"cpp\":\"std::vector<bool>\""),
              std::string::npos);
}

} // namespace
