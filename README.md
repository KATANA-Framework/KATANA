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

**Этап 1 — Базовый runtime**
Реактор-per-core, pinning, epoll + vectored I/O, HTTP/1.1, arena-per-request, RFC7807, системные лимиты.
Цель: устойчивый server loop без гонок, ASan/LSan-clean; `hello-world p99 < 1.5–2.0 ms` на одном сокете.

**Этап 2 — OpenAPI → Compile-time API**
Генератор роутов/DTO/валидаторов/сериализации, compile-time таблицы.
Цель: любые расхождения API → ошибки компиляции; property-тесты валидаторов.

**Этап 3 — SQL-генерация**
Модели/репозитории/транзакции/UPSERT/bulk; libpq binary; пулы per-core.
Цель: отсутствует N+1 в демо-сервисе; стабильный p99 CRUD.

**Этап 4 — Redis/кэш**
Аннотации OpenAPI/SQL → idempotency/rate-limit/TTL/single-flight/инвалидация.
Цель: кэш и лимиты — часть контракта, не middleware.

**Этап 5 — Observability**
OTel/Prom/JSON-логи; готовые Grafana-дашборды.
Цель: видимость проблем из коробки: RPS, p95/p99/p999, очереди, backpressure, арены.

**Этап 6 — Dev-режим**
`katana dev`: hot-reload, clang+ccache, локальный PG/Redis/OTel/Prom, моки.
Цель: «code → reload → запрос» < 2 с; DX уровня Node/FastAPI.

**Этап 7 — Протоколы/производственные профили**
HTTP/2, HTTP/3/QUIC, sendfile/kTLS, io_uring; NUMA-aware раскладка.
Цель: линейный throughput, предсказуемые хвосты.

**Этап 8 — Тесты и CI**
E2E, property-тесты, фуззинг HTTP, нагрузочные профили, flamegraph, performance-budget.
Цель: производительность — гарантируемое свойство сборки.

**Этап 9 — Кодоген расширений**
SDK (TS/Go/Rust/Python), административные интерфейсы из схем, миграции и проверка совместимости.
Цель: самодостаточный контракт, минимум ручной поддержки клиентов.

**Этап 10 — Стабилизация и прод-кейс**
Целевой прод-проект, профили, фиксы узких мест, стандартные конфигурации, рекомендации деплоя.
Цель: подтверждение применимости в реальном проде.

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

**Быстрый старт (Mid layer)**

# Шаг 1 — создать проект
katana new mysvc --layer mid
cd mysvc

# Структура проекта (ключевые директории)
# api/   — OpenAPI спецификация (источник истины для API)
# sql/   — SQL схемы и запросы (источник истины для моделей/репозиториев)
# src/   — ваша бизнес-логика
# gen/   — автогенерируемый код (не редактируется вручную)
# include/ — заголовки для пользовательского кода

# Шаг 2 — описать API в OpenAPI
# (например api/openapi.yaml)
# paths:
#   /users/{id}:
#     get:
#       x-katana-cache: { ttl: 10s }
#       responses:
#         200: { $ref: "#/components/schemas/User" }

# Шаг 3 — описать SQL в sql/*.sql
# (например sql/get_user.sql)
# -- name: get_user :one
# SELECT id, name FROM users WHERE id = $1;

# Шаг 4 — сгенерировать типобезопасный API и репозитории
katana gen openapi -i api/openapi.yaml -o gen/ --strict
katana gen sql -i sql/ -o gen/

# Шаг 5 — реализовать бизнес-логику
# пример: src/users_controller.cpp

#include <gen/users_api.hpp>

class UsersController : public gen::UsersApi {
public:
  katana::result<UserDto> get_user(int64_t id, katana::ctx& ctx) override {
    auto user = repo_.get_user(ctx, id); // типобезопасный репозиторий из SQL
    if (!user)
      return ctx.problem.not_found("user.not_found").detail("id", id);
    return *user;
  }
private:
  gen::UsersRepo repo_; // автогенерированный репозиторий
};

# Шаг 6 — запустить dev-режим
katana dev --hot

# Автоматически поднимется:
# - локальный PostgreSQL
# - локальный Redis
# - Prometheus + Grafana + OpenTelemetry коллектор
# - hot-reload контроллеров без перезапуска реакторов

# Шаг 7 — открыть сервис
curl http://localhost:8080/users/1
Готово — сервис работает:

API строго соответствует OpenAPI (любое расхождение → ошибка компиляции).

SQL строго соответствует моделям (несоответствие → ошибка генерации).

Обработка запроса выполняется внутри одного реактора (нет межпоточных гонок).

Память выделяется в арене запроса и освобождается одной операцией.

Метрики и трейсы подключены из коробки.
