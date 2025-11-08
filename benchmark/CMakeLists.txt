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
