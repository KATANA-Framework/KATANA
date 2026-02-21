# Codegen Examples

Актуальные примеры `katana_gen`, которые собираются через CMake и регенерируют код из OpenAPI при сборке.

## Активные примеры

- `compute_api/` — CPU-нагрузка на parse/validate/serialize (`/compute/sum`).
- `validation_api/` — валидации форматов и диапазонов (`/user/register`).
- `text_api/` — несколько endpoint'ов с enum/transform-логикой.
- `benchmark_api/` — широкий API для codegen-бенчей (CRUD + compute + validation).
- `task_api/` — сложная спецификация для стресс-теста генератора.

`products_api/` сейчас legacy-папка (только `main.cpp`, без подключенного CMake/OpenAPI пайплайна).

## Сборка

```bash
cmake --preset bench
cmake --build --preset bench --target compute_api validation_api text_api benchmark_api task_api
```

Бинарники появляются в `build/bench/examples/codegen/*/`.

## Генерация через инструменты

В каждом активном примере `CMakeLists.txt` вызывает `katana_gen openapi ... --emit all --inline-naming operation` и пишет файлы в локальный `generated/`.

Ручная регенерация (из корня проекта):

```bash
./build/bench/katana_gen openapi -i examples/codegen/compute_api/api.yaml -o examples/codegen/compute_api/generated --emit all --inline-naming operation
```

Аналогично для остальных `api.yaml`.
