# KATANA Framework

KATANA — серверный фреймворк на C++ для разработки высоконагруженных API и систем с жёсткими требованиями к хвостовым задержкам (p95/p99/p999), контролируемым использованием ресурсов и предсказуемым поведением под нагрузкой.

Цель — устранить разрыв между:

1. быстрым DX (Python/Node) с нестабильной производительностью,
2. «сырыми» высокопроизводительными системами (C++/Rust/Go) со сложной эксплуатацией.

Фреймворк фиксирует **модель исполнения**, **модель памяти** и **контракт API**, чтобы обеспечить стабильные задержки без усложнения доменной логики.

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

* Нет глобального состояния и межпоточных локов.
* Нет очередей на общий пул БД.
* Нет handoff между потоками во время обработки запроса.
* p99/p999 стабильны и контролируемы.

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

Pinning:

* Linux: `sched_setaffinity`, NUMA,
* Windows: `SetThreadAffinityMask / GROUP_AFFINITY`,
* macOS: мягкие подсказки планировщику.

Dev-флаг `--no-pin` отключает pinning. CI собирает Linux/macOS/Windows; производительные тесты — только Linux.

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

### Этап 1 — Базовый runtime

**Цель**: устойчивый server loop без гонок, ASan/LSan-clean; `hello-world p99 < 1.5–2.0 ms` на одном сокете.

- [ ] Реализовать базовый event loop на epoll
  - [ ] Абстракция `Reactor` с методами `run()`, `stop()`, `schedule()`
  - [ ] Регистрация file descriptors (EPOLLIN/EPOLLOUT/EPOLLET)
  - [ ] Обработка событий с таймаутами
- [ ] Reactor-per-core архитектура
  - [ ] Создание N реакторов по числу CPU cores
  - [ ] Thread pinning через `sched_setaffinity`
  - [ ] Изоляция состояния между реакторами
- [ ] Vectored I/O
  - [ ] Обёртки над `readv`/`writev`
  - [ ] Управление scatter-gather буферами
- [ ] Arena allocator (per-request)
  - [ ] Монотонный аллокатор с фиксированными блоками
  - [ ] Интеграция с `std::pmr::memory_resource`
  - [ ] Reset арены после завершения запроса
- [ ] HTTP/1.1 parser
  - [ ] Парсинг request line (method, URI, version)
  - [ ] Парсинг заголовков (складывание multiline)
  - [ ] Chunked transfer encoding
  - [ ] Keep-alive и connection pooling
- [ ] HTTP/1.1 serializer
  - [ ] Формирование response (status line, headers, body)
  - [ ] Поддержка `Content-Length` и `Transfer-Encoding: chunked`
- [ ] RFC 7807 (Problem Details)
  - [ ] Структура `Problem` (type, title, status, detail, instance)
  - [ ] Хелперы для типовых ошибок (400, 404, 500, 503)
- [ ] Системные лимиты
  - [ ] `rlimit` для файловых дескрипторов
  - [ ] Лимиты на размер заголовков/body
  - [ ] Graceful shutdown с дедлайном
- [ ] Базовые тесты
  - [ ] Unit-тесты парсера HTTP
  - [ ] ASan/LSan прогоны
  - [ ] Hello-world benchmark (wrk/h2load)

---

### Этап 2 — OpenAPI → Compile-time API

**Цель**: любые расхождения API → ошибки компиляции; property-тесты валидаторов.

- [ ] Парсер OpenAPI 3.x
  - [ ] Загрузка YAML/JSON спецификации
  - [ ] Валидация соответствия стандарту OpenAPI
  - [ ] Построение AST (paths, schemas, parameters)
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
- [ ] CLI команда `katana gen openapi`
  - [ ] Опции: `-i`, `-o`, `--layer`, `--strict`
  - [ ] Вывод статистики генерации
- [ ] Тесты
  - [ ] Property-тесты валидаторов (fuzzing входов)
  - [ ] Round-trip ser/deser тесты
  - [ ] Compile-time проверка на несуществующий endpoint

---

### Этап 3 — SQL-генерация

**Цель**: отсутствует N+1 в демо-сервисе; стабильный p99 CRUD.

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

