# KATANA Performance Memory

## Current Baseline
- Stage 2 по framework/codegen уже в основном сделан; текущий bottleneck сидит не в generated dispatch, а в shared `http_server` / socket / reactor path.
- Исторические данные пользователя по Ubuntu VM: `compute_api` и `hello` шли слишком близко друг к другу, а `perf stat` показывал примерно `~13.1-13.55 us CPU/request`, что упирает 6 vCPU VM в потолок заметно ниже целевых `600K+`.

## What Is Confirmed Now
- Текущее дерево уже не соответствует полностью откатанному baseline из описания пользователя: в `katana/core/include/katana/core/http.hpp` и `katana/core/src/http_server.cpp` уже есть direct parser-buffer API:
  - `parse_available()`
  - `writable_input_span()`
  - `commit_input()`
  - `prepare_for_next_request()`
- Parser больше не делает eager `~10 MB` allocation на каждый connection reset. Сейчас у него lazy/resizable buffer на `unique_ptr<char[]>` с `INITIAL_BUFFER_CAPACITY = 16 KB` и shrink policy через `RETAIN_BUFFER_CAPACITY = 64 KB`.
- URI и header values в текущем дереве уже живут как borrowed views из parser buffer, но их безопасность при `compact_buffer()` / `reserve_buffer()` нужно перепроверить; считать этот вопрос закрытым пока нельзя.
- В `katana/core/include/katana/core/http_server.hpp` у `connection_state` всё ещё оставался лишний per-connection state из старой модели, в частности неиспользуемый `read_buffer`, а write side всё ещё крутился через отдельный `io_buffer` и два почти одинаковых flush loop'а.

## Benchmarks Seen In This Tree
- Framework benchmark path на VM уже был около `~1.00-1.02M ops/sec`.
- Generated API dispatch benchmark был около `~1.48M ops/sec`.
- Последние VM network прогоны до текущей write-side правки:
  - `hello`:
    - `c128`: `~128K req/sec`
    - `c512`: `~179K req/sec`
  - `compute_api`:
    - `c128`: `~124K req/sec`
    - `c512`: `~157K req/sec`
- Вывод из этих прогонов: дополнительное снижение parser metadata copies само по себе не дало network breakthrough; shared orchestration/write path остаётся главным подозреваемым.

## Known Problems
- `katana/core/src/http_server.cpp` держал duplicated pending-write logic:
  - один loop в начале `handle_connection()` для already pending response
  - второй почти такой же loop сразу после serialization нового response
- Parse error path сериализовал `400 Bad Request`, но затем сразу ронял watch, то есть ответ фактически мог не уйти в сокет до закрытия.
- В `connection_state` сохранялся stale `read_buffer`, хотя текущий read path уже читает прямо в parser-owned buffer.

## Current Hypothesis
- Следующий low-risk шаг должен упростить именно shared response/write orchestration:
  - убрать лишний `io_buffer` bookkeeping из common write path
  - держать pending response как один contiguous `std::string` + `write_pos`
  - свести flush logic к одному helper path
  - убрать stale per-connection read buffer
- Если это даст measurable win на VM, значит остаточная цена сидела не только в inbound parser path, но и в общем response/orchestration слое.
- Если прироста снова почти не будет, дальше надо идти глубже в syscall density / epoll lifecycle / reactor bookkeeping и уже мерить это отдельными in-process counters.

## Recent Focus
- `http_server` переведён на contiguous pending-response flow без тяжёлого redesign всего сервера.
- После этой правки были повторно прогнаны локальная верификация и VM network benchmark для `hello` / `compute_api`.

## Latest Changes
- `katana/core/include/katana/core/http_server.hpp`
  - удалён stale `read_buffer` из `connection_state`
  - `write_buffer` заменён на `std::string active_response + size_t write_pos`
  - убран лишний `waiting_for_write`; `watch_events` остаётся единственным cache режима watch
- `katana/core/src/http_server.cpp`
  - write/common-case path сведён к одному `flush_active_response(...)`
  - parse error path теперь действительно пытается отправить `400 Bad Request` перед close, а не просто сериализует в буфер и дропает watch
  - удалён мёртвый `accept_connection(...)`, который уже не использовался текущим `run()`
  - исправлена классификация `read_eof` vs `read_error`
- `katana/core/src/http.cpp`
  - исправлен unsigned-underflow в capacity checks `writable_input_span()` / `commit_input()`
  - этот баг приводил к heap overwrite на `HttpParser.ExcessivelyLongHeader`
- `examples/hello_world_server.cpp`
  - пример полностью переписан на `katana::http::server`
  - сохранены маршруты `GET /` и `GET /hello/{name}`
  - добавлена стандартная конфигурация через `HELLO_PORT` / `PORT` и `KATANA_WORKERS`

## Latest Verification
- Локально:
  - полная сборка проходит
  - `integration_tests` проходят полностью
  - `unit_tests` больше не падает на `HttpParser.ExcessivelyLongHeader`
  - отдельный example build с `ENABLE_EXAMPLES=ON` собирает `hello_world_server`
  - smoke-check нового `hello_world_server` проходит: `GET / -> Hello, World!`, `GET /hello/test -> Hello test!`
- Оставшиеся локальные unit failures сейчас не из этой пачки:
  - `ReactorTest.RegisterFdWithTimeout`
  - `ReactorTest.MetricsTracking`
  - `ReactorPoolTest.MetricsAggregation`
  - несколько `VectoredIO` тестов в `test_io_buffer.cpp`

## Latest VM Numbers
- `compute_api` на Ubuntu VM после write-path правки:
  - `c128`: `150411 req/sec`, p50 `426 us`, p95 `3125 us`, p99 `5241 us`
  - `c512`: `178194 req/sec`, p50 `1467 us`, p95 `6736 us`, p99 `10639 us`
- Относительно предыдущего зафиксированного baseline для этого дерева:
  - `c128`: рост примерно с `~124K` до `~150K` (`+21%`)
  - `c512`: рост примерно с `~157K` до `~178K` (`+13%`)
- `hello_world_server` после переписывания на shared `http::server` path:
  - `c128`: `182066 req/sec`, p50 `334 us`, p99 `5760 us`
  - `c512`: `197811 req/sec`, p50 `1350 us`, p99 `14570 us`
- Старые числа для `hello_world_server` на custom server больше не являются актуальным benchmark baseline для shared path.

