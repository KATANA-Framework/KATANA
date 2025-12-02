#include "katana/core/openapi_ast.hpp"
#include "katana/core/openapi_loader.hpp"

#include <gtest/gtest.h>

using namespace katana;
using namespace katana::openapi;

TEST(OpenAPIAST, BuildDocumentWithSchemasAndOperations) {
    monotonic_arena arena;
    document doc(arena);

    auto& id_schema = doc.add_inline_schema();
    id_schema.kind = schema_kind::integer;
    id_schema.format = arena_string<>("int64", arena_allocator<char>(&arena));

    auto& user = doc.add_schema("User");
    user.kind = schema_kind::object;
    user.properties.emplace_back(
        property{arena_string<>("id", arena_allocator<char>(&arena)), &id_schema, true});
    user.properties.emplace_back(
        property{arena_string<>("name", arena_allocator<char>(&arena)), nullptr, true});

    auto& path = doc.add_path("/users/{id}");
    path.operations.emplace_back(&arena);
    auto& op = path.operations.back();
    op.operation_id = arena_string<>("getUser", arena_allocator<char>(&arena));
    op.parameters.emplace_back(&arena);
    auto& param = op.parameters.back();
    param.name = arena_string<>("id", arena_allocator<char>(&arena));
    param.in = param_location::path;
    param.required = true;
    param.type = &id_schema;
    param.description = arena_string<>("User ID", arena_allocator<char>(&arena));

    op.responses.emplace_back(&arena);
    auto& resp = op.responses.back();
    resp.status = 200;
    resp.description = arena_string<>("User found", arena_allocator<char>(&arena));
    resp.body = &user;

    EXPECT_EQ(doc.schemas.size(), 2U);
    EXPECT_EQ(doc.paths.size(), 1U);
    EXPECT_EQ(user.properties.size(), 2U);
    EXPECT_TRUE(user.properties[0].required);
    EXPECT_EQ(op.parameters.size(), 1U);
    EXPECT_EQ(op.responses.size(), 1U);
    EXPECT_EQ(op.responses[0].status, 200);
    EXPECT_EQ(op.parameters[0].type, &id_schema);
    EXPECT_EQ(op.responses[0].body, &user);
}

TEST(OpenAPILoader, RejectsEmpty) {
    monotonic_arena arena;
    auto res = openapi::load_from_string("", arena);
    EXPECT_FALSE(res);
    EXPECT_EQ(res.error(), make_error_code(error_code::openapi_parse_error));
}

TEST(OpenAPILoader, AcceptsVersionHint) {
    const std::string spec = R"({
      "openapi": "3.1.0",
      "info": { "title": "stub", "version": "1.0.0" },
      "paths": {}
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->openapi_version, "3.x");
    EXPECT_TRUE(res->paths.empty());
    EXPECT_EQ(res->info_title, "stub");
    EXPECT_EQ(res->info_version, "1.0.0");
}

TEST(OpenAPILoader, ParsesPathKeysAndMethodsShallow) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "svc", "version": "2.0" },
      "paths": {
        "/users/{id}": { "get": { "summary": "get user", "operationId": "getUser" }, "post": {} },
        "/health": { "get": {} }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
    ASSERT_EQ(res->paths.size(), 2U);
    EXPECT_EQ(res->paths[0].operations.size(), 2U);
    EXPECT_EQ(res->paths[1].operations.size(), 1U);
    EXPECT_EQ(res->paths[0].operations[0].method, http::method::get);
    EXPECT_EQ(res->paths[0].operations[1].method, http::method::post);
    EXPECT_EQ(res->paths[1].operations[0].method, http::method::get);
    EXPECT_EQ(res->paths[0].operations[0].summary, "get user");
    EXPECT_EQ(res->paths[0].operations[0].operation_id, "getUser");
}

TEST(OpenAPILoader, InvalidVersionRejected) {
    const std::string spec = R"({
      "openapi": "2.0.0",
      "info": { "title": "bad", "version": "0.1" },
      "paths": {}
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    EXPECT_FALSE(res);
    EXPECT_EQ(res.error(), make_error_code(error_code::openapi_invalid_spec));
}

TEST(OpenAPILoader, ParsesRequestBodyAndResponsesShallow) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "svc", "version": "1.0" },
      "paths": {
        "/items": {
          "post": {
            "operationId": "createItem",
            "requestBody": {
              "description": "body desc",
              "content": { "application/json": { "schema": { } } }
            },
            "responses": {
              "201": { "description": "created" },
              "400": { "description": "bad" }
            }
          }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
    ASSERT_EQ(res->paths.size(), 1U);
    ASSERT_EQ(res->paths[0].operations.size(), 1U);
    const auto& op = res->paths[0].operations[0];
    ASSERT_NE(op.body, nullptr);
    EXPECT_EQ(op.body->description, "body desc");
    EXPECT_EQ(op.body->content_type, "application/json");
    ASSERT_EQ(op.responses.size(), 2U);
    EXPECT_EQ(op.responses[0].status, 201);
    EXPECT_EQ(op.responses[0].description, "created");
    EXPECT_EQ(op.responses[1].status, 400);
    EXPECT_EQ(op.responses[1].description, "bad");
}

TEST(OpenAPILoader, ParsesSchemasShallowObjectArrayString) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "svc", "version": "1.0" },
      "paths": {
        "/items": {
          "post": {
            "requestBody": {
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object",
                    "properties": {
                      "name": { "type": "string", "minLength": 1, "nullable": true, "enum": ["a","b"] },
                      "tags": { "type": "array", "items": { "type": "string" } }
                    },
                    "required": ["name"]
                  }
                }
              }
            },
            "responses": { "200": { "description": "ok" } }
          }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
    ASSERT_EQ(res->paths.size(), 1U);
    ASSERT_EQ(res->paths[0].operations.size(), 1U);
    auto* body = res->paths[0].operations[0].body;
    ASSERT_NE(body, nullptr);
    EXPECT_EQ(body->content_type, "application/json");
    // We don't yet materialize schema tree into request_body; placeholder test to ensure no crash.
}
