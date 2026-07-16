#include "katana/core/arena.hpp"
#include "katana/core/serde.hpp"

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

} // namespace

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

    bool run_codegen(const std::string& spec_file,
                     const std::string& emit = "all",
                     const std::string& extra_flags = "") {
        auto katana_gen = fs::path(KATANA_GEN_BIN);
        if (!fs::exists(katana_gen)) {
            katana_gen = fs::path("./katana_gen");
        }
        if (!fs::exists(katana_gen)) {
            return false;
        }

        std::string cmd = shell_quote(katana_gen) + " openapi -i " +
                          shell_quote(temp_dir / spec_file) + " -o " + shell_quote(temp_dir) +
                          " --emit " + emit;
        if (!extra_flags.empty()) {
            cmd += " " + extra_flags;
        }
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
    // Check for validation infrastructure (unified error types are always generated)
    EXPECT_NE(validator_content.find("validation"), std::string::npos);
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

    // A schema no operation references cannot be proven one-directional, so the default
    // --serdes server keeps both directions for it (library specs stay fully usable).
    auto json_content = read_generated_file("generated_json.hpp");
    EXPECT_FALSE(json_content.empty());
    EXPECT_NE(json_content.find("parse_Config"), std::string::npos);
    EXPECT_NE(json_content.find("serialize_Config"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, SerdesServerModePrunesDeadDirections) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Directional API
  version: 1.0.0
paths:
  /widgets:
    post:
      operationId: createWidget
      requestBody:
        required: true
        content:
          application/json:
            schema:
              $ref: '#/components/schemas/CreateWidgetRequest'
      responses:
        '200':
          description: OK
          content:
            application/json:
              schema:
                $ref: '#/components/schemas/Widget'
components:
  schemas:
    CreateWidgetRequest:
      type: object
      required: [name]
      properties:
        name:
          type: string
    Widget:
      type: object
      properties:
        id:
          type: integer
        name:
          type: string
)";

    create_openapi_spec("directional.yaml", spec);

    // Default (--serdes server): the server parses+validates request types and serializes
    // response types; the mirror halves are dead code and must not be emitted.
    ASSERT_TRUE(run_codegen("directional.yaml", "dto,validator,serdes"));
    auto json_content = read_generated_file("generated_json.hpp");
    EXPECT_NE(json_content.find("parse_CreateWidgetRequest"), std::string::npos);
    EXPECT_EQ(json_content.find("serialize_CreateWidgetRequest"), std::string::npos);
    EXPECT_NE(json_content.find("serialize_Widget"), std::string::npos);
    EXPECT_EQ(json_content.find("parse_Widget"), std::string::npos);
    auto validators = read_generated_file("generated_validators.hpp");
    EXPECT_NE(validators.find("validate_CreateWidgetRequest"), std::string::npos);
    EXPECT_EQ(validators.find("validate_Widget"), std::string::npos);

    // --serdes client mirrors the directions (a C++ integration-test client parses
    // responses and serializes requests).
    ASSERT_TRUE(run_codegen("directional.yaml", "dto,serdes", "--serdes client"));
    json_content = read_generated_file("generated_json.hpp");
    EXPECT_NE(json_content.find("parse_Widget"), std::string::npos);
    EXPECT_EQ(json_content.find("serialize_Widget"), std::string::npos);
    EXPECT_NE(json_content.find("serialize_CreateWidgetRequest"), std::string::npos);
    EXPECT_EQ(json_content.find("parse_CreateWidgetRequest"), std::string::npos);

    // --serdes all keeps every schema in both directions (the pre-pruning behavior).
    ASSERT_TRUE(run_codegen("directional.yaml", "dto,serdes", "--serdes all"));
    json_content = read_generated_file("generated_json.hpp");
    EXPECT_NE(json_content.find("parse_Widget"), std::string::npos);
    EXPECT_NE(json_content.find("serialize_Widget"), std::string::npos);
    EXPECT_NE(json_content.find("parse_CreateWidgetRequest"), std::string::npos);
    EXPECT_NE(json_content.find("serialize_CreateWidgetRequest"), std::string::npos);
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
    // Check that validator file contains validation logic
    EXPECT_NE(validator_content.find("validation"), std::string::npos);
    EXPECT_NE(validator_content.find("validate_Tags"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, GeneratesBindingsWithParamsAndNegotiation) {
    const char* spec = R"(
openapi: 3.0.0
info: { title: Test API, version: 1.0.0 }
paths:
  /items/{id}:
    post:
      operationId: updateItem
      parameters:
        - name: id
          in: path
          required: true
          schema: { type: integer }
        - name: page
          in: query
          schema: { type: integer }
        - name: X-Trace
          in: header
          required: true
          schema: { type: string }
        - name: session
          in: cookie
          schema: { type: string }
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                name: { type: string }
      responses:
        '200':
          description: ok
          content:
            application/json:
              schema: { type: string }
)";

    create_openapi_spec("test.yaml", spec);
    ASSERT_TRUE(run_codegen("test.yaml", "all"));

    auto handlers = read_generated_file("generated_handlers.hpp");
    auto bindings = read_generated_file("generated_router_bindings.hpp");
    EXPECT_NE(handlers.find("struct async_api_handler"), std::string::npos);
    EXPECT_NE(handlers.find("struct async_api_handler_base : api_handler, async_api_handler"),
              std::string::npos);
    EXPECT_NE(handlers.find("virtual bool update_item_async"), std::string::npos);
    EXPECT_NE(handlers.find("katana::http::async_response_writer out"), std::string::npos);
    EXPECT_NE(handlers.find("problem_details::not_implemented"), std::string::npos);
    EXPECT_NE(bindings.find("dynamic_cast<async_api_handler*>(&handler)"), std::string::npos);
    EXPECT_NE(bindings.find("ctx.reset_deferred_response()"), std::string::npos);
    EXPECT_NE(bindings.find("katana::http::async_response_writer"), std::string::npos);
    EXPECT_NE(bindings.find("query_param(req.uri, \"page\")"), std::string::npos);
    EXPECT_NE(bindings.find("req.headers.get(\"X-Trace\")"), std::string::npos);
    EXPECT_NE(bindings.find("cookie_param(req, \"session\")"), std::string::npos);
    EXPECT_NE(bindings.find("unsupported Content-Type"), std::string::npos);
    EXPECT_NE(bindings.find("not_acceptable"), std::string::npos);

    auto dtos = read_generated_file("generated_dtos.hpp");
    EXPECT_NE(dtos.find("updateItem_request"), std::string::npos); // inline schema gets a named id
    EXPECT_NE(dtos.find("layer:"), std::string::npos);             // layer banner is emitted
}

TEST_F(CodegenIntegrationTest, RouterBindingsUseStrictParamParsing) {
    const char* spec = R"(
openapi: 3.0.0
info: { title: Strict Params API, version: 1.0.0 }
paths:
  /flags/{enabled}:
    get:
      operationId: getFlags
      parameters:
        - name: enabled
          in: path
          required: true
          schema: { type: boolean }
        - name: page
          in: query
          required: true
          schema: { type: integer }
        - name: ratio
          in: query
          required: true
          schema: { type: number }
      responses:
        '200':
          description: ok
)";

    create_openapi_spec("strict_params.yaml", spec);
    ASSERT_TRUE(run_codegen("strict_params.yaml", "all"));

    auto bindings = read_generated_file("generated_router_bindings.hpp");
    EXPECT_NE(bindings.find("invalid path param enabled"), std::string::npos);
    EXPECT_NE(bindings.find("else if (*p_enabled == \"false\") enabled = false;"),
              std::string::npos);
    EXPECT_NE(bindings.find("if (ec != std::errc() || ptr != p_page->data() + p_page->size())"),
              std::string::npos);
    EXPECT_NE(bindings.find("if (ec != std::errc() || ptr != p_ratio->data() + p_ratio->size())"),
              std::string::npos);
}

