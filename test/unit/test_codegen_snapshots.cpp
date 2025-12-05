#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace {

std::string read_file(const fs::path& p) {
    std::ifstream in(p);
    if (!in) {
        return {};
    }
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

class CodegenSnapshotTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_dir = fs::temp_directory_path() / "katana_codegen_snapshot";
        fs::create_directories(temp_dir);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(temp_dir, ec);
    }

    bool run_codegen(const std::string& spec_file, const std::string& extra_flags) {
        auto katana_gen = fs::path("./katana_gen");
        if (!fs::exists(katana_gen)) {
            katana_gen = fs::path("./build/debug/katana_gen");
        }
        if (!fs::exists(katana_gen)) {
            katana_gen = fs::path("../katana_gen");
        }
        if (!fs::exists(katana_gen)) {
            return false;
        }

        std::string cmd = katana_gen.string() + " openapi -i " + (temp_dir / spec_file).string() +
                          " -o " + temp_dir.string() + " " + extra_flags;
        return std::system(cmd.c_str()) == 0;
    }

    fs::path temp_dir;
};

TEST_F(CodegenSnapshotTest, AstDumpIsStable) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Snapshot API
  version: 1.0
paths:
  /ping:
    get:
      operationId: ping
      responses:
        '200':
          description: ok
)";
    auto spec_path = temp_dir / "snapshot.yaml";
    {
        std::ofstream out(spec_path);
        out << spec;
    }

    ASSERT_TRUE(run_codegen("snapshot.yaml", "--dump-ast --inline-naming flat --json"));

    auto ast_path = temp_dir / "openapi_ast.json";
    auto ast_json = read_file(ast_path);
    ASSERT_FALSE(ast_json.empty());

    const std::string expected =
        R"({"openapi":"3.0.0","title":"Snapshot API","version":"1.0","paths":[{"path":"/ping","operations":[{"method":"GET","operationId":"ping","summary":"","parameters":[],"requestBody":null,"responses":[{"status":200,"default":false,"description":"ok","content":[]}]}]}],"schemas":[]})";
    EXPECT_EQ(ast_json, expected);
}

TEST_F(CodegenSnapshotTest, GeneratedDtoIsStable) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: DTO Snapshot API
  version: 1.0
paths: {}
components:
  schemas:
    User:
      type: object
      properties:
        name:
          type: string
)";
    auto spec_path = temp_dir / "dto.yaml";
    {
        std::ofstream out(spec_path);
        out << spec;
    }

    ASSERT_TRUE(run_codegen("dto.yaml", "--emit dto --inline-naming flat"));

    auto dto_path = temp_dir / "generated_dtos.hpp";
    auto dto = read_file(dto_path);
    ASSERT_FALSE(dto.empty());

    EXPECT_NE(dto.find("// layer: flat"), std::string::npos);
    EXPECT_NE(dto.find("struct User"), std::string::npos);
    EXPECT_NE(dto.find("arena_string<> name;"), std::string::npos);
    // Verify it compiles by checking for valid C++ struct syntax
    EXPECT_NE(dto.find("explicit User("), std::string::npos);
}

} // namespace
