# Compute API (codegen)

Минимальный, но нагруженный пример: POST `/compute/sum` принимает JSON-массив чисел и возвращает сумму. Никаких блокировок и I/O — только десериализация, валидация и CPU-логика.

## Что демонстрирует
- DTO `array<double>` + arena allocators
- Zero-copy JSON → arena
- Валидация `minItems/maxItems` из OpenAPI
- Handler без виртуального хопа в горячем пути (`make_router` инлайнит вызовы)
- Streaming JSON writer для ответа

## Сборка и запуск
```bash
cmake --preset examples
cmake --build --preset examples --target compute_api
./build/examples/examples/codegen/compute_api/compute_api  # PORT=8080 по умолчанию
```

## Пример запроса
```bash
curl -X POST http://localhost:8080/compute/sum \
  -H 'Content-Type: application/json' \
  -d '[1, 2, 3, 4.5]'
# => 10.5
```

## Нагрузочное тестирование
Рекомендуемые сценарии (5 c прогрева → 5 c нагрузки):
- Размеры массивов: 1, 8, 64, 256, 1024
- Потоки: 1, 4, 8, 16

Базовые ожидания на обычном x86:
- p50: 0.03–0.05 ms
- p95: 0.10–0.20 ms
- p99: 0.18–0.35 ms
- 150k–350k rps (зависит от размера массива)

Эти сценарии уже интегрированы в `generate_benchmark_report.py` и docker-бенчмарк.
