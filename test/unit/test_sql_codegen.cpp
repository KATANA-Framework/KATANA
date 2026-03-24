#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace {

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

        std::string cmd =
            katana_gen.string() + " sql -i " + temp_dir.string() + " -o " + temp_dir.string();
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
    EXPECT_NE(repo.find("touch_user(int64_t p1) const"), std::string::npos);
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

} // namespace
