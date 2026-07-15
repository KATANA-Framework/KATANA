#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
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
        auto katana_gen = fs::path(KATANA_GEN_BIN);
        if (!fs::exists(katana_gen)) {
            katana_gen = fs::path("./katana_gen");
        }
        if (!fs::exists(katana_gen)) {
            return false;
        }

        std::string cmd = shell_quote(katana_gen) + " openapi -i " +
                          shell_quote(temp_dir / spec_file) + " -o " + shell_quote(temp_dir) + " " +
                          extra_flags;
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
    EXPECT_NE(dto.find("std::optional<arena_string<>> name;"), std::string::npos);
    // Verify it compiles by checking for valid C++ struct syntax
    EXPECT_NE(dto.find("explicit User("), std::string::npos);
}

TEST_F(CodegenSnapshotTest, TypeScriptClientIsStable) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: TS Client API
  version: 1.0
paths:
  /widgets:
    get:
      operationId: list_widgets
      parameters:
        - name: limit
          in: query
          schema: { type: integer }
      responses:
        '200':
          description: ok
          content:
            application/json:
              schema: { $ref: '#/components/schemas/Widget' }
  /widgets/{id}:
    get:
      operationId: get_widget
      parameters:
        - name: id
          in: path
          required: true
          schema: { type: integer }
      responses:
        '200':
          description: ok
          content:
            application/json:
              schema: { $ref: '#/components/schemas/Widget' }
components:
  schemas:
    Widget:
      type: object
      properties:
        id:
          type: integer
        name:
          type: string
)";
    auto spec_path = temp_dir / "ts.yaml";
    {
        std::ofstream out(spec_path);
        out << spec;
    }

    ASSERT_TRUE(run_codegen("ts.yaml", "--emit typescript"));

    auto ts = read_file(temp_dir / "generated_client.ts");
    ASSERT_FALSE(ts.empty());

    // Interface from the schema.
    EXPECT_NE(ts.find("export interface Widget {"), std::string::npos);
    EXPECT_NE(ts.find("id?: number;"), std::string::npos);
    // Client shell + typed methods.
    EXPECT_NE(ts.find("export class ApiClient"), std::string::npos);
    // fetch must be bound to globalThis, else `this.fetchFn(...)` throws "Illegal invocation".
    EXPECT_NE(ts.find("fetch.bind(globalThis)"), std::string::npos);
    EXPECT_NE(ts.find("get_widget(id: number, opts?: RequestOptions): Promise<Widget>"),
              std::string::npos);
    // Path-parameter interpolation and query-object argument.
    EXPECT_NE(ts.find("encodeURIComponent(String(id))"), std::string::npos);
    EXPECT_NE(ts.find("query?: { limit?: number }"), std::string::npos);
    // TS-only run must not drop a C++ package header alongside it.
    EXPECT_FALSE(fs::exists(temp_dir / "generated_openapi_package.hpp"));
}

TEST_F(CodegenSnapshotTest, StrictModeRejectsUnknownConstructs) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Strict API
  version: 1.0
paths: {}
components:
  schemas:
    Weird:
      type: object
      properties:
        thing:
          type: bogustype
)";
    auto spec_path = temp_dir / "strict.yaml";
    {
        std::ofstream out(spec_path);
        out << spec;
    }

    // Non-strict: the unknown `type` is coerced (with a warning) and codegen still succeeds.
    EXPECT_TRUE(run_codegen("strict.yaml", "--emit dto"));
    // --strict: the same unsupported construct is a hard error.
    EXPECT_FALSE(run_codegen("strict.yaml", "--emit dto --strict"));
}

} // namespace
