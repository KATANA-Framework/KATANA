# KATANA OpenAPI

Arena-backed OpenAPI 3.x parser и AST для генерации compile-time API.

## Обзор

KATANA OpenAPI loader парсит OpenAPI 3.x спецификации (JSON/YAML) и строит arena-backed AST, который используется для кодогенерации типобезопасных роутов, DTO, валидаторов и сериализаторов.

### Ключевые особенности

- ✅ **Arena-backed** — вся память выделяется из монотонной арены
- ✅ **JSON и YAML** — поддержка обоих форматов
- ✅ **OpenAPI 3.x** — валидация версии спецификации
- ✅ **Rich schema parsing** — objects, arrays, strings, numbers, enums, refs
- ✅ **Validation constraints** — minLength, pattern, required, nullable, etc.
- ✅ **Move-only semantics** — нет случайных копирований
- ✅ **Zero dependencies** — легковесный парсер без тяжёлых библиотек
- ✅ **Content negotiation aware bindings** — Content-Type/Accept, optional параметры, осмысленные имена inline-схем
- ✅ **AST dump** — включает схемы для отладки (openapi_ast.json)

---

## Current Status

**Stage 2: OpenAPI Parser + Code Generator** ✅

**Парсинг реализовано:**
- ✅ Парсинг OpenAPI версий (3.0.x, 3.1.x) с валидацией
- ✅ Парсинг info (title, version)
- ✅ Парсинг paths и operations (GET, POST, PUT, DELETE, PATCH)
- ✅ Парсинг parameters (path, query, header, cookie) с style/explode
- ✅ Парсинг requestBody и responses
- ✅ Full schema parsing (type, format, properties, items, enum as vector)
- ✅ `$ref` resolution с cycle detection
- ✅ `allOf` merge с "most restrictive wins" strategy
- ✅ Validation constraints (min/max, minLength/maxLength, pattern, required, nullable, uniqueItems, enum)
- ✅ Specification validation (operationId uniqueness, HTTP codes)
- ✅ YAML support через katana::serde::yaml_to_json
- ℹ️ YAML parser — лёгкий, без anchors/tags; дубликаты ключей считаются ошибкой с указанием строки (для сложных YAML стоит использовать JSON или плоский YAML)

**Кодогенерация реализована:**
- ✅ DTO codegen с pmr arena allocators
- ✅ JSON parsers codegen через katana::serde (zero-copy где возможно)
- ✅ Constexpr route table codegen для интеграции с router
- ✅ katana_gen CLI с флагами --emit, --alloc, --layer
- ✅ Handler interfaces + router bindings: optional параметры, санитизация имён, Content-Type/Accept negotiation, парсинг тела по медиатипу
- ✅ Unified validation errors (`katana/core/validation.hpp`) и compile-time metadata в DTO; валидация использует одни и те же коды ошибок во всех артефактах
- ✅ Descriptor-based JSON parser (`katana/core/json_parser.hpp`) для сгенерированных парсеров: zero-alloc, аренный ввод/вывод, проверки min/max длины и массивов

---

## Quick Start

### Загрузка OpenAPI спецификации

```cpp
#include "katana/core/openapi_loader.hpp"
#include "katana/core/openapi_ast.hpp"
#include "katana/core/arena.hpp"

using namespace katana::openapi;

// JSON спецификация
const std::string spec = R"({
  "openapi": "3.0.0",
  "info": {
    "title": "My API",
    "version": "1.0.0"
  },
  "paths": {
    "/users/{id}": {
      "get": {
        "operationId": "getUser",
        "parameters": [
          {
            "name": "id",
            "in": "path",
            "required": true,
            "schema": { "type": "integer" }
          }
        ],
        "responses": {
          "200": {
            "description": "User found",
            "content": {
              "application/json": {
                "schema": {
                  "type": "object",
                  "properties": {
                    "id": { "type": "integer" },
                    "name": { "type": "string" }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
})";

// Парсинг
monotonic_arena arena;
auto result = load_from_string(spec, arena);

if (!result) {
    std::cerr << "Parse error: " << result.error().message() << "\n";
    return 1;
}

document& doc = *result;

// Доступ к данным
std::cout << "API: " << doc.info_title << " v" << doc.info_version << "\n";
std::cout << "Paths: " << doc.paths.size() << "\n";

for (const auto& path : doc.paths) {
    std::cout << "  " << path.path << "\n";
    for (const auto& op : path.operations) {
        std::cout << "    " << http::method_to_string(op.method)
                  << " " << op.operation_id << "\n";
    }
}
```

