# Pulse — reference fullstack template. A standalone CMake project that consumes KATANA and shows
# the whole path: two OpenAPI contracts + two SQL catalogs → generated C++ (DTOs, JSON, router, SQL
# repositories, Row↔DTO bridge) and a generated TypeScript client for the web/ frontend.
#
# Build (in-repo): cmake -S . -B build && cmake --build build     (auto-finds the KATANA repo)
# or point at a checkout:   cmake -S . -B build -DKATANA_ROOT=/path/to/KATANA
cmake_minimum_required(VERSION 3.20)
project(pulse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer an installed KATANA package; otherwise build the in-repo copy via add_subdirectory. Either
# way we get katana::core, katana::gen, and the katana_add_openapi/sql/typescript helpers.
find_package(katana QUIET)
if(NOT katana_FOUND)
    set(KATANA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." CACHE PATH "Path to the KATANA repo root")
    if(NOT EXISTS "${KATANA_ROOT}/CMakeLists.txt")
        message(FATAL_ERROR "KATANA not found: install it, or set -DKATANA_ROOT=<repo root>.")
    endif()
    set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(ENABLE_BENCHMARKS OFF CACHE BOOL "" FORCE)
    set(ENABLE_TOOLS ON CACHE BOOL "" FORCE)
    add_subdirectory("${KATANA_ROOT}" "${CMAKE_BINARY_DIR}/_deps/katana" EXCLUDE_FROM_ALL)
endif()

set(GEN "${CMAKE_CURRENT_SOURCE_DIR}/generated")

# --- Catalog contract: DTOs/JSON/router + SQL repository + Row↔DTO bridge, namespace `catalog`. ---
katana_add_openapi(pulse_catalog_api
    CONTRACT ${CMAKE_CURRENT_SOURCE_DIR}/catalog.yaml
    NAMESPACE catalog
    OUTPUT ${GEN}/catalog)
katana_add_sql(pulse_catalog_sql
    DIR ${CMAKE_CURRENT_SOURCE_DIR}/sql/catalog
    NAMESPACE catalog
    OUTPUT ${GEN}/catalog
    BRIDGE_OPENAPI ${CMAKE_CURRENT_SOURCE_DIR}/catalog.yaml)

# --- Analytics contract, namespace `analytics`. ---
katana_add_openapi(pulse_analytics_api
    CONTRACT ${CMAKE_CURRENT_SOURCE_DIR}/analytics.yaml
    NAMESPACE analytics
    OUTPUT ${GEN}/analytics)
katana_add_sql(pulse_analytics_sql
    DIR ${CMAKE_CURRENT_SOURCE_DIR}/sql/analytics
    NAMESPACE analytics
    OUTPUT ${GEN}/analytics
    BRIDGE_OPENAPI ${CMAKE_CURRENT_SOURCE_DIR}/analytics.yaml)

# --- Generated TypeScript clients for the frontend (web/lib/<contract>/generated_client.ts). ---
katana_add_typescript(pulse_catalog_client
    CONTRACT ${CMAKE_CURRENT_SOURCE_DIR}/catalog.yaml
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/web/lib/catalog)
katana_add_typescript(pulse_analytics_client
    CONTRACT ${CMAKE_CURRENT_SOURCE_DIR}/analytics.yaml
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/web/lib/analytics)

add_executable(pulse main.cpp)
target_link_libraries(pulse PRIVATE
    katana::core
    pulse_catalog_api pulse_catalog_sql
    pulse_analytics_api pulse_analytics_sql)
# main.cpp includes "generated/<contract>/...", so the project root is on the include path.
target_include_directories(pulse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