## Latest Full Suite On VM
- Полный staged run `scripts/run_benchmarks.py --stage 1 2 3 4 5 6 7 8 9 10 --perf-stat --include-runs` прошёл успешно.
- Общая длительность: `~760.6 s`.
- Stage 5:
  - `HTTP E2E Keep-Alive (16 conn x 200 req)`: `2145 req/sec`, p50 `4661 us`, p95 `21498 us`, p99 `32734 us`
- Stage 6:
  - `Generated API dispatch+parse`: `1.93M ops/sec`
- Stage 7:
  - `benchmark_api_codegen_valid_mix`: `539K ops/sec`
- Stage 8:
  - `benchmark_api_framework_valid_mix`: `926K ops/sec`
- Stage 9 canonical external hello load:
  - `wrk hello_world GET /`: `106532 req/sec`, p50 `2175 us`, p95 `7934 us`, p99 `15359 us`
- Stage 10 canonical external compute load:
  - `wrk compute_api POST /compute/sum`: `149859 req/sec`, p50 `1050 us`, p95 `5430 us`, p99 `8130 us`
- Важная методологическая оговорка:
  - `--perf-stat` внутри runner не работал на VM, пока `kernel.perf_event_paranoid` был `4`
  - canonical stage 9/10 используют не лучший для этой 6-core VM client profile; они годятся как репер, но не как peak throughput point

## Latest Load Sweep On VM
- Отдельный tuning sweep по shared-path серверам (`workers={4,6}`, `wrk threads={4,6}`, `connections={128,256,512}`) показал:
  - `hello` best throughput: `187468 req/sec` at `workers=6, wrk_t=6, c=512`
  - `hello` best balanced point: `181638 req/sec` at `workers=4, wrk_t=4, c=256`, p50 `699 us`, p95 `4024 us`, p99 `6659 us`
  - `compute_api` best throughput: `178143 req/sec` at `workers=4, wrk_t=4, c=512`
- Следствие:
  - canonical stage 9 (`12 threads`, `256 conn`) заметно недооценивает текущий `hello` на этой VM
  - canonical stage 10 (`6 threads`, `256 conn`) тоже не является peak point, но ошибка меньше

## Latest Deep Perf
- После снижения `kernel.perf_event_paranoid` до `-1` удалось снять software-event `perf stat` и `perf record`.
- Hardware counters (`cycles`, `instructions`, `branches`, `cache-*`) на этой VM всё равно `not supported`; значит IPC / cycles-per-request с этой виртуалки честно не получить.
- Более чистый attach-style `perf stat` поверх server PID дал:
  - `hello` (`workers=6`, `t=6`, `c=512`): `~13.81 s task-clock` за `~99757 req/sec`, то есть `~13.85 us CPU/request`
  - `compute_api` (`workers=4`, `t=4`, `c=512`): `~13.23 s task-clock` за `~88669 req/sec`, то есть `~14.92 us CPU/request`
- `perf record/report` по shared-path hotspot'ам:
  - `hello`:
    - `handle_connection`: `~70.5%`
    - `flush_active_response`: `~44.8%`
    - `recv` path: `~12.9%`
    - `epoll_wait`: `~28.7%`
    - `parse_available`: `~3.0%`
    - `response::serialize_into`: `~1.4%`
    - handler/`response::ok`: low single digits
  - `compute_api`:
    - `handle_connection`: `~81.5%`
    - `flush_active_response`: `~49.2%`
    - `recv` path: `~12.5%`
    - `epoll_wait`: `~17.7%`
    - `parse_available`: `~6.5%`
    - generated dispatch: `~5.6%`
    - actual `compute_handler::compute_sum`: `~2.4%`
    - `response::serialize_into`: `~1.9%`
- Это ещё раз подтверждает:
  - business logic и generated dispatch не доминируют
  - основной потолок сидит в shared `handle_connection` / send / recv / kernel TCP path

## Latest Syscall Metrics Instrumentation
- Добавлена env-gated in-process телеметрия shared network/core path:
  - новый internal header: `katana/core/include/katana/core/detail/syscall_metrics.hpp`
  - новая реализация: `katana/core/src/syscall_metrics.cpp`
  - `katana/core/src/tcp_socket.cpp` теперь считает реальные `recv/send` syscall calls и `would_block`
  - `katana/core/src/epoll_reactor.cpp` теперь считает `epoll_wait`, ready events и `epoll_ctl_{add,mod,del}`
  - `katana/core/src/http_server.cpp` считает `completed_requests` и поднимает background reporter через `KATANA_SYSCALL_METRICS=1`
- Дизайн counters:
  - slot per thread, `alignas(64)`
  - hot path пишет только в thread-local slot
  - агрегация идёт отдельным reporter thread раз в `KATANA_SYSCALL_METRICS_INTERVAL_MS` (по умолчанию `250 ms`)
  - нет shared hot-path atomics между worker threads, так что false sharing от общих counters исключён
- Локальная верификация после встраивания counters:
  - `wsl cmake --preset debug && cmake --build --preset debug` проходит
  - `ctest --test-dir build/debug -R integration_tests --output-on-failure` проходит

## Latest VM Syscall Ratios
- На Ubuntu VM сняты отдельные tuned прогоны с `KATANA_SYSCALL_METRICS=1`:
  - `hello`: `workers=6`, `wrk t=6`, `c=512`, `187352 req/sec`
  - `compute_api`: `workers=4`, `wrk t=4`, `c=512`, `176865 req/sec`
- Aggregated steady-state counters по первым `24 x 500 ms = 12 s` interval samples (последний shutdown interval исключён):
  - `hello`:
    - `recv_per_req ≈ 1.0000`
    - `send_per_req ≈ 1.0000`
    - `socket_syscalls_per_req ≈ 2.0000`
    - `epoll_wait_per_req ≈ 0.0795`
    - `epoll_ctl_add_per_req ≈ 0.00023`
    - `epoll_ctl_mod_per_req = 0`
    - `epoll_ctl_total_per_req ≈ 0.00023`
    - `ready_events_per_wait ≈ 12.57`
    - `recv_would_block_per_req = 0`
    - `send_would_block_per_req = 0`
  - `compute_api`:
    - `recv_per_req ≈ 1.0000`
    - `send_per_req ≈ 1.0000`
    - `socket_syscalls_per_req ≈ 2.0000`
    - `epoll_wait_per_req ≈ 0.0625`
    - `epoll_ctl_add_per_req ≈ 0.00024`
    - `epoll_ctl_mod_per_req = 0`
    - `epoll_ctl_total_per_req ≈ 0.00024`
    - `ready_events_per_wait ≈ 16.00`
    - `recv_would_block_per_req = 0`
    - `send_would_block_per_req = 0`
