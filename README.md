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
* 03.06 08:21 — chore: sync README with benchmark results (557717e)
<!-- LATEST_UPDATE_END -->

## Текущее состояние (реальность)

**Реализовано (Stage 1 + Stage 2 + Stage 3 + рабочий Stage 4 slice):**
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
  - x-katana-* extensions (declarative; parsed + comment-only, runtime enforcement — Stage 5)
- ✅ Unit/integration/fuzz тесты
- ✅ Media type registry и общий механизм `Content-Type`/`Accept` для JSON, CBOR, MessagePack
- ✅ Conformance harness для generated router/bindings по OpenAPI fixture (`test/conformance`, 15 end-to-end сценариев)
- ✅ **SQL-first data layer**
  - `katana_gen sql` для `.sql`-каталогов с `:one/:many/:exec`
  - typed SQL models/repositories
  - typed PostgreSQL array params/results для V1 bulk path (`bigint[]`, `text[]`, `bool[]`, `float8[]`)
  - `UPSERT` и basic bulk через обычный SQL contract (`INSERT ... ON CONFLICT`, `UNNEST(...)`)
  - PostgreSQL runtime на `libpq` + prepared statements
  - pool/executor path для generated repositories
  - транзакции и live PostgreSQL integration tests
  - canonical demo CRUD service на `OpenAPI + generated SQL`
  - `wrk`-based HTTP load path для read-heavy и mixed CRUD сценариев
  - Stage 4 SQL perf budget/gate в benchmark runner (`--stage 15 16 17 18 --fail-on-budget`)

**В разработке / не реализовано:**
- ⏳ Stage 4 closeout: финальная vertical-slice полировка и stage sign-off
- ⏳ Redis клиент
- ⏳ OpenTelemetry tracing
- ⏳ Prometheus metrics
- ⏳ Structured logging
- ✅ Canonical Linux/WSL test path и preset-based CI runner (`scripts/run_canonical_pipeline.sh`, `make ci`)

Разделы README/ARCHITECTURE описывают целевое состояние фреймворка. То, что уже работает — помечено ✅ выше.

## Getting Started (сегодня)

1. Зависимости: CMake ≥ 3.20, Ninja, Clang ≥ 16 или GCC ≥ 12, `liburing-dev` (для io_uring пресетов).
2. Конфигурация: `cmake --preset debug` (доступны также `release`, `asan`, `tsan`, `ubsan`, `io_uring-*`, `bench`, `examples`).
3. Сборка: `cmake --build --preset debug`.
4. Тесты: `ctest --preset debug` (используется лёгкий gtest-совместимый харнес из `test/gtest/gtest.h`).
5. Canonical preset pipeline для Linux/WSL/CI: `./scripts/run_canonical_pipeline.sh --preset debug` или `make ci PRESET=debug`.
6. Примеры: `cmake --build --preset examples && ./build/examples/hello_world_server`.
7. Бенчмарки: `cmake --preset bench && cmake --build --preset bench && python3 scripts/run_benchmarks.py --include-e2e` (если build trees несколько, можно явно задать `--build-dir ...`; для reference SQL цифр полезно `--cpu-governor performance`).
8. Удобно через Makefile: `make build PRESET=debug`, `make test PRESET=debug`, `make ci PRESET=debug`, `make bench`, `make fuzz`, `make profile`.
9. CRUD бенч: по умолчанию in-memory; для высокого RPS можно задать `KATANA_CRUD_BACKEND=memcached` (опционально `MEMCACHED_HOST`/`MEMCACHED_PORT`). Docker бенч-сборка поднимает memcached автоматически.

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
- `generated_handlers.hpp` — sync `api_handler`, optional `async_api_handler` и `async_api_handler_base` для async-first сервисов
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
- ✅ x-katana-* extensions — declarative annotations: parsed from spec, emitted as comments in generated code (cache, alloc, rate-limit). Runtime enforcement — Stage 5

📖 **Подробная документация:** [docs/OPENAPI.md](docs/OPENAPI.md)

---

## Разработка и стиль

* Форматирование — `.clang-format`, статический анализ — `.clang-tidy`.
* Локальный авто-линт: `pip install pre-commit && pre-commit install` (clang-format, cmake-format, базовые проверки YAML/конфликтов).
* Перед PR: `./scripts/run_canonical_pipeline.sh --preset debug`; низкоуровневые изменения гонять тем же runner'ом с sanitizer-пресетами (`asan/tsan/ubsan`).
* Дополнительно: гайды в `CONTRIBUTING.md` и `docs/TESTING.md`.
* Для тестирования без реактора добавлены харнесы: `test/support/http_handler_harness.hpp` (оборачивает handler над Request/Response) и `test/support/virtual_event_loop.hpp` (фейковый event loop с виртуальным временем).

