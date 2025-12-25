add_executable(simple_benchmark simple_benchmark.cpp)

target_compile_options(simple_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(simple_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(performance_benchmark performance_benchmark.cpp)

target_compile_options(performance_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(performance_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(mpsc_benchmark mpsc_benchmark.cpp)

target_compile_options(mpsc_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(mpsc_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(timer_benchmark timer_benchmark.cpp)

target_compile_options(timer_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(timer_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(headers_benchmark headers_benchmark.cpp)

target_compile_options(headers_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(headers_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(io_buffer_benchmark io_buffer_benchmark.cpp)

target_compile_options(io_buffer_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(io_buffer_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(router_benchmark router_benchmark.cpp)

target_compile_options(router_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(router_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(openapi_benchmark openapi_benchmark.cpp)

target_compile_options(openapi_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(openapi_benchmark
    PRIVATE
        katana_core
        pthread
)

set(GENERATED_BENCH_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(GENERATED_BENCH_SOURCES
    ${GENERATED_BENCH_DIR}/generated_dtos.hpp
    ${GENERATED_BENCH_DIR}/generated_handlers.hpp
    ${GENERATED_BENCH_DIR}/generated_json.hpp
    ${GENERATED_BENCH_DIR}/generated_routes.hpp
    ${GENERATED_BENCH_DIR}/generated_router_bindings.hpp
    ${GENERATED_BENCH_DIR}/generated_validators.hpp
)

add_custom_command(
    OUTPUT ${GENERATED_BENCH_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_BENCH_DIR}
    COMMAND katana_gen openapi -i ${CMAKE_CURRENT_SOURCE_DIR}/test_api.yaml -o ${GENERATED_BENCH_DIR} --emit all
    DEPENDS katana_gen ${CMAKE_CURRENT_SOURCE_DIR}/test_api.yaml
    COMMENT "Generating benchmark API code from test_api.yaml"
)

add_custom_target(generated_api_codegen ALL DEPENDS ${GENERATED_BENCH_SOURCES})

add_executable(generated_api_benchmark generated_api_benchmark.cpp ${GENERATED_BENCH_SOURCES})

target_compile_options(generated_api_benchmark
    PRIVATE
        -O3
        -march=native
)

target_include_directories(generated_api_benchmark
    PRIVATE
        ${GENERATED_BENCH_DIR}
)

target_link_libraries(generated_api_benchmark
    PRIVATE
        katana_core
        pthread
)
add_dependencies(generated_api_benchmark generated_api_codegen)

add_executable(cycle_profiling cycle_profiling.cpp ${GENERATED_BENCH_SOURCES})

target_compile_options(cycle_profiling
    PRIVATE
        -O3
        -march=native
        -fno-omit-frame-pointer
)

target_include_directories(cycle_profiling
    PRIVATE
        ${GENERATED_BENCH_DIR}
)

target_link_libraries(cycle_profiling
    PRIVATE
        katana_core
        pthread
)
add_dependencies(cycle_profiling generated_api_codegen)

add_executable(pure_routing_benchmark pure_routing_benchmark.cpp ${GENERATED_BENCH_SOURCES})

target_compile_options(pure_routing_benchmark
    PRIVATE
        -O3
        -march=native
        -fno-omit-frame-pointer
)

target_include_directories(pure_routing_benchmark
    PRIVATE
        ${GENERATED_BENCH_DIR}
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(pure_routing_benchmark
    PRIVATE
        katana_core
        pthread
)
add_dependencies(pure_routing_benchmark generated_api_codegen)

add_executable(comprehensive_profiling comprehensive_profiling.cpp ${GENERATED_BENCH_SOURCES})

target_compile_options(comprehensive_profiling
    PRIVATE
        -O3
        -march=native
        -fno-omit-frame-pointer
)

target_include_directories(comprehensive_profiling
    PRIVATE
        ${GENERATED_BENCH_DIR}
)

target_link_libraries(comprehensive_profiling
    PRIVATE
        katana_core
        pthread
)
add_dependencies(comprehensive_profiling generated_api_codegen)

add_executable(router_optimization_benchmark router_optimization_benchmark.cpp)

target_compile_options(router_optimization_benchmark
    PRIVATE
        -O3
        -march=native
        -fno-omit-frame-pointer
)

target_include_directories(router_optimization_benchmark
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(router_optimization_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(products_api_benchmark products_api_benchmark.cpp)

target_compile_options(products_api_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(products_api_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(json_benchmark json_benchmark.cpp)

target_compile_options(json_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(json_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(json_parsing_benchmark json_parsing_benchmark.cpp)

target_compile_options(json_parsing_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(json_parsing_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(api_load_client api_load_client.cpp)

target_compile_options(api_load_client
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(api_load_client
    PRIVATE
        katana_core
        pthread
)

add_executable(detailed_profiling detailed_profiling.cpp)

target_compile_options(detailed_profiling
    PRIVATE
        -O3
        -march=native
        -fno-omit-frame-pointer
)

target_include_directories(detailed_profiling
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(detailed_profiling
    PRIVATE
        katana_core
        pthread
)

add_executable(optimized_api_benchmark optimized_api_benchmark.cpp)

target_compile_options(optimized_api_benchmark
    PRIVATE
        -O3
        -march=native
        -fno-omit-frame-pointer
)

target_include_directories(optimized_api_benchmark
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(optimized_api_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(generated_json_benchmark generated_json_benchmark.cpp)

target_compile_options(generated_json_benchmark
    PRIVATE
        -O3
        -march=native
)

target_include_directories(generated_json_benchmark
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(generated_json_benchmark
    PRIVATE
        katana_core
        pthread
)

add_executable(simd_whitespace_benchmark simd_whitespace_benchmark.cpp)

target_compile_options(simd_whitespace_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(simd_whitespace_benchmark
    PRIVATE
        katana_core
        pthread
)
