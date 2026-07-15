# Using KATANA as a git submodule

KATANA is built to be vendored into your project and consumed with `add_subdirectory`.

```sh
git submodule add <katana-repo-url> third_party/katana
```

```cmake
# your CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(myservice CXX)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(third_party/katana)   # builds katana_core + katana_gen; tests/examples are OFF

# Generate code from your contracts (one helper call per namespaced contract):
katana_add_openapi(catalog
    CONTRACT ${CMAKE_CURRENT_SOURCE_DIR}/contracts/catalog.yaml
    NAMESPACE catalog)                 # NAMESPACE is required if you link >1 contract per binary
katana_add_sql(catalog_sql
    DIR ${CMAKE_CURRENT_SOURCE_DIR}/sql/catalog
    NAMESPACE catalog)

add_executable(myservice src/main.cpp)
target_link_libraries(myservice PRIVATE katana::core catalog catalog_sql)
```

In code, include the generated headers by their fixed names (quoted):

```cpp
#include "generated_dtos.hpp"            // catalog::Product, catalog::CreateProductRequest, …
#include "generated_router_bindings.hpp" // catalog::make_router, catalog::api_handler
#include "generated_sql_repository.hpp"  // catalog::generated_repository
```

## What `add_subdirectory` gives you

- `katana::core` — the runtime library (HTTP server, router, SQL/Redis, policies, observability),
  with its public include directory already on your target.
- `katana::gen` / `katana_gen` — the code generator, driven for you by the two helper functions.
- `katana_add_openapi(<name> CONTRACT <spec> [NAMESPACE <ns>] [OUTPUT <dir>] [EMIT …]
  [INLINE_NAMING …])` and `katana_add_sql(<name> DIR <dir> [NAMESPACE <ns>] …)` — each defines an
  INTERFACE target you link; it regenerates when the contract/SQL changes and puts the output dir
  on your include path.

Because tests and examples default OFF for a submodule, the parent build stays lean. Flip them on
with `-DENABLE_TESTING=ON` / `-DENABLE_EXAMPLES=ON` if you want them.

## Multiple contracts in one binary

Give each contract a distinct `NAMESPACE` and link them all — KATANA namespaces every generated
symbol (OpenAPI under `<ns>::`, SQL under `katana::sql::<ns>::`), and `katana::http::composite_router`
mounts the generated routers on one server. See `examples/services/pulse` for a worked example.
