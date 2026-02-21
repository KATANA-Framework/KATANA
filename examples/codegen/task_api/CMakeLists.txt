cmake_minimum_required(VERSION 3.20)

# Task API example — wide OpenAPI surface for codegen stress tests

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_dtos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_json.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_routes.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_handlers.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_validators.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_router_bindings.hpp
    COMMAND ${CMAKE_BINARY_DIR}/katana_gen openapi
            -i ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml
            -o ${CMAKE_CURRENT_SOURCE_DIR}/generated
            --emit all
            --inline-naming operation
    DEPENDS
        api.yaml
        katana_gen
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating Task API code from OpenAPI spec"
    VERBATIM
)

add_custom_target(task_api_generate
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_dtos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_json.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_routes.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_handlers.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_validators.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_router_bindings.hpp
)

add_executable(task_api main.cpp)
add_dependencies(task_api task_api_generate)

target_link_libraries(task_api PRIVATE katana_core)

target_include_directories(task_api PRIVATE
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
message(STATUS "Configuring Task API Example")
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
message(STATUS "  OpenAPI Spec:    ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml")
message(STATUS "  Generated Code:  ${CMAKE_CURRENT_SOURCE_DIR}/generated/")
message(STATUS "  Target:          task_api")
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
