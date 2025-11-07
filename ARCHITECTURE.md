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

### Этапы 1-3: Zero-Dependency Foundation

Этапы 1-3 реализуют подход **без внешних зависимостей**, используя только C++23 stdlib и прямые Linux syscalls. Преимущества:

- Полный контроль над производительностью
- Упрощенная сборка и развертывание
- Уменьшенная attack surface
- Быстрая компиляция

**Основная реализация (этапы 1-3):**
- Собственный epoll-based reactor (без Asio)
- Собственный HTTP/1.1 парсер (без Boost.Beast)
- std::pmr для управления памятью
- std::expected для обработки ошибок
- Прямые syscalls для I/O

### Этап 4+: Выборочные зависимости

Внешние зависимости будут добавляться выборочно на поздних этапах:

**Асинхронность и сеть** (этап 7+)

Опционально: **standalone Asio** / **Boost.Asio**, **Boost.Beast** для HTTP/2, WebSocket

**Форматирование и логирование** (этап 5+)

**fmt**, **spdlog**

**Сериализация** (этап 4+)

**nlohmann::json**, **yaml-cpp**, **toml++**

**SQL** (этап 4+)

**PostgreSQL** (libpq), **SQLite3**

**Кэш** (этап 4+)

**redis-plus-plus**

**Безопасность** (этап 7+)

**OpenSSL**/**BoringSSL**, **jwt-cpp**

**Наблюдаемость** (этап 5-6)

**OpenTelemetry C++ SDK**, **Prometheus-cpp**

**Тестирование**

**GoogleTest**, **libFuzzer**/**LLVM** (fuzzing)

**Качество кода**

**clang-tidy**, **sanitizers** (ASan/UBSan/TSan/LSan)

---

## Слои (логическая архитектура)

### 1. katana/core (Runtime)

**Reactor-per-core**: по CPU core — свой event loop (собственный epoll-based reactor с thread pinning).

**Scheduler**: планировщик задач, привязка к ядрам, lock-free MPSC очереди.

**Event Loop**: Edge-triggered epoll, vectored I/O (readv/writev), поддержка EPOLLONESHOT.

**Allocators**: arena-per-request (std::pmr::monotonic_buffer_resource), zero-copy где возможно.

**Timers/Clock**: монотонные таймеры, wheel-timers (512 слотов, гранулярность 100ms).

**Safety**: строгий RAII, std::expected для ошибок, запрет сырого `new`/`delete`.

**Примечание**: Этапы 1-3 используют собственную реализацию. Будущие этапы могут опционально интегрировать Asio/корутины.

### 2. katana/net

**Connect/Accept**: TCP/TLS, UDP/QUIC (опционально), DNS-resolve.

**Backpressure**: ограничение in-flight запросов, ручки flow-control.

**Zero-copy**: буфера как `asio::const_buffer`/`mutable_buffer`, реюз через пулы.

### 3. katana/http

**Server**: Собственный HTTP/1.1 парсер с chunked encoding, keep-alive, лимитами безопасности.

**Parser**: Zero-copy парсинг, строгое соответствие RFC 7230, настраиваемые лимиты размеров.

**Serializer**: Поддержка Content-Length и chunked transfer encoding.

**Error Mapping**: std::expected → Problem Details (RFC 7807).

**Router**: (этап 2+) таблица маршрутов из OpenAPI, статическая диспетчеризация.

**Middleware**: (этап 3+) логирование, аутентификация, rate-limit, CORS, трейсинг.

**HTTP/2**: (этап 7+) через nghttp2 или собственную реализацию.

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

### Этап 1 (текущий)

1. **Accept соединения** → Edge-triggered epoll → регистрация FD с таймаутами
2. **HTTP parse**: Собственный инкрементальный парсер → объект Request с arena allocation
3. **Handler**: Синхронный обработчик → генерация Response
4. **Сериализация ответа**: HTTP/1.1 сериализатор → Vectored I/O запись
5. **Keep-alive**: Переиспользование соединения или graceful close
6. **Метрики**: Атомарные счетчики (tasks, events, timers, exceptions)

### Этап 2+ (запланировано)

1. **Accept соединения** → HTTP parse → **Router dispatch** (из OpenAPI)
2. **Middleware chain**: трейсинг → аутентификация → rate-limit → и т.д.
3. **Handler** (опционально `co_await`): валидация → бизнес-логика → SQL/Cache вызовы
4. **Сериализация ответа**, запись в сокет
5. **Метрики**: p50/p95/p99 гистограммы; **Трейсинг**: OpenTelemetry spans

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
