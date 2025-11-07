# KATANA Architecture

Детальное описание архитектуры, принципов проектирования и технических решений фреймворка.

---

## Цели и принципы

### Производительность по умолчанию

Reactor-per-core архитектура без глобальных очередей и локов, минимальные аллокации, zero-copy где возможно.

### Безопасность кода

Строгий RAII, запрет `new`/`delete`, проверки статическим анализом (clang-tidy), sanitizers в CI (ASan/UBSan/TSan).

### Детерминированная структура проекта

API-first подход: OpenAPI и SQL как единственные источники истины, автоматическая кодогенерация.

### Наблюдаемость

Метрики p50/p95/p99, распределенный трейсинг, структурированные логи с корреляцией trace_id/span_id.

### Расширяемость

Слоистая архитектура, плагины, чистые контракты между компонентами, слабые зависимости.

---

## Дерево репозитория

```
KATANA/
├── cmake/                  # toolchains, presets
├── third_party/            # pinned dependencies (if needed)
├── tools/
│   └── codegen/           # generators: OpenAPI→routes, SQL→models/repos
├── katana/
│   ├── core/              # runtime core (event loop, scheduler, allocators, time)
│   ├── net/               # TCP/UDP, TLS, DNS (Asio), connectors/acceptors
│   ├── http/              # HTTP/1.1 + HTTP/2 (Boost.Beast), routing, middleware
│   ├── rpc/               # gRPC/Cap'n Proto (optional)
│   ├── sql/               # DAL: drivers, pool, migrations, repos (generated)
│   ├── cache/             # Redis (redis-plus-plus), in-process cache
│   ├── config/            # config loading/validation (TOML/YAML), hot-reload
│   ├── logging/           # spdlog/fmt + trace_id correlation
│   ├── tracing/           # OpenTelemetry OTLP exporter
│   ├── metrics/           # Prometheus pull/OTLP push, system & business metrics
│   ├── security/          # TLS (OpenSSL/BoringSSL), JWT (jwt-cpp), RBAC/ABAC hooks
│   └── util/              # gsl, expected, outcome, small_vec, ring_buffer
├── examples/
│   ├── mid_service/       # "mid layer" service from OpenAPI/SQL
│   └── low_service/       # "low layer" with manual control
├── test/
│   ├── unit/
│   ├── integration/
│   └── fuzz/
├── benchmarks/
├── .clang-format
├── .clang-tidy
└── CMakeLists.txt
```

---

## Внешние зависимости (политика)

### Асинхронность и сеть

**standalone Asio** (или Boost.Asio), **Boost.Beast** (HTTP/WebSocket)

### Форматирование и логирование

**fmt**, **spdlog**

### Сериализация

**nlohmann::json** (или Boost.JSON для скорости), **yaml-cpp**, **toml++**

### Метапомощники

**gsl-lite**/**MS GSL**, **tl::expected** (до std::expected), **nonstd::span** (если нет std::span)

### SQL

**PostgreSQL** (libpq), **SQLite3**; миграции — встроенные или schemacpp

### Кэш

**redis-plus-plus**

### Безопасность

**OpenSSL**/**BoringSSL**, **jwt-cpp**

### Наблюдаемость

**OpenTelemetry C++ SDK**, **Prometheus-cpp**

### Тестирование

**GoogleTest**, **RapidCheck** (property-based), **libFuzzer**/**LLVM** (fuzzing)

### Качество кода

**clang-tidy**, **include-what-you-use**, **sanitizers** (ASan/UBSan/TSan), **gcov**/**llvm-cov**

---

## Слои (логическая архитектура)

### 1. katana/core (Runtime)

**Reactor-per-core**: по CPU core — свой event loop (Asio `io_context` + work guard).

**Scheduler**: планировщик задач, привязка к ядрам, work stealing между локальными пулами.

**Coroutines**: C++20 `co_await` адаптеры поверх Asio (таймауты, отмена, cancellation token).

**Allocators**: аренные/пулы (pmr), small-buffer оптимизации, ring/circular buffers.

