# Генератор кода KATANA (`katana_gen`)

CLI генерирует DTO, валидаторы, JSON (де)сериализацию, биндинги роутера и интерфейсы хендлеров под архитектуру ядра (router, arena-аллокаторы, zero-copy I/O).

## Как вызвать

```bash
katana_gen openapi -i test_api.yaml -o ./generated --emit all --alloc pmr
```

Ключи:
- `--emit dto|validator|serdes|router|handler|all` — что генерировать (по умолчанию `all`).
- `--alloc pmr|std` — выбирай `pmr` для арен и zero-alloc горячего пути.
- `--layer flat|layered` — стиль слоёв (flat по умолчанию).
- `--dump-ast` — сохранить `openapi_ast.json`.
- `--strict` — упасть на любой ошибке спеки.

Артефакты:
- `generated_dtos.hpp` — DTO/enum’ы (arena-aware при `--alloc pmr`).
- `generated_validators.hpp` — проверки required/enum.
- `generated_json.hpp` — JSON парсинг/сериализация.
- `generated_routes.hpp` — compile-time метаданные маршрутов.
- `generated_handlers.hpp` — интерфейс хендлера.
- `generated_router_bindings.hpp` — статический router, связанный с хендлером.

### Быстрый старт
```bash
katana_gen openapi -i benchmark/test_api.yaml -o benchmark/generated --emit all --alloc pmr
cmake --build build --target generated_api_benchmark
```

## Производительность и архитектура

- При `--alloc pmr` код использует `katana::monotonic_arena`; держи арену на запрос и переиспользуй.
- Биндинги возвращают **stateless/static router** — создаётся один раз, без аллокаций на запрос.
- DTO/парсер без кучи при `arena_string`/`arena_vector`; старайся везде `pmr` на hot path.
- Параметры пути — `string_view`/примитивы, не копируй их.
- В хендлерах собирай ответ с предвычисленными заголовками и `serialize_into`, переиспользуя буфер.

## Регенерация для бенчмарков

Бенчи `openapi_benchmark` и `generated_api_benchmark` ждут файлы в `benchmark/generated`. Обновить:

```bash
katana_gen openapi -i benchmark/test_api.yaml -o benchmark/generated --emit all --alloc pmr
cmake --build build --target generated_api_benchmark openapi_benchmark
```

## Замечания для контрибьюторов

- Новые шаблоны делай аллокатор-независимыми: arena только под `--alloc pmr`.
- Не конкатенируй `std::string` на горячем пути — используй stack `to_chars` и префиксы заголовков.
- Не создавай router на каждый запрос — держи статический (как генерируется).
- Валидация — простые проверки required/enum; никаких `std::regex` и тяжёлых зависимостей.