### Загрузка из файла

```cpp
auto result = load_from_file("api/openapi.yaml", arena);

if (!result) {
    std::cerr << "Failed to load: " << result.error().message() << "\n";
    return 1;
}

document& doc = *result;
```

---

## OpenAPI Document Structure

### Document

```cpp
struct document {
    monotonic_arena* arena_;
    arena_vector<schema> schemas;           // Все схемы (inline + components)
    arena_vector<path_item> paths;          // Все пути
    arena_string<> openapi_version;         // "3.x"
    arena_string<> info_title;              // API title
    arena_string<> info_version;            // API version

    // Методы
    schema& add_schema(std::string_view name);
    path_item& add_path(std::string_view path);
    schema& add_inline_schema();
};
```

**Пример:**
```cpp
document doc(arena);
doc.openapi_version;  // "3.x"
doc.info_title;       // "My API"
doc.info_version;     // "1.0.0"
```

---

## Paths and Operations

### Path Item

```cpp
struct path_item {
    arena_string<> path;                   // "/users/{id}"
    arena_vector<operation> operations;    // GET, POST, etc.
};
```

### Operation

```cpp
struct operation {
    http::method method;                   // GET, POST, PUT, DELETE, etc.
    arena_string<> operation_id;           // "getUser"
    arena_string<> summary;                // "Get user by ID"
    arena_string<> description;            // Long description
    arena_vector<parameter> parameters;    // Path/query/header params
    request_body* body;                    // Request body (nullable)
    arena_vector<response> responses;      // 200, 404, etc.
};
```

**Пример:**
```cpp
for (const auto& path : doc.paths) {
    std::cout << "Path: " << path.path << "\n";

    for (const auto& op : path.operations) {
        std::cout << "  " << http::method_to_string(op.method) << " "
                  << op.operation_id << "\n";

        // Parameters
        for (const auto& param : op.parameters) {
            std::cout << "    Param: " << param.name
                      << " (required: " << param.required << ")\n";
        }

        // Request body
        if (op.body) {
            if (const auto* media = op.body->first_media()) {
                std::cout << "    Body: " << media->content_type << "\n";
            }
        }

        // Responses
        for (const auto& resp : op.responses) {
            std::cout << "    Response " << resp.status << ": "
                      << resp.description << "\n";
        }
    }
}
```

---

## Parameters

### Parameter

```cpp
enum class param_location : uint8_t {
    path,    // /users/{id}
    query,   // /users?page=1
    header,  // Authorization: Bearer token
    cookie   // Cookie: session=xyz
};

struct parameter {
    arena_string<> name;               // "id", "page", etc.
    param_location in;                 // где находится параметр
    bool required;                     // обязательный?
    const schema* type;                // тип параметра
    arena_string<> description;        // описание
};
```

**Пример:**
```cpp
for (const auto& param : op.parameters) {
    std::cout << "Parameter: " << param.name << "\n";
    std::cout << "  Location: ";
    switch (param.in) {
        case param_location::path:   std::cout << "path\n"; break;
        case param_location::query:  std::cout << "query\n"; break;
        case param_location::header: std::cout << "header\n"; break;
        case param_location::cookie: std::cout << "cookie\n"; break;
    }
    std::cout << "  Required: " << (param.required ? "yes" : "no") << "\n";
}
```

---

## Request Body

### Media Type

```cpp
struct media_type {
    arena_string<> content_type; // "application/json"
    const schema* type;          // Схема для контента (nullable)
};
```

### Request Body

```cpp
struct request_body {
    arena_string<> description;         // Описание
    arena_vector<media_type> content;   // Список media types

    const media_type* first_media() const; // helper, возвращает первый media type или nullptr
};
```

**Пример:**
```cpp
if (op.body) {
    std::cout << "Request Body:\n";
    if (const auto* media = op.body->first_media()) {
        std::cout << "  Content-Type: " << media->content_type << "\n";
        std::cout << "  Description: " << op.body->description << "\n";

        if (media->type) {
            std::cout << "  Schema: " << media->type->kind << "\n";
        }
    }
}
```

---

## Responses

### Response