- Практический вывод из прямых counters:
  - tuned wrk load уже работает почти в теоретическом минимуме по socket syscall density: `1 recv + 1 send` на request
  - в peak-конфиге нет measurable `epoll_ctl MOD` churn и нет `send/recv EAGAIN` churn
  - `epoll_wait` уже хорошо батчит события (`~12.6-16 ready events/wait`)
  - значит на текущем tuned profile нас не душат:
    - лишние `EPOLL_CTL_MOD`
    - дробление response на несколько `send`
    - repeated `EAGAIN` wakeups на read/write side
  - следовательно, следующий крупный прирост не придёт из микрооптимизации syscall density: bottleneck сидит глубже, в стоимости самих `send/recv/epoll_wait` + shared user-space FSM вокруг них

## Current Conclusion
- Гипотеза про остаточную цену shared response/orchestration path подтвердилась частично:
  - low-risk упрощение write side дало реальный прирост `compute_api`
  - но даже после этого VM остаётся далеко от цели `600K`, значит глубокий bottleneck всё ещё сидит ниже: в syscall density / reactor lifecycle / remaining shared path cost
- Новый `hello_world_server`, уже сидящий на shared `http::server`, даёт `~182K-198K req/sec` и остаётся в том же порядке величины, что и `compute_api`; это ещё раз подтверждает, что основной потолок сидит в общем network/core path, а не в business logic конкретного endpoint.
- После полного suite + tuning sweep картина уточнилась:
  - реальный current best point на этой VM ближе к `~187K hello` и `~178K compute`, а не к canonical `stage 9/10` числам
  - но даже best tuned numbers всё равно кратно ниже целей `600K` / `~1M`
  - даже под software-event perf стоимость остаётся в районе `~14 us CPU/request`, то есть главная задача не изменилась: резко резать shared network/core cost
- Прямые in-process counters уже сняли главный открытый вопрос про syscall density:
  - tuned path не течёт по `epoll_ctl_mod`
  - tuned path не течёт по multiple `send`
  - tuned path не течёт по `EAGAIN` churn
- Следующий разумный шаг теперь другой:
  - не тратить время на `epoll_ctl MOD` cleanup как primary lever для peak wrk profile
  - не тратить время на `writev` как primary lever, пока `send_per_req` уже `≈1.0`
  - разбирать shared CPU cost внутри `handle_connection` / parser / response object lifecycle и kernel TCP/send path
  - отдельно проверить, даст ли persistent `EPOLLET` что-то на overloaded / tail-latency profile, но peak throughput на tuned load он, судя по counters, не должен сдвинуть радикально

## 2026-03-12 Memory / Allocation Pass
- Дополнительно сняты page-fault метрики на tuned VM profile:
  - `hello`: `191078 req/sec`, `page-faults=1618`, `page-faults/request ≈ 0.0008468`, `major-faults=0`
  - `compute_api`: `171865 req/sec`, `page-faults=2111`, `page-faults/request ≈ 0.0012283`, `major-faults=0`
- Вывод по page-faults:
  - steady-state throughput не упирается в VM/page subsystem
  - major faults отсутствуют

## 2026-03-12 Memory Counters
- В syscall metrics добавлены counters по памяти/копированию:
  - arena: `arena_alloc_calls`, `arena_alloc_bytes`, `arena_new_blocks`
  - parser: `parser_reserve/grow`, `parser_buffer_copy_bytes`, `parser_compact_move_bytes`
  - response: `response_serialize_bytes`, `response_output_grow`
- Для этого инструментированы:
  - `katana/core/src/arena.cpp`
  - `katana/core/src/http.cpp`

## 2026-03-12 Key Root Cause: hello temporary response path
- На `hello` был найден конкретный лишний cost:
  - старый handler стиль делал `out = response::ok(...)`
  - это создаёт временный `response res;`
  - у такого `response` свой `headers_map(nullptr)` и свой fallback `owned_arena_{4096}`
  - первый же `assign_text()/set_known()` внутри temp response открывал новый arena block
- Прямое подтверждение counters до фикса:
  - `hello` steady-state: `arena_new_blocks_per_req ≈ 1.0`
- Фикс:
  - `examples/hello_world_server.cpp` переведён на прямой in-place path:
    - `out.assign_text("Hello, World!")`
    - `out.assign_text(std::move(body))`
- Эффект после фикса temp response bug:
  - steady-state `hello` counters: `arena_new_blocks_per_req = 0`
  - first clean no-metrics VM run: `189766 req/sec`
  - improved clean no-metrics VM run after in-place fix: `198321 req/sec`
  - latency improved примерно до:
    - `p50 ~1526 us`
    - `p95 ~6379 us`
    - `p99 ~9300 us`

## 2026-03-12 Shared response-header cost reduction
- После `hello`-specific фикса был сделан ещё один framework-level pass:
  - `response::assign_text()` больше не materialize-ит `Content-Length` в `headers`
    - serializer и так synthesizes `Content-Length` без сохранения header entry
  - `response::assign_json()` и `response::assign_error()` используют borrowed static `Content-Type`
  - `http_server.cpp` теперь ставит `Connection: close/keep-alive` через known borrowed header, а не через allocating `set_header("Connection", ...)`
- Итоговый эффект counters после этого pass:
  - `hello` steady-state:
    - `arena_alloc_calls_per_req = 0`
    - `arena_alloc_bytes_per_req = 0`
    - `socket_syscalls_per_req = 2.0`
  - `compute_api` steady-state:
    - `arena_alloc_calls_per_req = 1.0`
    - `arena_alloc_bytes_per_req ≈ 64`
    - до патча было `4 allocs/request` и `~94 B/request`
- RPS после response-header pass:
  - `compute_api` tuned no-metrics: `182652 req/sec`
  - `compute_api` tuned with metrics: `183203 req/sec`
  - `hello` tuned with metrics: `188659 req/sec`
  - `hello` tuned no-metrics rerun дал `192037 req/sec`
