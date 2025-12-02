# KATANA Router

Compile-time, zero-allocation HTTP router для KATANA фреймворка.

## Обзор

KATANA Router обеспечивает типобезопасный routing с compile-time парсингом путей и zero-allocation dispatch в hot path. Роутер автоматически обрабатывает 404/405 ошибки, генерирует RFC 7807 Problem Details и поддерживает middleware chains.

### Ключевые особенности

- ✅ **Compile-time парсинг путей** — ошибки в `/users/{id}` видны на этапе компиляции
- ✅ **Zero-allocation hot path** — никаких heap-аллокаций при dispatch
- ✅ **Автоматический RFC 7807** — 404/405 автоматически мапятся в Problem Details
- ✅ **Allow header** — автоматическая генерация для 405 Method Not Allowed
- ✅ **Path parameters** — автоматическое извлечение параметров
- ✅ **Middleware chains** — композиция middleware без виртуальных вызовов
- ✅ **Smart routing** — приоритизация статических сегментов над параметрами

---

## Quick Start

### Простейший роутер

```cpp
#include "katana/core/router.hpp"
#include "katana/core/http.hpp"

using namespace katana::http;

// Определяем роуты
route_entry routes[] = {
    {method::get,
     path_pattern::from_literal<"/">(),
     handler_fn([](const request& req, request_context& ctx) {
         return response::ok("Hello, World!");
     })},

    {method::get,
     path_pattern::from_literal<"/users/{id}">(),
     handler_fn([](const request& req, request_context& ctx) {
         auto id = ctx.params.get("id").value_or("unknown");
         return response::ok(std::string("User ID: ") + std::string(id));
     })},
};

// Создаём роутер
router r(routes);

// Dispatch request
monotonic_arena arena;
request_context ctx{arena};
request req;
req.http_method = method::get;
req.uri = "/users/42";

auto resp = dispatch_or_problem(r, req, ctx);
// resp.status == 200
// resp.body == "User ID: 42"
```

---

## Path Patterns

### Compile-time парсинг

Path patterns парсятся на этапе компиляции через `from_literal<>()`:

```cpp
// ✅ Валидно
auto pattern = path_pattern::from_literal<"/users/{id}">();
auto pattern = path_pattern::from_literal<"/orders/{orderId}/items/{itemId}">();
auto pattern = path_pattern::from_literal<"/static/about">();

// ❌ Ошибки компиляции
auto pattern = path_pattern::from_literal<"users">(); // должно начинаться с '/'
auto pattern = path_pattern::from_literal<"/users/{id">();  // незакрытая скобка
auto pattern = path_pattern::from_literal<"/users/{}">();   // пустое имя параметра
```

### Синтаксис параметров

```cpp
// Литеральные сегменты
"/users"              // только точное совпадение

// Параметры (любое значение)
"/users/{id}"         // /users/123, /users/alice
"/users/{id}/posts"   // /users/42/posts

// Множественные параметры
"/orders/{orderId}/items/{itemId}"  // /orders/10/items/5
```

### Приоритизация

Статические сегменты имеют больший приоритет, чем параметры:

```cpp
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/me">(), handler_me},      // приоритет 1
    {method::get, path_pattern::from_literal<"/users/{id}">(), handler_id},    // приоритет 2
};

router r(routes);

// GET /users/me    → вызовет handler_me (точное совпадение)
// GET /users/42    → вызовет handler_id (параметр)
```

**Алгоритм приоритизации:**
```cpp
score = literal_count * 16 + (MAX_ROUTE_SEGMENTS - param_count)
```

Чем больше литеральных сегментов, тем выше приоритет.

---

## Path Parameters

### Извлечение параметров

Параметры автоматически извлекаются в `request_context::params`:

```cpp
{method::get,
 path_pattern::from_literal<"/users/{id}">(),
 handler_fn([](const request& req, request_context& ctx) {
     // Получение параметра
     auto id = ctx.params.get("id");  // std::optional<std::string_view>

     if (!id) {
         return response::error(problem_details::bad_request("Missing id"));
     }

     // Безопасный fallback
     auto name = ctx.params.get("name").value_or("anonymous");

     return response::ok(std::string(*id));
 })}
```

### Множественные параметры

```cpp
{method::get,
 path_pattern::from_literal<"/orders/{orderId}/items/{itemId}">(),
 handler_fn([](const request& req, request_context& ctx) {
     auto order_id = ctx.params.get("orderId");
     auto item_id = ctx.params.get("itemId");

     // Оба параметра доступны
     return response::ok("OK");
 })}
```

### Парсинг параметров