```cpp
struct response {
    int status;                         // 200, 404, etc.
    bool is_default;                    // true если это "default" ветка
    arena_string<> description;         // "User found"
    arena_vector<media_type> content;   // Media types для ответа

    const media_type* first_media() const; // helper
};
```

**Пример:**
```cpp
for (const auto& resp : op.responses) {
    std::cout << "Response " << resp.status << ":\n";
    std::cout << "  Description: " << resp.description << "\n";

    for (const auto& media : resp.content) {
        std::cout << "  Content-Type: " << media.content_type << "\n";
        if (media.type) {
            std::cout << "    Schema kind: " << static_cast<int>(media.type->kind) << "\n";
        }
    }
}
```

---

## Schemas

### Schema Types

```cpp
enum class schema_kind : uint8_t {
    object,      // {"type": "object"}
    array,       // {"type": "array"}
    string,      // {"type": "string"}
    integer,     // {"type": "integer"}
    number,      // {"type": "number"}
    boolean,     // {"type": "boolean"}
    null_type    // {"type": "null"}
};
```

### Schema

```cpp
struct schema {
    schema_kind kind;                  // Тип схемы
    arena_string<> name;               // Имя (для components/schemas)
    arena_string<> format;             // "int64", "date-time", "email", etc.
    arena_string<> ref;                // "$ref" path (пока не resolves)
    arena_string<> description;        // Описание
    arena_string<> pattern;            // Regex pattern для string
    arena_string<> discriminator;      // Для polymorphism
    arena_string<> default_value;      // Значение по умолчанию

    // Array-specific
    const schema* items;               // Тип элементов массива

    // Object-specific
    arena_vector<property> properties; // Свойства объекта
    bool additional_properties_allowed;
    const schema* additional_properties;

    // Validation
    bool nullable;                     // Может быть null
    bool deprecated;                   // Устаревшее поле
    bool unique_items;                 // Для массивов
    std::optional<double> minimum;
    std::optional<double> maximum;
    std::optional<double> exclusive_minimum;
    std::optional<double> exclusive_maximum;
    std::optional<double> multiple_of;
    std::optional<size_t> min_length;
    std::optional<size_t> max_length;
    std::optional<size_t> min_items;
    std::optional<size_t> max_items;
    arena_string<> enum_values;        // Semicolon-separated

    bool required;                     // Для properties
    bool is_ref;                       // Это $ref?
};
```

### Object Schema

```yaml
type: object
properties:
  id:
    type: integer
    format: int64
  name:
    type: string
    minLength: 1
    maxLength: 100
  email:
    type: string
    format: email
required:
  - id
  - name
```

**Доступ к properties:**
```cpp
if (schema->kind == schema_kind::object) {
    for (const auto& prop : schema->properties) {
        std::cout << "Property: " << prop.name << "\n";
        std::cout << "  Required: " << (prop.required ? "yes" : "no") << "\n";

        if (prop.type) {
            std::cout << "  Type: ";
            switch (prop.type->kind) {
                case schema_kind::string:  std::cout << "string\n"; break;
                case schema_kind::integer: std::cout << "integer\n"; break;
                // etc.
            }

            // Validation constraints
            if (prop.type->min_length) {
                std::cout << "  Min length: " << *prop.type->min_length << "\n";
            }
            if (prop.type->max_length) {
                std::cout << "  Max length: " << *prop.type->max_length << "\n";
            }
            if (!prop.type->pattern.empty()) {
                std::cout << "  Pattern: " << prop.type->pattern << "\n";
            }
        }
    }
}
```

### Array Schema

```yaml
type: array
items:
  type: string
minItems: 1
maxItems: 10
uniqueItems: true
```

**Доступ к items:**
```cpp
if (schema->kind == schema_kind::array) {
    std::cout << "Array of:\n";

    if (schema->items) {
        std::cout << "  Item type: ";
        switch (schema->items->kind) {
            case schema_kind::string:  std::cout << "string\n"; break;
            case schema_kind::object:  std::cout << "object\n"; break;
            // etc.
        }
    }

    if (schema->min_items) {
        std::cout << "  Min items: " << *schema->min_items << "\n";
    }
    if (schema->max_items) {
        std::cout << "  Max items: " << *schema->max_items << "\n";
    }
    if (schema->unique_items) {
        std::cout << "  Unique items: yes\n";
    }
}
```

### String Schema

