#include "katana/core/openapi_ast.hpp"
#include "katana/core/openapi_loader.hpp"

#include <cmath>
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
    resp.content.emplace_back(&arena);
    resp.content.back().content_type =
        arena_string<>("application/json", arena_allocator<char>(&arena));
    resp.content.back().type = &user;

    EXPECT_EQ(doc.schemas.size(), 2U);
    EXPECT_EQ(doc.paths.size(), 1U);
    EXPECT_EQ(user.properties.size(), 2U);
    EXPECT_TRUE(user.properties[0].required);
    EXPECT_EQ(op.parameters.size(), 1U);
    EXPECT_EQ(op.responses.size(), 1U);
    EXPECT_EQ(op.responses[0].status, 200);
    EXPECT_EQ(op.parameters[0].type, &id_schema);
    ASSERT_FALSE(op.responses[0].content.empty());
    EXPECT_EQ(op.responses[0].content.front().type, &user);
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
    EXPECT_EQ(res->openapi_version, "3.1.0");
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
    const auto* body_media = op.body->first_media();
    ASSERT_NE(body_media, nullptr);
    EXPECT_EQ(body_media->content_type, "application/json");
    ASSERT_EQ(op.responses.size(), 2U);
    EXPECT_EQ(op.responses[0].status, 201);
    EXPECT_EQ(op.responses[0].description, "created");
    EXPECT_TRUE(op.responses[0].content.empty());
    EXPECT_EQ(op.responses[1].status, 400);
    EXPECT_EQ(op.responses[1].description, "bad");
}

TEST(OpenAPILoader, ParsesMultipleResponseContentAndDefault) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "svc", "version": "1.0" },
      "paths": {
        "/items": {
          "get": {
            "responses": {
              "200": {
                "description": "ok",
                "content": {
                  "application/json": { "schema": { "type": "object" } },
                  "application/xml": { "schema": { "type": "string" } }
                }
              },
              "default": {
                "description": "fail",
                "content": {
                  "application/problem+json": { "schema": { "type": "string" } }
                }
              }
            }
          }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
    ASSERT_EQ(res->paths.size(), 1U);
    const auto& op = res->paths[0].operations[0];
    ASSERT_EQ(op.responses.size(), 2U);

    const auto& ok = op.responses[0];
    EXPECT_EQ(ok.status, 200);
    EXPECT_FALSE(ok.is_default);
    ASSERT_EQ(ok.content.size(), 2U);
    EXPECT_EQ(ok.content[0].content_type, "application/json");
    EXPECT_EQ(ok.content[1].content_type, "application/xml");

    const auto& def = op.responses[1];
    EXPECT_TRUE(def.is_default);
    ASSERT_EQ(def.content.size(), 1U);
    EXPECT_EQ(def.content[0].content_type, "application/problem+json");
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
    const auto* media = body->first_media();
    ASSERT_NE(media, nullptr);
    EXPECT_EQ(media->content_type, "application/json");
    // Schema tree should be materialized.
    ASSERT_NE(media->type, nullptr);
}

TEST(OpenAPILoader, AcceptsYamlVersionHint) {
    const std::string spec = R"(openapi: 3.0.0
info:
  title: svc
  version: 2.0
paths: {}
)";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->openapi_version, "3.0.0");
    EXPECT_TRUE(res->paths.empty());
    EXPECT_EQ(res->info_title, "svc");
    EXPECT_EQ(res->info_version, "2.0");
}

