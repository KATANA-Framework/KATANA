add_executable(latency_benchmark latency_benchmark.cpp)

target_compile_options(latency_benchmark
    PRIVATE
        -O3
)

target_link_libraries(latency_benchmark
    PRIVATE
        pthread
)

add_executable(http_load_benchmark http_load_benchmark.cpp)

target_compile_options(http_load_benchmark
    PRIVATE
        -O3
)

target_link_libraries(http_load_benchmark
    PRIVATE
        pthread
)