```yaml
type: string
format: email
minLength: 5
maxLength: 100
pattern: "^[a-z]+@[a-z]+\\.[a-z]+$"
enum: ["admin", "user", "guest"]
```

**Доступ к constraints:**
```cpp
if (schema->kind == schema_kind::string) {
    if (!schema->format.empty()) {
        std::cout << "Format: " << schema->format << "\n";
    }
    if (schema->min_length) {
        std::cout << "Min length: " << *schema->min_length << "\n";
    }
    if (schema->max_length) {
        std::cout << "Max length: " << *schema->max_length << "\n";
    }
    if (!schema->pattern.empty()) {
        std::cout << "Pattern: " << schema->pattern << "\n";
    }
    if (!schema->enum_values.empty()) {
        std::cout << "Enum: " << schema->enum_values << "\n";
        // enum_values is semicolon-separated: "admin;user;guest"
    }
}
```

### Number/Integer Schema

```yaml
type: integer
format: int64
minimum: 0
maximum: 100
multipleOf: 5
```

**Доступ к constraints:**
```cpp
if (schema->kind == schema_kind::integer || schema->kind == schema_kind::number) {
    if (schema->minimum) {
        std::cout << "Minimum: " << *schema->minimum << "\n";
    }
    if (schema->maximum) {
        std::cout << "Maximum: " << *schema->maximum << "\n";
    }
    if (schema->multiple_of) {
        std::cout << "Multiple of: " << *schema->multiple_of << "\n";
    }
}
```

---

## YAML Support

OpenAPI loader поддерживает и JSON, и YAML:

```yaml
openapi: 3.0.0
info:
  title: My API
  version: 1.0.0
paths:
  /users/{id}:
    get:
      operationId: getUser
      parameters:
        - name: id
          in: path
          required: true
          schema:
            type: integer
      responses:
        '200':
          description: User found
```

**Загрузка:**
```cpp
const std::string yaml_spec = R"(
openapi: 3.0.0
info:
  title: My API
  version: 1.0
paths: {}
)";

monotonic_arena arena;
auto result = load_from_string(yaml_spec, arena);

if (result) {
    std::cout << "Loaded YAML: " << result->info_title << "\n";
}
```

---

## Error Handling

### Error Codes

```cpp
enum class error_code {
    openapi_parse_error,      // Невалидный JSON/YAML
    openapi_invalid_spec,     // Неподдерживаемая версия OpenAPI
    // ...
};
```

### Обработка ошибок

```cpp
auto result = load_from_string(spec, arena);

if (!result) {
    auto error = result.error();
    std::cerr << "Error: " << error.message() << "\n";

    switch (static_cast<error_code>(error.value())) {
        case error_code::openapi_parse_error:
            std::cerr << "Invalid JSON/YAML syntax\n";
            break;
        case error_code::openapi_invalid_spec:
            std::cerr << "Unsupported OpenAPI version (expected 3.x)\n";
            break;
        default:
            std::cerr << "Unknown error\n";
    }

    return 1;
}
```

---

## Arena Memory Management

Вся память AST выделяется из монотонной арены:

```cpp
monotonic_arena arena;
auto result = load_from_string(spec, arena);

if (result) {
    document& doc = *result;

    // Все строки и векторы используют arena
    // doc.info_title — arena_string
    // doc.paths — arena_vector
    // schema->properties — arena_vector
}

// После выхода из scope, arena освобождает всё сразу
```

**Преимущества:**
- ✅ Быстрая аллокация (bump allocator)
- ✅ Нет фрагментации памяти
- ✅ Освобождение всего AST одной операцией
- ✅ Cache-friendly (локальность данных)

---

## Usage Patterns

### 1. Инспекция спецификации

```cpp
monotonic_arena arena;
auto doc = load_from_file("api.yaml", arena);

if (!doc) {
    std::cerr << "Failed to load\n";
    return;
}

std::cout << "API: " << doc->info_title << "\n";
std::cout << "Endpoints:\n";

for (const auto& path : doc->paths) {
    for (const auto& op : path.operations) {
        std::cout << "  " << http::method_to_string(op.method)
                  << " " << path.path << "\n";
    }
}
```

### 2. Извлечение схем