TEST_F(CodegenIntegrationTest, GeneratesFormatValidators) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Format API
  version: 1.0.0
paths: {}
components:
  schemas:
    Formats:
      type: object
      properties:
        emailField:
          type: string
          format: email
        uuidField:
          type: string
          format: uuid
        ts:
          type: string
          format: date-time
)";

    create_openapi_spec("formats.yaml", spec);
    ASSERT_TRUE(run_codegen("formats.yaml", "dto,validator"));

    auto validator_content = read_generated_file("generated_validators.hpp");
    EXPECT_NE(validator_content.find("is_valid_email"), std::string::npos);
    EXPECT_NE(validator_content.find("invalid_email_format"), std::string::npos);
    EXPECT_NE(validator_content.find("invalid_uuid_format"), std::string::npos);
    EXPECT_NE(validator_content.find("invalid_datetime_format"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, RouterBindingsUseNegotiation) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Negotiation API
  version: 1.0.0
paths:
  /items:
    post:
      operationId: createItem
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                name:
                  type: string
      responses:
        '200':
          description: ok
          content:
            application/json:
              schema:
                type: string
)";

    create_openapi_spec("negotiation.yaml", spec);
    ASSERT_TRUE(run_codegen("negotiation.yaml")); // default emit all, generates router bindings

    auto bindings = read_generated_file("generated_router_bindings.hpp");
    EXPECT_NE(bindings.find("not_acceptable"), std::string::npos);
    EXPECT_NE(bindings.find("unsupported Content-Type"), std::string::npos);
    EXPECT_NE(bindings.find("set_header(\"Content-Type\""), std::string::npos);
}

