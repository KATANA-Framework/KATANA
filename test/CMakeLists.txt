include(FetchContent)

set(GENERATED_SQL_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated_sql)
set(GENERATED_SQL_TEST_SOURCES
    ${GENERATED_SQL_TEST_DIR}/generated_sql_models.hpp
    ${GENERATED_SQL_TEST_DIR}/generated_sql_repository.hpp
    ${GENERATED_SQL_TEST_DIR}/generated_sql_package.hpp
)

add_custom_command(
    OUTPUT ${GENERATED_SQL_TEST_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_SQL_TEST_DIR}
    COMMAND katana_gen sql -i ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/sql/basic -o ${GENERATED_SQL_TEST_DIR}
    DEPENDS katana_gen
            ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/sql/basic/bulk_upsert_users.sql
            ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/sql/basic/get_user.sql
            ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/sql/basic/list_users.sql
            ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/sql/basic/touch_user.sql
            ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/sql/basic/upsert_user.sql
    COMMENT "Generating SQL repository fixture for unit tests"
)

add_custom_target(generated_sql_test_codegen DEPENDS ${GENERATED_SQL_TEST_SOURCES})

set(GENERATED_BENCHMARK_API_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_openapi)
set(GENERATED_BENCHMARK_API_TEST_SOURCES
    ${GENERATED_BENCHMARK_API_TEST_DIR}/generated_dtos.hpp
    ${GENERATED_BENCHMARK_API_TEST_DIR}/generated_handlers.hpp
    ${GENERATED_BENCHMARK_API_TEST_DIR}/generated_json.hpp
    ${GENERATED_BENCHMARK_API_TEST_DIR}/generated_routes.hpp
    ${GENERATED_BENCHMARK_API_TEST_DIR}/generated_router_bindings.hpp
    ${GENERATED_BENCHMARK_API_TEST_DIR}/generated_validators.hpp
    ${GENERATED_BENCHMARK_API_TEST_DIR}/generated_openapi_package.hpp
)

add_custom_command(
    OUTPUT ${GENERATED_BENCHMARK_API_TEST_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_BENCHMARK_API_TEST_DIR}
    COMMAND katana_gen openapi
            -i ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/api.yaml
            -o ${GENERATED_BENCHMARK_API_TEST_DIR}
            --emit all
            --inline-naming operation
    DEPENDS katana_gen ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/api.yaml
    COMMENT "Generating benchmark_api OpenAPI fixture for tests"
)

add_custom_target(generated_benchmark_api_test_codegen DEPENDS ${GENERATED_BENCHMARK_API_TEST_SOURCES})

set(GENERATED_BENCHMARK_API_SQL_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/benchmark_api_generated_sql)
set(GENERATED_BENCHMARK_API_SQL_TEST_SOURCES
    ${GENERATED_BENCHMARK_API_SQL_TEST_DIR}/generated_sql_models.hpp
    ${GENERATED_BENCHMARK_API_SQL_TEST_DIR}/generated_sql_repository.hpp
    ${GENERATED_BENCHMARK_API_SQL_TEST_DIR}/generated_sql_package.hpp
)

add_custom_command(
    OUTPUT ${GENERATED_BENCHMARK_API_SQL_TEST_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_BENCHMARK_API_SQL_TEST_DIR}
    COMMAND katana_gen sql
            -i ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/sql
            -o ${GENERATED_BENCHMARK_API_SQL_TEST_DIR}
    DEPENDS katana_gen
            ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/sql/create_item.sql
            ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/sql/delete_item.sql
            ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/sql/get_item.sql
            ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/sql/list_items_page_all.sql
            ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/sql/list_items_page_by_category.sql
            ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/sql/update_item.sql
    COMMENT "Generating benchmark_api SQL fixture for tests"
)

add_custom_target(generated_benchmark_api_sql_test_codegen
    DEPENDS ${GENERATED_BENCHMARK_API_SQL_TEST_SOURCES}
)

