cmake_minimum_required(VERSION 3.20...4.2)
project(katana VERSION 0.1.0 LANGUAGES CXX)

# Are we the top-level project, or pulled in via add_subdirectory() as a git submodule?
# (PROJECT_IS_TOP_LEVEL exists from CMake 3.21; provide a fallback for 3.20.)
if(DEFINED PROJECT_IS_TOP_LEVEL)
    set(KATANA_IS_TOP_LEVEL ${PROJECT_IS_TOP_LEVEL})
elseif(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(KATANA_IS_TOP_LEVEL ON)
else()
    set(KATANA_IS_TOP_LEVEL OFF)
endif()

include(CheckIPOSupported)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# KATANA uses no C++20 modules; disable the (GCC/Ninja) module dependency scan that C++23 would
# otherwise turn on — it breaks in some toolchain environments and buys us nothing.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(KATANA_POLL "epoll" CACHE STRING "I/O polling backend: epoll or io_uring")
set_property(CACHE KATANA_POLL PROPERTY STRINGS "epoll" "io_uring")
option(ENABLE_POSTGRES "Enable PostgreSQL/libpq runtime" ON)
option(ENABLE_TLS "Enable TLS/HTTPS + auth crypto (OpenSSL)" ON)
option(ENABLE_COMPRESSION "Enable response compression (zlib/zstd/brotli)" ON)

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wshadow
    -Wpedantic
)

set(KATANA_ENABLE_IPO OFF)
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    check_ipo_supported(RESULT katana_ipo_supported OUTPUT katana_ipo_error LANGUAGES CXX)
    if(katana_ipo_supported)
        set(KATANA_ENABLE_IPO ON)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            if(CMAKE_CXX_COMPILER_AR MATCHES "-NOTFOUND$" OR
               CMAKE_CXX_COMPILER_RANLIB MATCHES "-NOTFOUND$")
                set(KATANA_ENABLE_IPO OFF)
                message(STATUS "Disabling IPO for Clang: archiver/ranlib helpers are unavailable")
            endif()
        endif()
    else()
        message(STATUS "Disabling IPO: ${katana_ipo_error}")
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(
        -O3
        -DNDEBUG
        -march=native
        -mtune=native
    )
    if(KATANA_ENABLE_IPO)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(
        -O3
        -g
        -DNDEBUG
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
        -march=native
        -mtune=native
    )
    if(KATANA_ENABLE_IPO)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
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
    list(APPEND KATANA_PUBLIC_DEFINES KATANA_USE_IO_URING)
    message(STATUS "Using io_uring backend")
elseif(KATANA_POLL STREQUAL "epoll")
    set(REACTOR_SOURCE katana/core/src/epoll_reactor.cpp)
    set(REACTOR_LIBS "")
    set(REACTOR_INCLUDE_DIRS "")
    list(APPEND KATANA_PUBLIC_DEFINES KATANA_USE_EPOLL)
    message(STATUS "Using epoll backend")
else()
    message(FATAL_ERROR "Invalid KATANA_POLL value: ${KATANA_POLL}. Must be 'epoll' or 'io_uring'")
endif()

set(POSTGRES_SOURCE)
set(POSTGRES_LIBS)
set(POSTGRES_INCLUDE_DIRS)
if(ENABLE_POSTGRES)
    find_package(PostgreSQL QUIET)
    if(PostgreSQL_FOUND)
        set(POSTGRES_SOURCE katana/core/src/postgres.cpp)
        set(POSTGRES_LIBS ${PostgreSQL_LIBRARIES})
        set(POSTGRES_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIRS})
        list(APPEND KATANA_PUBLIC_DEFINES KATANA_HAS_LIBPQ)
        message(STATUS "Using PostgreSQL/libpq runtime")
    else()
        message(STATUS "PostgreSQL/libpq not found; SQL runtime disabled")
    endif()
endif()

