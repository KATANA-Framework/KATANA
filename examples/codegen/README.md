# Codegen Examples

Два минимальных, но нагруженных примера для демонстрации `katana_gen` на реальных профилях нагрузки.

## Доступные примеры

### 1. Compute API (`compute_api/`)
POST `/compute/sum` принимает массив чисел (`minItems=1, maxItems=1024`) и возвращает сумму. Чистая CPU-логика без I/O и блокировок — идеальный стресс-тест сериализации/валидации.

### 2. Validation API (`validation_api/`)
POST `/user/register` валидирует email/пароль/age (required + nullable + format + min/max) и возвращает `"ok"`. Стрессует валидаторы и Problem Details.

## Структура
```
codegen/
├── README.md
├── compute_api/
│   ├── api.yaml            # OpenAPI спецификация (/compute/sum)
│   ├── main.cpp            # Handler + server
│   ├── CMakeLists.txt      # Генерация katana_gen + сборка
│   ├── README.md           # Как гонять и бенчить
│   └── generated/          # Автогенерированные DTO/JSON/validators/routes
└── validation_api/
    ├── api.yaml            # OpenAPI спецификация (/user/register)
    ├── main.cpp
    ├── CMakeLists.txt
    ├── README.md
    └── generated/
```

## Сборка всех примеров
```bash
# Из корня проекта
cmake --preset examples
cmake --build --preset examples --target compute_api validation_api
```

Оба бинаря оказываются в `build/examples/examples/codegen/*/`.

## Использование кодогенератора
Генерация в примерах триггерится через CMake, но можно вручную:
```bash
./build/katana_gen openapi -i api.yaml -o generated --emit all --inline-naming operation
```
Эмитятся DTO + JSON + validators + routes + router bindings.

## Бенчмарки
`generate_benchmark_report.py` и docker-бенчмарки запускают оба примера:
- Compute API: смешанные размеры массивов (1/8/64/256/1024) под 1/4/8/16 потоков
- Validation API: смесь валидных/невалидных запросов под 4–8 потоков

Метрики пишутся в `BENCHMARK_RESULTS.md` вместе с остальными бенчами фреймворка.
Если установлен `wrk`, бенчмарки используют его как нагрузочный клиент (в контейнере он уже есть); иначе падают на упрощённый TCP-клиент с более низкой отдачей.