**Timers/Clock**: монотонные таймеры, wheel-timers для массовых таймеров.

**Safety**: только RAII-ресурсы, `gsl::not_null`, запрет сырого `new`/`delete`.

### 2. katana/net

**Connect/Accept**: TCP/TLS, UDP/QUIC (опционально), DNS-resolve.

**Backpressure**: ограничение in-flight запросов, ручки flow-control.

**Zero-copy**: буфера как `asio::const_buffer`/`mutable_buffer`, реюз через пулы.

### 3. katana/http

**Server**: на Boost.Beast, HTTP/1.1; HTTP/2 — опционально (nghttp2/llhttp).

**Router**: таблица маршрутов (сгенерирована из OpenAPI), статическая диспетчеризация по методу+пути.

**Middleware**: логирование, аутентификация, rate-limit, CORS, gzip/brotli, tracing.

**Error Mapping**: exceptions → Problem Details (RFC 7807), `expected<T, E>` → 4xx/5xx.

### 4. katana/sql

**Drivers**: PostgreSQL/SQLite, неблокирующие операции через worker-pool (per-core connection pools).

**Codegen**: `sql/*.sql` → Repo/DTO/RowMapper (compile-time/constexpr генерация где возможно).

**Transactions**: scoped транзакции (RAII), retry-policy, idempotency keys.

**Migrations**: встроенный runner, версионирование, checksum.

### 5. katana/cache

**Redis**: per-core connection pools, TTL-кэш, write-behind/write-through опционально.

**Local Cache**: lock-free LRU/LFU (folly-подобный), шардирование по core.

### 6. katana/config

**Sources**: файл, env, CLI; merge-стратегия, schema-валидация.

**Hot reload**: сигнал + валидированная пересборка runtime-конфигов.

### 7. katana/tracing + metrics + logging