- Практический вывод:
  - response/header lifecycle действительно содержал measurable waste
  - но даже после его срезания мы всё ещё очень далеко от `600K compute` / `~1M hello`
  - значит этот слой был важным, но не главным оставшимся потолком

## 2026-03-12 What remains after response cleanup
- Оставшийся `compute_api` `1 alloc/request` почти наверняка не shared response path, а request-body parsing:
  - `compute_sum_request` в generated DTO = `arena_vector<double>`
  - load script шлёт `[1,2,3,4,5,6,7,8]`
  - `64 B/request` очень хорошо совпадает с резервом под `8 * sizeof(double)`
- Значит текущая рабочая гипотеза такая:
  - остаточный alloc в `compute_api` теперь сидит в generated request parse / input DTO materialization
  - shared response side по small-header allocs уже в основном вычищен

## 2026-03-12 SVO + safe response builder
- В `katana/core/include/katana/core/arena.hpp` `arena_vector` больше не просто alias:
  - добавлен реальный `arena_vector<T, InlineCapacity = 0>`
  - `InlineCapacity = 0` сохраняет старое поведение через `std::vector<T, arena_allocator<T>>`
  - `InlineCapacity > 0` даёт inline storage и spill в arena только после переполнения inline capacity
- В `tools/katana_gen/dto_generator.cpp` добавлен ограниченный generator-side rollout:
  - top-level scalar array aliases теперь могут эмититься как `arena_vector<T, N>`
  - current heuristic intentionally narrow: только top-level alias arrays для scalar item kinds (`number`, `integer`, `boolean`)
  - default inline capacity = `8`
  - `extract_arena_vector_inner_type()` обновлён под `arena_vector<T, N>`
- Подтверждённый результат на `compute_api`:
  - generated DTO на VM после rebuild: `using compute_sum_request = arena_vector<double, 8>;`
  - tuned metrics run:
    - `176949 req/sec`
    - `arena_alloc_calls_per_req = 0`
    - `arena_alloc_bytes_per_req = 0`
    - `socket_syscalls_per_req = 2.0`
  - tuned no-metrics rerun:
    - `173005 req/sec`
    - `p50 1578 us`
    - `p95 7010 us`
    - `p99 11517 us`
- Ключевой вывод по SVO:
  - гипотеза про оставшиеся `64 B/request` была правильной
  - request-parse alloc в `compute_api` полностью убран
  - но throughput вырос только умеренно и всё ещё остаётся в том же порядке величины
- `perf stat` attach после полного removal parse alloc:
  - `compute_api`: `1437256 req` за `8.10s`
  - `task-clock = 19984.44 ms`
  - `CPU/request ≈ 13.90 us`
- Сравнение с предыдущим состоянием:
  - раньше tuned attach-style `compute_api` был около `14.9 us/request`
  - после SVO стало около `13.9 us/request`
  - то есть parse-path cleanup дал measurable, но не transformational improvement
- Практический вывод:
  - generated request-body alloc path теперь дочищен
  - оставшийся потолок снова подтверждён как shared network/core/kernel path
  - даже с нулевыми arena allocs на happy-path `compute_api` мы всё ещё очень далеко от `600K`

## 2026-03-12 Additive safe-by-default response API
- В `katana/core/include/katana/core/http.hpp` и `katana/core/src/http.cpp` добавлен additive слой:
  - `canonical_reason_phrase(status)`
  - `response_builder`
  - `respond::into(out).text/json/problem/created_json/no_content()`
- `response::with_status()` теперь обновляет canonical reason phrase, а не оставляет stale reason.
  - Это убирает старый footgun вида `response::json(...).with_status(201)` => `201 OK`
- Новая safe-by-default API уже заведена в часть call sites:
  - core error mapping в router/content negotiation теперь использует in-place path через `respond::into(out).problem(...)`
  - hot `201/204` call sites в `products_api`, `task_api` и `benchmark/products_api_benchmark.cpp` переведены на `created_json()` / `no_content()`
- Это изменение в первую очередь architectural:
  - оно не даёт большого RPS скачка само по себе
  - но убирает класс регрессий, где временный `response` снова незаметно тащит alloc path или stale status reason

## 2026-03-12 Pipelined network benchmark reset
- Пользовательская гипотеза про load-generator bias подтвердилась.
  - Старые `wrk` / keep-alive сценарии были ping-pong: один запрос, один ответ, без real pipelining.
  - Это искажало выводы о том, что `hello` и `compute_api` якобы одинаково упираются в shared path.
- Добавлены два новых load scripts:
  - `test/load/scripts/hello_pipeline.lua`
  - `test/load/scripts/compute_sum_pipeline.lua`
  - Они возвращают из `request()` сразу склеенный payload из `pipeline_depth` запросов.
  - `pipeline_depth` сейчас configurable через `KATANA_PIPELINE_DEPTH`, default `20`.
- `wrk` с pipeline payload реально работает как грубый HTTP/1.1 pipelining client.
  - На `hello` (`4 threads`, `256 conns`, depth `20`) серверные syscall metrics ушли в:
    - `recv_per_req ≈ 0.050`
    - `send_per_req ≈ 1.000`
    - `socket_syscalls_per_req ≈ 1.050`
  - Значит за один `recv` парсер реально проглатывает примерно `20` запросов, как и ожидалось.
- Ключевой результат по `hello`:
  - old non-pipelined tuned baseline был около `180-200K req/sec`
  - pipelined `wrk` дал `421749.94 req/sec`
  - это более чем `2x` рост без изменения server code
- Ключевой результат по `compute_api`:
  - pipelined `wrk` depth `20` дал `178888.87 req/sec`
  - серверные syscall metrics тоже ушли в:
    - `recv_per_req ≈ 0.050`
    - `send_per_req ≈ 1.000`
    - `epoll_wait_per_req ≈ 0.001`
    - `arena_alloc_calls_per_req = 0`
  - но throughput почти не изменился относительно прежнего tuned baseline
- Самый важный новый вывод:
  - предыдущее равенство `hello ≈ compute_api` было в значительной степени артефактом ping-pong клиента
  - under pipelining `hello` уходит в совершенно другой класс throughput, а `compute_api` остаётся рядом со старым уровнем
  - это означает, что для `hello` inbound/shared network path действительно был очень важной частью потолка
  - но для `compute_api` после removal allocs и после removal ping-pong bias bottleneck уже не сводится к shared inbound path