```cpp
#include <charconv>

// Конвертация в число
auto id_str = ctx.params.get("id").value_or("");
int id = 0;
auto [ptr, ec] = std::from_chars(id_str.data(), id_str.data() + id_str.size(), id);
if (ec != std::errc{}) {
    return response::error(problem_details::bad_request("Invalid ID"));
}
```

---

## Request Handlers

### Handler signature

```cpp
using handler_fn = inplace_function<
    result<response>(const request&, request_context&),
    160
>;
```

Хендлеры принимают:
- `const request&` — HTTP запрос
- `request_context&` — контекст с ареной и path parameters

Возвращают:
- `result<response>` — либо response, либо error_code

### Lambda handlers

```cpp
handler_fn([](const request& req, request_context& ctx) {
    return response::ok("Hello!");
})
```

### Capturing state

```cpp
struct user_repository { /* ... */ };

user_repository repo;

handler_fn([&repo](const request& req, request_context& ctx) {
    auto users = repo.find_all();
    // ...
    return response::ok("OK");
})
```

### Error handling

```cpp
handler_fn([](const request& req, request_context& ctx) {
    // Возврат error_code
    if (something_wrong) {
        return std::unexpected(make_error_code(error_code::bad_request));
    }

    // Возврат Problem Details
    if (not_found) {
        return response::error(problem_details::not_found("Resource not found"));
    }

    // Success
    return response::ok("Success");
})
```

---

## HTTP Methods

Поддерживаемые методы:

```cpp
method::get      // GET
method::head     // HEAD
method::post     // POST
method::put      // PUT
method::del      // DELETE
method::patch    // PATCH
method::options  // OPTIONS
```

### Множественные методы для одного пути

```cpp
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/{id}">(), get_user_handler},
    {method::put, path_pattern::from_literal<"/users/{id}">(), update_user_handler},
    {method::del, path_pattern::from_literal<"/users/{id}">(), delete_user_handler},
};
```

---

## Automatic Error Handling

### 404 Not Found

Автоматически возвращается, если путь не найден:

```cpp
// Нет роута для /missing
request req;
req.http_method = method::get;
req.uri = "/missing";

auto resp = dispatch_or_problem(r, req, ctx);
// resp.status == 404
// resp содержит RFC 7807 Problem Details
```

**Response:**
```json
{
  "type": "about:blank",
  "title": "Not Found",
  "status": 404,
  "detail": "The requested resource was not found"
}
```

### 405 Method Not Allowed

Автоматически возвращается, если метод не поддерживается для пути:

```cpp
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/{id}">(), handler},
};

router r(routes);

request req;
req.http_method = method::post;  // POST не поддерживается
req.uri = "/users/1";

auto resp = dispatch_or_problem(r, req, ctx);
// resp.status == 405
// resp.headers["Allow"] == "GET"
```

**Response:**
```http
HTTP/1.1 405 Method Not Allowed
Allow: GET
Content-Type: application/problem+json

{
  "type": "about:blank",
  "title": "Method Not Allowed",
  "status": 405
}
```

### Allow header

При 405 автоматически генерируется `Allow` header со всеми допустимыми методами:

```cpp
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/items/{id}">(), get_handler},
    {method::post, path_pattern::from_literal<"/items/{id}">(), post_handler},
    {method::del, path_pattern::from_literal<"/items/{id}">(), delete_handler},
};

// PUT /items/1 → 405 + "Allow: GET, POST, DELETE"
```

---

## Middleware

### Middleware signature

```cpp
using middleware_fn = inplace_function<
    result<response>(const request&, request_context&, next_fn),
    160
>;
```

Middleware принимает:
- `const request&` — запрос
- `request_context&` — контекст
- `next_fn` — функция для вызова следующего middleware/handler

### Logging middleware

```cpp
middleware_fn logging_middleware([](const request& req, request_context& ctx, next_fn next) {
    std::cout << "[REQUEST] " << method_to_string(req.http_method) << " " << req.uri << "\n";

    auto result = next();  // вызов следующего middleware/handler

    if (result) {
        std::cout << "[RESPONSE] " << result->status << "\n";
    }

    return result;
});
```

### Authentication middleware

```cpp
middleware_fn auth_middleware([](const request& req, request_context& ctx, next_fn next) {
    auto token = req.headers.get("Authorization");

    if (!token || !validate_token(*token)) {
        return result<response>(
            response::error(problem_details::unauthorized("Invalid token"))
        );
    }

    // Token valid, proceed
    return next();
});
```

### CORS middleware

```cpp
middleware_fn cors_middleware([](const request& req, request_context& ctx, next_fn next) {
    // Handle preflight
    if (req.http_method == method::options) {
        response resp;
        resp.status = 204;
        resp.set_header("Access-Control-Allow-Origin", "*");
        resp.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        return result<response>(std::move(resp));
    }

    // Normal request
    auto result = next();
    if (result) {
        result->set_header("Access-Control-Allow-Origin", "*");
    }
    return result;
});
```

