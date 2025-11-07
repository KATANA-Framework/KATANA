include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    unit/test_reactor.cpp
    unit/test_reactor_pool.cpp
    unit/test_http.cpp
    unit/test_wheel_timer.cpp
)

target_link_libraries(unit_tests
    katana_core
    GTest::gtest_main
    pthread
)

add_executable(integration_tests
    integration/test_http_server.cpp
)

target_link_libraries(integration_tests
    katana_core
    GTest::gtest_main
    pthread
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
gtest_discover_tests(integration_tests)
