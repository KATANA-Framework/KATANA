#include "katana/core/problem.hpp"

#include <gtest/gtest.h>

using namespace katana;

TEST(ProblemDetailsTest, DefaultConstructor) {
    problem_details p;

    EXPECT_EQ(p.type, "about:blank");
    EXPECT_EQ(p.title, "");
    EXPECT_EQ(p.status, 500);
    EXPECT_FALSE(p.detail.has_value());
    EXPECT_FALSE(p.instance.has_value());
    EXPECT_TRUE(p.extensions.empty());
}

TEST(ProblemDetailsTest, ToJsonMinimal) {
    problem_details p;
    p.type = "https://example.com/error";
    p.title = "Error Title";
    p.status = 400;

    std::string json = p.to_json();

    EXPECT_NE(json.find("\"type\":\"https://example.com/error\""), std::string::npos);
    EXPECT_NE(json.find("\"title\":\"Error Title\""), std::string::npos);
    EXPECT_NE(json.find("\"status\":400"), std::string::npos);
}

TEST(ProblemDetailsTest, ToJsonWithDetail) {
    problem_details p;
    p.type = "about:blank";
    p.title = "Error";
    p.status = 400;
    p.detail = "Detailed error message";

    std::string json = p.to_json();

    EXPECT_NE(json.find("\"detail\":\"Detailed error message\""), std::string::npos);
}

TEST(ProblemDetailsTest, ToJsonWithInstance) {
    problem_details p;
    p.type = "about:blank";
    p.title = "Error";
    p.status = 400;
    p.instance = "/api/v1/resource";

    std::string json = p.to_json();

    EXPECT_NE(json.find("\"instance\":\"/api/v1/resource\""), std::string::npos);
}

TEST(ProblemDetailsTest, ToJsonWithExtensions) {
    problem_details p;
    p.type = "about:blank";
    p.title = "Error";
    p.status = 400;
    p.extensions["custom_field"] = "custom_value";
    p.extensions["another_field"] = "another_value";

    std::string json = p.to_json();

    EXPECT_NE(json.find("\"custom_field\":\"custom_value\""), std::string::npos);
    EXPECT_NE(json.find("\"another_field\":\"another_value\""), std::string::npos);
}

TEST(ProblemDetailsTest, ToJsonComplete) {
    problem_details p;
    p.type = "https://example.com/problems/validation";
    p.title = "Validation Failed";
    p.status = 422;
    p.detail = "The request body failed validation";
    p.instance = "/api/v1/users/create";
    p.extensions["field"] = "email";
    p.extensions["reason"] = "invalid format";

    std::string json = p.to_json();

    EXPECT_NE(json.find("\"type\":\"https://example.com/problems/validation\""), std::string::npos);
    EXPECT_NE(json.find("\"title\":\"Validation Failed\""), std::string::npos);
    EXPECT_NE(json.find("\"status\":422"), std::string::npos);
    EXPECT_NE(json.find("\"detail\":\"The request body failed validation\""), std::string::npos);
    EXPECT_NE(json.find("\"instance\":\"/api/v1/users/create\""), std::string::npos);
    EXPECT_NE(json.find("\"field\":\"email\""), std::string::npos);
    EXPECT_NE(json.find("\"reason\":\"invalid format\""), std::string::npos);
}

TEST(ProblemDetailsTest, BadRequest) {
    auto p = problem_details::bad_request();

    EXPECT_EQ(p.status, 400);
    EXPECT_EQ(p.title, "Bad Request");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, BadRequestWithDetail) {
    auto p = problem_details::bad_request("Invalid input");

    EXPECT_EQ(p.status, 400);
    EXPECT_EQ(p.title, "Bad Request");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "Invalid input");
}

TEST(ProblemDetailsTest, BadRequestEmptyDetail) {
    auto p = problem_details::bad_request("");

    EXPECT_EQ(p.status, 400);
    EXPECT_EQ(p.title, "Bad Request");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, Unauthorized) {
    auto p = problem_details::unauthorized();

    EXPECT_EQ(p.status, 401);
    EXPECT_EQ(p.title, "Unauthorized");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, UnauthorizedWithDetail) {
    auto p = problem_details::unauthorized("Missing authentication token");

    EXPECT_EQ(p.status, 401);
    EXPECT_EQ(p.title, "Unauthorized");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "Missing authentication token");
}

TEST(ProblemDetailsTest, Forbidden) {
    auto p = problem_details::forbidden();

    EXPECT_EQ(p.status, 403);
    EXPECT_EQ(p.title, "Forbidden");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, ForbiddenWithDetail) {
    auto p = problem_details::forbidden("Insufficient permissions");

    EXPECT_EQ(p.status, 403);
    EXPECT_EQ(p.title, "Forbidden");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "Insufficient permissions");
}

TEST(ProblemDetailsTest, NotFound) {
    auto p = problem_details::not_found();

    EXPECT_EQ(p.status, 404);
    EXPECT_EQ(p.title, "Not Found");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, NotFoundWithDetail) {
    auto p = problem_details::not_found("Resource not found");

    EXPECT_EQ(p.status, 404);
    EXPECT_EQ(p.title, "Not Found");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "Resource not found");
}

