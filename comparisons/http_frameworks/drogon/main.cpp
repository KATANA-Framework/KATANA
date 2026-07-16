#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <string_view>

#include <netinet/tcp.h>
#include <sys/socket.h>

namespace {

constexpr uint16_t kDefaultPort = 8080;
constexpr size_t kDefaultWorkers = 4;
constexpr size_t kMaxItems = 1024;
constexpr int64_t kDefaultSeedCount = 4096;
constexpr std::string_view kCreateTableSql =
    "CREATE TABLE IF NOT EXISTS katana_stage4_items ("
    "id BIGSERIAL PRIMARY KEY, "
    "name TEXT NOT NULL, "
    "description TEXT, "
    "price DOUBLE PRECISION NOT NULL, "
    "stock BIGINT, "
    "category TEXT NOT NULL, "
    "created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(), "
    "updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW()"
    ")";
constexpr std::string_view kCategoryIndexSql =
    "CREATE INDEX IF NOT EXISTS katana_stage4_items_category_id_cover_idx "
    "ON katana_stage4_items (category, id) INCLUDE (name, description, price, stock)";
constexpr std::string_view kDisableAutovacuumSql =
    "ALTER TABLE katana_stage4_items SET (autovacuum_enabled = false, "
    "toast.autovacuum_enabled = false)";
constexpr std::string_view kRestoreAutovacuumSql =
    "ALTER TABLE katana_stage4_items RESET (autovacuum_enabled, toast.autovacuum_enabled)";
constexpr std::string_view kResetItemsSql = "TRUNCATE katana_stage4_items RESTART IDENTITY";
constexpr std::string_view kAnalyzeItemsSql = "ANALYZE katana_stage4_items";
constexpr std::string_view kSeedItemsSql =
    "INSERT INTO katana_stage4_items (name, description, price, stock, category) "
    "SELECT "
    "'Seed-' || gs::text, "
    "'Seed item', "
    "10.0 + (gs % 100)::double precision, "
    "100 + (gs % 50)::bigint, "
    "CASE (gs % 6) "
    "  WHEN 0 THEN 'electronics' "
    "  WHEN 1 THEN 'books' "
    "  WHEN 2 THEN 'clothing' "
    "  WHEN 3 THEN 'food' "
    "  WHEN 4 THEN 'tools' "
    "  ELSE 'other' "
    "END "
    "FROM generate_series(1, $1::bigint) AS gs";
constexpr std::string_view kListItemsAllSql =
    "SELECT "
    "page.id::bigint AS id, "
    "page.name::text AS name, "
    "page.description::text AS description, "
    "page.price::float8 AS price, "
    "page.stock::bigint AS stock, "
    "page.category::text AS category_name, "
    "counts.total_count::bigint AS total_count "
    "FROM ( "
    "  SELECT COUNT(*)::bigint AS total_count "
    "  FROM katana_stage4_items "
    ") AS counts "
    "LEFT JOIN LATERAL ( "
    "  SELECT id, name, description, price, stock, category "
    "  FROM katana_stage4_items "
    "  ORDER BY id "
    "  LIMIT $1::bigint OFFSET $2::bigint "
    ") AS page ON TRUE";
constexpr std::string_view kListItemsByCategorySql =
    "SELECT "
    "page.id::bigint AS id, "
    "page.name::text AS name, "
    "page.description::text AS description, "
    "page.price::float8 AS price, "
    "page.stock::bigint AS stock, "
    "page.category::text AS category_name, "
    "counts.total_count::bigint AS total_count "
    "FROM ( "
    "  SELECT COUNT(*)::bigint AS total_count "
    "  FROM katana_stage4_items "
    "  WHERE category = $3::text "
    ") AS counts "
    "LEFT JOIN LATERAL ( "
    "  SELECT id, name, description, price, stock, category "
    "  FROM katana_stage4_items "
    "  WHERE category = $3::text "
    "  ORDER BY id "
    "  LIMIT $1::bigint OFFSET $2::bigint "
    ") AS page ON TRUE";
constexpr std::string_view kGetItemSql =
    "SELECT "
    "id::bigint AS id, "
    "name::text AS name, "
    "description::text AS description, "
    "price::float8 AS price, "
    "stock::bigint AS stock, "
    "category::text AS category_name "
    "FROM katana_stage4_items "
    "WHERE id = $1::bigint";
constexpr std::string_view kCreateItemSql =
    "INSERT INTO katana_stage4_items ("
    "  name, description, price, stock, category"
    ") VALUES ("
    "  $1::text, "
    "  CASE WHEN $2::bool THEN $3::text ELSE NULL END, "
    "  $4::float8, "
    "  CASE WHEN $5::bool THEN $6::bigint ELSE NULL END, "
    "  $7::text"
    ") RETURNING "
    "id::bigint AS id, "
    "name::text AS name, "
    "description::text AS description, "
    "price::float8 AS price, "
    "stock::bigint AS stock, "
    "category::text AS category_name";
constexpr std::string_view kUpdateItemSql =
    "UPDATE katana_stage4_items "
    "SET "
    "name = CASE WHEN $2::bool THEN $3::text ELSE name END, "
    "description = CASE WHEN $4::bool THEN $5::text ELSE description END, "
    "price = CASE WHEN $6::bool THEN $7::float8 ELSE price END, "
    "stock = CASE WHEN $8::bool THEN $9::bigint ELSE stock END, "
    "category = CASE WHEN $10::bool THEN $11::text ELSE category END, "
    "updated_at = NOW() "
    "WHERE id = $1::bigint "
    "RETURNING "
    "id::bigint AS id, "
    "name::text AS name, "
    "description::text AS description, "
    "price::float8 AS price, "
    "stock::bigint AS stock, "
    "category::text AS category_name";
constexpr std::string_view kDeleteItemSql =
    "DELETE FROM katana_stage4_items WHERE id = $1::bigint";

uint16_t read_port() {
    for (const char* key : {"PORT", "HELLO_PORT", "COMPUTE_PORT"}) {
        if (const char* value = std::getenv(key)) {
            const int parsed = std::atoi(value);
            if (parsed > 0 && parsed < 65536) {
                return static_cast<uint16_t>(parsed);
            }
        }
    }
    return kDefaultPort;
}

size_t read_workers() {
    for (const char* key : {"BENCH_WORKERS", "KATANA_WORKERS", "WORKERS"}) {
        if (const char* value = std::getenv(key)) {
            const int parsed = std::atoi(value);
            if (parsed > 0) {
                return static_cast<size_t>(parsed);
            }
        }
    }
    return kDefaultWorkers;
}

std::size_t read_size_env(const char* name, std::size_t fallback) {
    const char* raw = std::getenv(name);
    if (raw == nullptr || *raw == '\0') {
        return fallback;
    }

    std::size_t parsed = 0;
    const auto* begin = raw;
    const auto* end = raw + std::char_traits<char>::length(raw);
    const auto [ptr, ec] = std::from_chars(begin, end, parsed);
    if (ec != std::errc{} || ptr != end) {
        return fallback;
    }
    return parsed;
}

bool read_bool_env(const char* name, bool fallback) {
    const char* raw = std::getenv(name);
    if (raw == nullptr || *raw == '\0') {
        return fallback;
    }

    const std::string_view value(raw);
    if (value == "1" || value == "true" || value == "yes") {
        return true;
    }
    if (value == "0" || value == "false" || value == "no") {
        return false;
    }
    return fallback;
}

std::optional<std::string> read_postgres_dsn() {
    for (const char* key : {"KATANA_BENCHMARK_API_POSTGRES_DSN", "KATANA_TEST_POSTGRES_DSN"}) {
        if (const char* value = std::getenv(key)) {
            if (*value != '\0') {
                return std::string(value);
            }
        }
    }
    return std::nullopt;
}

bool is_valid_category(std::string_view value) {
    for (const std::string_view candidate :
         {"electronics", "books", "clothing", "food", "tools", "other"}) {
        if (candidate == value) {
            return true;
        }
    }
    return false;
}

std::optional<int64_t> parse_i64(std::string_view value) {
    if (value.empty()) {
        return std::nullopt;
    }
    int64_t parsed = 0;
    const auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), parsed);
    if (ec != std::errc{} || ptr != value.data() + value.size()) {
        return std::nullopt;
    }
    return parsed;
}