### 📈 Сравнение HTTP-фреймворков

Сравнивали KATANA с:

- `actix-web`
- `axum`
- `ntex`
- `Drogon`
- `FastAPI`

Стенд сравнения:

- Ubuntu 24.04 VM
- один сервер за раз, без параллельных запусков
- те же `wrk` Lua-сценарии, что и у KATANA stage 9-12
- `median of 3`
- `workers=4` для всех серверов

Подробная методика и команды лежат в [comparisons/http_frameworks/README.md](comparisons/http_frameworks/README.md).

![HTTP framework comparison](comparisons/table.jpg)

<!-- BENCH_SUMMARY_START -->

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

Не меняют стандарт, добавляют **декларативные** аннотации на уровне operation.
На текущем этапе (Stage 2–3) аннотации:

* **парсятся** loader'ом из OpenAPI JSON/YAML,
* **сохраняются** в AST (`operation` struct),
* **выводятся** в generated код как комментарии (`// @cache:`, `// @alloc:`, `// @rate-limit:`).

**Runtime enforcement** (фактическое применение кэша, rate limit, аллокации) запланировано на Stage 5.

| Extension | Accepted values | Example |
|---|---|---|
| `x-katana-cache` | string (`"300s"`, `"5m"`) or `true`/`false` | `x-katana-cache: "5m"` |
| `x-katana-alloc` | string (`"pool"`, `"arena"`) or number (`4096`) | `x-katana-alloc: "pool"` |
| `x-katana-rate-limit` | string (`"100/s"`, `"1000/m"`) | `x-katana-rate-limit: "100/s"` |

Расширения, упомянутые в roadmap, но ещё не реализованные:
`x-katana-idempotency` (Stage 5).

Подробная спецификация: [docs/X_KATANA_EXTENSIONS.md](docs/X_KATANA_EXTENSIONS.md).

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

**Статус**: завершён.

**Что считаем закрытым**
- compile-time router с middleware, `dispatch_or_problem`, 404/405 и `Allow` header.
- arena-backed OpenAPI loader для JSON/YAML, `$ref`, `allOf`, базовой валидации спецификации.
- `katana_gen`: DTO, валидаторы, JSON parsers/serializers, route table, handler interfaces, router bindings.
- unit/integration/benchmark контур для router, loader и codegen.

**Почему этап закрыт**
- API-контракт теперь материализуется в код: роуты, DTO и bindings генерируются из OpenAPI и валидируются тестами.
- Stage 2 больше не рассматривается как backlog для доработки; новые работы поверх OpenAPI идут уже как развитие Stage 3+.

**Границы этапа**
- В Stage 2 не входят SQL, Redis, OpenTelemetry, Prometheus, structured logging и production orchestration.
- Дополнительные протоколы и SDK-генераторы вынесены в дальний backlog и не блокируют следующий этап.

---

### Этап 3 — Runtime Hardening + Contract Surface

**Фокус**: довести текущий HTTP/OpenAPI runtime до состояния, на которое можно безопасно навешивать storage и policy-слой.

**Что входит**
- media type registry и общий механизм `Content-Type`/`Accept` для JSON, CBOR, MessagePack;
- единый conformance harness для generated router/bindings по OpenAPI fixtures;
- стабилизация тестового контура: одинаковые сценарии запуска в Linux/WSL, perf smoke, нормальный preset path;
- ~~ревизия `x-katana-*` extension contract~~ — **выполнено**: см. [docs/X_KATANA_EXTENSIONS.md](docs/X_KATANA_EXTENSIONS.md).

**Текущий статус**
- ✅ media type registry уже подключён в runtime/codegen; CBOR/MessagePack на Stage 3 остаются codec stubs;
- ✅ conformance harness уже существует: [docs/CONFORMANCE.md](docs/CONFORMANCE.md), fixture в `test/conformance/fixtures/petstore_minimal.json`, pre-generated headers committed;
- ✅ ревизия `x-katana-*` contract завершена;
- ✅ canonical Linux/WSL CI path сведён к единому preset runner'у и используется в workflow'ах;
- ✅ Stage 3 closeout завершён.