TEST_F(CodegenIntegrationTest, RouterBindingsDefineJsonConstantForRequestOnlyJsonRoutes) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Request Only JSON API
  version: 1.0.0
paths:
  /jobs:
    post:
      operationId: createJob
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                name:
                  type: string
      responses:
        '204':
          description: accepted
)";

    create_openapi_spec("request_only_json.yaml", spec);
    ASSERT_TRUE(run_codegen("request_only_json.yaml"));

    auto bindings = read_generated_file("generated_router_bindings.hpp");
    EXPECT_NE(bindings.find("constexpr std::string_view kJsonContentType = \"application/json\""),
              std::string::npos);
    EXPECT_NE(bindings.find("katana::http_utils::detail::media_type_token(*content_type)"),
              std::string::npos);
}

TEST_F(CodegenIntegrationTest, RouterBindingsTranscodeNonJsonRequestCodecs) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Multi Codec Request API
  version: 1.0.0
paths:
  /jobs:
    post:
      operationId: createJob
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                name:
                  type: string
          application/cbor:
            schema:
              type: object
              properties:
                name:
                  type: string
      responses:
        '204':
          description: accepted
)";

    create_openapi_spec("request_multicodec.yaml", spec);
    ASSERT_TRUE(run_codegen("request_multicodec.yaml"));

    auto bindings = read_generated_file("generated_router_bindings.hpp");
    EXPECT_NE(
        bindings.find("const auto& request_content_type = content_types_0[*content_type_index];"),
        std::string::npos);
    EXPECT_NE(bindings.find("request_content_type.format != katana::http::media_format::json"),
              std::string::npos);
    // A declared binary body is transcoded to JSON and fed to the same generated parser (F11).
    EXPECT_NE(bindings.find("katana::serde::transcode_to_json(req.body, request_content_type.format)"),
              std::string::npos);
    EXPECT_NE(bindings.find("body_view = transcoded_body;"), std::string::npos);
    EXPECT_NE(bindings.find("(body_view, &ctx.arena)"), std::string::npos);
    // No longer stubbed for the request path.
    EXPECT_EQ(bindings.find("codec for Content-Type is not implemented"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, RouterBindingsStubNonJsonResponseCodecs) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Multi Codec Response API
  version: 1.0.0
paths:
  /jobs:
    get:
      operationId: listJobs
      responses:
        '200':
          description: ok
          content:
            application/json:
              schema:
                type: object
                properties:
                  name:
                    type: string
            application/cbor:
              schema:
                type: object
                properties:
                  name:
                    type: string
)";

    create_openapi_spec("response_multicodec.yaml", spec);
    ASSERT_TRUE(run_codegen("response_multicodec.yaml"));

    auto bindings = read_generated_file("generated_router_bindings.hpp");
    EXPECT_NE(bindings.find(
                  "auto negotiated_content_type = negotiate_response_type(req, content_types_0);"),
              std::string::npos);
    EXPECT_NE(bindings.find("katana::http::infer_media_format(response_content_type) != "
                            "katana::http::media_format::json"),
              std::string::npos);
    EXPECT_NE(bindings.find("problem_details::not_implemented"), std::string::npos);
    EXPECT_NE(bindings.find("codec for Accept is not implemented"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, EmitsXKatanaExtensionCommentsInGeneratedHandlers) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Extensions API
  version: 1.0.0
paths:
  /jobs:
    get:
      operationId: listJobs
      x-katana-cache: "5m"
      x-katana-alloc: pool
      x-katana-rate-limit: "100/s"
      x-katana-idempotency: required
      responses:
        '200':
          description: ok
)";

    create_openapi_spec("extensions.yaml", spec);
    ASSERT_TRUE(run_codegen("extensions.yaml", "all"));

    auto handlers = read_generated_file("generated_handlers.hpp");
    auto bindings = read_generated_file("generated_router_bindings.hpp");
    EXPECT_NE(handlers.find("// @cache: 5m"), std::string::npos);
    EXPECT_NE(handlers.find("// @alloc: pool"), std::string::npos);
    EXPECT_NE(handlers.find("// @rate-limit: 100/s"), std::string::npos);
    EXPECT_NE(handlers.find("// @idempotency: required"), std::string::npos);
    EXPECT_NE(bindings.find("route_policy_view"), std::string::npos);
    EXPECT_NE(bindings.find("route_policies_"), std::string::npos);
    EXPECT_NE(bindings.find("ctx.route_policy = &route_policies_[0]"), std::string::npos);
    EXPECT_NE(bindings.find("apply_route_policy_executor(req, ctx, out)"), std::string::npos);
    EXPECT_NE(bindings.find("route_idempotency_policy_view"), std::string::npos);
    EXPECT_NE(bindings.find("\"listJobs\""), std::string::npos);
}