std::optional<std::string> normalize_optional_text(const Json::Value& value) {
    if (value.isNull()) {
        return std::nullopt;
    }
    if (!value.isString()) {
        return std::nullopt;
    }

    const std::string text = value.asString();
    if (text.empty()) {
        return std::nullopt;
    }
    return text;
}

bool is_valid_uuid(std::string_view value) {
    static const std::regex pattern(
        "^[0-9a-fA-F]{8}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{12}$");
    return std::regex_match(value.begin(), value.end(), pattern);
}

drogon::HttpResponsePtr make_text_response(drogon::HttpStatusCode status, std::string body) {
    auto response = drogon::HttpResponse::newHttpResponse();
    response->setStatusCode(status);
    response->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    response->setBody(std::move(body));
    return response;
}

Json::Value row_to_item_json(const drogon::orm::Row& row) {
    Json::Value item(Json::objectValue);
    item["id"] = Json::Int64(row["id"].as<int64_t>());
    item["name"] = row["name"].as<std::string>();
    if (!row["description"].isNull()) {
        item["description"] = row["description"].as<std::string>();
    }
    item["price"] = row["price"].as<double>();
    if (!row["stock"].isNull()) {
        item["stock"] = Json::Int64(row["stock"].as<int64_t>());
    }
    item["category"] = row["category_name"].as<std::string>();
    return item;
}