# TLS/HTTPS termination + the crypto that auth (JWT HS/RS/ES) reuses — OpenSSL. Same optional-dep
# shape as PostgreSQL: KATANA_HAS_OPENSSL becomes a PUBLIC usage requirement so find_package(katana)
# consumers inherit it. Build with -DENABLE_TLS=OFF to drop the dependency (no HTTPS, no JWT).
set(TLS_SOURCE)
set(TLS_LIBS)
if(ENABLE_TLS)
    find_package(OpenSSL)
    if(OpenSSL_FOUND)
        set(TLS_SOURCE katana/core/src/tls.cpp katana/core/src/jwt.cpp katana/core/src/auth.cpp)
        set(TLS_LIBS OpenSSL::SSL OpenSSL::Crypto)
        list(APPEND KATANA_PUBLIC_DEFINES KATANA_HAS_OPENSSL)
        message(STATUS "Using OpenSSL (TLS + auth crypto)")
    else()
        message(STATUS "OpenSSL not found; TLS/HTTPS and JWT auth disabled")
    endif()
endif()

# Response compression: zlib (gzip), zstd, brotli. gzip via find_package(ZLIB); zstd/brotli linked by
# name (present on any modern distro). Turn off with -DENABLE_COMPRESSION=OFF.
set(COMPRESSION_SOURCE)
set(COMPRESSION_LIBS)
if(ENABLE_COMPRESSION)
    find_package(ZLIB)
    if(ZLIB_FOUND)
        set(COMPRESSION_SOURCE katana/core/src/compression.cpp)
        set(COMPRESSION_LIBS ZLIB::ZLIB zstd brotlienc brotlicommon)
        list(APPEND KATANA_PUBLIC_DEFINES KATANA_HAS_COMPRESSION)
        message(STATUS "Using response compression (gzip/zstd/brotli)")
    else()
        message(STATUS "zlib not found; response compression disabled")
    endif()
endif()

add_library(katana_core STATIC
    ${REACTOR_SOURCE}
    ${POSTGRES_SOURCE}
    ${TLS_SOURCE}
    ${COMPRESSION_SOURCE}
    katana/core/src/cpu_info.cpp
    katana/core/src/reactor_pool.cpp
    katana/core/src/syscall_metrics.cpp
    katana/core/src/io_buffer.cpp
    katana/core/src/arena.cpp
    katana/core/src/http_client.cpp
    katana/core/src/problem.cpp
    katana/core/src/openapi_loader.cpp
    katana/core/src/redis_client.cpp
    katana/core/src/http.cpp
    katana/core/src/http_field.cpp
    katana/core/src/http_server.cpp
    katana/core/src/handler_context.cpp
    katana/core/src/system_limits.cpp
    katana/core/src/shutdown.cpp
    katana/core/src/tcp_socket.cpp
    katana/core/src/tcp_listener.cpp
)

target_include_directories(katana_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/katana/core/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${REACTOR_INCLUDE_DIRS}
    ${POSTGRES_INCLUDE_DIRS}
)

# Backend/runtime feature macros are a PUBLIC usage requirement: public headers (e.g.
# reactor_impl.hpp) switch on KATANA_USE_EPOLL/KATANA_USE_IO_URING, so a consumer that
# includes them must see the same define. Attaching it to the target (not a directory-scope
# add_compile_definitions) means it propagates automatically — no hand-setting required.
if(KATANA_PUBLIC_DEFINES)
    target_compile_definitions(katana_core PUBLIC ${KATANA_PUBLIC_DEFINES})
endif()

if(REACTOR_LIBS)
    target_link_libraries(katana_core PUBLIC ${REACTOR_LIBS})
endif()
if(POSTGRES_LIBS)
    target_link_libraries(katana_core PUBLIC ${POSTGRES_LIBS})
endif()
if(TLS_LIBS)
    target_link_libraries(katana_core PUBLIC ${TLS_LIBS})
endif()
if(COMPRESSION_LIBS)
    target_link_libraries(katana_core PUBLIC ${COMPRESSION_LIBS})
endif()

# Namespaced alias for consumers: target_link_libraries(app PRIVATE katana::core)
if(NOT TARGET katana::core)
    add_library(katana::core ALIAS katana_core)
endif()