**Что не входит**
- SQL runtime, Redis runtime, tracing exporters.

**DoD**
- весь current HTTP/OpenAPI контур зелёный в canonical Linux/WSL CI;
- media type registry подключён и используется codegen/runtime без дублирования логики;
- есть conformance suite для generated endpoints, а не только unit-тесты отдельных артефактов;
- README и docs описывают фактическое состояние, а не целевую картину.

---

### Этап 4 — PostgreSQL / SQL-first Data Layer

**Фокус**: добавить data access как часть контракта, а не как ручной слой вокруг framework.

**Текущее состояние**
- ✅ формат SQL-артефактов и аннотаций (`:one`, `:many`, `:exec`)
- ✅ `katana gen sql` с генерацией typed repositories и result-models
- ✅ `UPSERT` через обычный SQL contract (`INSERT ... ON CONFLICT`)
- ✅ basic bulk path через typed array params + `UNNEST(...)`
- ✅ PostgreSQL runtime на `libpq`/prepared statements
- ✅ pool-backed execution path для generated repositories
- ✅ базовые транзакции
- ✅ integration tests с PostgreSQL
- ✅ canonical demo CRUD service на `OpenAPI + generated SQL`
- ✅ canonical CRUD lifecycle integration over generated router
- ✅ N+1 guard: single SQL round-trip per CRUD op в live PostgreSQL integration tests
- ✅ generated async repository methods (`*_async`) для typed SQL contract
- ✅ generated async OpenAPI handler contract (`async_api_handler` / `async_api_handler_base`)
- ✅ `wrk`-bench path для read-heavy и mixed CRUD сценариев
- ✅ formal benchmark budget / gate для Stage 15-18 (`scripts/run_benchmarks.py --fail-on-budget`)
- ✅ README/docs hardening по supported SQL contract и V1 ограничениям
- ⏳ финальная demo vertical-slice полировка

**Что входит**
- формат SQL-артефактов и аннотаций (`:one`, `:many`, `:exec`);
- `katana gen sql` с генерацией typed repositories и result-models;
- scalar + 1-D array parameters/results для PostgreSQL V1 (`bigint[]`, `text[]`, `bool[]`, `float8[]`);
- PostgreSQL runtime на `libpq`/prepared statements с per-reactor pools;
- транзакции, UPSERT, базовые bulk-операции;
- integration tests с PostgreSQL и benchmark на CRUD/read-heavy сценариях.

**Поддерживаемый SQL contract (V1)**
- `:one` / `:many` / `:exec` как публичные режимы генерации;
- `UPSERT` описывается обычным `INSERT ... ON CONFLICT ... RETURNING`;
- bulk insert/update path описывается обычным SQL через `UNNEST($1::type[], ...)`;
- scalar types: `bool`, `int2/int4/int8`, `float4/float8/numeric`, `text`;
- array types: `bool[]`, `int2[]`, `int4[]`, `int8[]`, `float4[]`, `float8[]`, `numeric[]`, `text[]`.

**Ограничения V1**
- bulk path сейчас ориентирован на PostgreSQL array params, а не на отдельный batch DSL;
- multidimensional arrays и composite array elements не поддерживаются;
- generated async HTTP/SQL contracts уже есть, но benchmark/demo write path по умолчанию всё ещё остаётся на sync `libpq`, потому что текущий blocking runtime пока не даёт бесплатного выигрыша на durable mixed CRUD;
- reactor-bound nonblocking PostgreSQL executor уже есть для generated async `query/exec` внутри HTTP handler scope, но mixed CRUD всё ещё в первую очередь ограничен durable write path и queueing pressure нужно учитывать отдельно от чистой SQL latency.