TEST(OpenAPILoader, ParsesYamlWithArraysAndSchemas) {
    const std::string spec = R"(openapi: 3.0.0
info:
  title: svc
  version: 1.1
paths:
  /items:
    post:
      requestBody:
        content:
          application/json:
            schema:
              type: object
              properties:
                name:
                  type: string
                  pattern: "^[a-z]+$"
                tags:
                  type: array
                  items:
                    type: string
                  uniqueItems: true
                price:
                  type: number
                  multipleOf: 0.01
              required:
                - name
              additionalProperties: false
              discriminator: kind
      responses:
        '200':
          description: ok
)";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
    ASSERT_EQ(res->paths.size(), 1U);
    ASSERT_EQ(res->paths[0].operations.size(), 1U);
    const auto& op = res->paths[0].operations[0];
    ASSERT_NE(op.body, nullptr);
    const auto* media = op.body->first_media();
    ASSERT_NE(media, nullptr);
    EXPECT_EQ(media->content_type, "application/json");
    ASSERT_NE(media->type, nullptr);
    auto* schema = media->type;
    ASSERT_EQ(schema->properties.size(), 3U);
    const openapi::schema* name_schema = nullptr;
    const openapi::schema* tags = nullptr;
    const openapi::schema* price = nullptr;
    for (const auto& p : schema->properties) {
        if (p.name == "name") {
            name_schema = p.type;
            EXPECT_TRUE(p.required);
        } else if (p.name == "tags") {
            tags = p.type;
        } else if (p.name == "price") {
            price = p.type;
        }
    }
    ASSERT_NE(name_schema, nullptr);
    ASSERT_NE(tags, nullptr);
    EXPECT_EQ(tags->kind, schema_kind::array);
    EXPECT_TRUE(tags->unique_items);
    ASSERT_NE(tags->items, nullptr);
    EXPECT_EQ(tags->items->kind, schema_kind::string);
    ASSERT_NE(price, nullptr);
    EXPECT_EQ(price->kind, schema_kind::number);
    ASSERT_TRUE(price->multiple_of.has_value());
    EXPECT_TRUE(std::fabs(*price->multiple_of - 0.01) < 1e-6);
    EXPECT_FALSE(schema->additional_properties_allowed);
    EXPECT_EQ(schema->discriminator, "kind");
    ASSERT_EQ(op.responses.size(), 1U);
    EXPECT_EQ(op.responses[0].status, 200);
    EXPECT_EQ(op.responses[0].description, "ok");
}

TEST(OpenAPILoader, ParsesComponentSchemas) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "test", "version": "1.0" },
      "components": {
        "schemas": {
          "User": {
            "type": "object",
            "properties": {
              "id": { "type": "integer" },
              "name": { "type": "string" }
            }
          }
        }
      },
      "paths": {}
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);

    bool found_user = false;
    for (const auto& s : res->schemas) {
        if (s.name == "User") {
            found_user = true;
            EXPECT_EQ(s.kind, schema_kind::object);
            EXPECT_EQ(s.properties.size(), 2U);
        }
    }
    ASSERT_TRUE(found_user);
}

TEST(OpenAPILoader, ResolvesSimpleSchemaRef) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "test", "version": "1.0" },
      "components": {
        "schemas": {
          "User": {
            "type": "object",
            "properties": {
              "id": { "type": "integer" },
              "name": { "type": "string" }
            }
          }
        }
      },
      "paths": {
        "/users": {
          "post": {
            "requestBody": {
              "content": {
                "application/json": {
                  "schema": { "$ref": "#/components/schemas/User" }
                }
              }
            },
            "responses": { "201": { "description": "created" } }
          }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);

    bool found_user = false;
    for (const auto& s : res->schemas) {
        if (s.name == "User") {
            found_user = true;
            EXPECT_EQ(s.kind, schema_kind::object);
            EXPECT_EQ(s.properties.size(), 2U);
        }
    }
    ASSERT_TRUE(found_user);

    ASSERT_EQ(res->paths.size(), 1U);
    const auto& op = res->paths[0].operations[0];
    ASSERT_NE(op.body, nullptr);
    const auto* media = op.body->first_media();
    ASSERT_NE(media, nullptr);
    ASSERT_NE(media->type, nullptr);
    EXPECT_EQ(media->type->kind, schema_kind::object);
    EXPECT_EQ(media->type->properties.size(), 2U);
}