- Следствия для дальнейшей работы:
  - старый тезис "на этой VM физически нельзя подойти к 400K+" уже нельзя считать верным: `hello` практически дошёл до этого уровня
  - цель `~1M hello` всё ещё не взята, но gap уже намного меньше и сейчас более честно связан с remaining response/send CPU cost
  - цель `600K compute_api` всё ещё далека; теперь это выглядит скорее как проблема compute path (JSON parse/serialize/generated business path + response send), а не просто network ingress
- Осторожность по интерпретации latency:
  - `wrk` latency под pipelining нельзя напрямую сравнивать с latency из non-pipelined runs
  - при глубоком pipelining latency уже включает сильное queueing на соединении
  - поэтому для этого режима throughput и server-side syscall ratios сейчас надёжнее, чем p95/p99 как абсолютные SLA-метрики
- Дополнительная проверка генераторов:
  - `httperf` проверен и отброшен как основной saturating client
  - он умеет pipeline knobs, но на практике на этой VM быстро упирается в `FD_SETSIZE` / `fd-unavail`
  - в default Ubuntu repo `weighttp` и `tcpkali` не нашлись
  - `h2load` при необходимости можно получить через `nghttp2-client`, но пока прямой need нет, потому что `wrk + pipeline.lua` уже дал нужный сигнал

## 2026-03-12 Pipelined depth sweep and independent hello cross-check
- Depth sweep по `wrk + pipeline.lua` на Ubuntu VM показал реальную форму кривых:
  - `hello`, `w4/t4/c256`:
    - depth `1`: `56875 req/sec`
    - depth `5`: `292244 req/sec`
    - depth `10`: `307211 req/sec`
    - depth `20`: `419006 req/sec`
    - depth `40`: `419479 req/sec` в одном прогоне и `~301K` в другом, то есть после `20` рост уже неустойчивый
  - `compute_api`, `w4/t4/c256`:
    - depth `1`: `141507 req/sec`
    - depth `5`: `136562 req/sec`
    - depth `10`: `156553 req/sec`
    - depth `20`: `174269 req/sec`
    - depth `40`: сломанный режим, `261159` non-2xx responses и throughput деградирует до `~25.9K req/sec`
- Важный вывод:
  - для `hello` pipelining действительно снимает главный входной bottleneck
  - для `compute_api` даже при `recv_per_req ≈ 0.05` сервер почти не растёт, значит main cost уже не ingress
  - `compute_api` также имеет correctness cliff на слишком глубоком pipelining (`depth=40`) для POST path; это нужно держать в памяти как отдельный баг/ограничение
- Независимая проверка `hello` через другой клиент:
  - установлен `nghttp2-client`, доступен `h2load`
  - `h2load --h1 -m1` (`t6/c512`) дал `114543 req/sec`
  - `h2load --h1 -m20` (`t6/c512`) дал `372485 req/sec`
  - это подтверждает сам принцип: внешний клиент с настоящим HTTP/1.1 pipelining действительно открывает для `hello` существенно более высокий потолок, чем ping-pong режим
- Отдельное замечание по `perf`:
  - initial `perf stat -p $pid` для `hello` сначала дал мусор, потому что tids снимались слишком рано
  - причина найдена: в `server::run()` `on_start` и listener уже подняты до `pool.start()`
  - корректный attach надо делать только после проверки, что `NLWP > 1`

## 2026-03-12 Pipelined perf: hello vs compute
- После корректного wait на `NLWP > 1`:
  - `hello`, `w4/t4/c256/depth20`:
    - `478913 req/sec`
    - `NLWP = 5`
    - `task-clock = 6546.60 ms` за `8s` attach window
    - грубая оценка `CPU/request ≈ 1.71 us`
    - `context-switches = 1966` за окно, то есть очень мало
  - `compute_api`, `w6/t6/c512/depth20`:
    - `188223 req/sec`
    - `NLWP = 7`
    - `task-clock = 21042.95 ms`
    - `CPU/request ≈ 13.98 us`
    - `context-switches = 84027`
- Это фактически ломает старую гипотезу про общий shared-path потолок:
  - под честным pipelining `hello` и `compute_api` живут уже в радикально разных CPU/request классах
  - значит shared ingress path был критичен для `hello`, но не объясняет текущий потолок `compute_api`
  - оставшийся gap для `compute_api` сидит уже в parse/serialize/dispatch/business-response path

## 2026-03-12 Compute single-pass array parse
- Новая рабочая гипотеза после pipelined reset:
  - даже с нулевыми arena allocs `compute_api` продолжает платить фиксированную цену за generated JSON parse
  - конкретно `parse_compute_sum_request()` делал лишний pre-scan массива только ради `reserve_hint`, а потом второй проход для реального `parse_double()`
  - для `compute_sum_request = arena_vector<double, 8>` этот pre-scan стал уже просто чистым waste
- Изменение:
  - в `tools/katana_gen/json_generator.cpp` generator обновлён так, чтобы для inline scalar arena arrays не генерировать reserve pre-scan
  - regenerated `examples/codegen/compute_api/generated/generated_json.hpp`
  - `parse_compute_sum_request(...)` теперь идёт одним проходом без count-commas stage
- Локальная проверка:
  - `wsl cmake --build build/bench-wsl --target katana_gen compute_api -j4`
  - `wsl ctest --test-dir build/bench-wsl -R integration_tests --output-on-failure`
  - всё прошло
- VM результаты после ручной синхронизации regenerated `compute_api`:
  - `w4/t4/c256/depth20`: `203951 req/sec`
  - это против прежних `174269 req/sec` на тех же pipelined settings
  - server-side ratios остались чистыми:
    - `recv_per_req ≈ 0.050`
    - `send_per_req = 1.000`
    - `epoll_wait_per_req ≈ 0.001`
    - `arena_alloc_calls_per_req = 0`
  - `response_serialize_bytes_per_req ≈ 96`
- Pipelined perf после single-pass parse:
  - `compute_api`, `w4/t4/c256/depth20`:
    - `200553 req/sec`
    - `NLWP = 5`
    - `task-clock = 20654.51 ms`
    - `CPU/request ≈ 12.87 us`
    - `context-switches = 103143`
- Практический вывод:
  - removal второго прохода по массиву дал уже заметный выигрыш, около `+17%` по RPS на контролируемом pipelined профиле
  - `CPU/request` у `compute_api` упал примерно с `~13.98 us` до `~12.87 us`
  - это всё ещё очень далеко от необходимых `600K`, но теперь improvement снова material, а не шум
  - следующий рациональный слой для `compute_api`: response path и generic router/binding overhead, а не network ingress