**Tracing**: OpenTelemetry (spans вокруг handler'ов), propagate trace_id via headers.

**Metrics**: Prometheus endpoint `/metrics`; системные (CPU, RSS), сетевые (accepts, RPS), latency (p50/p95/p99), ошибки по классам.

**Logging**: структурный, корреляция по trace_id/span_id, уровни/сэмплинг.

### 8. katana/security

**TLS**: контекст, ключи, ciphers, OCSP stapling (при необходимости).

**AuthN/AuthZ**: JWT verify, роли/правила (RBAC/ABAC), политики как middleware.

---

## Кодогенерация (OpenAPI/SQL → код)

### OpenAPI

**Генерируем**: роуты, request DTO, response DTO, валидацию, сериализацию.

**Аннотации-расширения**: `x-katana-cache`, `x-katana-timeout`, `x-katana-rate-limit`.

### SQL

**Генерируем**: модели, маппинг строк, репозитории, типобезопасные параметры.

### Команда

```bash
katana codegen api/openapi.yaml sql/*.sql -o gen/
```

### Границы

Весь генерируемый код уходит в `gen/`, **не редактируется вручную**; ручной код — в `src/`.

---

## Пайплайн обработки запроса (типовой)

1. **Accept соединения** → HTTP parse (Beast) → Router dispatch
2. **Middleware chain**: tracing → auth → rate-limit → etc.
3. **Handler** (`co_await`): валидация → бизнес-логика → SQL/Cache вызовы → доменные события
4. **Сериализация ответа**, запись в сокет
5. **Метрики**: инкременты, latency buckets; **Трейсинг**: spans закрываются

---

## Стандарты кода и «без unsafe»

### RAII only

Ресурсы оборачиваются в типы с детерминированным временем жизни.

### Запрет сырого `new`/`delete`

Проверка через clang-tidy check + grep в CI.

### Нулевые указатели

`gsl::not_null`, `std::optional`/`tl::optional` вместо сырых nullable-указателей.

### Границы

`std::span` для буферов/вьюшек, явные `string_view`.

### Касты

`narrow`/`narrow_cast` (GSL) вместо C-style cast.

### Конкурентность

Никакого «общего» mutable-состояния, шардирование per-core, lock-free структуры — только обоснованно.

### Исключения

В бизнес-слое — `expected`/`Outcome`; в инфраструктуре — допускаются, но обязателен mapping + `noexcept`-гарантии на границах.

### Сборка

```
-Wall -Wextra -Werror -Wconversion -Wshadow -Wpedantic
```

LTO (Link-Time Optimization) в релизе.

### ABI/visibility

Скрывать символы по умолчанию (`visibility=hidden`), экспорт — явно.

---

## Память и аллокации

### Arena-per-request

Всё, относящееся к запросу, выделяется из арены и освобождается одной операцией.

### Monotonic allocator

Фиксированные блоки, reset после завершения запроса.

### std::pmr

Использование `std::pmr::memory_resource` для кастомизации аллокатора.

### Режимы

- `arena` (по умолчанию)
- `std::pmr` с настраиваемой `memory_resource`
- стандартный `new`/`delete` (флаг `--no-arena` в dev)

---

## Конкурентность и изоляция

### Reactor-per-core

Каждое ядро CPU получает независимый event loop без shared state.

### Нет глобальных локов

Никаких `std::mutex` в hot-path, состояние изолировано.

### Per-core пулы

Соединения к БД, Redis, кэш — всё per-core.

### Нет handoff между потоками

Запрос обрабатывается от начала до конца на одном ядре.

---

## Тестирование

### Unit-тесты

GoogleTest для компонентов, моки для внешних зависимостей.

### Integration-тесты

Testcontainers (PostgreSQL, Redis), реальные зависимости.

### Property-based тесты

RapidCheck для валидаторов, сериализаторов.

### Fuzzing

libFuzzer для HTTP-парсера, входных данных.

### E2E тесты

Автогенерация из OpenAPI, проверка контрактов.

### Performance-budget

Регрессия p99 > 10% → fail сборки.

---

## Observability

### Метрики (Prometheus)

- HTTP: `http_requests_total`, `http_request_duration_seconds`
- SQL: `db_query_duration_seconds`, `db_connections_active`
- Redis: `redis_commands_total`, `redis_command_duration_seconds`
- System: CPU per-core, memory, arena usage
- Backpressure: `reactor_queue_length`, `reactor_processing_delay`

### Трейсинг (OpenTelemetry)

- Span на каждый HTTP-запрос
- Span на SQL-запросы (с query text)
- Span на Redis-операции
- W3C Trace Context propagation

### Логирование (structured JSON)

- Формат: timestamp, level, message, trace_id, span_id
- Контекстные поля: request_id, user_id, endpoint
- Уровни: DEBUG, INFO, WARN, ERROR

---

## Безопасность

### TLS

BoringSSL/OpenSSL, kTLS offload (Linux), OCSP stapling.

### JWT

jwt-cpp для валидации токенов.

### RBAC/ABAC

Политики как middleware, декларативное описание прав.

### Защита от инъекций

Только prepared statements, запрет строковых конкатенаций SQL.

### Лимиты

Размер заголовков/body, таймауты, rate limiting.

---

## Dev Experience

### Hot-reload

Пересборка только изменённых контроллеров, динамическая загрузка `.so`.

### Быстрая сборка

clang + lld, ccache, precompiled headers.

### Автоподнятие зависимостей

Docker Compose с PostgreSQL, Redis, Prometheus, Grafana, Jaeger.

### Моки

`--mock-db`, `--mock-cache` для разработки без внешних зависимостей.

### Debug режим

`--no-arena`, `--no-pin`, AddressSanitizer.

---

## Production

### Профили

`dev`, `staging`, `prod`, `prod-high-throughput`, `prod-low-latency`

### Настройки

Число реакторов, размеры пулов, TTL кэша, TCP параметры.

### Deployment

Dockerfile (multi-stage), Kubernetes manifests, health checks.

### Мониторинг

Алерты на p99 degradation, error rate spike, connection pool exhaustion.

---

## Ссылки

- [README.md](README.md) — обзор фреймворка, быстрый старт
- `/docs/RFCs` — спецификации Core/Codegen/Lint
- `/docs/Conformance` — тесты на соответствие стандартам
