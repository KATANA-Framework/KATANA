cmake_minimum_required(VERSION 3.20)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_executable(http_parser_fuzz http_parser_fuzz.cpp)

    target_link_libraries(http_parser_fuzz
        PRIVATE
            katana_core
    )

    target_compile_options(http_parser_fuzz
        PRIVATE
            -g
            -O1
            -fsanitize=fuzzer,address,undefined
    )

    target_link_options(http_parser_fuzz
        PRIVATE
            -fsanitize=fuzzer,address,undefined
    )

    add_executable(openapi_parser_fuzz openapi_parser_fuzz.cpp)

    target_link_libraries(openapi_parser_fuzz
        PRIVATE
            katana_core
    )

    target_compile_options(openapi_parser_fuzz
        PRIVATE
            -g
            -O1
            -fsanitize=fuzzer,address,undefined
    )

    target_link_options(openapi_parser_fuzz
        PRIVATE
            -fsanitize=fuzzer,address,undefined
    )

    message(STATUS "Fuzzing targets enabled (Clang detected)")
else()
    message(STATUS "Fuzzing targets require Clang compiler")
endif()
