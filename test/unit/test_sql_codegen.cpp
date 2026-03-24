#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

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
        temp_dir = fs::temp_directory_path() / "katana_sql_codegen_test";
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
        R"({"queries":[{"name":"list_users","mode":"many","path":"users.sql","params":[{"index":1,"type":"bool","cpp":"bool"}],"columns":[{"name":"id","type":"bigint","cpp":"int64_t"},{"name":"name","type":"text","cpp":"std::string"}]}]})";
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
