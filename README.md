# KATANA Framework

KATANA — серверный фреймворк на C++ для разработки высоконагруженных API и систем с жёсткими требованиями к хвостовым задержкам (p95/p99/p999), контролируемым использованием ресурсов и предсказуемым поведением под нагрузкой.

> 📊 **[Benchmark Results](BENCHMARK_RESULTS.md)** — актуальные результаты производительности фреймворка

Цель — устранить разрыв между:

1. быстрым DX (Python/Node) с нестабильной производительностью,
2. «сырыми» высокопроизводительными системами (C++/Rust/Go) со сложной эксплуатацией.

Фреймворк фиксирует **модель исполнения**, **модель памяти** и **контракт API**, чтобы обеспечить стабильные задержки без усложнения доменной логики.

> 📖 Детальное описание архитектуры, принципов проектирования и технических решений — [ARCHITECTURE.md](ARCHITECTURE.md)

---

## Последние обновления

<!-- LATEST_UPDATE_START -->
* 26.12 13:41 — fix(http): resolve critical keep-alive bugs causing 33% ECONNRESET errors (b8bfae6)
<!-- LATEST_UPDATE_END -->

## Текущее состояние (реальность)

**Реализовано (Stage 1 + Stage 2):**
- ✅ Epoll/io_uring reactor + reactor_pool
- ✅ Арены/IO-буфера
- ✅ HTTP/1.1 парсер/сериализация
- ✅ Wheel timer
- ✅ TCP listener/socket helpers
- ✅ **Router** — compile-time routing с middleware
- ✅ **OpenAPI loader** — парсинг JSON/YAML спецификаций с $ref resolution
- ✅ **katana_gen** — кодогенератор из OpenAPI spec
  - DTO с pmr allocators
  - JSON parsers и serializers (katana::serde)
  - Validators для ключевых OpenAPI constraints (min/max, minLength/maxLength, pattern, enum, uniqueItems)
  - Enum → enum class codegen
  - Handler interfaces и router bindings с content negotiation и optional-параметрами
  - Constexpr route tables с compile-time type safety
  - x-katana-* extensions
- ✅ Unit/integration/fuzz тесты

**В разработке / не реализовано:**
- ⏳ SQL генерация/libpq
- ⏳ Redis клиент
- ⏳ OpenTelemetry tracing
- ⏳ Prometheus metrics
- ⏳ Structured logging
- ⏳ Media type registry (CBOR/MessagePack) — отнесено в Stage 3

Разделы README/ARCHITECTURE описывают целевое состояние фреймворка. То, что уже работает — помечено ✅ выше.

## Getting Started (сегодня)

1. Зависимости: CMake ≥ 3.20, Ninja, Clang ≥ 16 или GCC ≥ 12, `liburing-dev` (для io_uring пресетов).
2. Конфигурация: `cmake --preset debug` (доступны также `release`, `asan`, `tsan`, `ubsan`, `io_uring-*`, `bench`, `examples`).
3. Сборка: `cmake --build --preset debug`.
4. Тесты: `ctest --preset debug` (используется лёгкий gtest-совместимый харнес из `test/gtest/gtest.h`).
5. Примеры: `cmake --build --preset examples && ./build/examples/hello_world_server`.
6. Бенчмарки: `cmake --preset bench && cmake --build --preset bench && ./build/bench/benchmark/performance_benchmark`.
7. Удобно через Makefile: `make build PRESET=debug`, `make test PRESET=debug`, `make bench`, `make fuzz`, `make profile`.
8. CRUD бенч: по умолчанию in-memory; для высокого RPS можно задать `KATANA_CRUD_BACKEND=memcached` (опционально `MEMCACHED_HOST`/`MEMCACHED_PORT`). Docker бенч-сборка поднимает memcached автоматически.

## Router Quick Start (Stage 2)

KATANA Router — compile-time, zero-allocation HTTP роутер с автоматической обработкой ошибок и middleware.

### Простейший пример

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
         return response::ok(std::string("User: ") + std::string(id));
     })},
};

router r(routes);

// Dispatch запроса
monotonic_arena arena;
request_context ctx{arena};
request req;
req.http_method = method::get;
req.uri = "/users/42";

response resp = dispatch_or_problem(r, req, ctx);
// resp.status == 200, resp.body == "User: 42"
```

### Middleware пример

```cpp
// Logging middleware
middleware_fn logging([](const request& req, request_context& ctx, next_fn next) {
    std::cout << "[" << method_to_string(req.http_method) << "] " << req.uri << "\n";
    return next();
});

// Auth middleware
middleware_fn auth([](const request& req, request_context& ctx, next_fn next) {
    auto token = req.headers.get("Authorization");
    if (!token || !validate(*token)) {
        return response::error(problem_details::unauthorized("Invalid token"));
    }
    return next();
});

// Применяем middleware
std::array<middleware_fn, 2> chain = {logging, auth};

route_entry routes[] = {
    {method::get,
     path_pattern::from_literal<"/protected">(),
     protected_handler,
     make_middleware_chain(chain)},
};
```

### Автоматические ошибки

- **404 Not Found** — автоматически возвращается для несуществующих путей
- **405 Method Not Allowed** — с автоматическим `Allow` header
- **RFC 7807 Problem Details** — стандартизированный формат ошибок

### Примеры

- `examples/router_rest_api.cpp` — полный REST API с CRUD
- `examples/middleware_examples.cpp` — примеры всех middleware (logging, auth, CORS, rate limiting)
- `examples/hello_world_server.cpp` — минимальный HTTP сервер

📖 **Подробная документация:** [docs/ROUTER.md](docs/ROUTER.md)

---

## OpenAPI Code Generator (Stage 2)

Arena-backed парсер OpenAPI 3.x спецификаций (JSON/YAML) + кодогенератор.

### katana_gen CLI

```bash
# Сборка
cmake --build --preset debug --target katana_gen

# Генерация всего (DTOs + JSON parsers + route table)
./build/debug/katana_gen openapi -i api/openapi.yaml -o gen --emit all

# Только DTOs с pmr аллокаторами
./build/debug/katana_gen openapi -i api/openapi.yaml -o gen --emit dto --alloc pmr

