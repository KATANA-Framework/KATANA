# Simple CRUD API with Codegen

Простой пример in-memory CRUD API для управления задачами (tasks), демонстрирующий использование кодогенерации из OpenAPI спецификации и новых high-level абстракций фреймворка.

## 🎯 Что демонстрирует этот пример

- ✅ **OpenAPI → Code Generation**: Автоматическая генерация DTOs, JSON parsers, validators, routes
- ✅ **Zero Boilerplate Server**: Использование `katana::http::server` (400 строк → 220 строк)
- ✅ **Fluent Response API**: Удобное построение ответов через method chaining
- ✅ **Type-Safe Routing**: Compile-time проверка маршрутов и параметров
- ✅ **Arena Allocation**: Per-request память с автоматической очисткой

## Структура

```
simple_crud/
├── api.yaml              # OpenAPI 3.0 спецификация
├── main_codegen.cpp      # Полная реализация (старый стиль, ~400 строк)
├── main_simple.cpp       # Упрощённая реализация (новый стиль, ~220 строк)
├── generated/            # Автогенерированный код
│   ├── generated_dtos.hpp
│   ├── generated_json.hpp
│   ├── generated_routes.hpp
│   ├── generated_handlers.hpp
│   ├── generated_validators.hpp
│   └── generated_router_bindings.hpp
├── CMakeLists.txt        # Конфигурация сборки с автогенерацией
└── README.md             # Эта документация
```

## API Endpoints

- `GET /tasks` - получить список всех задач
- `POST /tasks` - создать новую задачу
- `GET /tasks/{id}` - получить задачу по ID
- `PUT /tasks/{id}` - обновить задачу
- `DELETE /tasks/{id}` - удалить задачу

## 🚀 Быстрый старт

### Сборка

Из корня проекта:

```bash
cmake --preset examples
cmake --build --preset examples --target simple_crud_simple

# Запуск упрощённой версии
./build/examples/examples/codegen/simple_crud/simple_crud_simple
```

Или полной версии:

```bash
cmake --build --preset examples --target simple_crud_codegen
./build/examples/examples/codegen/simple_crud/simple_crud_codegen
```

### Автоматическая генерация кода

Генерация происходит автоматически при сборке через CMake:

```cmake
add_custom_command(
    OUTPUT generated/*.hpp
    COMMAND katana_gen openapi
            -i api.yaml
            -o generated
            --emit all
    DEPENDS api.yaml katana_gen
)
```

**Сгенерированные файлы:**
- `generated_dtos.hpp` - структуры Task, CreateTaskRequest, UpdateTaskRequest с arena allocators
- `generated_json.hpp` - JSON parsers и serializers (zero-copy где возможно)
- `generated_validators.hpp` - валидаторы с полной поддержкой OpenAPI constraints
- `generated_routes.hpp` - constexpr route table
- `generated_handlers.hpp` - интерфейс handler'а для type-safe реализации
- `generated_router_bindings.hpp` - автоматическая привязка routes к handler'у

## Использование API

### Создание задачи

```bash
curl -X POST http://localhost:8080/tasks \
  -H "Content-Type: application/json" \
  -d '{"title":"Buy milk","completed":false}'
```

Ответ:
```json
{"id":1,"title":"Buy milk","completed":false}
```

### Получение всех задач

```bash
curl http://localhost:8080/tasks
```

Ответ:
```json
[
  {"id":1,"title":"Buy milk","completed":false},
  {"id":2,"title":"Write code","completed":true}
]
```

### Получение задачи по ID

```bash
curl http://localhost:8080/tasks/1
```

Ответ:
```json
{"id":1,"title":"Buy milk","completed":false}
```

### Обновление задачи

```bash
curl -X PUT http://localhost:8080/tasks/1 \
  -H "Content-Type: application/json" \
  -d '{"completed":true}'
```

Ответ:
```json
{"id":1,"title":"Buy milk","completed":true}
```

### Удаление задачи

```bash
curl -X DELETE http://localhost:8080/tasks/1
```

Ответ: `204 No Content`

## 📊 Результат улучшений

### До: main_codegen.cpp (~400 строк)

Ручное управление reactor pool, connection state, парсингом, и сериализацией:

```cpp
int main() {
    TaskRepository repo;
    TaskApiHandler handler(repo);
    const auto& api_router = generated::make_router(handler);

    // 200+ строк boilerplate для:
    // - Создания TCP listener
    // - Настройки reactor_pool
    // - Управления connection_state
    // - Обработки accept/read/write
    // - Graceful shutdown
    // ...

    return 0;
}
```

### После: main_simple.cpp (~220 строк)

Использование high-level абстракций:

