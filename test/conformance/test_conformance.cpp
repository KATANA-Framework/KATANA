#include "conformance/generated/generated_dtos.hpp"
#include "conformance/generated/generated_handlers.hpp"
#include "conformance/generated/generated_router_bindings.hpp"
#include "katana/core/http.hpp"
#include "katana/core/router.hpp"
#include "support/http_handler_harness.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace katana;
using namespace katana::http;

namespace {

bool contains(std::string_view haystack, std::string_view needle) {
    return haystack.find(needle) != std::string_view::npos;
}

class ConformanceHandler final : public generated::api_handler {
public:
    int list_calls{0};
    int create_calls{0};
    int get_calls{0};
    int delete_calls{0};

    std::optional<int64_t> last_limit;
    std::string last_trace;
    std::optional<std::string> last_session;
    std::optional<int64_t> last_pet_id;
    std::string last_pet_name;
    int64_t last_pet_age{0};
    std::string last_owner_email;

    result<void> list_pets(std::optional<int64_t> limit,
                           std::string_view X_Trace,
                           std::optional<std::string_view> session,
                           response& out) override {
        ++list_calls;
        last_limit = limit;
        last_trace.assign(X_Trace);
        if (session) {
            last_session = std::string(*session);
        } else {
            last_session.reset();
        }

        std::string body = "{\"trace\":\"" + last_trace + "\",\"limitApplied\":" +
                           std::to_string(limit.value_or(25));
        if (last_session) {
            body += ",\"session\":\"" + *last_session + "\"";
        }
        body += "}";

        out.with_status(200).with_body(std::move(body));
        return {};
    }

    result<void> create_pet(const PetCreateRequest& body, response& out) override {
        ++create_calls;
        last_pet_name.assign(body.name.begin(), body.name.end());
        last_pet_age = body.age;
        last_owner_email.assign(body.ownerEmail.begin(), body.ownerEmail.end());

        out.with_status(201).with_body("{\"id\":101,\"name\":\"" + last_pet_name +
                                       "\",\"age\":" + std::to_string(last_pet_age) +
                                       ",\"ownerEmail\":\"" + last_owner_email + "\"}");
        return {};
    }

    result<void> get_pet(int64_t petId, response& out) override {
        ++get_calls;
        last_pet_id = petId;

        out.with_status(200).with_body("{\"id\":" + std::to_string(petId) +
                                       ",\"name\":\"pet-" + std::to_string(petId) +
                                       "\",\"age\":4,\"ownerEmail\":\"pet@example.com\"}");
        return {};
    }

    result<void> delete_pet(int64_t petId, response& out) override {
        ++delete_calls;
        last_pet_id = petId;
        respond::into(out).no_content();
        return {};
    }
};

class ConformanceTest : public ::testing::Test {
protected:
    ConformanceTest()
        : router(handler), harness([this](const request& req, monotonic_arena& arena) {
              request_context ctx{arena};
              response out;
              dispatch_or_problem(router.router(), req, ctx, out);
              return out;
          }) {}

    response run_request(std::string_view method_name,
                         std::string_view uri,
                         std::vector<std::pair<std::string, std::string>> headers = {},
                         std::string body = {}) {
        std::string raw;
        raw.reserve(256 + body.size());
        raw.append(method_name);
        raw.push_back(' ');
        raw.append(uri);
        raw.append(" HTTP/1.1\r\nHost: test\r\n");

        bool has_content_length = false;
        for (const auto& [name, value] : headers) {
            if (name == "Content-Length") {
                has_content_length = true;
            }
            raw.append(name);
            raw.append(": ");
            raw.append(value);
            raw.append("\r\n");
        }

        if (!body.empty() && !has_content_length) {
            raw.append("Content-Length: ");
            raw.append(std::to_string(body.size()));
            raw.append("\r\n");
        }

        raw.append("\r\n");
        raw.append(body);
        return harness.run_raw(std::move(raw));
    }

