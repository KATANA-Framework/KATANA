cmake_minimum_required(VERSION 3.20)

# This CMakeLists.txt can be used standalone or as part of the main project

# Note: This file is included from the main CMakeLists.txt
# The katana_core target is already available

# Generate code from OpenAPI spec
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
    DEPENDS
        api.yaml
        katana_gen
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating code from OpenAPI spec (api.yaml)"
    VERBATIM
)

# Custom target for code generation
add_custom_target(simple_crud_codegen_generate
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_dtos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_json.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated_routes.hpp
)

# Main executable with generated code (original version)
add_executable(simple_crud_codegen main_codegen.cpp)
add_dependencies(simple_crud_codegen simple_crud_codegen_generate)
target_link_libraries(simple_crud_codegen PRIVATE katana_core)
target_include_directories(simple_crud_codegen PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Simplified version using http::server abstraction
add_executable(simple_crud_simple main_simple.cpp)
add_dependencies(simple_crud_simple simple_crud_codegen_generate)
target_link_libraries(simple_crud_simple PRIVATE katana_core)
target_include_directories(simple_crud_simple PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Copy api.yaml to build directory for reference
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/api.yaml
    ${CMAKE_CURRENT_BINARY_DIR}/api.yaml
    COPYONLY
)

# Print build info
message(STATUS "Configuring simple_crud_codegen example")
message(STATUS "  Source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  Binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