TEST_F(CodegenIntegrationTest, InlineNamingFlagProducesFlatNames) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Inline Naming API
  version: 1.0.0
paths:
  /reports:
    post:
      operationId: createReport
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                id:
                  type: string
      responses:
        '201':
          description: created
          content:
            application/json:
              schema:
                type: object
                properties:
                  status:
                    type: string
)";

    create_openapi_spec("inline.yaml", spec);
    ASSERT_TRUE(run_codegen("inline.yaml", "all", "--inline-naming flat --dump-ast"));

    auto dto_content = read_generated_file("generated_dtos.hpp");
    EXPECT_NE(dto_content.find("InlineSchema1"), std::string::npos);
    EXPECT_NE(dto_content.find("InlineSchema2"), std::string::npos);

    auto ast_dump = read_generated_file("openapi_ast.json");
    EXPECT_NE(ast_dump.find("\"id\":\"InlineSchema1\""), std::string::npos);
}

TEST_F(CodegenIntegrationTest, SanitizesPropertyNamesAndKeepsOptionalPropertiesOptional) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Property Names API
  version: 1.0.0
paths: {}
components:
  schemas:
    WeirdFields:
      type: object
      required:
        - display-name
      properties:
        display-name:
          type: string
        class:
          type: integer
        tags:
          type: array
          items:
            type: integer
)";

    create_openapi_spec("weird_fields.yaml", spec);
    ASSERT_TRUE(run_codegen("weird_fields.yaml", "dto,serdes,validator"));

    auto dtos = read_generated_file("generated_dtos.hpp");
    EXPECT_NE(dtos.find("arena_string<> display_name;"), std::string::npos);
    EXPECT_NE(dtos.find("std::optional<int64_t> class_;"), std::string::npos);
    EXPECT_NE(dtos.find("std::optional<arena_vector<int64_t"), std::string::npos);

    auto json = read_generated_file("generated_json.hpp");
    EXPECT_NE(json.find("*key == \"display-name\""), std::string::npos);
    EXPECT_NE(json.find("obj.display_name ="), std::string::npos);
    EXPECT_NE(json.find("obj.tags.emplace(arena);"), std::string::npos);
    EXPECT_NE(json.find("(*obj.tags).push_back"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, GeneratesDistinctEnumIdentifiersAndEscapedPatterns) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Enum Pattern API
  version: 1.0.0
paths: {}
components:
  schemas:
    Mode:
      type: string
      enum: ["a-b", "a_b"]
    Patterned:
      type: object
      properties:
        value:
          type: string
          pattern: '^[A-Z]+\.[0-9]+$'
)";

    create_openapi_spec("enum_pattern.yaml", spec);
    ASSERT_TRUE(run_codegen("enum_pattern.yaml", "dto,validator"));

    auto dtos = read_generated_file("generated_dtos.hpp");
    EXPECT_NE(dtos.find("enum class Mode_enum"), std::string::npos);
    EXPECT_NE(dtos.find("a_b,"), std::string::npos);
    EXPECT_NE(dtos.find("a_b_2"), std::string::npos);
    EXPECT_NE(dtos.find("^[A-Z]+\\\\\\\\.[0-9]+$"), std::string::npos);

    auto validators = read_generated_file("generated_validators.hpp");
    EXPECT_NE(validators.find("std::regex re_{\"^[A-Z]+\\\\\\\\.[0-9]+$\"}"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, ValidatorsDoNotExitEarlyForOptionalUniqueItemsArrays) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Unique Items API
  version: 1.0.0
paths: {}
components:
  schemas:
    UniqueList:
      type: object
      properties:
        items:
          type: array
          uniqueItems: true
          items:
            type: integer
        label:
          type: string
          minLength: 3
)";

    create_openapi_spec("unique_items.yaml", spec);
    ASSERT_TRUE(run_codegen("unique_items.yaml", "dto,validator,serdes"));

    auto validators = read_generated_file("generated_validators.hpp");
    EXPECT_NE(validators.find("nullable/omitted array: uniqueness check does not apply"),
              std::string::npos);
    EXPECT_EQ(validators.find("if (!obj.items) {\n            return std::nullopt;"),
              std::string::npos);
    EXPECT_NE(validators.find("if ((*obj.items).size() <= 64)"), std::string::npos);
    EXPECT_EQ(validators.find("*obj.items.size()"), std::string::npos);
    EXPECT_NE(validators.find("obj.label"), std::string::npos);

    auto json = read_generated_file("generated_json.hpp");
    EXPECT_NE(json.find("json.append(\"\\\"items\\\":\");"), std::string::npos);
    EXPECT_NE(json.find("if (!obj.items) {\n        json.append(\"null\");\n    } else {"),
              std::string::npos);
    EXPECT_NE(json.find("obj.items->size()"), std::string::npos);
    EXPECT_EQ(json.find("json.append(\"null\");\n        break;"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, RouterBindingsUseOwnedRoutersAndSharedHttpUtilities) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Router Ownership API
  version: 1.0.0
paths:
  /items:
    post:
      operationId: createItem
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                name:
                  type: string
      responses:
        '200':
          description: ok
          content:
            application/json:
              schema:
                type: string
        '204':
          description: no content
)";

    create_openapi_spec("router_ownership.yaml", spec);
    ASSERT_TRUE(run_codegen("router_ownership.yaml", "all"));

    auto bindings = read_generated_file("generated_router_bindings.hpp");
    EXPECT_NE(bindings.find("class generated_router"), std::string::npos);
    EXPECT_NE(bindings.find("handler_ptr = &handler"), std::string::npos);
    const bool uses_generic_accept_negotiation =
        bindings.find("negotiate_response_type(req, route_0_produces)") != std::string::npos;
    const bool uses_single_json_accept_fast_path =
        bindings.find("constexpr std::string_view kJsonContentType = \"application/json\"") !=
            std::string::npos &&
        bindings.find("auto accept = req.headers.get(katana::http::field::accept);") !=
            std::string::npos;
    EXPECT_TRUE(uses_generic_accept_negotiation || uses_single_json_accept_fast_path);

    const bool uses_generic_content_type_negotiation =
        bindings.find("find_content_type(req.headers.get(katana::http::field::content_type), "
                      "route_0_consumes)") != std::string::npos;
    const bool uses_single_json_content_type_fast_path =
        bindings.find("katana::http_utils::detail::media_type_token(*content_type)") !=
            std::string::npos &&
        bindings.find("katana::http_utils::detail::ascii_iequals(") != std::string::npos;
    EXPECT_TRUE(uses_generic_content_type_negotiation || uses_single_json_content_type_fast_path);
    EXPECT_NE(bindings.find("out.status != 204 && !out.body.empty()"), std::string::npos);
    EXPECT_NE(bindings.find("class generated_server"), std::string::npos);
    EXPECT_EQ(bindings.find("static Handler handler_instance"), std::string::npos);
    EXPECT_EQ(bindings.find("static katana::http::router router_instance"), std::string::npos);
}