# Только route table (constexpr)
./build/debug/katana_gen openapi -i api/openapi.yaml -o gen --emit router

# С AST дампом для отладки
./build/debug/katana_gen openapi -i api/openapi.yaml -o gen --dump-ast
```

**Флаги:**
- `--emit <targets>` — что генерировать: `dto`, `serdes`, `router`, `all` (default: `all`)
- `--alloc <type>` — тип аллокатора: `pmr`, `std` (default: `pmr`)
- `--layer <mode>` — архитектура: `flat`, `layered` (default: `flat`)
- `--inline-naming <style>` — именование inline-схем: `operation` (default), `flat`
- `--check` — только валидация спецификации без генерации файлов
- `--dump-ast` — сохранить AST в JSON для отладки
- `--strict` — строгая валидация, не игнорировать ошибки

**Генерируемые файлы:**
- `generated_dtos.hpp` — C++ структуры с arena allocators
- `generated_json.hpp` — JSON parsers через katana::serde
- `generated_routes.hpp` — constexpr route table для router
- `generated_handlers.hpp` — интерфейсы хендлеров с типизированными параметрами
- `generated_router_bindings.hpp` — glue с разбором path/query/header/cookie, optional-значениями и Content-Type/Accept negotiation

### Quick Start

```cpp
#include "katana/core/openapi_loader.hpp"
#include "katana/core/arena.hpp"

const std::string spec = R"({
  "openapi": "3.0.0",
  "info": {"title": "My API", "version": "1.0"},
  "paths": {
    "/users/{id}": {
      "get": {
        "operationId": "getUser",
        "parameters": [{"name": "id", "in": "path", "required": true}]
      }
    }
  }
})";

monotonic_arena arena;
auto result = katana::openapi::load_from_string(spec, arena);

