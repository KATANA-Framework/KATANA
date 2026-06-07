# KATANA OpenAPI

Arena-backed OpenAPI 3.x parser and AST for generating compile-time APIs.

## Overview

The KATANA OpenAPI loader parses OpenAPI 3.x specifications (JSON/YAML) and builds an arena-backed AST, which is used to generate type-safe routes, DTOs, validators, and serializers.

### Key features

- ✅ **Arena-backed** — all memory is allocated from a monotonic arena
- ✅ **JSON and YAML** — both formats are supported
- ✅ **OpenAPI 3.x** — spec version validation
- ✅ **Rich schema parsing** — objects, arrays, strings, numbers, enums, refs
- ✅ **Validation constraints** — minLength, pattern, required, nullable, etc.
- ✅ **Move-only semantics** — no accidental copies
- ✅ **Zero dependencies** — lightweight parser with no heavy libraries
- ✅ **Content negotiation aware bindings** — Content-Type/Accept, optional parameters, meaningful names for inline schemas
- ✅ **AST dump** — includes schemas for debugging (openapi_ast.json)

---

## Current Status

**Stage 2: OpenAPI Parser + Code Generator** ✅

**Parsing implemented:**
- ✅ Parsing OpenAPI versions (3.0.x, 3.1.x) with validation
- ✅ Parsing info (title, version)
- ✅ Parsing paths and operations (GET, POST, PUT, DELETE, PATCH)
- ✅ Parsing parameters (path, query, header, cookie) with style/explode
- ✅ Parsing requestBody and responses
- ✅ Full schema parsing (type, format, properties, items, enum as vector)
- ✅ `$ref` resolution with cycle detection
- ✅ `allOf` merge with a "most restrictive wins" strategy
- ✅ Validation constraints (min/max, minLength/maxLength, pattern, required, nullable, uniqueItems, enum)
- ✅ Specification validation (operationId uniqueness, HTTP codes)
- ✅ YAML support via katana::serde::yaml_to_json
- ℹ️ YAML parser — lightweight; both block and flow (inline) style are supported and can be mixed (e.g. `- { name: q1, in: query, schema: { type: integer } }`); anchors/tags are not supported, and duplicate keys are treated as an error with a line number

**Code generation implemented:**
- ✅ DTO codegen with pmr arena allocators
- ✅ JSON parsers codegen via katana::serde (zero-copy where possible)
- ✅ Constexpr route table codegen for integration with the router
- ✅ katana_gen CLI with --emit, --alloc, --layer flags
- ✅ Handler interfaces + router bindings: optional parameters, name sanitization, Content-Type/Accept negotiation, body parsing by media type
- ✅ Unified validation errors (`katana/core/validation.hpp`) and compile-time metadata in DTOs; validation uses the same error codes across all artifacts
- ✅ Descriptor-based JSON parser (`katana/core/json_parser.hpp`) for generated parsers: zero-alloc, arena input/output, min/max length and array checks

---

## Quick Start

### Loading an OpenAPI specification

```cpp
#include "katana/core/openapi_loader.hpp"
#include "katana/core/openapi_ast.hpp"
#include "katana/core/arena.hpp"

using namespace katana::openapi;

// JSON specification
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

// Parsing
monotonic_arena arena;
auto result = load_from_string(spec, arena);

if (!result) {
    std::cerr << "Parse error: " << result.error().message() << "\n";
    return 1;
}

document& doc = *result;

// Accessing the data
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

### Loading from a file

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
    arena_vector<schema> schemas;           // All schemas (inline + components)
    arena_vector<path_item> paths;          // All paths
    arena_string<> openapi_version;         // "3.x"
    arena_string<> info_title;              // API title
    arena_string<> info_version;            // API version

    // Methods
    schema& add_schema(std::string_view name);
    path_item& add_path(std::string_view path);
    schema& add_inline_schema();
};
```

**Example:**
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

**Example:**
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
    param_location in;                 // where the parameter is located
    bool required;                     // required?
    const schema* type;                // parameter type
    arena_string<> description;        // description
};
```

**Example:**
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
    const schema* type;          // Schema for the content (nullable)
};
```

### Request Body

```cpp
struct request_body {
    arena_string<> description;         // Description
    arena_vector<media_type> content;   // List of media types

    const media_type* first_media() const; // helper, returns the first media type or nullptr
};
```

**Example:**
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
    bool is_default;                    // true if this is the "default" branch
    arena_string<> description;         // "User found"
    arena_vector<media_type> content;   // Media types for the response

    const media_type* first_media() const; // helper
};
```

**Example:**
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
    schema_kind kind;                  // Schema type
    arena_string<> name;               // Name (for components/schemas)
    arena_string<> format;             // "int64", "date-time", "email", etc.
    arena_string<> ref;                // "$ref" path (not yet resolved)
    arena_string<> description;        // Description
    arena_string<> pattern;            // Regex pattern for string
    arena_string<> discriminator;      // For polymorphism
    arena_string<> default_value;      // Default value

    // Array-specific
    const schema* items;               // Array element type

    // Object-specific
    arena_vector<property> properties; // Object properties
    bool additional_properties_allowed;
    const schema* additional_properties;

    // Validation
    bool nullable;                     // Can be null
    bool deprecated;                   // Deprecated field
    bool unique_items;                 // For arrays
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

    bool required;                     // For properties
    bool is_ref;                       // Is this a $ref?
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

**Accessing properties:**
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

**Accessing items:**
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

**Accessing constraints:**
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

**Accessing constraints:**
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

The OpenAPI loader supports both JSON and YAML:

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

**Loading:**
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
    openapi_parse_error,      // Invalid JSON/YAML
    openapi_invalid_spec,     // Unsupported OpenAPI version
    // ...
};
```