TEST(ProblemDetailsTest, MethodNotAllowed) {
    auto p = problem_details::method_not_allowed();

    EXPECT_EQ(p.status, 405);
    EXPECT_EQ(p.title, "Method Not Allowed");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, MethodNotAllowedWithDetail) {
    auto p = problem_details::method_not_allowed("POST not allowed on this resource");

    EXPECT_EQ(p.status, 405);
    EXPECT_EQ(p.title, "Method Not Allowed");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "POST not allowed on this resource");
}

TEST(ProblemDetailsTest, Conflict) {
    auto p = problem_details::conflict();

    EXPECT_EQ(p.status, 409);
    EXPECT_EQ(p.title, "Conflict");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, ConflictWithDetail) {
    auto p = problem_details::conflict("Resource already exists");

    EXPECT_EQ(p.status, 409);
    EXPECT_EQ(p.title, "Conflict");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "Resource already exists");
}

TEST(ProblemDetailsTest, UnprocessableEntity) {
    auto p = problem_details::unprocessable_entity();

    EXPECT_EQ(p.status, 422);
    EXPECT_EQ(p.title, "Unprocessable Entity");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, UnprocessableEntityWithDetail) {
    auto p = problem_details::unprocessable_entity("Validation failed");

    EXPECT_EQ(p.status, 422);
    EXPECT_EQ(p.title, "Unprocessable Entity");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "Validation failed");
}

TEST(ProblemDetailsTest, InternalServerError) {
    auto p = problem_details::internal_server_error();

    EXPECT_EQ(p.status, 500);
    EXPECT_EQ(p.title, "Internal Server Error");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, InternalServerErrorWithDetail) {
    auto p = problem_details::internal_server_error("Database connection failed");

    EXPECT_EQ(p.status, 500);
    EXPECT_EQ(p.title, "Internal Server Error");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "Database connection failed");
}

TEST(ProblemDetailsTest, ServiceUnavailable) {
    auto p = problem_details::service_unavailable();

    EXPECT_EQ(p.status, 503);
    EXPECT_EQ(p.title, "Service Unavailable");
    EXPECT_FALSE(p.detail.has_value());
}

TEST(ProblemDetailsTest, ServiceUnavailableWithDetail) {
    auto p = problem_details::service_unavailable("System is under maintenance");

    EXPECT_EQ(p.status, 503);
    EXPECT_EQ(p.title, "Service Unavailable");
    ASSERT_TRUE(p.detail.has_value());
    EXPECT_EQ(*p.detail, "System is under maintenance");
}

TEST(ProblemDetailsTest, CopyConstructor) {
    problem_details p1;
    p1.type = "test";
    p1.title = "Test Title";
    p1.status = 404;
    p1.detail = "Test detail";
    p1.instance = "/test";
    p1.extensions["key"] = "value";

    problem_details p2(p1);

    EXPECT_EQ(p2.type, p1.type);
    EXPECT_EQ(p2.title, p1.title);
    EXPECT_EQ(p2.status, p1.status);
    EXPECT_EQ(p2.detail, p1.detail);
    EXPECT_EQ(p2.instance, p1.instance);
    EXPECT_EQ(p2.extensions, p1.extensions);
}

TEST(ProblemDetailsTest, MoveConstructor) {
    problem_details p1;
    p1.type = "test";
    p1.title = "Test Title";
    p1.status = 404;
    p1.detail = "Test detail";

    problem_details p2(std::move(p1));

    EXPECT_EQ(p2.type, "test");
    EXPECT_EQ(p2.title, "Test Title");
    EXPECT_EQ(p2.status, 404);
    ASSERT_TRUE(p2.detail.has_value());
    EXPECT_EQ(*p2.detail, "Test detail");
}

TEST(ProblemDetailsTest, CopyAssignment) {
    problem_details p1;
    p1.type = "test";
    p1.title = "Test Title";
    p1.status = 404;

    problem_details p2;
    p2 = p1;

    EXPECT_EQ(p2.type, p1.type);
    EXPECT_EQ(p2.title, p1.title);
    EXPECT_EQ(p2.status, p1.status);
}

TEST(ProblemDetailsTest, MoveAssignment) {
    problem_details p1;
    p1.type = "test";
    p1.title = "Test Title";
    p1.status = 404;

    problem_details p2;
    p2 = std::move(p1);

    EXPECT_EQ(p2.type, "test");
    EXPECT_EQ(p2.title, "Test Title");
    EXPECT_EQ(p2.status, 404);
}

TEST(ProblemDetailsTest, ToJsonEmptyExtensions) {
    problem_details p;
    p.type = "test";
    p.title = "Title";
    p.status = 200;

    std::string json = p.to_json();

    // Should still be valid JSON without extensions
    EXPECT_NE(json.find("{"), std::string::npos);
    EXPECT_NE(json.find("}"), std::string::npos);
}

TEST(ProblemDetailsTest, MultipleExtensions) {
    problem_details p;
    p.type = "test";
    p.title = "Title";
    p.status = 400;
    p.extensions["field1"] = "value1";
    p.extensions["field2"] = "value2";
    p.extensions["field3"] = "value3";

    std::string json = p.to_json();

    EXPECT_NE(json.find("\"field1\":\"value1\""), std::string::npos);
    EXPECT_NE(json.find("\"field2\":\"value2\""), std::string::npos);
    EXPECT_NE(json.find("\"field3\":\"value3\""), std::string::npos);
}