if (result) {
    std::cout << "API: " << result->info_title << "\n";
    for (const auto& path : result->paths) {
        std::cout << "  " << path.path << "\n";
    }
}
```

**Парсинг поддерживает:**
- ✅ JSON и YAML форматы
- ✅ Paths, operations, parameters (с style/explode)
- ✅ Request body и responses
- ✅ Schemas (object, array, string, number, boolean, enum, etc.)
- ✅ $ref resolution с cycle detection
- ✅ allOf merge (most restrictive constraints)
- ✅ Validation constraints (minLength/maxLength, min/max, pattern, required, etc.)
- ✅ Specification validation (version 3.x, operationId uniqueness, HTTP codes)

**Кодогенерация:**
- ✅ DTOs с pmr arena allocators
- ✅ JSON parsers и serializers с katana::serde (zero-copy где возможно)
- ✅ Validators с полной поддержкой OpenAPI constraints (minLength/maxLength, pattern, min/max, enum, format validators, uniqueItems)
- ✅ Enum → enum class codegen с to_string/from_string
- ✅ Format validators (email, uuid, date-time, uri, ipv4, hostname)
- ✅ Handler interfaces из OpenAPI operations
- ✅ Constexpr route tables с compile-time metadata для type safety
- ✅ x-katana-* extensions (cache, alloc, rate-limit)

📖 **Подробная документация:** [docs/OPENAPI.md](docs/OPENAPI.md)

---

## Разработка и стиль

* Форматирование — `.clang-format`, статический анализ — `.clang-tidy`.
* Локальный авто-линт: `pip install pre-commit && pre-commit install` (clang-format, cmake-format, базовые проверки YAML/конфликтов).
* Перед PR: `cmake --build --preset debug && ctest --preset debug`; низкоуровневые изменения гонять с sanitizer-пресетами (`asan/tsan/ubsan`).
* Дополнительно: гайды в `CONTRIBUTING.md` и `docs/TESTING.md`.
* Для тестирования без реактора добавлены харнесы: `test/support/http_handler_harness.hpp` (оборачивает handler над Request/Response) и `test/support/virtual_event_loop.hpp` (фейковый event loop с виртуальным временем).

### 📈 Качество бенчмарков

<!-- BENCH_SUMMARY_START -->
* Отчёт сгенерирован 2025-12-26 13:38:56.
* Keep-alive: throughput 11382.085 req/s, p99 0.084 ms, 523695 samples.
* Масштабирование: 128 коннектов — 268700 req/s; 8 потоков — 252413 req/s.
* Устойчивость: sustained 42352.832 req/s, всего 211774 requests.
<!-- BENCH_SUMMARY_END -->

---

## Проблемы, которые решает KATANA

* Непредсказуемые задержки из-за общих пулов, локов, GC и межпоточных гонок.
* N+1, неявные запросы и планы в ORM.
* Ручной бойлерплейт DTO/валидации/сериализации, расхождение документации и API.
* Несистемное кэширование, «магические» middleware и неявный rate limiting.
* Слабая наблюдаемость: проблемы «ищутся по логам», а не видны сразу.

---

## Архитектурная модель

### Reactor-per-core

Каждое ядро CPU получает свой независимый event loop (реактор), свой пул соединений БД и свой кэш. Запрос **целиком** обрабатывается внутри одного реактора.

* Нет глобального состояния в data-plane и межпоточных локов; остаются только узкие сервисные координаторы (shutdown, выдача thread id) вне критического пути запроса.
* Нет очередей на общий пул БД.
* Нет handoff между потоками во время обработки запроса.
* p99/p999 стабильны и контролируемы.

**Thread pinning опционален**: корректность обеспечивается полной изоляцией реакторов в data-plane (shared только сервисные счётчики: shutdown, выдача thread id). Pinning — это оптимизация производительности (CPU cache locality, NUMA), но не требование корректности.

### Управление памятью

Модель **arena-per-request** (монотонный аллокатор): всё, что относится к запросу, выделяется из арены и освобождается одним действием после завершения.

Режимы:

* `arena` (по умолчанию),
* `std::pmr` с настраиваемой `memory_resource`,
* стандартный `new/delete` (флаг `--no-arena` в dev).

DTO используют `std::pmr::*` и `std::string_view`. Стандартные контейнеры не подменяются насильно.

### Сетевой I/O и протоколы

* Базовый backend: **epoll** + vectored I/O (`readv/writev`).
* HTTP/1.1 в базовой поставке.
* Производственные профили: HTTP/2 (HPACK), HTTP/3/QUIC.
* Zero-copy статика через `sendfile`/kTLS при поддержке ядра.
* `io_uring` как опциональный backend.

---

## Типобезопасный API и кодогенерация

**OpenAPI + SQL** — источники истины.

Генератор создаёт:

* compile-time маршрутизацию (без строковых сравнений и рефлексии в runtime),
* DTO со строгой типизацией,
* валидаторы,
* сериализацию/десериализацию,
* статические таблицы маршрутов,
* (опционально) клиентские SDK (TS/Go/Rust/Python).

Несоответствия контракту становятся **ошибками компиляции**.

### Расширения OpenAPI (`x-katana-*`)

Не меняют стандарт, добавляют декларативные политики:

* аллокация (`arena` / `pmr` / `heap`),
* выбор сериализатора (`zero-copy` / `dom`),
* кэш (TTL, stale-while-revalidate, ключи инвалидации),
* rate limiting и idempotency,
* требования консистентности и дедлайны.

---

## Многоуровневый кодоген: High / Mid / Low

### High level — «нулевой бойлерплейт»

Вход: OpenAPI + SQL.
Выход: готовый сервис (контроллеры-заглушки, DTO, валидаторы, маршруты, middleware-pipeline, docker/dev-stack, CI-чекеры, SDK). Генерируемый код по умолчанию read-only, расширения через `partial`/hook-файлы.

Команда:

* `katana gen high -i api/openapi.yaml -s sql/ -o svc/`

### Mid level — «кастомизация при гарантиях»

Генерируются интерфейсы контроллеров и все инфраструктурные части (DTO/валидация/сериализация/роуты). Разработчик реализует только бизнес-логику, не теряя типобезопасности.

Пример:

* генерируется `gen::UsersApi` с виртуальными методами,
* разработчик пишет `class UsersController : public gen::UsersApi { … }`.

### Low level — «raw доступ для power-users»

Прямой доступ к реактору, сокетам, libpq (binary), Redis-клиенту, syscalls, kTLS/sendfile/io_uring. Полный контроль — полная ответственность (дедлайны, арены и безопасность на стороне разработчика). Доступны helper-обёртки из Mid/High.

---

## Работа с БД

ORM **не используется**.

SQL в `.sql` — источник истины для генератора:

* структуры моделей,
* репозитории,
* транзакции,
* bulk-операции,
* типобезопасные фильтры/сортировки,
* UPSERT и стратегии конфликтов.

Особенности:

* **libpq binary protocol** + только **prepared statements**,
* пулы соединений **per-core**,
* явный **prefetch** вместо N+1,
* строгие дедлайны на операции.

---

## Кэш и Redis

Кэш/идемпотентность/rate-limit описываются в OpenAPI/SQL-аннотациях.

Поддерживаются:

* single-flight,
* TTL с jitter,
* кэшируемые GET,
* защищённые POST,
* политики инвалидации по ключам/префиксам.

Инфраструктура генерируется автоматически, без ручных middleware.

---

## Наблюдаемость и диагностика

Встроены:

* OpenTelemetry-трейсы (end-to-end),
* Prometheus-метрики: RPS, p50/p95/p99/p999, длины очередей, состояние backpressure, использование арен, ошибки БД/Redis,
* структурные JSON-логи.

В CI:

* нагрузочные профили,
* flamegraph,
* регрессия производительности (**деградация p99 → fail сборки**).

---

## Дев-режим (DX)

`katana dev`:

* hot-reload контроллеров **без** рестарта реакторов,
* быстрые сборки (clang + ccache),
* автоподнятие локальных PG/Redis/Prometheus/OpenTelemetry,
* отключение NUMA-pinning на dev,
* моки репозиториев.

Цель — скорость итераций, сравнимая с Node/FastAPI, при сохранении производственной модели исполнения.

---

## Инструментарий (CLI)

### Создание проекта

* `katana new <project-name> --layer {high|mid|low}`
  Создаёт каркас: `CMakeLists.txt`, `src/`, `include/`, `api/`, `sql/`, базовый `main` с реактором.

### Генерация API

* `katana gen openapi -i api/openapi.yaml -o gen/ --layer {high|mid} --json {ondemand|dom|zero-copy} --alloc {arena|pmr|heap} --strict`

### Генерация SQL-репозиториев

* `katana gen sql -i sql/ -o gen/`

### Режим разработки

* `katana dev [--hot] [--no-arena] [--mock-db] [--mock-cache] [--no-pin]`

### Миграции БД

* `katana db migrate up|down`
* `katana db status`
* `katana db create`

### Нагрузочное тестирование

* `katana bench -c bench/profile.toml --strict-latency --export-grafana --export-flame`

### Диагностика окружения

* `katana doctor` — проверка ядра/rlimit/kTLS/io_uring/таймеров и рекомендации по тюнингу.

---

## Политика и линтер (policy as code)

`katana lint` (clang-based, SARIF-отчёты для PR):

**Память/арены**

* запрет `new/delete` в контроллерах (только арена/pmr),
* запрет захвата аренных буферов в долгоживущие лямбды/`std::function`.

**Блокировки**

* запрет `std::mutex`/`std::condition_variable` в hot-path.

**Сеть/БД**

* обязательные дедлайны,
* только prepared-SQL,
* запрет строковых конкатенаций SQL.

**API-контракт**

* несоответствие сгенерированным сигнатурам — **ошибка**,
* DTO без валидации — **ошибка**.

**Perf**

* предупреждения о копиях больших объектов,
* `std::function` в критическом пути.

**Безопасность**

* лимиты на заголовки/тело, корректная обработка `Expect: 100-continue`.

Профили правил: `dev`, `prod-strict`, `legacy` (используются в CI).

---

## Кросс-платформенная поддержка

Абстракция I/O-backend:

* Linux: `epoll` / `io_uring`,
* macOS: `kqueue`,
* Windows: `IOCP`.

Сборка: `-DKATANA_POLL={epoll|io_uring|kqueue|iocp}` (автовыбор по платформе).
TLS: BoringSSL/OpenSSL; на Linux — kTLS при поддержке ядра.

Thread pinning (опциональная оптимизация):

* Linux: `sched_setaffinity`, NUMA-aware размещение,
* Windows: `SetThreadAffinityMask / GROUP_AFFINITY`,
* macOS: thread affinity API.

**Примечание**: корректность работы не зависит от pinning благодаря изоляции реакторов; единственные shared-счётчики (shutdown, выдача thread id) находятся в control-plane и не участвуют в обработке запросов. Pinning улучшает производительность (cache locality), но не обязателен. Dev-флаг `--no-pin` отключает pinning. CI собирает Linux/macOS/Windows; производительные тесты — только Linux.

---

## Безопасность и тестирование

* Фуззинг HTTP-парсера (libFuzzer) — обязателен в CI.
* Property-тесты валидаторов, round-trip ser/deser для DTO.
* E2E-тесты по контракту генерируются автоматически.
* Conformance-suite для runtime/генератора.
* Performance-budget: регрессия p99/p999 → fail.

---

## Границы применимости

Подходит:

* высоконагруженные API/шлюзы,
* биллинг/транзакционные системы/идемпотентные протоколы,
* ML-inference шлюзы и real-time инфраструктура.

Не подходит:

* MVP/CRUD без мониторинга,
* команды, не готовые к метрикам и нагрузочному анализу.

---

## Дорожная карта

### Этап 1 — Базовый runtime ✅

**Цель**: устойчивый server loop без гонок, ASan/LSan-clean; `hello-world p99 < 1.5–2.0 ms` на одном сокете.

- [x] Реализовать базовый event loop на epoll
  - [x] Абстракция `Reactor` с методами `run()`, `stop()`, `schedule()`
  - [x] Регистрация file descriptors (EPOLLIN/EPOLLOUT/EPOLLET)
  - [x] Обработка событий с таймаутами (wheel_timer)
- [x] Reactor-per-core архитектура
  - [x] Создание N реакторов по числу CPU cores
  - [x] Полная изоляция состояния между реакторами в data-plane (shared только сервисные счётчики: shutdown, выдача thread id)
  - [x] Опционально: thread pinning через `sched_setaffinity` (оптимизация производительности)
- [x] Vectored I/O
  - [x] Обёртки над `readv`/`writev`
  - [x] Управление scatter-gather буферами
- [x] Arena allocator (per-request)
  - [x] Монотонный аллокатор с фиксированными блоками
  - [x] Интеграция с `std::pmr::memory_resource`
  - [x] Reset арены после завершения запроса
- [x] HTTP/1.1 parser
  - [x] Парсинг request line (method, URI, version)
  - [x] Парсинг заголовков (складывание multiline)
  - [x] Chunked transfer encoding
  - [x] Keep-alive и connection pooling
- [x] HTTP/1.1 serializer
  - [x] Формирование response (status line, headers, body)
  - [x] Поддержка `Content-Length` и `Transfer-Encoding: chunked`
- [x] RFC 7807 (Problem Details)
  - [x] Структура `Problem` (type, title, status, detail, instance)
  - [x] Хелперы для типовых ошибок (400, 404, 500, 503)
- [x] Системные лимиты
  - [x] `rlimit` для файловых дескрипторов
  - [x] Лимиты на размер заголовков/body
  - [x] Graceful shutdown с дедлайном
- [x] Базовые тесты
  - [x] Unit-тесты парсера HTTP (39 tests passing)
  - [x] ASan/LSan прогоны (в CI pipeline)
  - [x] Hello-world benchmark (latency_benchmark)

**Дополнительно реализовано (STL-style RAII API)**:
- [x] `tcp_socket` — RAII wrapper для сокетов с move semantics
- [x] `tcp_listener` — RAII factory для accept с fluent interface
- [x] `fd_watch` — RAII registration handle для автоматического unregister
- [x] STL-compatible iterator interface для `reactor_pool` (range-based for loops)
- [x] Примеры: `raii_echo_server`, `raii_http_server` с monadic composition

---

### Этап 2 — OpenAPI → Compile-time API

**Цель**: любые расхождения API → ошибки компиляции; property-тесты валидаторов.

**Подэтапы и DoD**

1. ✅ Роутер + middleware-каркас
   - compile-time таблица маршрутов без аллокаций в hot-path; поддержка path params, 404/405/415.
   - middleware-chain с единым ABI (`req, ctx → result<response>`), конвейер без virtual/heap в критике.
   - тесты: path matching, приоритет статических/динамических сегментов, бенч dispatch-only.
2. ✅ Парсер OpenAPI → AST
   - YAML/JSON загрузка (без тяжёлых зависимостей), валидация спецификации, `$ref`-resolution, лимиты.
   - AST для paths/schemas/params/responses с нормализацией типов/format.
   - тесты: фикстуры валидных/битых спецификаций, property-тесты инвариантов AST.
3. ✅ DTO + валидация + JSON ser/deser
   - DTO на `std::pmr`/arena, `string_view` по умолчанию, enums → `enum class`.
   - валидаторы для required/ranges/pattern/uniqueItems/custom formats; nullable/optional корректно разведены.
   - JSON: zero-copy/ondemand профиль по умолчанию, streaming serializer; round-trip/property/fuzz тесты.
4. ✅ Генерация роутов и интерфейсов контроллеров
   - compile-time привязка метода/пути к сигнатурам контроллеров; статические ошибки при расхождении.
   - автоген интерфейсов контроллеров (виртуальный/CRTP слой), ручной код реализует только бизнес-логику.
   - тесты: negative compile-time кейсы, интеграция на фиктивной спецификации.
5. Интеграция в runtime + контрактные проверки
   - подключение роутера к HTTP серверу, хуки для auth/logging/tracing (пока no-op), дедлайны/лимиты.
   - conformance-тесты по OpenAPI фикстурам, latency/alloc бенч (dispatch+parse без бизнес-кода).
   - DoD: p99 dispatch не хуже текущего baseline, 0 heap alloc в hot-path, CI preset с unit+property+perf smoke.

**Stage 2.1 (router + middleware skeleton)**
- API: `katana/core/router.hpp`
  - `path_pattern::from_literal<"/users/{id}">()` — compile-time парсинг, приоритет статических сегментов над параметрами, query string отрезается.
  - `handler_fn` сигнатура: `(const http::request&, http::request_context&) -> result<http::response>`.
  - `middleware_fn` сигнатура: `(req, ctx, next_fn) -> result<http::response>`; `make_middleware_chain(std::array<middleware_fn, N>)` собирает цепочку без heap.
  - `request_context` содержит `monotonic_arena&` и `path_params` (lookup по имени без аллокаций).
  - Ошибки: `not_found` (404) и `method_not_allowed` (405) через `katana::error_code` без исключений; `dispatch_or_problem` мапит их в RFC7807 + `Allow` header.
  - `router_handler` — адаптер к харнесам/серверу: `(req, arena) -> response`, zero-alloc hot-path.
- Минимальный скелет исполнения без привязки к серверу; готов для подключения к HTTP loop и future OpenAPI codegen.
- Инварианты hot-path: без виртуальных вызовов/heap-alloc, линейное сканирование таблицы маршрутов со специфичностью по числу literal-сегментов.
- Бенч: `router_benchmark` (ENABLE_BENCHMARKS=ON) — dispatch hits/misses/405 без аллокаций.

Пример использования:

```cpp
using namespace katana::http;