## 2026-03-12 Agent findings worth keeping
- Агент по `compute_api` подтвердил ranking remaining hotspots:
  - biggest candidates после alloc cleanup: generated JSON parse, generic response serialization, generic router/binding overhead
  - сам sum loop и validator не выглядят существенными
- Агент по vectored send подтвердил:
  - low-risk MVP возможен только как persistent `head + body` send state в `connection_state`
  - текущий contiguous `active_response` path можно заменить на `serialize_head_into(...) + moved body + sendmsg/writev`
  - главный риск не syscall API, а lifetime и partial-write bookkeeping across `EAGAIN`

## 2026-03-12 Clean-port correction and hybrid send follow-up
- Важная коррекция:
  - прежний `depth=40` POST pipeline bug на `compute_api` оказался ложным сигналом из-за грязного `:8080`
  - на clean-port (`18081`) текущий код стабильно отдаёт `0` ошибок и на `depth=20`, и на `depth=40`
  - вывод: parser/FSM не были реальной причиной того `400 Bad Request`; там был contamination от старого listener/process state
- Локальный временный sync-сервер для VM использовался только для копирования файлов и затем остановлен; временный `tmp_local_parser_debug.sh` удалён
- Первый `writev/sendmsg` MVP действительно дал регрессионные риски:
  - tiny `hello/compute` responses ушли с дешёвого `send()` fast path на `sendmsg`
  - был найден баг resume-пути: pending-write check смотрел только на размер `active_response`, а не `head + body`
  - также single-iovec resume всё ещё платил через `sendmsg`
- Исправления:
  - в `http_server.cpp` pending-write проверка переведена на `head + body`
  - маленькие non-chunked ответы возвращены на contiguous `serialize_into(...) + send()` path
  - в `tcp_socket.cpp` `writev(count == 1)` теперь падает обратно в `write()`
- После hybrid fix, clean-port VM:
  - `hello`, `w4/t4/c256/depth20`: `274228 req/sec`
  - `compute_api`, `w4/t4/c256/depth20`: `214973 req/sec`
  - `compute_api`, `w4/t4/c256/depth40`: `234707 req/sec`
  - `hello`, peak no-perf rerun `w4/t4/c512/depth20`: `298432 req/sec`
  - `compute_api`, peak no-perf rerun `w4/t4/c512/depth40`: `267480 req/sec`
- Perf после hybrid fix показал следующий важный сдвиг:
  - верхняя зона у `hello` и `compute_api` сидит уже почти целиком в `katana::tcp_socket::write -> inet_sendmsg/tcp_sendmsg/tcp_write_xmit`
  - это означает, что следующий реальный рычаг уже не input path, а send-side batching

## 2026-03-12 Output batching breakthrough
- Ключевая новая гипотеза:
  - даже под pipelining у сервера `recv_per_req` уже был `~0.05`, но `send_per_req` оставался `~1.0`
  - значит мы амортизировали вход, но не амортизировали выход; сервер по-прежнему слал почти каждый ответ отдельным `send`
- Реализация:
  - в `http_server` добавлен минимальный batching только для маленьких non-chunked responses
  - batch собирается в один contiguous `active_response` buffer до лимита `64 KiB`
  - для batching используется `response_scratch`, после сериализации каждого ответа `arena` и parser сразу reset-ятся на следующий request
  - split `head + body` path для крупных ответов не переписывался глубоко; batching targeting только hot small-response workloads (`hello`, `compute_api`)
- Серверные метрики после batching:
  - `hello`, `w4/t4/c512/depth20`:
    - `Requests/sec = 1450969`
    - `recv_per_req = 0.050`
    - `send_per_req = 0.050`
    - `socket_syscalls_per_req = 0.100`
  - `compute_api`, `w4/t4/c512/depth40`:
    - `Requests/sec = 748625`
    - `recv_per_req = 0.050`
    - `send_per_req = 0.025`
    - `socket_syscalls_per_req = 0.075`
- Это и есть переломный момент:
  - после batching shared send-side TCP cost перестал платиться per response
  - throughput goals для текущей VM фактически закрыты

## 2026-03-12 Final validated operating points
- Clean no-metrics validation + `perf stat`:
  - `hello`, `w4/t4/c512/depth20`:
    - `1370428 req/sec`
    - latency: `p50 3.91 ms`, `p99 18.88 ms`
    - `task-clock = 15254.98 ms` за `8s`
    - грубо `CPU/request ≈ 1.39 us`
  - `compute_api`, `w4/t4/c512/depth40`:
    - `748356 req/sec`
    - latency: `p50 14.29 ms`, `p99 49.99 ms`
    - `task-clock = 21318.31 ms` за `8s`
    - грубо `CPU/request ≈ 3.56 us`
- Depth sweep для latency/throughput trade-off:
  - `hello`, `w4/t4/c512/depth5`: `643110 req/sec`, `p50 3.39 ms`, `p99 113.99 ms`
  - `hello`, `w4/t4/c512/depth10`: `994283 req/sec`, `p50 3.13 ms`, `p99 20.15 ms`
  - `hello`, `w4/t4/c512/depth20`: `1375012 req/sec`, `p50 3.95 ms`, `p99 16.69 ms`
  - `compute_api`, `w4/t4/c512/depth10`: `619261 req/sec`, `p50 4.80 ms`, `p99 17.36 ms`
  - `compute_api`, `w4/t4/c512/depth20`: `685703 req/sec`, `p50 8.32 ms`, `p99 32.42 ms`
  - `compute_api`, `w4/t4/c512/depth40`: `744104 req/sec`, `p50 14.35 ms`, `p99 50.16 ms`
- Практический вывод:
  - если цель — просто взять KPI по throughput, текущий надёжный peak point:
    - `hello`: `w4/t4/c512/depth20`
    - `compute_api`: `w4/t4/c512/depth40`
  - если нужен более мягкий latency/throughput operating point:
    - `hello`: `depth10` почти держит `~1M` и имеет лучший `p50`
    - `compute_api`: `depth10` уже проходит `600K` и имеет заметно лучше хвосты, чем `depth40`