**Benchmark budget / gate**
- Базовый gate для SQL части закреплён в `scripts/run_benchmarks.py` и включается через `--fail-on-budget`.
- Рекомендуемый прогон: `python3 scripts/run_benchmarks.py --stage 15 16 17 18 --fail-on-budget --cpu-governor performance`.
- `--cpu-governor performance` пытается переключить governor через sysfs или `sudo -n`; если прав не хватает, выставьте governor вручную перед прогоном.
- Для budget-решения используйте default repeat policy runner’а; single-run `wrk` на Stage 17/18 годится для локального smoke, но не для жёсткого perf-решения.
- Для steady-state диагностики есть отдельный режим `--wrk-reuse-server`: он переиспользует один живой сервер на весь wrk stage, убирает cold-start noise и лучше показывает hot path, но не заменяет default cold gate.
- Stage 16 runner теперь масштабирует `KATANA_SQL_BENCH_THREADS` по машине, а Stage 18 использует отдельный peak-profile worker count; это важно для blocking `libpq` path и не эквивалентно старому legacy `4-thread` запуску.
- Для benchmark fixture без `docker-proxy` на Linux используйте `docker/sql/docker-compose.hostnet.yml` или `KATANA_SQL_DOCKER_NETWORK_MODE=host` в `scripts/run_sql_postgres_smoke.sh`; в этом режиме DSN по умолчанию `host=127.0.0.1 port=5432 ...`.
- Это local sanity gate для текущего blocking `libpq` path; reference numbers из `BENCHMARK_RESULTS.md` нужно собирать при `cpu_governor=performance`.

| Scope | Benchmark | Budget |
|-------|-----------|--------|
| Stage 15 | `postgres repo get_user (1 row)` | `>= 12k ops/sec`, `mean <= 85 us`, `tail p99 <= 140 us` |
| Stage 15 | `postgres repo list_users (128 rows)` | `>= 9k ops/sec`, `mean <= 120 us`, `tail p99 <= 170 us` |
| Stage 15 | `postgres repo touch_user (exec)` | `>= 10.5k ops/sec`, `mean <= 100 us`, `tail p99 <= 120 us` |
| Stage 16 | `postgres concurrent get_user` | `>= 40k ops/sec`, `mean <= 26 us` |
| Stage 16 | `postgres concurrent mixed workload` | `>= 9k ops/sec`, `mean <= 120 us` |
| Stage 17 | `wrk benchmark_api SQL read-heavy` | `>= 24k req/sec`, `p50 <= 11 ms`, `p95 <= 19 ms`, `p99 <= 26 ms`, `errors = 0` |
| Stage 18 | `wrk benchmark_api SQL mixed CRUD` | `>= 14.5k req/sec`, `p50 <= 19 ms`, `p95 <= 32 ms`, `p99 <= 45 ms`, `errors = 0` |

**Как интерпретировать текущие SQL benchmark bottlenecks**
- `touch_user` против `touch_user tx32` показывает, что durable write path сейчас в первую очередь упирается в per-statement commit/WAL flush, а не в mapper/codegen overhead.
- HTTP `read-heavy` и `mixed CRUD` latency отражают не только SQL time, но и queueing на worker threads, потому что demo path выполняет sync `libpq` вызовы прямо в handler thread.
- На текущей fixture заметен и host/container tax: PostgreSQL гоняется через Docker port publishing, и `perf` показывает заметную долю cycles в `docker-proxy`; для reference numbers лучше native Postgres или host-network profile.
- Для list path отдельный контроль нужен именно на query shape и table growth; поэтому mixed CRUD script теперь обязан балансировать `POST` созданием и `DELETE` удалением, а не просто раздувать таблицу.
- Переход с sync `libpq` на async path требует не только нового SQL executor, но и async completion в HTTP runtime; текущий рабочий migration path зафиксирован в `docs/profiling/sql_async_migration.md`.

**Зависимости**
- Stage 3 должен закрыть contract/runtime harness и нормальный test path.

**DoD**
- generated repository API стабилен и покрыт integration tests;
- на демо CRUD-сервисе нет N+1 в canonical сценариях;
- p99 для базовых CRUD flows укладывается в утверждённый benchmark budget;
- SQL слой не вносит скрытые heap-heavy пути в hot path без явного решения.

---

### Этап 5 — Redis + Contract Policies

**Фокус**: перенести cache/idempotency/rate-limit из ad-hoc middleware в явный контракт runtime/codegen.

**Что входит**
- Redis client/runtime с pipelining и per-reactor pools;
- контракт `x-katana-cache`, `x-katana-idempotency`, `x-katana-rate-limit`;
- generated wrappers для cache lookup/store/invalidation;
- single-flight и stale-while-revalidate для read-heavy use cases;
- E2E tests на cache hit/miss, idempotency replay и 429 semantics.

**Что не входит**
- произвольная distributed workflow orchestration;
- advanced stream processing и pub/sub beyond minimal invalidation needs.

**DoD**
- policy-аннотации влияют на generated/runtime behaviour предсказуемо и тестируемо;
- cache/idempotency/rate-limit работают без ручного glue-кода в пользовательском сервисе;
- latency и error semantics зафиксированы benchmark и conformance тестами.

