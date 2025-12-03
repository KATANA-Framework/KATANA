#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class CodegenIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        temp_dir = fs::temp_directory_path() / "katana_codegen_test";
        fs::create_directories(temp_dir);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(temp_dir, ec);
    }

    void create_openapi_spec(const std::string& filename, const std::string& content) {
        auto spec_path = temp_dir / filename;
        std::ofstream out(spec_path);
        out << content;
    }

    bool run_codegen(const std::string& spec_file, const std::string& emit = "all") {
        auto katana_gen = fs::path("./katana_gen");
        if (!fs::exists(katana_gen)) {
            katana_gen = fs::path("./build/debug/katana_gen");
        }
        if (!fs::exists(katana_gen)) {
            katana_gen = fs::path("../katana_gen"); // when running from build/debug/test
        }
        if (!fs::exists(katana_gen)) {
            return false;
        }

        std::string cmd = katana_gen.string() + " openapi -i " + (temp_dir / spec_file).string() +
                          " -o " + temp_dir.string() + " --emit " + emit;
        return std::system(cmd.c_str()) == 0;
    }

    std::string read_generated_file(const std::string& filename) {
        auto path = temp_dir / filename;
        std::ifstream in(path);
        if (!in) {
            return "";
        }
        return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }

    fs::path temp_dir;
};

TEST_F(CodegenIntegrationTest, GeneratesValidDTOs) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Test API
  version: 1.0.0
paths: {}
components:
  schemas:
    User:
      type: object
      required:
        - id
        - name
      properties:
        id:
          type: integer
        name:
          type: string
        email:
          type: string
)";

    create_openapi_spec("test.yaml", spec);
    ASSERT_TRUE(run_codegen("test.yaml", "dto"));

    auto dto_content = read_generated_file("generated_dtos.hpp");
    EXPECT_FALSE(dto_content.empty());
    EXPECT_NE(dto_content.find("struct User"), std::string::npos);
    EXPECT_NE(dto_content.find("int64_t id"), std::string::npos);
    EXPECT_NE(dto_content.find("arena_string<> name"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, GeneratesValidators) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Test API
  version: 1.0.0
paths: {}
components:
  schemas:
    Product:
      type: object
      required:
        - name
        - price
      properties:
        name:
          type: string
          minLength: 3
          maxLength: 100
        price:
          type: number
          minimum: 0
          exclusiveMaximum: 1000000
)";

    create_openapi_spec("test.yaml", spec);
    ASSERT_TRUE(run_codegen("test.yaml", "dto,validator"));

    auto validator_content = read_generated_file("generated_validators.hpp");
    EXPECT_FALSE(validator_content.empty());
    EXPECT_NE(validator_content.find("validate_Product"), std::string::npos);
    EXPECT_NE(validator_content.find("min: 3"), std::string::npos);
    EXPECT_NE(validator_content.find("max: 100"), std::string::npos);
    EXPECT_NE(validator_content.find("min: 0"), std::string::npos);
    EXPECT_NE(validator_content.find("value must be less than"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, GeneratesJSONParsers) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Test API
  version: 1.0.0
paths: {}
components:
  schemas:
    Config:
      type: object
      properties:
        enabled:
          type: boolean
        timeout:
          type: integer
        tags:
          type: array
          items:
            type: string
)";

    create_openapi_spec("test.yaml", spec);
    ASSERT_TRUE(run_codegen("test.yaml", "dto,serdes"));

    auto json_content = read_generated_file("generated_json.hpp");
    EXPECT_FALSE(json_content.empty());
    EXPECT_NE(json_content.find("parse_Config"), std::string::npos);
    EXPECT_NE(json_content.find("serialize_Config"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, GeneratesRouteTable) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Test API
  version: 1.0.0
paths:
  /users:
    get:
      operationId: listUsers
      responses:
        '200':
          description: OK
    post:
      operationId: createUser
      responses:
        '201':
          description: Created
  /users/{id}:
    get:
      operationId: getUser
      parameters:
        - name: id
          in: path
          required: true
          schema:
            type: integer
      responses:
        '200':
          description: OK
)";

    create_openapi_spec("test.yaml", spec);
    ASSERT_TRUE(run_codegen("test.yaml", "router"));

    auto router_content = read_generated_file("generated_routes.hpp");
    EXPECT_FALSE(router_content.empty());
    EXPECT_NE(router_content.find("route_entry routes[]"), std::string::npos);
    EXPECT_NE(router_content.find("/users"), std::string::npos);
    EXPECT_NE(router_content.find("listUsers"), std::string::npos);
    EXPECT_NE(router_content.find("createUser"), std::string::npos);
    EXPECT_NE(router_content.find("getUser"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, ValidatesArrayConstraints) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Test API
  version: 1.0.0
paths: {}
components:
  schemas:
    Tags:
      type: object
      properties:
        items:
          type: array
          minItems: 1
          maxItems: 5
          items:
            type: string
)";

    create_openapi_spec("test.yaml", spec);
    ASSERT_TRUE(run_codegen("test.yaml", "dto,validator"));

    auto validator_content = read_generated_file("generated_validators.hpp");
    EXPECT_NE(validator_content.find("min items: 1"), std::string::npos);
    EXPECT_NE(validator_content.find("max items: 5"), std::string::npos);
}
