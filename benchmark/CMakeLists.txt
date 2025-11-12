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