```cpp
// Найти все объектные схемы
for (const auto& schema : doc->schemas) {
    if (schema.kind == schema_kind::object && !schema.name.empty()) {
        std::cout << "Schema: " << schema.name << "\n";

        for (const auto& prop : schema.properties) {
            std::cout << "  - " << prop.name
                      << (prop.required ? " (required)" : "") << "\n";
        }
    }
}
```

### 3. Валидация параметров

```cpp
for (const auto& path : doc->paths) {
    for (const auto& op : path.operations) {
        for (const auto& param : op.parameters) {
            if (param.required && param.in == param_location::path) {
                std::cout << "Required path param: " << param.name << "\n";

                // Можно генерировать валидаторы
                if (param.type && param.type->kind == schema_kind::integer) {
                    std::cout << "  → validate as integer\n";
                }
            }
        }
    }
}
```

---

## Testing

### Unit Tests

```cpp
TEST(OpenAPILoader, AcceptsVersionHint) {
    const std::string spec = R"({
      "openapi": "3.1.0",
      "info": { "title": "stub", "version": "1.0.0" },
      "paths": {}
    })";

    monotonic_arena arena;
    auto res = openapi::load_from_string(spec, arena);

    ASSERT_TRUE(res);
    EXPECT_EQ(res->openapi_version, "3.x");
    EXPECT_EQ(res->info_title, "stub");
    EXPECT_EQ(res->info_version, "1.0.0");
}
```

Больше тестов в `test/unit/test_openapi_ast.cpp`.

---

## Limitations (Current Implementation)

### Не поддерживается (пока):

1. **`$ref` resolution** — ссылки не разрешаются, сохраняются как строки
2. **`allOf`, `oneOf`, `anyOf`** — схемы composition пока shallow
3. **Components** — `components/schemas` не материализуются отдельно
4. **Security schemes** — не парсятся
5. **Servers** — не парсятся
6. **Examples** — не парсятся
7. **External docs** — не парсятся

Эти фичи будут добавлены в следующих этапах Stage 2.

---

## Roadmap

### Stage 2.3: Schema Resolution

- [ ] `$ref` resolution для schemas
- [ ] Полная материализация schema tree
- [ ] `allOf`, `oneOf`, `anyOf` composition
- [ ] Components/schemas namespace

### Stage 2.4: DTO Generation

- [ ] C++ struct codegen из schemas
- [ ] `std::pmr` allocators
- [ ] `std::string_view` для zero-copy
- [ ] Enum → `enum class`

### Stage 2.5: Validation Generation

- [ ] Required field validators
- [ ] String constraints (minLength, pattern, format)
- [ ] Number constraints (min, max, multipleOf)
- [ ] Array constraints (minItems, uniqueItems)

### Stage 2.6: Route Table Codegen

- [ ] Compile-time route table из paths
- [ ] Binding operation → handler signature
- [ ] Type-safe parameter extraction

---

## Best Practices

### 1. Используйте arena для всех операций

```cpp
// ✅ Good
monotonic_arena arena;
auto doc = load_from_string(spec, arena);

// ❌ Bad (arena на стеке будет destroyed)
{
    monotonic_arena arena;
    auto doc = load_from_string(spec, arena);
}
// doc содержит dangling pointers!
```

### 2. Проверяйте nullable поля

```cpp
// Schema fields могут быть nullptr
if (op.body) {
    if (const auto* media = op.body->first_media()) {
        std::cout << "Body: " << media->content_type << "\n";
    }
}

if (schema->items) {
    std::cout << "Array item type: " << schema->items->kind << "\n";
}
```

### 3. Валидируйте OpenAPI версию

```cpp
auto doc = load_from_string(spec, arena);

if (!doc) {
    if (doc.error() == make_error_code(error_code::openapi_invalid_spec)) {
        std::cerr << "Unsupported OpenAPI version (expected 3.x)\n";
    }
    return 1;
}
```

---

## Examples

Полные примеры в `test/unit/test_openapi_ast.cpp`:

- Построение AST вручную
- Парсинг JSON спецификаций
- Парсинг YAML спецификаций
- Обход schemas и operations
- Валидация constraints

---

## See Also

- [ROUTER.md](ROUTER.md) — HTTP Router документация
- [ARCHITECTURE.md](../ARCHITECTURE.md) — Общая архитектура
- [OpenAPI 3.0 Specification](https://spec.openapis.org/oas/v3.0.3)
- [OpenAPI 3.1 Specification](https://spec.openapis.org/oas/v3.1.0)