```cpp
int main() {
    TaskRepository repo;
    TaskApiHandler handler(repo);
    const auto& api_router = generated::make_router(handler);

    // ВСЁ! Server abstraction делает остальное
    return http::server(api_router)
        .listen(8080)
        .workers(4)
        .on_start([]() {
            std::cout << "✨ Server started!\n";
        })
        .on_request([](const request& req, const response& resp) {
            std::cout << method_to_string(req.http_method) << " "
                      << req.uri << " -> " << resp.status << "\n";
        })
        .run();
}
```

**Сокращение кода: 50% (400 → 220 строк)**

### Fluent Response API

До:
```cpp
response resp;
resp.status = 201;
resp.set_header("Content-Type", "application/json");
resp.body = serialize_Task(task);
return resp;
```

После:
```cpp
return response::json(serialize_Task(task)).with_status(201);
```

### Полная поддержка массивов

Генератор теперь создаёт парсеры и сериализаторы для массивов:

```cpp
// Автоматически сгенерировано
std::optional<std::vector<Task>> parse_Task_array(std::string_view json, monotonic_arena* arena);
std::string serialize_Task_array(const std::vector<Task>& tasks);

// Использование в handler'е
response list_tasks(const request&, request_context& ctx) override {
    auto tasks = repo_.list_all(&ctx.arena);
    return response::json(serialize_Task_array(tasks));  // Просто работает!
}
```

## 🎓 Что реализует пример

### TaskRepository (In-Memory Storage)

Thread-safe in-memory хранилище для задач:
- CRUD операции с std::mutex для синхронизации
- Автоинкремент ID для новых задач
- Преобразование между внутренним std::string и arena_string для DTOs

### TaskApiHandler (Generated Interface Implementation)

Наследуется от `generated::api_handler` и реализует все endpoint'ы:

```cpp
class TaskApiHandler : public generated::api_handler {
public:
    response list_tasks(const request&, request_context& ctx) override;
    response create_task(const request&, request_context& ctx, const CreateTaskRequest& body) override;
    response get_task(const request&, request_context& ctx, std::string_view id_str) override;
    response update_task(const request&, request_context& ctx, std::string_view id_str, const UpdateTaskRequest& body) override;
    response delete_task(const request&, request_context&, std::string_view id_str) override;
};
```

**Ключевые особенности:**
- ✅ Type-safe: компилятор проверяет сигнатуры методов
- ✅ Автоматический routing: `generated::make_router(handler)` создаёт полный router
- ✅ Валидация: встроенные validators проверяют DTO перед обработкой
- ✅ RFC 7807: стандартизированные ошибки через `problem_details`

### Main (Server Setup)

Две версии для сравнения:
1. **main_codegen.cpp**: Полная реализация с явным управлением reactor/connections
2. **main_simple.cpp**: Упрощённая версия с `http::server` abstraction

## 🔍 Детали генерации

### DTOs с Arena Allocators

```cpp
struct Task {
    int64_t id;
    arena_string title;        // zero-copy string_view + arena storage
    arena_string description;
    bool completed;

    explicit Task(monotonic_arena* arena)
        : title(arena), description(arena) {}
};
```

### JSON Parsing (Zero-Copy)

```cpp
std::optional<Task> parse_Task(std::string_view json, monotonic_arena* arena) {
    json_cursor cur{json.data(), json.data() + json.size()};
    Task obj(arena);

    // ... zero-copy parsing с минимальными аллокациями

    return obj;
}
```

### Validation

```cpp
std::optional<validation_error> validate_CreateTaskRequest(const CreateTaskRequest& obj) {
    if (obj.title.view().empty()) {
        return validation_error{"title", "must not be empty"};
    }
    // ... больше проверок из OpenAPI spec
    return std::nullopt;
}
```

### Router Bindings

```cpp
const router& make_router(api_handler& handler) {
    static route_entry routes[] = {
        {method::get, path_pattern::from_literal<"/tasks">(), ...},
        {method::post, path_pattern::from_literal<"/tasks">(), ...},
        // ... остальные routes
    };
    static router r(routes);
    return r;
}
```

## 📚 Связанная документация

- [HTTP Server Abstraction](../../../docs/HTTP_SERVER.md) - Документация по `katana::http::server`
- [OpenAPI Codegen](../../../docs/OPENAPI.md) - Полное руководство по кодогенератору
- [Router Documentation](../../../docs/ROUTER.md) - Детали роутинга и middleware
- [Architecture](../../../ARCHITECTURE.md) - Архитектурные принципы фреймворка

## 💡 Ключевые выводы

1. **Минимум boilerplate**: Server abstraction сокращает код в 2 раза
2. **Type safety**: Compile-time проверки через generated interfaces
3. **Zero-copy**: Минимум аллокаций благодаря arena и string_view
4. **Production ready**: Полная валидация, error handling, graceful shutdown
5. **Developer friendly**: Fluent API, автоматическая генерация, простая интеграция
