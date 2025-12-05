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
    unit/test_virtual_event_loop.cpp
    unit/test_http_handler_harness.cpp
    unit/test_tcp_socket.cpp
    unit/test_tcp_listener.cpp
    unit/test_system_limits.cpp
    unit/test_shutdown.cpp
    unit/test_problem.cpp
    unit/test_router.cpp
    unit/test_openapi_ast.cpp
    unit/test_codegen_integration.cpp
    unit/test_codegen_snapshots.cpp
    unit/test_json_parser.cpp
)

target_link_libraries(unit_tests
    katana_core
    pthread
)

target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(integration_tests
    main.cpp
    integration/test_http_server.cpp
    integration/test_fixture_load.cpp
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
