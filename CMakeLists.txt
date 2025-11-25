cmake_minimum_required(VERSION 3.20...4.2)
project(katana VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(KATANA_POLL "epoll" CACHE STRING "I/O polling backend: epoll or io_uring")
set_property(CACHE KATANA_POLL PROPERTY STRINGS "epoll" "io_uring")

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wshadow
    -Wpedantic
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(
        -O3
        -DNDEBUG
        -march=native
        -mtune=native
    )
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

if(KATANA_POLL STREQUAL "io_uring")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBURING REQUIRED liburing>=2.0)
    set(REACTOR_SOURCE katana/core/src/io_uring_reactor.cpp)
    set(REACTOR_LIBS ${LIBURING_LIBRARIES})
    set(REACTOR_INCLUDE_DIRS ${LIBURING_INCLUDE_DIRS})
    add_compile_definitions(KATANA_USE_IO_URING)
    message(STATUS "Using io_uring backend")
elseif(KATANA_POLL STREQUAL "epoll")
    set(REACTOR_SOURCE katana/core/src/epoll_reactor.cpp)
    set(REACTOR_LIBS "")
    set(REACTOR_INCLUDE_DIRS "")
    add_compile_definitions(KATANA_USE_EPOLL)
    message(STATUS "Using epoll backend")
else()
    message(FATAL_ERROR "Invalid KATANA_POLL value: ${KATANA_POLL}. Must be 'epoll' or 'io_uring'")
endif()

add_library(katana_core STATIC
    ${REACTOR_SOURCE}
    katana/core/src/cpu_info.cpp
    katana/core/src/reactor_pool.cpp
    katana/core/src/io_buffer.cpp
    katana/core/src/arena.cpp
    katana/core/src/problem.cpp
    katana/core/src/http.cpp
    katana/core/src/http_field.cpp
    katana/core/src/system_limits.cpp
    katana/core/src/shutdown.cpp
    katana/core/src/tcp_socket.cpp
    katana/core/src/tcp_listener.cpp
)

target_include_directories(katana_core PUBLIC
    katana/core/include
    ${REACTOR_INCLUDE_DIRS}
)

if(REACTOR_LIBS)
    target_link_libraries(katana_core PUBLIC ${REACTOR_LIBS})
endif()

option(ENABLE_TESTING "Enable testing" ON)
option(ENABLE_BENCHMARKS "Enable benchmarks" OFF)
option(ENABLE_FUZZING "Enable fuzzing" OFF)
option(ENABLE_EXAMPLES "Enable examples" OFF)

if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

if(ENABLE_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

if(ENABLE_FUZZING AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_subdirectory(test/fuzz)
endif()

if(ENABLE_EXAMPLES)
    add_executable(basic_reactor_example examples/basic_reactor.cpp)
    target_link_libraries(basic_reactor_example katana_core)

    add_executable(hello_world_server examples/hello_world_server.cpp)
    target_link_libraries(hello_world_server katana_core)

    add_executable(raii_echo_server examples/raii_echo_server.cpp)
    target_link_libraries(raii_echo_server katana_core)

    add_executable(raii_http_server examples/raii_http_server.cpp)
    target_link_libraries(raii_http_server katana_core)

    add_executable(simple_rest_api examples/simple_rest_api.cpp)
    target_link_libraries(simple_rest_api katana_core)
endif()
add_executable(debug_test debug_test.cpp)
target_link_libraries(debug_test PRIVATE katana_core)
target_compile_features(debug_test PRIVATE cxx_std_20)