TEST(OpenAPILoader, ResolvesNestedSchemaRef) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "test", "version": "1.0" },
      "components": {
        "schemas": {
          "Address": {
            "type": "object",
            "properties": {
              "street": { "type": "string" },
              "city": { "type": "string" }
            }
          },
          "User": {
            "type": "object",
            "properties": {
              "id": { "type": "integer" },
              "address": { "$ref": "#/components/schemas/Address" }
            }
          }
        }
      },
      "paths": {
        "/users": {
          "post": {
            "requestBody": {
              "content": {
                "application/json": {
                  "schema": { "$ref": "#/components/schemas/User" }
                }
              }
            },
            "responses": { "201": { "description": "created" } }
          }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
}

TEST(OpenAPILoader, HandlesMissingSchemaRef) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "test", "version": "1.0" },
      "paths": {
        "/users": {
          "post": {
            "requestBody": {
              "content": {
                "application/json": {
                  "schema": { "$ref": "#/components/schemas/NonExistent" }
                }
              }
            },
            "responses": { "201": { "description": "created" } }
          }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
}

TEST(OpenAPILoader, MergesAllOfSchemas) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "test", "version": "1.0" },
      "components": {
        "schemas": {
          "Base": {
            "type": "object",
            "properties": {
              "id": { "type": "integer" },
              "created": { "type": "string", "format": "date-time" }
            },
            "required": ["id"]
          },
          "WithName": {
            "type": "object",
            "properties": {
              "name": { "type": "string", "minLength": 1, "maxLength": 100 }
            },
            "required": ["name"]
          },
          "User": {
            "allOf": [
              { "$ref": "#/components/schemas/Base" },
              { "$ref": "#/components/schemas/WithName" },
              {
                "type": "object",
                "properties": {
                  "email": { "type": "string", "format": "email" }
                }
              }
            ]
          }
        }
      },
      "paths": {}
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);

    const schema* user = nullptr;
    for (const auto& s : res->schemas) {
        if (s.name == "User") {
            user = &s;
            break;
        }
    }
    ASSERT_NE(user, nullptr);
    EXPECT_EQ(user->kind, schema_kind::object);

    // After merge, User should have properties from all allOf schemas
    EXPECT_EQ(user->properties.size(), 4U);

    bool has_id = false, has_name = false, has_email = false, has_created = false;
    for (const auto& prop : user->properties) {
        if (prop.name == "id") {
            has_id = true;
            EXPECT_EQ(prop.type->kind, schema_kind::integer);
        } else if (prop.name == "name") {
            has_name = true;
            EXPECT_EQ(prop.type->kind, schema_kind::string);
            EXPECT_EQ(prop.type->min_length, 1U);
            EXPECT_EQ(prop.type->max_length, 100U);
        } else if (prop.name == "email") {
            has_email = true;
            EXPECT_EQ(prop.type->kind, schema_kind::string);
            EXPECT_EQ(prop.type->format, "email");
        } else if (prop.name == "created") {
            has_created = true;
            EXPECT_EQ(prop.type->kind, schema_kind::string);
            EXPECT_EQ(prop.type->format, "date-time");
        }
    }
    EXPECT_TRUE(has_id);
    EXPECT_TRUE(has_name);
    EXPECT_TRUE(has_email);
    EXPECT_TRUE(has_created);

    // allOf should be cleared after merge
    EXPECT_TRUE(user->all_of.empty());
}

TEST(OpenAPILoader, RejectsDuplicateOperationId) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "test", "version": "1.0" },
      "paths": {
        "/users": {
          "get": { "operationId": "getUser", "responses": { "200": { "description": "ok" } } }
        },
        "/items": {
          "get": { "operationId": "getUser", "responses": { "200": { "description": "ok" } } }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    EXPECT_FALSE(res);
    EXPECT_EQ(res.error(), make_error_code(error_code::openapi_invalid_spec));
}

TEST(OpenAPILoader, RejectsInvalidHttpCode) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "test", "version": "1.0" },
      "paths": {
        "/users": {
          "get": {
            "responses": {
              "999": { "description": "invalid code" }
            }
          }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    EXPECT_FALSE(res);
    EXPECT_EQ(res.error(), make_error_code(error_code::openapi_invalid_spec));
}

TEST(OpenAPILoader, AcceptsValidHttpCodes) {
    const std::string spec = R"({
      "openapi": "3.0.0",
      "info": { "title": "test", "version": "1.0" },
      "paths": {
        "/users": {
          "get": {
            "responses": {
              "200": { "description": "ok" },
              "404": { "description": "not found" },
              "500": { "description": "error" },
              "default": { "description": "default" }
            }
          }
        }
      }
    })";
    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);
    ASSERT_TRUE(res);
}