TEST_F(CodegenIntegrationTest, NamespaceFlagWrapsAllSymbolsForMultiContract) {
    const char* spec = R"(
openapi: 3.0.0
info:
  title: Users API
  version: 1.0.0
paths:
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
          description: ok
          content:
            application/json:
              schema:
                $ref: '#/components/schemas/User'
components:
  schemas:
    User:
      type: object
      required: [id]
      properties:
        id:
          type: integer
        name:
          type: string
)";
    create_openapi_spec("users.yaml", spec);
    ASSERT_TRUE(run_codegen("users.yaml", "all", "--namespace svc_users"));

    // DTOs: wrapped in the namespace, with includes kept OUTSIDE it (before the open brace).
    auto dtos = read_generated_file("generated_dtos.hpp");
    ASSERT_FALSE(dtos.empty());
    const size_t ns_open = dtos.find("namespace svc_users {");
    ASSERT_NE(ns_open, std::string::npos);
    EXPECT_NE(dtos.find("}  // namespace svc_users"), std::string::npos);
    const size_t struct_pos = dtos.find("struct User");
    EXPECT_NE(struct_pos, std::string::npos);
    EXPECT_GT(struct_pos, ns_open); // the DTO is inside the namespace
    // Every #include sits before the namespace opens (so <optional> etc. aren't svc_users::std).
    size_t scan = 0;
    while (true) {
        const size_t inc = dtos.find("#include", scan);
        if (inc == std::string::npos) {
            break;
        }
        EXPECT_LT(inc, ns_open);
        scan = inc + 1;
    }

    // Router bindings use the chosen namespace instead of the hardcoded `generated`.
    auto bindings = read_generated_file("generated_router_bindings.hpp");
    ASSERT_FALSE(bindings.empty());
    EXPECT_NE(bindings.find("namespace svc_users {"), std::string::npos);
    EXPECT_EQ(bindings.find("namespace generated {"), std::string::npos);
}