### Middleware chains

```cpp
std::array<middleware_fn, 3> global_middleware = {
    logging_middleware(),
    cors_middleware(),
    auth_middleware(),
};

auto chain = make_middleware_chain(global_middleware);

route_entry routes[] = {
    {method::get,
     path_pattern::from_literal<"/protected">(),
     handler,
     chain},  // применить middleware chain
};
```

**Порядок выполнения:**
```
Request → logging → cors → auth → handler → auth → cors → logging → Response
          ↓         ↓      ↓        ↓          ↑      ↑      ↑
          before    before before  execute    after  after  after
```

### Per-route middleware

Разные роуты могут иметь разные middleware:

```cpp
auto public_middleware = make_middleware_chain(std::array{logging_middleware()});
auto protected_middleware = make_middleware_chain(std::array{
    logging_middleware(),
    auth_middleware()
});

route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/public">(),
     public_handler, public_middleware},

    {method::get, path_pattern::from_literal<"/protected">(),
     protected_handler, protected_middleware},
};
```

---

## Query String Handling

Query strings автоматически отрезаются при matching:

```cpp
// GET /users/42?page=1&limit=10
// Будет match с /users/{id}, params = {id: "42"}

{method::get,
 path_pattern::from_literal<"/users/{id}">(),
 handler_fn([](const request& req, request_context& ctx) {
     auto id = ctx.params.get("id");  // "42"

     // Query string доступен через req.uri
     // Парсинг query parameters — ручной (пока)
     auto query_start = req.uri.find('?');
     if (query_start != std::string_view::npos) {
         auto query = req.uri.substr(query_start + 1);
         // parse query...
     }

     return response::ok("OK");
 })}
```

---

## Request Context

```cpp
struct request_context {
    monotonic_arena& arena;     // arena для аллокаций
    path_params params;         // извлечённые path parameters
};
```

### Arena allocations

```cpp
handler_fn([](const request& req, request_context& ctx) {
    // Аллокация из арены
    auto* buffer = ctx.arena.allocate(1024);

    // Строки из арены
    arena_string<> str(ctx.arena);
    str.append("Hello");

    return response::ok("OK");
    // Arena автоматически reset после завершения запроса
})
```

---

## Dispatch API

### `dispatch()`

Простой dispatch без автоматической обработки ошибок:

```cpp
router r(routes);
monotonic_arena arena;
request_context ctx{arena};

result<response> res = r.dispatch(req, ctx);

if (res) {
    // Success
    send_response(*res);
} else {
    // Error (404, 405, etc.)
    auto error_code = res.error();
    handle_error(error_code);
}
```

### `dispatch_with_info()`

Dispatch с дополнительной информацией:

```cpp
dispatch_result res = r.dispatch_with_info(req, ctx);

if (res.path_matched) {
    // Путь найден, но метод неверный (405)
    std::string allow = allow_header_from_mask(res.allowed_methods_mask);
    // allow == "GET, POST"
} else {
    // Путь не найден (404)
}
```

### `dispatch_or_problem()`

**Рекомендуемый способ** — автоматически мапит ошибки в RFC 7807:

```cpp
response resp = dispatch_or_problem(r, req, ctx);
// Всегда возвращает response (200, 404, 405, 500)
send_response(resp);
```

---

## Integration with HTTP Server

### Using `router_handler`

Простая интеграция с существующим HTTP сервером:

```cpp
router r(routes);
http::router_handler handler(r);

// В вашем HTTP server loop:
monotonic_arena arena;
const request& req = parse_request(...);

response resp = handler(req, arena);
send_response(resp);
```

---

## Performance Characteristics

### Compile-time guarantees

- ✅ Path patterns валидируются на этапе компиляции
- ✅ Никакого runtime парсинга путей
- ✅ Все структуры данных constexpr-safe

### Zero-allocation hot path

```cpp
// Dispatch — zero heap allocations
route_entry routes[] = { /* ... */ };
router r(routes);  // routes хранятся как std::span

monotonic_arena arena;
request_context ctx{arena};  // path_params на стеке

auto resp = r.dispatch(req, ctx);  // no heap alloc
```

**Что НЕ аллоцируется:**
- Route table (передаётся как span)
- Path parameters (fixed-size array на стеке)
- Middleware chain (передаётся как pointer + size)

**Что аллоцируется:**
- Response body (если динамический)
- Headers (если добавляются в handler)

### Routing complexity

- **Time:** O(N) где N — количество routes
- **Space:** O(1) stack space
- **Optimization:** Linear scan с early exit на first match

