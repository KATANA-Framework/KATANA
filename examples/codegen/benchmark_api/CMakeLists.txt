cmake_minimum_required(VERSION 3.20)

# Benchmark API — unified service exercising all katana_gen code paths

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_dtos.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_json.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_routes.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_handlers.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_validators.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_router_bindings.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_openapi_package.hpp
    COMMAND ${CMAKE_BINARY_DIR}/katana_gen openapi
            -i ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml
            -o ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi
            --emit all
            --inline-naming operation
    DEPENDS
        api.yaml
        katana_gen
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating Benchmark API code from OpenAPI spec"
    VERBATIM
)

add_custom_target(benchmark_api_generate
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_dtos.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_json.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_routes.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_handlers.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_validators.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_router_bindings.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/generated_openapi_package.hpp
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql/generated_sql_models.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql/generated_sql_repository.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql/generated_sql_package.hpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql
    COMMAND ${CMAKE_BINARY_DIR}/katana_gen sql
            -i ${CMAKE_CURRENT_SOURCE_DIR}/sql
            -o ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql
    DEPENDS
        katana_gen
        ${CMAKE_CURRENT_SOURCE_DIR}/sql/create_item.sql
        ${CMAKE_CURRENT_SOURCE_DIR}/sql/delete_item.sql
        ${CMAKE_CURRENT_SOURCE_DIR}/sql/get_item.sql
        ${CMAKE_CURRENT_SOURCE_DIR}/sql/list_items_page.sql
        ${CMAKE_CURRENT_SOURCE_DIR}/sql/update_item.sql
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating Benchmark API SQL repository"
    VERBATIM
)

add_custom_target(benchmark_api_sql_generate
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql/generated_sql_models.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql/generated_sql_repository.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql/generated_sql_package.hpp
)

add_executable(benchmark_api
    main.cpp
    demo_service.cpp
)
add_dependencies(benchmark_api benchmark_api_generate benchmark_api_sql_generate)

target_link_libraries(benchmark_api PRIVATE katana_core)

target_include_directories(benchmark_api PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml
    ${CMAKE_CURRENT_BINARY_DIR}/api.yaml
    COPYONLY
)

message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
message(STATUS "Configuring Benchmark API Example")
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
message(STATUS "  OpenAPI Spec:    ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml")
message(STATUS "  Generated Code:  ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi/")
message(STATUS "  Generated SQL:   ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql/")
message(STATUS "  Target:          benchmark_api")
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