# Tests/examples build by default only when KATANA is the top-level project; a submodule gets
# just the library + tools unless the parent opts in.
option(ENABLE_TESTING "Enable testing" ${KATANA_IS_TOP_LEVEL})
option(ENABLE_BENCHMARKS "Enable benchmarks" OFF)
option(ENABLE_FUZZING "Enable fuzzing" OFF)
option(ENABLE_EXAMPLES "Enable examples" ${KATANA_IS_TOP_LEVEL})
option(ENABLE_TOOLS "Enable CLI/tools" ON)
# Install/export rules (find_package(katana) support). On by default when top-level; a parent
# that pulls KATANA in via add_subdirectory usually does not want to install it, so it defaults off.
option(KATANA_INSTALL "Generate install/export rules for find_package(katana)" ${KATANA_IS_TOP_LEVEL})

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

    add_executable(hello_world_router_server examples/hello_world_router_server.cpp)
    target_link_libraries(hello_world_router_server katana_core)

    add_executable(raii_echo_server examples/raii_echo_server.cpp)
    target_link_libraries(raii_echo_server katana_core)

    add_executable(raii_http_server examples/raii_http_server.cpp)
    target_link_libraries(raii_http_server katana_core)

    add_executable(simple_rest_api examples/simple_rest_api.cpp)
    target_link_libraries(simple_rest_api katana_core)

    add_executable(router_rest_api examples/router_rest_api.cpp)
    target_link_libraries(router_rest_api katana_core)

    add_executable(middleware_examples examples/middleware_examples.cpp)
    target_link_libraries(middleware_examples katana_core)

    # Codegen examples
    add_subdirectory(examples/codegen/compute_api)
    add_subdirectory(examples/codegen/validation_api)
    add_subdirectory(examples/codegen/text_api)
    add_subdirectory(examples/codegen/benchmark_api)
    add_subdirectory(examples/codegen/task_api)
endif()

if(ENABLE_TOOLS)
    add_executable(katana_gen
        tools/katana_gen.cpp
        tools/katana_gen/options.cpp
        tools/katana_gen/generator_utils.cpp
        tools/katana_gen/ast_dump.cpp
        tools/katana_gen/dto_generator.cpp
        tools/katana_gen/json_generator.cpp
        tools/katana_gen/sql_parser.cpp
        tools/katana_gen/sql_generator.cpp
        tools/katana_gen/validator_generator.cpp
        tools/katana_gen/router_generator.cpp
        tools/katana_gen/typescript_generator.cpp
        tools/katana_gen/bridge_generator.cpp
    )
    target_include_directories(katana_gen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tools)
    target_link_libraries(katana_gen katana_core)
    target_compile_features(katana_gen PRIVATE cxx_std_20)
    target_compile_options(katana_gen PRIVATE -Wall -Wextra -Werror -Wpedantic)
    if(NOT TARGET katana::gen)
        add_executable(katana::gen ALIAS katana_gen)
    endif()
    # Codegen helper functions (katana_add_openapi / katana_add_sql) for consumers — only useful
    # when the generator is built.
    include(KatanaCodegen)
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Install / export — make `find_package(katana)` work for standalone consumers.
# Produces katana::core (+ katana::gen when tools are built) and ships the codegen
# helpers, so a consumer never has to reference paths inside this repo or hand-set
# backend macros like KATANA_USE_EPOLL.
# ─────────────────────────────────────────────────────────────────────────────
if(KATANA_INSTALL)
    # EXPORT_NAME makes the imported targets `katana::core` / `katana::gen` (matching the in-tree
    # aliases), not `katana::katana_core` / `katana::katana_gen` (ALIAS targets aren't exportable).
    set(KATANA_EXPORT_TARGETS katana_core)
    set_target_properties(katana_core PROPERTIES EXPORT_NAME core)
    if(ENABLE_TOOLS)
        list(APPEND KATANA_EXPORT_TARGETS katana_gen)
        set_target_properties(katana_gen PROPERTIES EXPORT_NAME gen)
    endif()

    install(TARGETS ${KATANA_EXPORT_TARGETS}
        EXPORT katanaTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # Public headers: the whole katana/ tree (core + sql) under the include prefix.
    install(DIRECTORY katana/core/include/katana
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # Namespaced imported targets (katana::core, katana::gen).
    install(EXPORT katanaTargets
        FILE katanaTargets.cmake
        NAMESPACE katana::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/katana)

    # Codegen helper module ships next to the config so find_package pulls in katana_add_*.
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/KatanaCodegen.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/katana)

    # Package config + version.
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/katanaConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/katanaConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/katana)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/katanaConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/katanaConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/katanaConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/katana)
endif()