Для большого количества роутов (> 100) рекомендуется group by prefix или использовать будущую генерацию compile-time routing table из OpenAPI.

---

## Best Practices

### 1. Используйте `dispatch_or_problem()`

```cpp
// ✅ Good
response resp = dispatch_or_problem(r, req, ctx);

// ❌ Avoid (требует ручной обработки ошибок)
result<response> res = r.dispatch(req, ctx);
if (!res) { /* handle 404/405 manually */ }
```

### 2. Статические роуты перед параметрами

```cpp
// ✅ Good (автоматическая приоритизация)
route_entry routes[] = {
    {method::get, path_pattern::from_literal<"/users/me">(), ...},
    {method::get, path_pattern::from_literal<"/users/{id}">(), ...},
};

// Порядок не важен — статические всегда приоритетнее
```

### 3. Валидируйте параметры

```cpp
handler_fn([](const request& req, request_context& ctx) {
    auto id_str = ctx.params.get("id");
    if (!id_str) {
        return response::error(problem_details::bad_request("Missing id"));
    }

    int id;
    auto [ptr, ec] = std::from_chars(id_str->data(), id_str->data() + id_str->size(), id);
    if (ec != std::errc{}) {
        return response::error(problem_details::bad_request("Invalid id format"));
    }

    if (id <= 0) {
        return response::error(problem_details::bad_request("Id must be positive"));
    }

    // id valid
    return response::ok("OK");
})
```

### 4. Middleware порядок имеет значение

```cpp
// ✅ Good (error recovery снаружи)
std::array<middleware_fn, 3> middleware = {
    error_recovery_middleware(),  // 1. Catch exceptions
    logging_middleware(),          // 2. Log request/response
    auth_middleware(),             // 3. Validate auth
};

// ❌ Bad (auth exceptions не будут caught)
std::array<middleware_fn, 3> middleware = {
    auth_middleware(),
    logging_middleware(),
    error_recovery_middleware(),
};
```

### 5. Группируйте роуты по префиксам

```cpp
// Для лучшей читаемости
route_entry routes[] = {
    // User routes
    {method::get, path_pattern::from_literal<"/api/users">(), ...},
    {method::get, path_pattern::from_literal<"/api/users/{id}">(), ...},
    {method::post, path_pattern::from_literal<"/api/users">(), ...},

    // Order routes
    {method::get, path_pattern::from_literal<"/api/orders">(), ...},
    {method::get, path_pattern::from_literal<"/api/orders/{id}">(), ...},
};
```

---

## Examples

Полные рабочие примеры:

- **`examples/router_rest_api.cpp`** — REST API с CRUD операциями
- **`examples/middleware_examples.cpp`** — Примеры всех типов middleware
- **`examples/hello_world_server.cpp`** — Простейший HTTP сервер с роутером

---

## Roadmap

### Planned Features

- [ ] **OpenAPI codegen** — автоматическая генерация роутов из OpenAPI спецификации
- [ ] **Query parameter parsing** — встроенный парсинг query strings
- [ ] **Compile-time routing table** — O(1) lookup для больших наборов роутов
- [ ] **Regex path parameters** — `/users/{id:\d+}` с валидацией на compile-time
- [ ] **Path prefixes** — группировка роутов по префиксам
- [ ] **Route metadata** — tags, descriptions для документации

### Future Middleware

- [ ] Rate limiting (per-IP, per-user)
- [ ] Caching (etag, conditional requests)
- [ ] Compression (gzip, brotli)
- [ ] Request ID propagation
- [ ] OpenTelemetry tracing

---

## Troubleshooting

### Compile error: "route path cannot be empty"

```cpp
// ❌ Error
auto pattern = path_pattern::from_literal<"">();

// ✅ Fix
auto pattern = path_pattern::from_literal<"/">();
```

### Compile error: "route path must start with '/'"

```cpp
// ❌ Error
auto pattern = path_pattern::from_literal<"users">();

// ✅ Fix
auto pattern = path_pattern::from_literal<"/users">();
```

### Path parameter not found

```cpp
// Проверяйте наличие параметра
auto id = ctx.params.get("id");
if (!id) {
    // Параметр отсутствует
    return response::error(problem_details::bad_request("Missing id parameter"));
}
```

### Middleware chain not executing

```cpp
// Убедитесь, что chain передан в route_entry
route_entry routes[] = {
    {method::get,
     path_pattern::from_literal<"/path">(),
     handler,
     middleware_chain},  // ← не забудьте!
};
```

---

## See Also

- [OPENAPI.md](OPENAPI.md) — OpenAPI loader и AST
- [ARCHITECTURE.md](../ARCHITECTURE.md) — Общая архитектура фреймворка
- [TESTING.md](TESTING.md) — Тестирование роутеров
