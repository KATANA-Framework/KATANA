add_executable(latency_benchmark latency_benchmark.cpp)

target_compile_options(latency_benchmark
    PRIVATE
        -O3
)

target_link_libraries(latency_benchmark
    PRIVATE
        pthread
)

add_executable(comprehensive_benchmark comprehensive_benchmark.cpp)

target_compile_options(comprehensive_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(comprehensive_benchmark
    PRIVATE
        pthread
)

add_executable(simple_benchmark simple_benchmark.cpp)

target_compile_options(simple_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(simple_benchmark
    PRIVATE
        pthread
)

add_executable(async_benchmark async_benchmark.cpp)

target_compile_options(async_benchmark
    PRIVATE
        -O3
        -march=native
)

target_link_libraries(async_benchmark
    PRIVATE
        pthread
)