route_entry routes[] = {
  {method::get,
   path_pattern::from_literal<"/users/{id}">(),
   handler_fn([](const request& req, request_context& ctx) {
     auto id = ctx.params.get("id").value_or("");
     return response::ok(std::string{id});
   })}
};

router r(routes);
monotonic_arena arena;
request_context ctx{arena};
request req;
req.http_method = method::get;
req.uri = "/users/42";
auto res = dispatch_or_problem(r, req, ctx); // 404/405 → ProblemDetails + Allow
```

- [ ] Парсер OpenAPI 3.x
  - [ ] Загрузка YAML/JSON спецификации
  - [ ] Валидация соответствия стандарту OpenAPI
  - [x] Построение базового AST (paths, schemas, parameters) — `katana/core/openapi_ast.hpp` (arena-backed)
  - [x] Лоадер-каркас: `load_from_string` проверяет OpenAPI 3.x версию и возвращает AST-заготовку
- [ ] Генерация роутов
  - [ ] Compile-time таблица маршрутов (constexpr map)
  - [ ] Path templates → regex/prefix trees
  - [ ] Привязка HTTP-метода к handler-функции
- [ ] Генерация DTO
  - [ ] C++ структуры из `components/schemas`
  - [ ] Использование `std::string_view` для zero-copy
  - [ ] Вложенные объекты и массивы
  - [ ] Enum → `enum class`
- [ ] Генерация валидаторов
  - [ ] `required`, `minLength`, `maxLength`, `pattern`
  - [ ] `minimum`, `maximum`, `multipleOf`
  - [ ] `minItems`, `maxItems`, `uniqueItems`
  - [ ] Custom formats (email, uuid, date-time)
- [ ] Генерация сериализаторов/десериализаторов
  - [ ] JSON → DTO (simdjson ondemand режим)
  - [ ] DTO → JSON (streaming serializer)
  - [ ] Обработка nullable/optional полей
- [ ] Обработка `x-katana-*` расширений
  - [ ] `x-katana-alloc: {arena|pmr|heap}`
  - [ ] `x-katana-json: {ondemand|dom|zero-copy}`
  - [ ] `x-katana-cache`, `x-katana-rate-limit`
- [x] CLI команда `katana gen openapi`
  - [ ] Опции: `-i`, `-o`, `--layer`, `--strict`
  - [ ] Вывод статистики генерации
- [ ] Тесты
  - [ ] Property-тесты валидаторов (fuzzing входов)
  - [ ] Round-trip ser/deser тесты
  - [ ] Compile-time проверка на несуществующий endpoint

---

### Этап 3 — SQL-генерация

**Цель**: отсутствует N+1 в демо-сервисе; стабильный p99 CRUD.

- [ ] Media type registry + бинарные кодеки (CBOR/MessagePack)
  - [ ] Централизованное сопоставление MIME→кодек (request/response)
  - [ ] Content negotiation для JSON/CBOR/MessagePack
  - [ ] Интеграция в codegen и runtime (body parse/serialize)

- [ ] Парсер SQL-файлов
  - [ ] Аннотации `-- name: <query_name> :one|:many|:exec`
  - [ ] Извлечение параметров `$1`, `$2`, ...
  - [ ] Определение возвращаемых колонок (через `EXPLAIN`)
- [ ] Генерация моделей
  - [ ] C++ структуры из `SELECT` полей
  - [ ] Маппинг SQL-типов → C++ (`int4` → `int32_t`, `text` → `std::string_view`)
  - [ ] Nullable колонки → `std::optional`
- [ ] Генерация репозиториев
  - [ ] Класс репозитория с методами по SQL-файлам
  - [ ] Методы принимают `katana::ctx&` и параметры запроса
  - [ ] Возвращают `result<T>` или `result<std::vector<T>>`
- [ ] Поддержка транзакций
  - [ ] `ctx.tx().begin()`, `commit()`, `rollback()`
  - [ ] RAII-обёртка для auto-rollback
  - [ ] Вложенные транзакции (savepoints)
- [ ] UPSERT и conflict resolution
  - [ ] `ON CONFLICT DO UPDATE`
  - [ ] `ON CONFLICT DO NOTHING`
  - [ ] Генерация методов из аннотаций
- [ ] Bulk-операции
  - [ ] Batch insert через `COPY` или `INSERT ... VALUES`
  - [ ] Batch update через `UPDATE ... FROM unnest()`
- [ ] libpq binary protocol
  - [ ] Prepared statements (PQprepare/PQexecPrepared)
  - [ ] Binary формат параметров и результатов
  - [ ] Обработка ошибок (PQresultStatus)
- [ ] Per-core connection pools
  - [ ] Каждый reactor владеет своим пулом
  - [ ] Настраиваемые размеры пула
  - [ ] Health checks и переподключения
- [ ] Prefetch механизм
  - [ ] Аннотация `x-katana-prefetch: [user.posts, user.profile]`
  - [ ] Генерация batch-запросов
  - [ ] Сборка результатов в единую структуру
- [ ] CLI команда `katana gen sql`
  - [ ] Опции: `-i`, `-o`, `--db-url` (для introspection)
- [ ] Тесты
  - [ ] Integration тесты с testcontainers (PostgreSQL)
  - [ ] Проверка отсутствия N+1 (query counter)
  - [ ] p99 benchmark для CRUD операций

---

### Этап 4 — Redis/кэш

**Цель**: кэш и лимиты — часть контракта, не middleware.

- [ ] Redis клиент (RESP3 protocol)
  - [ ] Async команды (GET/SET/DEL/EXPIRE)
  - [ ] Pipelining для batch операций
  - [ ] Connection pool per-core
- [ ] Парсинг аннотаций кэша
  - [ ] `x-katana-cache: { ttl, jitter, keys, invalidate_on }`
  - [ ] `x-katana-idempotency: { ttl, key_from }`
  - [ ] `x-katana-rate-limit: { requests, window, by }`
- [ ] Генерация кэширующих обёрток
  - [ ] Wrap контроллера: проверка кэша → handler → сохранение в кэш
  - [ ] Ключи из параметров запроса/заголовков
  - [ ] TTL с jitter для избежания thundering herd
- [ ] Single-flight механизм
  - [ ] Дедупликация параллельных запросов с одинаковым ключом
  - [ ] Ожидание завершения первого запроса
- [ ] Инвалидация кэша
  - [ ] `invalidate_on: [POST /users, DELETE /users/:id]`
  - [ ] Поддержка префиксов и wildcard-ключей
  - [ ] Генерация хуков инвалидации
- [ ] Idempotency для POST/PUT
  - [ ] `Idempotency-Key` header
  - [ ] Сохранение результата в Redis на TTL
  - [ ] Возврат кэшированного ответа при повторе
- [ ] Rate limiting
  - [ ] Token bucket / sliding window
  - [ ] Per-user, per-IP, global лимиты
  - [ ] Ответ 429 с `Retry-After`
- [ ] Stale-while-revalidate
  - [ ] Отдача устаревшего кэша при обновлении
  - [ ] Фоновое обновление
- [ ] Тесты
  - [ ] Unit-тесты Redis клиента
  - [ ] E2E тесты кэширования (cache hit/miss)
  - [ ] Проверка idempotency (повторные запросы)
  - [ ] Rate limit тесты (превышение лимита → 429)

---

### Этап 5 — Observability

**Цель**: видимость проблем из коробки: RPS, p95/p99/p999, очереди, backpressure, арены.

- [ ] OpenTelemetry интеграция
  - [ ] Span для каждого HTTP-запроса
  - [ ] Span для SQL-запросов (с query text)
  - [ ] Span для Redis-операций
  - [ ] Trace propagation (W3C Trace Context)
- [ ] Prometheus метрики
  - [ ] HTTP метрики: `http_requests_total`, `http_request_duration_seconds`
  - [ ] SQL метрики: `db_query_duration_seconds`, `db_connections_active`
  - [ ] Redis метрики: `redis_commands_total`, `redis_command_duration_seconds`
  - [ ] Системные метрики: CPU usage per-core, memory allocations
  - [ ] Arena метрики: `arena_bytes_allocated`, `arena_resets_total`
  - [ ] Backpressure: `reactor_queue_length`, `reactor_processing_delay`
- [ ] Структурные JSON-логи
  - [ ] Формат: timestamp, level, message, trace_id, span_id
  - [ ] Контекстные поля (request_id, user_id, endpoint)
  - [ ] Уровни: DEBUG, INFO, WARN, ERROR
- [ ] Готовые Grafana дашборды
  - [ ] Dashboard: HTTP Overview (RPS, latencies, error rate)
  - [ ] Dashboard: Database (queries/sec, latencies, pool usage)
  - [ ] Dashboard: Redis (ops/sec, hit rate, latencies)
  - [ ] Dashboard: System (CPU, memory, arena usage)
- [ ] Экспорт метрик
  - [ ] Prometheus scrape endpoint `/metrics`
  - [ ] OTLP exporter для traces (gRPC/HTTP)
- [ ] Тесты
  - [ ] Проверка генерации spans
  - [ ] Проверка счётчиков метрик (increment after request)
  - [ ] JSON log parsing тесты

---

### Этап 6 — Dev-режим

**Цель**: «code → reload → запрос» < 2 с; DX уровня Node/FastAPI.

- [ ] CLI команда `katana dev`
  - [ ] Опции: `--hot`, `--no-arena`, `--mock-db`, `--mock-cache`, `--no-pin`
  - [ ] Автоподнятие зависимостей (docker-compose)
- [ ] Hot-reload контроллеров
  - [ ] File watcher (inotify/kqueue)
  - [ ] Пересборка только изменённых контроллеров (incremental)
  - [ ] Динамическая загрузка `.so` без рестарта реактора
  - [ ] Graceful transition (новые запросы → новый код, старые завершаются)
- [ ] Быстрая сборка
  - [ ] clang + lld (fast linker)
  - [ ] ccache для кэширования объектных файлов
  - [ ] Precompiled headers для stdlib и framework headers
- [ ] Автоподнятие зависимостей
  - [ ] PostgreSQL (testcontainers или docker-compose)
  - [ ] Redis
  - [ ] Prometheus
  - [ ] Grafana (с преднастроенными дашбордами)
  - [ ] Jaeger/Tempo для трейсов
- [ ] Моки репозиториев
  - [ ] `--mock-db`: использовать in-memory хранилище вместо PG
  - [ ] Предзаполненные данные для разработки
- [ ] Моки Redis
  - [ ] `--mock-cache`: in-memory реализация
- [ ] Отключение оптимизаций для dev
  - [ ] `--no-arena`: использовать стандартный аллокатор
  - [ ] `--no-pin`: не привязывать потоки к ядрам
  - [ ] Debug symbols и AddressSanitizer
- [ ] Тесты
  - [ ] Проверка hot-reload (изменение → перезагрузка → новый код работает)
  - [ ] Время сборки < 2 секунд для изменения одного файла

---

### Этап 7 — Протоколы/производственные профили

**Цель**: линейный throughput, предсказуемые хвосты.

- [ ] HTTP/2 support
  - [ ] HPACK compression для заголовков
  - [ ] Stream multiplexing
  - [ ] Server push (опционально)
  - [ ] ALPN negotiation (h2/http/1.1)
- [ ] HTTP/3 / QUIC
  - [ ] QUIC transport (на базе picoquic/quiche)
  - [ ] QPACK для заголовков
  - [ ] 0-RTT connection establishment
- [ ] Zero-copy статика
  - [ ] `sendfile()` для больших файлов
  - [ ] kTLS для TLS offload в ядро (если поддерживается)
  - [ ] `splice()` для proxy режима
- [ ] io_uring backend
  - [ ] Абстракция `IoUringReactor`
  - [ ] Submission queue batching
  - [ ] Completion queue обработка
  - [ ] Fallback на epoll если io_uring недоступен
- [ ] NUMA-aware раскладка
  - [ ] Определение NUMA topology
  - [ ] Размещение реакторов на NUMA-локальных ядрах
  - [ ] Аллокация памяти из NUMA-локальных узлов
- [ ] TLS настройки
  - [ ] BoringSSL/OpenSSL интеграция
  - [ ] Кэш сессий TLS
  - [ ] OCSP stapling
- [ ] Настройки TCP
  - [ ] `TCP_NODELAY` для низких задержек
  - [ ] `SO_REUSEPORT` для распределения нагрузки
  - [ ] `TCP_FASTOPEN`
- [ ] Тесты
  - [ ] HTTP/2 compliance тесты (h2spec)
  - [ ] Benchmark HTTP/2 vs HTTP/1.1
  - [ ] io_uring throughput тесты
  - [ ] NUMA pinning влияние на p99

---

### Этап 8 — Тесты и CI

**Цель**: производительность — гарантируемое свойство сборки.

- [ ] E2E тесты
  - [ ] Автогенерация из OpenAPI (все endpoints)
  - [ ] Проверка response schemas
  - [ ] Проверка error cases (4xx, 5xx)
- [ ] Property-based тесты
  - [ ] Fuzzing валидаторов (random valid/invalid inputs)
  - [ ] Round-trip ser/deser для всех DTO
  - [ ] SQL injection тесты (prepared statements должны блокировать)
- [ ] Фуззинг HTTP-парсера
  - [ ] libFuzzer интеграция
  - [ ] Corpus семплов (валидные HTTP запросы)
  - [ ] Запуск в CI (обязательно)
- [ ] Нагрузочные профили
  - [ ] Профили: `light`, `medium`, `heavy`, `spike`
  - [ ] Инструмент: wrk/vegeta/Gatling
  - [ ] Сбор метрик: RPS, p50/p95/p99/p999, errors
- [ ] Flamegraph генерация
  - [ ] Профилирование через perf/dtrace
  - [ ] Генерация flamegraph.svg
  - [ ] Загрузка в CI artifacts
- [ ] Performance-budget
  - [ ] Определение baseline (например, `p99 < 5ms`)
  - [ ] Сравнение с предыдущим коммитом
  - [ ] Fail сборки при деградации > 10%
- [ ] CI pipeline
  - [ ] Build: Linux (gcc/clang), macOS (clang), Windows (MSVC)
  - [ ] Tests: unit, integration, E2E
  - [ ] Sanitizers: ASan, UBSan, TSan
  - [ ] Fuzzing: continuous fuzzing с OSS-Fuzz
  - [ ] Benchmark: автозапуск на каждом PR
  - [ ] Conformance: проверка соответствия RFCs
- [ ] Conformance suite
  - [ ] HTTP/1.1 conformance (httptest)
  - [ ] OpenAPI contract tests
  - [ ] SQL semantics tests
- [ ] Тесты
  - [ ] CI проходит для всех платформ
  - [ ] Fuzzer находит 0 crashes за 1 час
  - [ ] Performance budget не нарушается

---

### Этап 9 — Кодоген расширений

**Цель**: самодостаточный контракт, минимум ручной поддержки клиентов.

- [ ] SDK генераторы
  - [ ] TypeScript: fetch-based client, типы из OpenAPI
  - [ ] Go: net/http client, structs из schemas
  - [ ] Rust: reqwest client, serde structs
  - [ ] Python: httpx/requests client, pydantic models
- [ ] Общие фичи SDK
  - [ ] Retry с exponential backoff
  - [ ] Timeout настройка
  - [ ] Автоматическая десериализация ответов
  - [ ] Обработка RFC 7807 ошибок
- [ ] Административные интерфейсы
  - [ ] Генерация CRUD UI из OpenAPI (React/Vue/Svelte шаблоны)
  - [ ] Таблицы с пагинацией/сортировкой/фильтрацией
  - [ ] Формы создания/редактирования с валидацией
- [ ] Миграции БД
  - [ ] Автогенерация миграций из SQL-схем
  - [ ] Diff между версиями схемы
  - [ ] Up/down миграции
  - [ ] CLI: `katana db migrate up|down`, `katana db status`
- [ ] Проверка совместимости
  - [ ] OpenAPI breaking changes detection
  - [ ] SQL schema breaking changes
  - [ ] Semantic versioning enforcement
  - [ ] CI проверка совместимости с предыдущей версией
- [ ] Документация
  - [ ] Автогенерация API docs из OpenAPI (Swagger UI/ReDoc)
  - [ ] Примеры запросов для каждого endpoint
  - [ ] Changelog из git history + OpenAPI diff
- [ ] CLI команды
  - [ ] `katana gen sdk --lang {ts|go|rust|py} -i api/openapi.yaml -o sdk/`
  - [ ] `katana gen admin-ui --framework {react|vue|svelte} -o admin/`
  - [ ] `katana db create`, `katana db migrate`, `katana db status`
- [ ] Тесты
  - [ ] Сгенерированные SDK проходят E2E тесты
  - [ ] Admin UI отображает все endpoints
  - [ ] Миграции корректно применяются и откатываются

---

### Этап 10 — Стабилизация и прод-кейс

**Цель**: подтверждение применимости в реальном проде.

- [ ] Выбор целевого прод-проекта
  - [ ] Критерии: высокая нагрузка, строгие SLA, реальные пользователи
  - [ ] Примеры: биллинг-сервис, API gateway, real-time аналитика
- [ ] Профилирование в проде
  - [ ] CPU profiling (perf/flamegraph)
  - [ ] Memory profiling (heaptrack/massif)
  - [ ] Latency analysis (p95/p99/p999 breakdown)
  - [ ] Hotspot identification
- [ ] Фиксы узких мест
  - [ ] Оптимизация hot paths
  - [ ] Уменьшение аллокаций
  - [ ] Избежание system calls в критическом пути
  - [ ] Lock-free структуры данных где нужно
- [ ] Стандартные конфигурации
  - [ ] Profiles: `dev`, `staging`, `prod`, `prod-high-throughput`, `prod-low-latency`
  - [ ] Рекомендуемые настройки: число реакторов, размеры пулов, TTL кэша
  - [ ] Sysctl параметры (Linux): `net.core.somaxconn`, `net.ipv4.tcp_*`, etc.
- [ ] Рекомендации деплоя
  - [ ] Dockerfile (multi-stage build)
  - [ ] Kubernetes manifests (deployment, service, HPA)
  - [ ] Настройки ресурсов (CPU/memory requests/limits)
  - [ ] Health checks (liveness, readiness)
  - [ ] Graceful shutdown
- [ ] Мониторинг и алертинг
  - [ ] Алерты на p99 degradation
  - [ ] Алерты на error rate spike
  - [ ] Алерты на database/redis connection pool exhaustion
  - [ ] Runbook для типовых проблем
- [ ] Документация
  - [ ] Production checklist
  - [ ] Tuning guide
  - [ ] Troubleshooting guide
  - [ ] Performance best practices
- [ ] Метрики успеха
  - [ ] p99 < 5ms под нагрузкой
  - [ ] RPS > 100k на одном инстансе (hello-world)
  - [ ] 99.99% uptime
  - [ ] Zero memory leaks
- [ ] Тесты
  - [ ] Soak tests (24h+ under load)
  - [ ] Chaos engineering (kill random instances, network delays)
  - [ ] Load tests с production-like трафиком

---

## Организация репозитория

* `/cli` — katana CLI
* `/runtime` — reactor, io_backends, arena, http, sql, redis, telemetry
* `/codegen/core` — парсеры, AST, проверка совместимости
* `/codegen/templates` — C++/TS/Go/Rust шаблоны
* `/codegen/plugins` — расширения генератора
* `/tools/katana-lint` — правила и интеграция с clang-tidy/LibTooling
* `/tools/katana-dev` — dev orchestration
* `/examples/high_crud` — полный scaffold
* `/examples/mid_custom` — переопределение сериализации/валидатора
* `/examples/low_raw` — raw reactor/libpq/redis
* `/docs/RFCs` — спецификации Core/Codegen/Lint
* `/docs/Conformance` — тесты на соответствие

---

## Результат

* Предсказуемые задержки, контролируемый p99/p999.
* Отсутствие глобальных очередей/гонок/GC.
* SQL-first вместо ORM, но без ручного бойлерплейта.
* Кэш/идемпотентность/лимиты — часть контракта.
* Наблюдаемость и performance-budget из коробки.
* Разработчик пишет **бизнес-логику**, инструментарию поручены монотонные задачи.

## Быстрый старт (Mid layer)

```bash
# Шаг 1 — создать проект
katana new mysvc --layer mid
cd mysvc