Json::Value list_result_to_json(const drogon::orm::Result& rows, int64_t limit, int64_t offset) {
    Json::Value out(Json::objectValue);
    Json::Value items(Json::arrayValue);
    int64_t total = 0;
    for (const auto& row : rows) {
        if (!row["total_count"].isNull()) {
            total = row["total_count"].as<int64_t>();
        }
        if (row["id"].isNull()) {
            continue;
        }
        items.append(row_to_item_json(row));
    }
    out["items"] = std::move(items);
    out["total"] = Json::Int64(total);
    out["limit"] = Json::Int64(limit);
    out["offset"] = Json::Int64(offset);
    return out;
}

template <typename Callback>
void with_db_exceptions(Callback&& callback,
                        std::function<void(const drogon::HttpResponsePtr&)> out) {
    try {
        callback();
    } catch (const std::exception& error) {
        out(make_text_response(drogon::k500InternalServerError, error.what()));
    }
}

struct app_state {
    std::shared_ptr<drogon::orm::DbClient> db;
    std::chrono::steady_clock::time_point started_at = std::chrono::steady_clock::now();
};

bool bootstrap_stage4_fixture(app_state& state) {
    if (!state.db) {
        return true;
    }

    state.db->execSqlSync(std::string(kCreateTableSql));
    state.db->execSqlSync(std::string(kCategoryIndexSql));

    if (read_bool_env("KATANA_BENCHMARK_API_DISABLE_AUTOVACUUM", false)) {
        state.db->execSqlSync(std::string(kDisableAutovacuumSql));
    } else {
        state.db->execSqlSync(std::string(kRestoreAutovacuumSql));
    }

    if (read_bool_env("KATANA_BENCHMARK_API_RESET", true)) {
        state.db->execSqlSync(std::string(kResetItemsSql));
    }

    const auto seed_count =
        static_cast<int64_t>(read_size_env("KATANA_BENCHMARK_API_SEED_COUNT", kDefaultSeedCount));
    if (seed_count > 0) {
        state.db->execSqlSync(std::string(kSeedItemsSql), seed_count);
        state.db->execSqlSync(std::string(kAnalyzeItemsSql));
    }

    return true;
}

} // namespace

