# KATANA codegen helpers for consumers (e.g. when KATANA is a git submodule).
#
# These wrap the `katana_gen` tool in CMake custom commands and hand back an INTERFACE library
# carrying the generated include directory + a link to katana::core, so a contract becomes a
# normal dependency:
#
#   katana_add_openapi(catalog
#       CONTRACT ${CMAKE_CURRENT_SOURCE_DIR}/catalog.yaml
#       NAMESPACE catalog)              # required for multi-contract-in-one-binary
#   katana_add_sql(catalog_sql
#       DIR ${CMAKE_CURRENT_SOURCE_DIR}/sql/catalog
#       NAMESPACE catalog)
#   target_link_libraries(myservice PRIVATE catalog catalog_sql)
#
# Generated files keep their fixed names (generated_dtos.hpp, generated_sql_repository.hpp, …);
# include them with quotes. Each contract gets its own OUTPUT dir, so multiple namespaced
# contracts coexist (quoted #includes resolve in the including file's directory).

if(NOT COMMAND _katana_require_gen)
    function(_katana_require_gen)
        if(NOT TARGET katana_gen)
            message(FATAL_ERROR "katana_add_* requires the katana_gen target (ENABLE_TOOLS=ON).")
        endif()
    endfunction()
endif()

# katana_add_openapi(<name> CONTRACT <spec.yaml> [NAMESPACE <ns>] [OUTPUT <dir>]
#                    [EMIT <all|dto,...>] [INLINE_NAMING <operation|flat>])
function(katana_add_openapi NAME)
    _katana_require_gen()
    cmake_parse_arguments(ARG "" "CONTRACT;NAMESPACE;OUTPUT;EMIT;INLINE_NAMING" "" ${ARGN})
    if(NOT ARG_CONTRACT)
        message(FATAL_ERROR "katana_add_openapi(${NAME}): CONTRACT <spec.yaml> is required.")
    endif()
    if(NOT ARG_OUTPUT)
        set(ARG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_generated")
    endif()
    if(NOT ARG_EMIT)
        set(ARG_EMIT all)
    endif()
    if(NOT ARG_INLINE_NAMING)
        set(ARG_INLINE_NAMING operation)
    endif()
    set(ns_args "")
    if(ARG_NAMESPACE)
        set(ns_args --namespace ${ARG_NAMESPACE})
    endif()

    file(MAKE_DIRECTORY "${ARG_OUTPUT}")
    set(stamp "${ARG_OUTPUT}/.${NAME}.stamp")
    add_custom_command(
        OUTPUT "${stamp}"
        COMMAND $<TARGET_FILE:katana_gen> openapi -i "${ARG_CONTRACT}" -o "${ARG_OUTPUT}"
                --emit ${ARG_EMIT} --inline-naming ${ARG_INLINE_NAMING} ${ns_args}
        COMMAND ${CMAKE_COMMAND} -E touch "${stamp}"
        DEPENDS katana_gen "${ARG_CONTRACT}"
        COMMENT "katana_gen openapi: ${NAME}"
        VERBATIM)
    add_custom_target(${NAME}_codegen DEPENDS "${stamp}")

    add_library(${NAME} INTERFACE)
    add_dependencies(${NAME} ${NAME}_codegen)
    target_include_directories(${NAME} INTERFACE "${ARG_OUTPUT}")
    target_link_libraries(${NAME} INTERFACE katana_core)
endfunction()

# katana_add_sql(<name> DIR <sql_dir> [NAMESPACE <ns>] [OUTPUT <dir>] [EMIT <all|models,repository>])
function(katana_add_sql NAME)
    _katana_require_gen()
    cmake_parse_arguments(ARG "" "DIR;NAMESPACE;OUTPUT;EMIT" "" ${ARGN})
    if(NOT ARG_DIR)
        message(FATAL_ERROR "katana_add_sql(${NAME}): DIR <sql_dir> is required.")
    endif()
    if(NOT ARG_OUTPUT)
        set(ARG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_generated")
    endif()
    if(NOT ARG_EMIT)
        set(ARG_EMIT all)
    endif()
    set(ns_args "")
    if(ARG_NAMESPACE)
        set(ns_args --namespace ${ARG_NAMESPACE})
    endif()

    file(MAKE_DIRECTORY "${ARG_OUTPUT}")
    set(stamp "${ARG_OUTPUT}/.${NAME}.stamp")
    file(GLOB sql_inputs CONFIGURE_DEPENDS "${ARG_DIR}/*.sql")
    add_custom_command(
        OUTPUT "${stamp}"
        COMMAND $<TARGET_FILE:katana_gen> sql -i "${ARG_DIR}" -o "${ARG_OUTPUT}"
                --emit ${ARG_EMIT} ${ns_args}
        COMMAND ${CMAKE_COMMAND} -E touch "${stamp}"
        DEPENDS katana_gen ${sql_inputs}
        COMMENT "katana_gen sql: ${NAME}"
        VERBATIM)
    add_custom_target(${NAME}_codegen DEPENDS "${stamp}")

    add_library(${NAME} INTERFACE)
    add_dependencies(${NAME} ${NAME}_codegen)
    target_include_directories(${NAME} INTERFACE "${ARG_OUTPUT}")
    target_link_libraries(${NAME} INTERFACE katana_core)
endfunction()