# Ключевые директории:
# api/     — OpenAPI спецификация (источник истины для API)
# sql/     — SQL схемы и запросы (источник истины для моделей/репозиториев)
# gen/     — автогенерируемый код (не редактируется вручную)
# src/     — бизнес-логика приложения
# include/ — заголовки для пользовательского кода
````

### Шаг 2 — описать API (api/openapi.yaml)

```yaml
paths:
  /users/{id}:
    get:
      x-katana-cache:
        ttl: 10s
      responses:
        200:
          $ref: "#/components/schemas/User"

components:
  schemas:
    User:
      type: object
      properties:
        id: { type: integer }
        name: { type: string }
```

### Шаг 3 — описать SQL (sql/get_user.sql)

```sql
-- name: get_user :one
SELECT id, name
FROM users
WHERE id = $1;
```

### Шаг 4 — сгенерировать API и репозитории

```bash
katana gen openapi -i api/openapi.yaml -o gen/ --strict
katana gen sql -i sql/ -o gen/
```

### Шаг 5 — реализовать бизнес-логику (src/users_controller.cpp)

```cpp
#include <gen/users_api.hpp>

class UsersController : public gen::UsersApi {
public:
  katana::result<UserDto> get_user(int64_t id, katana::ctx& ctx) override {
    auto user = repo_.get_user(ctx, id);
    if (!user)
      return ctx.problem.not_found("user.not_found").detail("id", id);
    return *user;
  }

private:
  gen::UsersRepo repo_; // автогенерированный репозиторий
};
```

### Шаг 6 — запустить dev-режим

```bash
katana dev --hot
```

Автоматически поднимутся:

* локальный PostgreSQL
* локальный Redis
* Prometheus + Grafana + OpenTelemetry
* **hot-reload контроллеров без перезапуска реакторов**

### Шаг 7 — проверить

```bash
curl http://localhost:8080/users/1
```

---

## Что вы получаете

* API **соответствует OpenAPI** → любое расхождение = **ошибка компиляции**
* SQL **соответствует моделям** → несоответствие = **ошибка генерации**
* Запрос обрабатывается **внутри одного реактора** → **нет гонок и очередей**
* Память выделяется в **арене запроса** и освобождается **одной операцией**
* Метрики и трейсы **включены из коробки**

> Разработчик пишет **только бизнес-логику** — инфраструктура генерируется и контролируется инструментарием.