## 2026-03-12 Hardening and local WSL validation
- Hardening после batching:
  - добавлен unit test на partial `writev` ordering/blocking в `test/unit/test_tcp_socket.cpp`
  - добавлен integration test `HTTPServerPipeline.PreservesOrderingAcrossMixedSmallAndLargeResponses`
  - parser regression test на incremental pipelined POST исправлен как тестовая ошибка, а не parser bug:
    - `parser::state::complete` sticky до `prepare_for_next_request()`
    - helper теперь не пересчитывает уже завершённый 20-й request повторно
- Важное замечание по boundary integration:
  - исходный slow-reader вариант с tiny `SO_RCVBUF` давал ложный fail на WSL
  - сервер по `KATANA_CONN_DEBUG` реально flush-ил весь `small -> large -> small` pipeline корректно
  - проблема была в самом test harness: он не успевал дочитать `128 KiB` large response в WSL zero-window-like режиме
  - итоговый integration test оставлен как live mixed-ordering/bypass test без WSL-specific TCP throttling
- Локальная WSL e2e валидация снята вручную через `wrk` + pipeline Lua scripts:
  - `hello`, `w4/t4/c512/depth10`, `10s`:
    - `1410155 req/sec`
    - latency: `p50 1.877 ms`, `p95 7.104 ms`, `p99 11.153 ms`
    - errors: `0`
  - `hello`, `w4/t4/c512/depth20`, `5s`:
    - `2792957 req/sec`
    - latency: `p50 1.134 ms`, `p95 3.343 ms`, `p99 5.757 ms`
    - errors: `0`
  - `compute_api`, `w4/t4/c512/depth10`, `10s`:
    - `703412 req/sec`
    - latency: `p50 4.232 ms`, `p95 12.925 ms`, `p99 19.322 ms`
    - errors: `0`
  - `compute_api`, `w4/t4/c512/depth20`, `5s`:
    - `1284988 req/sec`
    - latency: `p50 4.237 ms`, `p95 8.840 ms`, `p99 12.886 ms`
    - errors: `0`
  - `compute_api`, `w4/t4/c512/depth40`, `5s`:
    - `1308480 req/sec`
    - latency: `p50 7.992 ms`, `p95 15.867 ms`, `p99 18.416 ms`
    - errors: `0`
- Практический статус:
  - KPI на локальном WSL закрыты с большим запасом
  - самый мягкий low-latency operating point, который всё ещё закрывает целевые KPI:
    - `hello`: `depth10`, `~1.41M req/sec`
    - `compute_api`: `depth10`, `~703K req/sec`
- Runner note:
  - `scripts/run_benchmarks.py --stage 9 10` в текущем `build/bench-wsl` layout не нашёл `hello_world_server`/`compute_api`
  - источником истины для локального WSL стали прямые ручные `wrk` прогоны; summary лежит в `profiling_results/wsl_manual/RESULTS.md`

## 2026-03-12 Benchmark pipeline refresh
- `scripts/run_benchmarks.py` обновлён до актуального pipeline state:
  - default pipeline теперь запускает все micro stages `1-4,6-8`, а `--include-e2e` добавляет новые `wrk`-based network stages `9-12`
  - stage `5` оставлен только как legacy keep-alive scenario и больше не является основным e2e benchmark path
  - runner теперь auto-detect'ит build tree (`build/bench` vs `build/bench-wsl`) и умеет принимать явный `--build-dir`
  - в environment summary теперь пишутся `build_dir`, `benchmark_dir`, `wrk_binary`
- wrk network stages переписаны на реальные operating points:
  - stage `9`: `hello`, canonical, `w4/t4/c512/depth10`, `10s`
  - stage `10`: `compute_api`, canonical, `w4/t4/c512/depth10`, `10s`
  - stage `11`: `hello`, peak, `w4/t4/c512/depth20`, `5s`
  - stage `12`: `compute_api`, peak, `w4/t4/c512/depth40`, `5s`
  - runner теперь прокидывает `wrk_env`, поэтому `KATANA_PIPELINE_DEPTH` задаётся stage config'ом, а не вручную
- wrk parsing и отчёты расширены:
  - парсятся `Requests/sec`, `Transfer/sec`, average latency из `Thread Stats`, custom `KATANA_*` percentiles/errors
  - в markdown добавлен отдельный `E2E Summary` блок с profile / workers / wrk threads+connections / pipeline depth / duration / throughput / avg+p50+p95+p99 / data rate / errors
  - stage sections теперь также печатают `description` и `config`
  - в отчёт добавлена методологическая оговорка, что при pipeline Lua scripts latency относится к batch/request() call, а не к isolated single request
- Валидация нового pipeline в WSL:
  - команда:
    - `python3 scripts/run_benchmarks.py --build-dir build/bench-wsl --no-build --no-auto-compare-baseline --repeats 1 --include-e2e --output benchmark_results/wsl_pipeline_refresh_20260312`
  - полный micro + e2e прогон завершился зелёно, JSON/Markdown артефакты лежат в:
    - `benchmark_results/wsl_pipeline_refresh_20260312/benchmark_results.json`
    - `benchmark_results/wsl_pipeline_refresh_20260312/BENCHMARK_RESULTS.md`
  - свежий run дал:
    - stage `9` hello canonical: `1.3M req/sec`, avg batch latency `4.38 ms`, p50 `2.331 ms`, p95 `14.259 ms`, p99 `25.031 ms`, errors `0`
    - stage `10` compute canonical: `1.2M req/sec`, avg batch latency `2.71 ms`, p50 `2.469 ms`, p95 `5.430 ms`, p99 `8.017 ms`, errors `0`
    - stage `11` hello peak: `2.8M req/sec`, avg batch latency `2.21 ms`, p50 `1.930 ms`, p95 `4.534 ms`, p99 `8.114 ms`, errors `0`
    - stage `12` compute peak: `1.4M req/sec`, avg batch latency `8.06 ms`, p50 `8.074 ms`, p95 `17.419 ms`, errors `0`
- Практический итог:
  - benchmark pipeline снова является источником истины и больше не требует ручных `wrk` прогонов для локального WSL baseline
  - microbenchmarks и e2e отчёты теперь собираются одним runner'ом и попадают в единый JSON/Markdown report

## 2026-03-12 Idle batching regression guard
- Зафиксирован и проверен важный latency-invariant batched response path:
  - batching не ждёт таймера, фиксированного числа ответов или заполнения буфера
  - склейка идёт только пока в parser buffer уже есть следующие request bytes на той же итерации event-loop
  - если `buffered_bytes() == 0`, сервер сразу идёт в `flush_ready_responses()` и затем обратно armed на `readable`