int main() {
    using namespace drogon;

    const uint16_t port = read_port();
    const size_t workers = read_workers();
    auto state = std::make_shared<app_state>();
    if (const auto dsn = read_postgres_dsn()) {
        // Same pool sizing formula as the deadpool/psycopg comparison targets:
        // workers * 4 clamped to [4, 64] connections.
        const std::size_t pool_size = std::clamp<std::size_t>(workers * 4, 4, 64);
        state->db = drogon::orm::DbClient::newPgClient(*dsn, pool_size);
        if (read_bool_env("KATANA_BENCHMARK_API_BOOTSTRAP", true)) {
            try {
                bootstrap_stage4_fixture(*state);
            } catch (const std::exception& error) {
                std::cerr << "drogon comparison server failed to bootstrap SQL fixture: "
                          << error.what() << std::endl;
                return 1;
            }
        }
    }

    app().setLogLevel(trantor::Logger::kWarn);
    app().setThreadNum(workers);
    app().setKeepaliveRequestsNumber(0);
    app().setPipeliningRequestsNumber(0);
    app().setIdleConnectionTimeout(0);
    app().setAfterAcceptSockOptCallback([](int fd) {
        int enabled = 1;
        (void)::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enabled, sizeof(enabled));
    });
    app().addListener("0.0.0.0", port);

    app().registerHandler(
        "/",
        [](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& callback) {
            callback(make_text_response(k200OK, "Hello, World!"));
        },
        {Get});

    app().registerHandler(
        "/compute/sum",
        [](const HttpRequestPtr& request, std::function<void(const HttpResponsePtr&)>&& callback) {
            const auto json = request->getJsonObject();
            if (!json || !json->isArray()) {
                callback(make_text_response(k400BadRequest, "payload must be a JSON array"));
                return;
            }

            const Json::ArrayIndex size = json->size();
            if (size == 0 || size > static_cast<Json::ArrayIndex>(kMaxItems)) {
                callback(
                    make_text_response(k400BadRequest, "payload must contain 1..=1024 numbers"));
                return;
            }

            double total = 0.0;
            for (const auto& item : *json) {
                if (!item.isNumeric()) {
                    callback(make_text_response(k400BadRequest,
                                                "payload must contain only numeric values"));
                    return;
                }
                total += item.asDouble();
            }

            Json::Value output(total);
            callback(HttpResponse::newHttpJsonResponse(output));
        },
        {Post});

    app().registerHandler(
        "/json",
        [](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& callback) {
            Json::Value payload(Json::objectValue);
            payload["message"] = "Hello, World!";
            callback(HttpResponse::newHttpJsonResponse(payload));
        },
        {Get});

    app().registerHandler(
        "/echo",
        [](const HttpRequestPtr& request, std::function<void(const HttpResponsePtr&)>&& callback) {
            const auto json = request->getJsonObject();
            if (!json || !json->isObject()) {
                callback(make_text_response(k400BadRequest, "payload must be a JSON object"));
                return;
            }
            const auto& body = *json;

            if (!body["message"].isString()) {
                callback(make_text_response(k400BadRequest, "message is required"));
                return;
            }
            const std::string message = body["message"].asString();
            if (message.size() > 4096U) {
                callback(make_text_response(k400BadRequest,
                                            "message must be <= 4096 characters"));
                return;
            }
            int64_t repeat = 1;
            if (body.isMember("repeat")) {
                if (!body["repeat"].isIntegral()) {
                    callback(make_text_response(k400BadRequest, "repeat must be 1..=100"));
                    return;
                }
                repeat = body["repeat"].asInt64();
            }
            if (repeat < 1 || repeat > 100) {
                callback(make_text_response(k400BadRequest, "repeat must be 1..=100"));
                return;
            }
            const bool uppercase =
                body.isMember("uppercase") && body["uppercase"].isBool() &&
                body["uppercase"].asBool();

            std::string payload;
            payload.reserve(message.size() * static_cast<std::size_t>(repeat));
            for (int64_t i = 0; i < repeat; ++i) {
                payload += message;
            }
            if (uppercase) {
                for (char& c : payload) {
                    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                }
            }

            Json::Value out(Json::objectValue);
            out["length"] = Json::Int64(static_cast<int64_t>(payload.size()));
            out["message"] = std::move(payload);
            callback(HttpResponse::newHttpJsonResponse(out));
        },
        {Post});

    app().registerHandler(
        "/compute/stats",
        [](const HttpRequestPtr& request, std::function<void(const HttpResponsePtr&)>&& callback) {
            const auto json = request->getJsonObject();
            if (!json || !json->isObject()) {
                callback(make_text_response(k400BadRequest, "payload must be a JSON object"));
                return;
            }
            const auto& body = *json;

            if (!body["values"].isArray()) {
                callback(make_text_response(k400BadRequest, "values is required"));
                return;
            }
            const auto& values = body["values"];
            const Json::ArrayIndex size = values.size();
            if (size == 0 || size > 10'000U) {
                callback(make_text_response(k400BadRequest,
                                            "values must contain 1..=10000 numbers"));
                return;
            }

            std::vector<double> parsed;
            parsed.reserve(size);
            for (const auto& item : values) {
                if (!item.isNumeric()) {
                    callback(make_text_response(k400BadRequest,
                                                "values must contain only numeric values"));
                    return;
                }
                parsed.push_back(item.asDouble());
            }

            double sum = 0.0;
            double min_value = parsed.front();
            double max_value = parsed.front();
            for (const double value : parsed) {
                sum += value;
                min_value = std::min(min_value, value);
                max_value = std::max(max_value, value);
            }

            Json::Value out(Json::objectValue);
            out["min"] = min_value;
            out["max"] = max_value;
            out["mean"] = sum / static_cast<double>(parsed.size());
            out["sum"] = sum;
            out["count"] = Json::Int64(static_cast<int64_t>(parsed.size()));

            const bool include_median = body.isMember("include_median") &&
                                        body["include_median"].isBool() &&
                                        body["include_median"].asBool();
            if (include_median) {
                std::vector<double> sorted = parsed;
                std::sort(sorted.begin(), sorted.end());
                const std::size_t count = sorted.size();
                out["median"] = (count % 2U == 0U)
                                    ? (sorted[count / 2U - 1U] + sorted[count / 2U]) * 0.5
                                    : sorted[count / 2U];
            }

            callback(HttpResponse::newHttpJsonResponse(out));
        },
        {Post});

    app().registerHandler(
        "/health",
        [state](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& callback) {
            Json::Value payload(Json::objectValue);
            payload["status"] = "ok";
            payload["uptime_ms"] = Json::Int64(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - state->started_at)
                    .count());
            callback(HttpResponse::newHttpJsonResponse(payload));
        },
        {Get});

    app().registerHandler(
        "/items",
        [state](const HttpRequestPtr& request,
                std::function<void(const HttpResponsePtr&)>&& callback) {
            if (!state->db) {
                callback(make_text_response(k503ServiceUnavailable,
                                            "postgres is not configured for this target"));
                return;
            }

            with_db_exceptions(
                [&]() {
                    if (request->method() == Get) {
                        const auto limit =
                            parse_i64(request->getParameter("limit")).value_or(10);
                        const auto offset =
                            parse_i64(request->getParameter("offset")).value_or(0);
                        if (limit < 1 || limit > 100) {
                            callback(make_text_response(k400BadRequest,
                                                        "limit must be 1..=100"));
                            return;
                        }
                        if (offset < 0) {
                            callback(make_text_response(k400BadRequest, "offset must be >= 0"));
                            return;
                        }

                        const std::string category = request->getParameter("category");
                        drogon::orm::Result rows(nullptr);
                        if (!category.empty()) {
                            if (!is_valid_category(category)) {
                                callback(make_text_response(k400BadRequest, "invalid category"));
                                return;
                            }
                            rows = state->db->execSqlSync(
                                std::string(kListItemsByCategorySql), limit, offset, category);
                        } else {
                            rows =
                                state->db->execSqlSync(std::string(kListItemsAllSql), limit, offset);
                        }
                        callback(HttpResponse::newHttpJsonResponse(
                            list_result_to_json(rows, limit, offset)));
                        return;
                    }

                    const auto request_id = request->getHeader("X-Request-Id");
                    if (request_id.empty() || !is_valid_uuid(request_id)) {
                        callback(make_text_response(k400BadRequest,
                                                    "X-Request-Id must be a UUID"));
                        return;
                    }

                    const auto json = request->getJsonObject();
                    if (!json || !json->isObject()) {
                        callback(
                            make_text_response(k400BadRequest, "payload must be a JSON object"));
                        return;
                    }
                    const auto& body = *json;

                    if (!body["name"].isString() || body["name"].asString().empty() ||
                        body["name"].asString().size() > 200U) {
                        callback(make_text_response(k400BadRequest,
                                                    "name must be 1..=200 characters"));
                        return;
                    }
                    if (body.isMember("description") &&
                        (!body["description"].isString() ||
                         body["description"].asString().size() > 2000U)) {
                        callback(make_text_response(k400BadRequest,
                                                    "description must be <= 2000 characters"));
                        return;
                    }
                    if (!body["price"].isNumeric()) {
                        callback(make_text_response(k400BadRequest, "price is required"));
                        return;
                    }
                    const double price = body["price"].asDouble();
                    if (price < 0.0 || price >= 1'000'000.0) {
                        callback(make_text_response(k400BadRequest,
                                                    "price must be >= 0 and < 1000000"));
                        return;
                    }
                    if (body.isMember("stock") &&
                        (!body["stock"].isIntegral() || body["stock"].asInt64() < 0 ||
                         body["stock"].asInt64() > 999'999)) {
                        callback(make_text_response(k400BadRequest,
                                                    "stock must be 0..=999999"));
                        return;
                    }
                    if (!body["category"].isString() ||
                        !is_valid_category(body["category"].asString())) {
                        callback(make_text_response(k400BadRequest, "invalid category"));
                        return;
                    }

                    const auto description = normalize_optional_text(body["description"]);
                    const bool has_stock = body.isMember("stock");
                    const int64_t stock = has_stock ? body["stock"].asInt64() : 0;
                    const auto rows = state->db->execSqlSync(std::string(kCreateItemSql),
                                                             body["name"].asString(),
                                                             description.has_value(),
                                                             description.value_or(std::string()),
                                                             price,
                                                             has_stock,
                                                             stock,
                                                             body["category"].asString());
                    auto response = HttpResponse::newHttpJsonResponse(row_to_item_json(rows[0]));
                    response->setStatusCode(k201Created);
                    callback(response);
                },
                callback);
        },
        {Get, Post});

    app().registerHandler(
        "/items/{1}",
        [state](const HttpRequestPtr& request,
                std::function<void(const HttpResponsePtr&)>&& callback,
                int64_t id) {
            if (!state->db) {
                callback(make_text_response(k503ServiceUnavailable,
                                            "postgres is not configured for this target"));
                return;
            }

            with_db_exceptions(
                [&]() {
                    if (request->method() == Get) {
                        const auto rows =
                            state->db->execSqlSync(std::string(kGetItemSql), id);
                        if (rows.empty()) {
                            callback(make_text_response(k404NotFound, "item not found"));
                            return;
                        }
                        callback(HttpResponse::newHttpJsonResponse(row_to_item_json(rows[0])));
                        return;
                    }

                    if (request->method() == Delete) {
                        const auto result =
                            state->db->execSqlSync(std::string(kDeleteItemSql), id);
                        if (result.affectedRows() == 0) {
                            callback(make_text_response(k404NotFound, "item not found"));
                            return;
                        }
                        auto response = HttpResponse::newHttpResponse();
                        response->setStatusCode(k204NoContent);
                        callback(response);
                        return;
                    }

                    const auto json = request->getJsonObject();
                    if (!json || !json->isObject()) {
                        callback(
                            make_text_response(k400BadRequest, "payload must be a JSON object"));
                        return;
                    }
                    const auto& body = *json;

                    if (body.isMember("name") &&
                        (!body["name"].isString() || body["name"].asString().empty() ||
                         body["name"].asString().size() > 200U)) {
                        callback(make_text_response(k400BadRequest,
                                                    "name must be 1..=200 characters"));
                        return;
                    }
                    if (body.isMember("description") &&
                        (!body["description"].isString() ||
                         body["description"].asString().size() > 2000U)) {
                        callback(make_text_response(k400BadRequest,
                                                    "description must be <= 2000 characters"));
                        return;
                    }
                    if (body.isMember("price") &&
                        (!body["price"].isNumeric() || body["price"].asDouble() < 0.0)) {
                        callback(make_text_response(k400BadRequest, "price must be >= 0"));
                        return;
                    }
                    if (body.isMember("stock") &&
                        (!body["stock"].isIntegral() || body["stock"].asInt64() < 0 ||
                         body["stock"].asInt64() > 999'999)) {
                        callback(make_text_response(k400BadRequest,
                                                    "stock must be 0..=999999"));
                        return;
                    }
                    if (body.isMember("category") &&
                        (!body["category"].isString() ||
                         !is_valid_category(body["category"].asString()))) {
                        callback(make_text_response(k400BadRequest, "invalid category"));
                        return;
                    }

                    const auto description = normalize_optional_text(body["description"]);
                    const std::string name =
                        body.isMember("name") ? body["name"].asString() : std::string();
                    const double price =
                        body.isMember("price") ? body["price"].asDouble() : 0.0;
                    const int64_t stock =
                        body.isMember("stock") ? body["stock"].asInt64() : 0;
                    const std::string category =
                        body.isMember("category") ? body["category"].asString() : std::string();

                    const auto rows = state->db->execSqlSync(std::string(kUpdateItemSql),
                                                             id,
                                                             body.isMember("name"),
                                                             name,
                                                             description.has_value(),
                                                             description.value_or(std::string()),
                                                             body.isMember("price"),
                                                             price,
                                                             body.isMember("stock"),
                                                             stock,
                                                             body.isMember("category"),
                                                             category);
                    if (rows.empty()) {
                        callback(make_text_response(k404NotFound, "item not found"));
                        return;
                    }
                    callback(HttpResponse::newHttpJsonResponse(row_to_item_json(rows[0])));
                },
                callback);
        },
        {Get, Put, Delete});

    std::cout << "drogon comparison server listening on :" << port << " with " << workers
              << " workers" << std::endl;

    app().run();
    return 0;
}
