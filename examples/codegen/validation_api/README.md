# Validation API (codegen)

Микросервис-вальидатор: POST `/user/register` проверяет email/пароль/age и сразу отвечает `"ok"` без бизнес-логики.

## Что демонстрирует
- Compile-time DTO + валидаторы (required, nullable, regex/format/email, minLength/maxLength, integer ranges)
- Zero-copy JSON → arena DTO
- Problem Details (422/400) при нарушении схемы
- Optional semantics: `age` может отсутствовать или быть `null`

## Сборка и запуск
```bash
cmake --preset examples
cmake --build --preset examples --target validation_api
./build/examples/examples/codegen/validation_api/validation_api  # PORT=8081 по умолчанию
```

## Пример запроса
```bash
curl -X POST http://localhost:8081/user/register \
  -H 'Content-Type: application/json' \
  -d '{"email":"a@b.com","password":"supersecret","age":21}'
# => "ok"
```

Неверный email или короткий пароль автоматически дают 400/422 с подробным `detail.path`.

## Нагрузочное тестирование
Сценарий: смесь валидных/невалидных запросов под 4–8 потоков (5 c прогрев → 5 c нагрузка).
- Метрики: success_rate, p50/p95/p99 для валидных и для reject-пути
- Используется в `generate_benchmark_report.py` и docker-бенчмарке