    static void expect_problem(const response& resp, int32_t status, std::string_view detail) {
        EXPECT_EQ(resp.status, status);
        EXPECT_TRUE(contains(resp.body, "\"status\":" + std::to_string(status)));
        EXPECT_TRUE(contains(resp.body, detail));
        auto content_type = resp.headers.get(field::content_type);
        ASSERT_TRUE(content_type.has_value());
        EXPECT_EQ(*content_type, "application/problem+json");
    }

    ConformanceHandler handler;
    generated::generated_router router;
    katana::test_support::HttpHandlerHarness harness;
};

TEST_F(ConformanceTest, ListPetsBindsQueryHeaderAndCookie) {
    auto resp = run_request("GET",
                            "/pets?limit=7",
                            {{"Accept", "application/json"},
                             {"X-Trace", "trace-123"},
                             {"Cookie", "session=sess-42"}});

    EXPECT_EQ(resp.status, 200);
    EXPECT_EQ(handler.list_calls, 1);
    EXPECT_EQ(handler.last_limit, std::optional<int64_t>(7));
    EXPECT_EQ(handler.last_trace, "trace-123");
    ASSERT_TRUE(handler.last_session.has_value());
    EXPECT_EQ(*handler.last_session, "sess-42");
    auto content_type = resp.headers.get(field::content_type);
    ASSERT_TRUE(content_type.has_value());
    EXPECT_EQ(*content_type, "application/json");
    EXPECT_TRUE(contains(resp.body, "\"trace\":\"trace-123\""));
    EXPECT_TRUE(contains(resp.body, "\"limitApplied\":7"));
    EXPECT_TRUE(contains(resp.body, "\"session\":\"sess-42\""));
}

TEST_F(ConformanceTest, ListPetsDefaultsToJsonWhenAcceptIsMissing) {
    auto resp = run_request("GET", "/pets", {{"X-Trace", "trace-default"}});

    EXPECT_EQ(resp.status, 200);
    EXPECT_EQ(handler.list_calls, 1);
    auto content_type = resp.headers.get(field::content_type);
    ASSERT_TRUE(content_type.has_value());
    EXPECT_EQ(*content_type, "application/json");
    EXPECT_TRUE(contains(resp.body, "\"limitApplied\":25"));
}

TEST_F(ConformanceTest, ListPetsRejectsMissingRequiredHeader) {
    auto resp = run_request("GET", "/pets", {{"Accept", "application/json"}});

    expect_problem(resp, 400, "missing param X-Trace");
    EXPECT_EQ(handler.list_calls, 0);
}

TEST_F(ConformanceTest, ListPetsRejectsInvalidQueryParam) {
    auto resp =
        run_request("GET", "/pets?limit=oops", {{"Accept", "application/json"}, {"X-Trace", "t"}});

    expect_problem(resp, 400, "invalid param limit");
    EXPECT_EQ(handler.list_calls, 0);
}

TEST_F(ConformanceTest, ListPetsRejectsNonJsonAcceptCodecBeforeHandler) {
    auto resp =
        run_request("GET", "/pets", {{"Accept", "application/cbor"}, {"X-Trace", "trace-cbor"}});

    expect_problem(resp, 501, "codec for Accept is not implemented");
    EXPECT_EQ(handler.list_calls, 0);
}

TEST_F(ConformanceTest, CreatePetAcceptsValidJson) {
    auto resp = run_request(
        "POST",
        "/pets",
        {{"Accept", "application/json"}, {"Content-Type", "application/json"}},
        R"({"name":"Milo","age":5,"ownerEmail":"owner@example.com"})");

    EXPECT_EQ(resp.status, 201);
    EXPECT_EQ(handler.create_calls, 1);
    EXPECT_EQ(handler.last_pet_name, "Milo");
    EXPECT_EQ(handler.last_pet_age, 5);
    EXPECT_EQ(handler.last_owner_email, "owner@example.com");
    auto content_type = resp.headers.get(field::content_type);
    ASSERT_TRUE(content_type.has_value());
    EXPECT_EQ(*content_type, "application/json");
    EXPECT_TRUE(contains(resp.body, "\"name\":\"Milo\""));
}

TEST_F(ConformanceTest, CreatePetRejectsMalformedJsonBeforeHandler) {
    auto resp = run_request("POST",
                            "/pets",
                            {{"Accept", "application/json"}, {"Content-Type", "application/json"}},
                            R"({"name":"Milo",)");

    expect_problem(resp, 400, "invalid request body");
    EXPECT_EQ(handler.create_calls, 0);
}

TEST_F(ConformanceTest, CreatePetRejectsValidationErrorBeforeHandler) {
    auto resp = run_request(
        "POST",
        "/pets",
        {{"Accept", "application/json"}, {"Content-Type", "application/json"}},
        R"({"name":"Mo","age":5,"ownerEmail":"bad-email"})");

    EXPECT_EQ(resp.status, 400);
    EXPECT_EQ(handler.create_calls, 0);
    EXPECT_TRUE(contains(resp.body, "\"status\":400"));
    EXPECT_TRUE(contains(resp.body, "name:"));
}

TEST_F(ConformanceTest, CreatePetRejectsUnsupportedMediaType) {
    auto resp = run_request("POST",
                            "/pets",
                            {{"Accept", "application/json"}, {"Content-Type", "application/xml"}},
                            "<pet/>");

    expect_problem(resp, 415, "unsupported Content-Type");
    EXPECT_EQ(handler.create_calls, 0);
}

TEST_F(ConformanceTest, CreatePetRejectsNonJsonRequestCodecBeforeHandler) {
    auto resp = run_request("POST",
                            "/pets",
                            {{"Accept", "application/json"}, {"Content-Type", "application/cbor"}},
                            "stub");

    expect_problem(resp, 501, "codec for Content-Type is not implemented");
    EXPECT_EQ(handler.create_calls, 0);
}

TEST_F(ConformanceTest, GetPetBindsPathParam) {
    auto resp = run_request("GET", "/pets/42", {{"Accept", "application/json"}});

    EXPECT_EQ(resp.status, 200);
    EXPECT_EQ(handler.get_calls, 1);
    EXPECT_EQ(handler.last_pet_id, std::optional<int64_t>(42));
    auto content_type = resp.headers.get(field::content_type);
    ASSERT_TRUE(content_type.has_value());
    EXPECT_EQ(*content_type, "application/json");
    EXPECT_TRUE(contains(resp.body, "\"id\":42"));
}

TEST_F(ConformanceTest, GetPetRejectsInvalidPathParam) {
    auto resp = run_request("GET", "/pets/abc", {{"Accept", "application/json"}});

    expect_problem(resp, 400, "invalid path param petId");
    EXPECT_EQ(handler.get_calls, 0);
}

TEST_F(ConformanceTest, DeletePetReturnsNoContent) {
    auto resp = run_request("DELETE", "/pets/77");

    EXPECT_EQ(resp.status, 204);
    EXPECT_EQ(handler.delete_calls, 1);
    EXPECT_EQ(handler.last_pet_id, std::optional<int64_t>(77));
    EXPECT_TRUE(resp.body.empty());
}

TEST_F(ConformanceTest, GeneratedRouterReturnsNotFound) {
    auto resp = run_request("GET", "/does-not-exist");

    expect_problem(resp, 404, "\"title\":\"Not Found\"");
}

TEST_F(ConformanceTest, GeneratedRouterReturnsMethodNotAllowedWithAllowHeader) {
    auto resp = run_request("PUT", "/pets/77");

    EXPECT_EQ(resp.status, 405);
    EXPECT_TRUE(contains(resp.body, "\"status\":405"));
    auto allow = resp.headers.get("Allow");
    ASSERT_TRUE(allow.has_value());
    EXPECT_EQ(*allow, "GET, DELETE");
}

} // namespace
