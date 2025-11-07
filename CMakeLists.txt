cmake_minimum_required(VERSION 3.20...4.2)
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
elseif(CMAKE_BUILD_TYPE STREQUAL "ASan")
    add_compile_options(-O1 -g -fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
elseif(CMAKE_BUILD_TYPE STREQUAL "LSan")
    add_compile_options(-O1 -g -fsanitize=leak -fno-omit-frame-pointer)
    add_link_options(-fsanitize=leak)
elseif(CMAKE_BUILD_TYPE STREQUAL "TSan")
    add_compile_options(-O1 -g -fsanitize=thread -fno-omit-frame-pointer)
    add_link_options(-fsanitize=thread)
elseif(CMAKE_BUILD_TYPE STREQUAL "UBSan")
    add_compile_options(-O1 -g -fsanitize=undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=undefined)
endif()

add_library(katana_core STATIC
    katana/core/src/epoll_reactor.cpp
    katana/core/src/cpu_info.cpp
    katana/core/src/reactor_pool.cpp
    katana/core/src/io_buffer.cpp
    katana/core/src/arena.cpp
    katana/core/src/problem.cpp
    katana/core/src/http.cpp
    katana/core/src/system_limits.cpp
    katana/core/src/shutdown.cpp
)

target_include_directories(katana_core PUBLIC
    katana/core/include
)

enable_testing()
add_subdirectory(test)

add_executable(basic_reactor_example examples/basic_reactor.cpp)
target_link_libraries(basic_reactor_example katana_core)

add_executable(hello_world_server examples/hello_world_server.cpp)
target_link_libraries(hello_world_server katana_core)
