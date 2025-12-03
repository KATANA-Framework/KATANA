# Codegen Examples

Примеры использования кодогенерации из OpenAPI спецификаций.

## Доступные примеры

### 1. Simple CRUD (`simple_crud/`)

Простой in-memory CRUD API для управления задачами (tasks).

**Возможности:**
- Полный набор REST операций (GET, POST, PUT, DELETE)
- OpenAPI 3.0 спецификация
- In-memory хранилище
- JSON сериализация/десериализация
- Валидация данных

**Статус:** Готов к использованию (с ручными DTOs, ожидается интеграция кодогена)

## Структура

```
codegen/
├── README.md           # Этот файл
└── simple_crud/        # Simple CRUD API пример
    ├── api.yaml        # OpenAPI спецификация
    ├── main.cpp        # Реализация
    ├── CMakeLists.txt  # Сборка
    └── README.md       # Документация
```

## Сборка всех примеров

```bash
# Из корня проекта
cmake --preset examples
cmake --build --preset examples
```

## Планы развития

Будущие примеры:

1. **Advanced CRUD** - пример с:
   - Пагинацией и фильтрацией
   - Сортировкой
   - Вложенными ресурсами
   - x-katana-cache расширениями

2. **Auth API** - пример с:
   - JWT токенами
   - Middleware аутентификации
   - Role-based access control

3. **Real-time API** - пример с:
   - WebSocket поддержкой
   - Server-Sent Events
   - Pub/Sub паттерном

4. **Database Integration** - пример с:
   - PostgreSQL через libpq
   - SQL кодогенерацией
   - Транзакциями

## Использование кодогенератора

После завершения разработки katana_gen:

```bash
# Генерация из OpenAPI спецификации
katana_gen openapi -i api.yaml -o generated --emit all

# Генерируемые файлы:
# - generated_dtos.hpp      - DTOs с pmr allocators
# - generated_json.hpp      - JSON парсеры/сериализаторы
# - generated_routes.hpp    - Compile-time route table
# - generated_validators.hpp - Валидаторы полей
```

## Связанная документация

- [OpenAPI Documentation](../../docs/OPENAPI.md)
- [Router Documentation](../../docs/ROUTER.md)
- [Architecture Overview](../../ARCHITECTURE.md)