### Handling errors

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

All AST memory is allocated from a monotonic arena:

```cpp
monotonic_arena arena;
auto result = load_from_string(spec, arena);

if (result) {
    document& doc = *result;

    // All strings and vectors use the arena
    // doc.info_title — arena_string
    // doc.paths — arena_vector
    // schema->properties — arena_vector
}

// Once the scope exits, the arena frees everything at once
```

**Benefits:**
- ✅ Fast allocation (bump allocator)
- ✅ No memory fragmentation
- ✅ Free the entire AST in a single operation
- ✅ Cache-friendly (data locality)

---

## Usage Patterns

### 1. Inspecting a specification

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

### 2. Extracting schemas

```cpp
// Find all object schemas
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

### 3. Validating parameters

```cpp
for (const auto& path : doc->paths) {
    for (const auto& op : path.operations) {
        for (const auto& param : op.parameters) {
            if (param.required && param.in == param_location::path) {
                std::cout << "Required path param: " << param.name << "\n";

                // You can generate validators
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

More tests in `test/unit/test_openapi_ast.cpp`.

---

## Limitations (Current Implementation)

### Not supported (yet):

1. **Security schemes** — not part of the runtime/codegen contract
2. **Servers** — not used by the generator or runtime
3. **Examples / example payloads** — not materialized into generated artifacts
4. **External docs** — ignored by loader/codegen
5. **Full OpenAPI coverage** — the focus stays on the runtime-critical subset, not a full mirror of the spec

---

## x-katana-\* Extensions

Katana supports vendor extensions with the `x-katana-` prefix at the OpenAPI operation level.
The extensions are parsed by the loader and passed into the AST, but at the current stage (Stage 3)
they **do not affect runtime behaviour** — they are declarative.

### Supported extensions

| Extension | Value type | Loader support | Codegen output | Runtime |
|---|---|---|---|---|
| `x-katana-cache` | `string` / `boolean` | ✅ parsed | comment `// @cache:` | Stage 5 |
| `x-katana-alloc` | `string` / `number` | ✅ parsed | comment `// @alloc:` | Stage 5 |
| `x-katana-rate-limit` | `string` | ✅ parsed | comment `// @rate-limit:` | Stage 5 |

**Object values** (e.g. `x-katana-cache: { ttl: 10s }`) are **not supported** and will be
silently ignored by the loader.

### Accepted value formats

- `x-katana-cache`: a string with a TTL (`"300s"`, `"5m"`) or a boolean (`true`/`false`).
- `x-katana-alloc`: a string (`"pool"`, `"arena"`, `"heap"`) or a number (`4096`).
- `x-katana-rate-limit`: a string of the form `"<count>/<unit>"` (`"100/s"`, `"1000/m"`).

### Codegen behavior

Extension values are emitted into the generated handler interface as comments:

```cpp
// GET /users/{id}
// @cache: 5m
// @alloc: pool
// @rate-limit: 100/s
katana::http::task<response> get_user(const get_user_request& req);
```

These comments serve as documentation and can be consumed by external tools.
Runtime enforcement is planned for Stage 5.

Full reference: [X_KATANA_EXTENSIONS.md](X_KATANA_EXTENSIONS.md).

---

## Roadmap

Stage 2 is closed. Further work on the OpenAPI layer now happens within the repository's general roadmap stages.

### Near-term OpenAPI tasks

- [x] conformance harness for generated endpoints against OpenAPI fixtures (see [CONFORMANCE.md](CONFORMANCE.md))
- [x] media type registry integration for JSON/CBOR/MessagePack
- [x] a clear specification of the supported `x-katana-*` extensions (see [X_KATANA_EXTENSIONS.md](X_KATANA_EXTENSIONS.md))
- [x] stabilization of the runtime/codegen test path in canonical Linux/WSL CI

### Later, after runtime stabilization

- [ ] integration of the generated contract with the SQL layer
- [ ] integration of policy annotations (`cache`, `idempotency`, `rate-limit`) with the runtime
- [ ] compatibility tooling and SDK generation, only after core milestones stabilize

---

## Best Practices

### 1. Use the arena for all operations

```cpp
// ✅ Good
monotonic_arena arena;
auto doc = load_from_string(spec, arena);

// ❌ Bad (arena on the stack will be destroyed)
{
    monotonic_arena arena;
    auto doc = load_from_string(spec, arena);
}
// doc holds dangling pointers!
```

### 2. Check nullable fields

```cpp
// Schema fields may be nullptr
if (op.body) {
    if (const auto* media = op.body->first_media()) {
        std::cout << "Body: " << media->content_type << "\n";
    }
}

if (schema->items) {
    std::cout << "Array item type: " << schema->items->kind << "\n";
}
```

### 3. Validate the OpenAPI version

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

Full examples in `test/unit/test_openapi_ast.cpp`:

- Building the AST by hand
- Parsing JSON specifications
- Parsing YAML specifications
- Walking schemas and operations
- Validating constraints

---

## See Also

- [ROUTER.md](ROUTER.md) — HTTP Router documentation
- [ARCHITECTURE.md](../ARCHITECTURE.md) — Overall architecture
- [OpenAPI 3.0 Specification](https://spec.openapis.org/oas/v3.0.3)
- [OpenAPI 3.1 Specification](https://spec.openapis.org/oas/v3.1.0)