- Добавлен integration test:
  - `HTTPServerPipeline.FlushesSingleSmallResponseWithoutWaitingForBatch`
  - файл: `test/integration/test_http_server.cpp`
  - сценарий:
    - открыть keep-alive соединение
    - отправить ровно один `GET /small`
    - дождаться `POLLIN` без отправки второго request
    - прочитать и проверить первый ответ
    - затем отправить второй `GET /small` и проверить, что keep-alive соединение остаётся рабочим
- Валидация:
  - `cmake --build build/bench-wsl --target integration_tests -j4`
  - `ctest --test-dir build/bench-wsl -R integration_tests --output-on-failure`
  - integration suite зелёный

## 2026-03-12 Example surface cleanup
- Разобран актуальный статус `examples/`:
  - top-level examples (`hello_world_server`, `router_rest_api`, `middleware_examples`, `raii_*`, `simple_rest_api`, `basic_reactor`) всё ещё используются как документация/smoke examples и не трогаются
  - active codegen examples остаются: `compute_api`, `validation_api`, `text_api`, `benchmark_api`, `task_api`
  - `examples/codegen/products_api/` больше не считается поддерживаемым примером: он не подключён в CMake и не участвует в benchmark pipeline
- Принято решение не удалять `examples/codegen/products_api/main.cpp` физически:
  - файл уже модифицирован в рабочем дереве
  - безопаснее вывести legacy path из поддерживаемой документации, чем сносить изменённый reference-код
- Сделанная чистка:
  - `examples/codegen/README.md` обновлён: `products_api` явно помечен как archived reference, а не активный пример
  - удалён `test/load/products_api_load_test.sh` как мёртвый load script, который ссылался на несуществующий build target
  - `docs/BENCHMARKING_METHODOLOGY.md` переведён с устаревшего `products_api` load-test narrative на актуальный unified pipeline:
    - source of truth: `scripts/run_benchmarks.py`
    - e2e stages: `hello_world_server` / `compute_api` canonical + peak wrk profiles
    - profiling/stability теперь описаны через current perf/wrk workflow, а не через deleted shell script
- После правок выполнена проверка ссылок:
  - оставшихся упоминаний `products_api_load_test.sh` и инструкций `cmake --build --preset examples --target products_api` в docs/README/test больше нет

## 2026-03-12 Diff and documentation consistency pass
- После review всего `git diff` добраны ещё несколько устаревших точек документации:
  - `README.md`: quick start для benchmark'ов переведён с одиночного `performance_benchmark` на unified runner `scripts/run_benchmarks.py --include-e2e`
  - `test/load/README.md`: теперь описывает maintained entrypoint через benchmark runner и актуальные `wrk` scripts (`hello_*`, `compute_sum*`), а не старые ad hoc сценарии как основной путь
  - `docs/BENCHMARKING.md`: benchmark system doc синхронизирован с текущим pipeline state:
    - `--include-e2e` означает maintained `wrk` stages для `hello_world_server` и `compute_api`
    - добавлены `benchmark_api_codegen_benchmark` и `benchmark_api_framework_benchmark`
    - integration section больше не ссылается на старые `examples` binaries как основной benchmark contract
- Приведены в соответствие generated/doc comments для codegen handler interfaces:
  - `tools/katana_gen/router_generator.cpp` и checked-in `generated_handlers.hpp` теперь рекомендуют `respond::into(out)...` / `out = response::...`, а не старый `response::ok(...)`-centric стиль
- Локальный `scripts/__pycache__/` остался только как незастейдженный runtime мусор; в commit не включать

## 2026-03-12 CI stabilization after benchmark/pipeline merge
- Разобран и починен первый красный слой GitHub Actions:
  - `Backend Matrix (epoll)` и `fuzzing` падали на `-Werror,-Wunused-lambda-capture` в `katana/core/src/syscall_metrics.cpp`
  - причина: `reporter_state::start()` создавал `std::thread([this, &registry] { ... })`, хотя `this` не использовался
  - фикс: удалён лишний capture
- Разобран и починен слой unit regressions, всплывший в `Code Coverage`:
  - `VectoredIO.*` падали потому, что generic helper `write_vectored()` в `katana/core/src/io_buffer.cpp` был переведён на `sendmsg()`, а unit tests гоняют его через `pipe2()`
  - на pipe fd это давало `ENOTSOCK (88)` вместо ожидаемого успешного `writev()` / `EAGAIN`
  - фикс: generic `write_vectored()` возвращён на `writev()`
- Разобран и починен слой reactor metrics regressions:
  - `ReactorTest.RegisterFdWithTimeout`, `ReactorTest.MetricsTracking` и `ReactorPoolTest.MetricsAggregation` ожидали, что reactor metrics являются always-on контрактом
  - фактически metrics были загейтаны через `KATANA_REACTOR_METRICS`, из-за чего snapshot'ы в тестах оставались нулевыми
  - фикс: internal reactor metrics снова считаются всегда; env-gate больше не отключает счётчики
- Линт/формат:
  - локально прогнан `pre-commit run --all-files`
  - hooks поправили trailing whitespace / EOF / clang-format по checked-in generated headers и нескольким benchmark/core файлам
  - после повторного прогона весь pre-commit зелёный
- Валидация после фиксов:
  - `wsl bash -lc 'cd /mnt/c/Users/Ya/OneDrive/Desktop/KATANA && cmake --build build/bench-wsl --target unit_tests -j4'`
  - `wsl bash -lc 'cd /mnt/c/Users/Ya/OneDrive/Desktop/KATANA && ./build/bench-wsl/test/unit_tests --gtest_filter="VectoredIO.*:ReactorPoolTest.MetricsAggregation:ReactorTest.RegisterFdWithTimeout:ReactorTest.MetricsTracking"'`
  - `wsl bash -lc 'cd /mnt/c/Users/Ya/OneDrive/Desktop/KATANA && pre-commit run --all-files'`
  - `wsl bash -lc 'cd /mnt/c/Users/Ya/OneDrive/Desktop/KATANA/build/bench-wsl && ctest --output-on-failure -j4'`
  - итог: `unit_tests`, `integration_tests` и весь локальный pre-commit pipeline зелёные
