include(FetchContent)

add_executable(unit_tests
    main.cpp
    unit/test_reactor.cpp
    unit/test_reactor_pool.cpp
    unit/test_http.cpp
    unit/test_wheel_timer.cpp
    unit/test_result.cpp
    unit/test_io_buffer.cpp
    unit/test_http_fuzzer_regression.cpp
)

target_link_libraries(unit_tests
    katana_core
    pthread
)

target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(integration_tests
    main.cpp
    integration/test_http_server.cpp
)

target_link_libraries(integration_tests
    katana_core
    pthread
)

target_include_directories(integration_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(run_unit_tests
    COMMAND unit_tests
    DEPENDS unit_tests
)

add_custom_target(run_integration_tests
    COMMAND integration_tests
    DEPENDS integration_tests
)

add_test(NAME unit_tests COMMAND unit_tests)
add_test(NAME integration_tests COMMAND integration_tests)
