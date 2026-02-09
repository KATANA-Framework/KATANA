cmake_minimum_required(VERSION 3.20)

# Generate code from OpenAPI spec
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_dtos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_validators.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_json.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_routes.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_handlers.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_router_bindings.hpp
    COMMAND katana_gen openapi
        -i ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml
        -o ${CMAKE_CURRENT_SOURCE_DIR}/generated
        --emit all
        --inline-naming operation
    DEPENDS
        katana_gen
        ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml
    COMMENT "Generating Task API code from OpenAPI spec"
    VERBATIM
)

# Task API executable
add_executable(task_api
    main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_dtos.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_validators.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_json.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_routes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_handlers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_router_bindings.hpp
)

target_include_directories(task_api PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(task_api PRIVATE katana_core)
target_compile_features(task_api PRIVATE cxx_std_20)
target_compile_options(task_api PRIVATE -Wall -Wextra -Wpedantic)