---

### Этап 6 — Observability + Production Readiness

**Фокус**: сделать runtime операбельным в реальной эксплуатации, а не только быстрым локально.

**Что входит**
- structured logging;
- Prometheus metrics для HTTP/reactor/arena/storage/cache;
- OpenTelemetry traces для request/storage/cache цепочек;
- production presets, health checks, graceful shutdown, базовые deploy recipes;
- runbooks и dashboards для p95/p99, saturation и pool exhaustion.

**Зависимости**
- storage и policy-слой должны быть уже стабилизированы, иначе метрики и traces будут зацементированы слишком рано.

**DoD**
- сервис можно поднять с метриками, логами и trace export без ручной переклейки framework internals;
- есть production checklist и baseline dashboards;
- soak/perf smoke прогоны входят в release path, а не выполняются вручную эпизодически.

---

### Этап 7 — Developer Workflow + Long-term Expansion

**Фокус**: ускорять DX и расширять platform surface только после стабилизации core milestones.

**Краткосрочно**
- `katana dev`, локальные mocks, ускорение incremental builds;
- CI/perf budgets, cross-platform sanity, conformance packaging.

**План DX-исправлений по итогам CaseCore**

CaseCore стал первым fullstack-проектом, который прогнал framework не только через unit/integration path, но и через реальный browser client, submodule-based standalone repo и contract-first backend flow. Это вскрыло не abstract backlog, а конкретные DX-проблемы.

**P0 — исправить то, что ломает standalone consumer flow**
- dependency-safe `CMake`: убрать допущения root-project режима, перестать опираться на `CMAKE_SOURCE_DIR` в consumer path, гарантировать корректный `add_subdirectory()`/install/export mode;
- выровнять propagated compile definitions/runtime backend selection, чтобы consumer target не должен был вручную чинить `KATANA_USE_EPOLL`/`KATANA_USE_IO_URING`;
- дать официальный browser-safe CORS middleware и preflight handling, чтобы separate frontend работал без ручного glue-кода;
- задокументировать canonical standalone layout: как отдельный сервис на `katana` должен подключать `katana_core` и `katana_gen`, где держать generated artifacts, как запускать dev/fullstack loop.

**P1 — улучшить ergonomics generated surface**
- нормализовать namespace policy generated code: DTO, aliases, handler types и helpers должны жить в предсказуемой публичной форме без смешения global namespace и `generated::`;
- сделать generated output более library-shaped: чище naming, меньше leakage внутренних helper-типов, стабильнее consumer-facing API;
- улучшить ошибки `katana_gen sql`: точнее показывать, какой файл/запрос/column mapping невалиден и какой формат ожидается генератором;
- формализовать две supported стратегии generated artifacts:
  1. ephemeral build output
  2. checked-in generated files inside repo
  и явно задокументировать recommended mode для каждой.

**P2 — закрыть fullstack/product DX gap**
- добавить generated TypeScript client из OpenAPI, чтобы frontend не писал fetch-layer руками поверх того же контракта;
- оформить product-ready middleware story: auth, rate limit, idempotency, cache policy и route metadata должны подключаться как готовая composition layer, а не собираться вручную вокруг generated router;
- дать официальный standalone reference service/template с backend + frontend + OpenAPI + SQL catalog + run scripts;
- добавить dev orchestration уровня `katana dev` для поднятия backend/frontend/deps одной командой и сокращения iteration loop.

**Ожидаемый эффект**
- Stage 7 должен сократить разрыв между “framework primitives существуют” и “на framework удобно собирать отдельный продукт”;
- цель не просто генерировать код, а довести consumer experience до состояния, где разработчик пишет доменную логику, а не чинит интеграционный слой вокруг framework;
- DoD для DX-части Stage 7: standalone fullstack reference service поднимается без ручных патчей framework internals, browser client проходит auth/CORS/error-flow, а generated contract потребляется без знания внутренних деталей codegen/runtime.

**Долгосрочно, вне ближайшего горизонта**
- HTTP/2, HTTP/3, deeper io_uring track, NUMA-aware placement;
- SDK generation, admin UI, migration tooling;
- продовый reference service и tuning guides под реальную нагрузку.

**Правило приоритезации**
- ни один пункт из long-term backlog не должен стартовать раньше, чем Stages 3-6 дадут стабильный contract/runtime/ops baseline.

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
      x-katana-cache: "10s"
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
