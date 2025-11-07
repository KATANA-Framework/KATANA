cmake_minimum_required(VERSION 3.20)
project(katana VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wshadow
    -Wpedantic
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g)
endif()

include_directories(katana/core/include)

add_library(katana_core STATIC
    katana/core/src/epoll_reactor.cpp
    katana/core/src/cpu_info.cpp
    katana/core/src/reactor_pool.cpp
    katana/core/src/io_buffer.cpp
    katana/core/src/arena.cpp
    katana/core/src/problem.cpp
)

target_include_directories(katana_core PUBLIC
    katana/core/include
)

enable_testing()
add_subdirectory(test)

add_executable(basic_reactor_example examples/basic_reactor.cpp)
target_link_libraries(basic_reactor_example katana_core)