add_executable(unit_tests
    main.cpp
    ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/demo_service.cpp
    unit/test_reactor.cpp
    unit/test_reactor_pool.cpp
    unit/test_content_negotiation.cpp
    unit/test_http.cpp
    unit/test_media_type_registry.cpp
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
    unit/test_log.cpp
    unit/test_redis_client.cpp
    unit/test_redis_policy_stores.cpp
    unit/test_openapi_ast.cpp
    unit/test_codegen_integration.cpp
    unit/test_codegen_snapshots.cpp
    unit/test_postgres_pool.cpp
    unit/test_sql_runtime.cpp
    unit/test_sql_codegen.cpp
    unit/test_generated_sql_repository.cpp
    unit/test_benchmark_api_demo.cpp
    unit/test_json_parser.cpp
)

target_link_libraries(unit_tests
    katana_core
    pthread
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GENERATED_SQL_TEST_DIR}
    ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api
)
add_dependencies(unit_tests
    katana_gen
    generated_sql_test_codegen
    generated_benchmark_api_test_codegen
    generated_benchmark_api_sql_test_codegen
)
target_compile_definitions(unit_tests PRIVATE KATANA_GEN_BIN="$<TARGET_FILE:katana_gen>")

add_executable(sql_unit_tests
    main.cpp
    ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/demo_service.cpp
    unit/test_postgres_pool.cpp
    unit/test_sql_runtime.cpp
    unit/test_sql_codegen.cpp
    unit/test_generated_sql_repository.cpp
    unit/test_benchmark_api_demo.cpp
)

target_link_libraries(sql_unit_tests
    katana_core
    pthread
)

target_include_directories(sql_unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GENERATED_SQL_TEST_DIR}
    ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api
)
add_dependencies(sql_unit_tests
    katana_gen
    generated_sql_test_codegen
    generated_benchmark_api_test_codegen
    generated_benchmark_api_sql_test_codegen
)
target_compile_definitions(sql_unit_tests PRIVATE KATANA_GEN_BIN="$<TARGET_FILE:katana_gen>")

add_executable(sql_integration_tests
    main.cpp
    ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/demo_service.cpp
    integration/test_benchmark_api_sql_demo.cpp
    integration/test_generated_sql_postgres.cpp
    integration/test_sql_postgres.cpp
)

target_link_libraries(sql_integration_tests
    katana_core
    pthread
)

target_include_directories(sql_integration_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GENERATED_SQL_TEST_DIR}
    ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api
)
add_dependencies(sql_integration_tests
    generated_sql_test_codegen
    generated_benchmark_api_test_codegen
    generated_benchmark_api_sql_test_codegen
)

add_executable(integration_tests
    main.cpp
    ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api/demo_service.cpp
    integration/test_benchmark_api_sql_demo.cpp
    integration/test_http_server.cpp
    integration/test_fixture_load.cpp
    integration/test_generated_sql_postgres.cpp
    integration/test_sql_postgres.cpp
)

target_link_libraries(integration_tests
    katana_core
    pthread
)

target_include_directories(integration_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GENERATED_SQL_TEST_DIR}
    ${CMAKE_SOURCE_DIR}/examples/codegen/benchmark_api
)
add_dependencies(integration_tests
    generated_sql_test_codegen
    generated_benchmark_api_test_codegen
    generated_benchmark_api_sql_test_codegen
)

add_executable(conformance_tests
    main.cpp
    conformance/test_conformance.cpp
)

target_link_libraries(conformance_tests
    katana_core
    pthread
)

target_include_directories(conformance_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_target_properties(conformance_tests PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
endif()

add_custom_target(run_unit_tests
    COMMAND unit_tests
    DEPENDS unit_tests
)

add_custom_target(run_sql_unit_tests
    COMMAND sql_unit_tests
    DEPENDS sql_unit_tests
)

add_custom_target(run_sql_integration_tests
    COMMAND sql_integration_tests
    DEPENDS sql_integration_tests
)

add_custom_target(run_integration_tests
    COMMAND integration_tests
    DEPENDS integration_tests
)

add_custom_target(run_conformance_tests
    COMMAND conformance_tests
    DEPENDS conformance_tests
)

add_test(NAME unit_tests COMMAND unit_tests)
add_test(NAME sql_unit_tests COMMAND sql_unit_tests)
add_test(NAME sql_integration_tests COMMAND sql_integration_tests)
add_test(NAME integration_tests COMMAND integration_tests)
add_test(NAME conformance_tests COMMAND conformance_tests)
