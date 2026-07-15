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

# Resolve the generator target name once: `katana_gen` when KATANA is in-tree (add_subdirectory),
# or the imported `katana::gen` when consumed via find_package(katana). Sets KATANA_GEN_TARGET in
# the caller's scope for use in custom commands.
if(NOT COMMAND _katana_require_gen)
    function(_katana_require_gen)
        if(TARGET katana_gen)
            set(KATANA_GEN_TARGET katana_gen PARENT_SCOPE)
        elseif(TARGET katana::gen)
            set(KATANA_GEN_TARGET katana::gen PARENT_SCOPE)
        else()
            message(FATAL_ERROR
                "katana_add_* requires the katana generator target "
                "(build KATANA with ENABLE_TOOLS=ON, or install it so find_package exports katana::gen).")
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
        COMMAND $<TARGET_FILE:${KATANA_GEN_TARGET}> openapi -i "${ARG_CONTRACT}" -o "${ARG_OUTPUT}"
                --emit ${ARG_EMIT} --inline-naming ${ARG_INLINE_NAMING} ${ns_args}
        COMMAND ${CMAKE_COMMAND} -E touch "${stamp}"
        DEPENDS ${KATANA_GEN_TARGET} "${ARG_CONTRACT}"
        COMMENT "katana_gen openapi: ${NAME}"
        VERBATIM)
    add_custom_target(${NAME}_codegen DEPENDS "${stamp}")

    add_library(${NAME} INTERFACE)
    add_dependencies(${NAME} ${NAME}_codegen)
    target_include_directories(${NAME} INTERFACE "${ARG_OUTPUT}")
    target_link_libraries(${NAME} INTERFACE katana::core)
endfunction()

# katana_add_typescript(<name> CONTRACT <spec.yaml> OUTPUT <dir> [INLINE_NAMING <operation|flat>])
#
# Generates a TypeScript client (generated_client.ts) from an OpenAPI contract into OUTPUT, wiring
# it as a build dependency of the given custom target name (${NAME}_ts). Unlike the C++ helpers it
# does not produce a library — the .ts file is consumed by a separate frontend build.
function(katana_add_typescript NAME)
    _katana_require_gen()
    cmake_parse_arguments(ARG "" "CONTRACT;OUTPUT;INLINE_NAMING" "" ${ARGN})
    if(NOT ARG_CONTRACT)
        message(FATAL_ERROR "katana_add_typescript(${NAME}): CONTRACT <spec.yaml> is required.")
    endif()
    if(NOT ARG_OUTPUT)
        message(FATAL_ERROR "katana_add_typescript(${NAME}): OUTPUT <dir> is required.")
    endif()
    if(NOT ARG_INLINE_NAMING)
        set(ARG_INLINE_NAMING operation)
    endif()

    file(MAKE_DIRECTORY "${ARG_OUTPUT}")
    set(client "${ARG_OUTPUT}/generated_client.ts")
    add_custom_command(
        OUTPUT "${client}"
        COMMAND $<TARGET_FILE:${KATANA_GEN_TARGET}> openapi -i "${ARG_CONTRACT}" -o "${ARG_OUTPUT}"
                --emit typescript --inline-naming ${ARG_INLINE_NAMING}
        DEPENDS ${KATANA_GEN_TARGET} "${ARG_CONTRACT}"
        COMMENT "katana_gen typescript: ${NAME}"
        VERBATIM)
    add_custom_target(${NAME}_ts ALL DEPENDS "${client}")
endfunction()

# katana_add_sql(<name> DIR <sql_dir> [NAMESPACE <ns>] [OUTPUT <dir>] [EMIT <all|models,repository>]
#                [BRIDGE_OPENAPI <spec.yaml>])
# BRIDGE_OPENAPI additionally emits generated_bridge.hpp with Row<->DTO converters against that
# contract (use the SAME NAMESPACE as the contract's DTOs so the types line up).
function(katana_add_sql NAME)
    _katana_require_gen()
    cmake_parse_arguments(ARG "" "DIR;NAMESPACE;OUTPUT;EMIT;BRIDGE_OPENAPI" "" ${ARGN})
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
    set(bridge_args "")
    set(bridge_dep "")
    if(ARG_BRIDGE_OPENAPI)
        set(bridge_args --openapi "${ARG_BRIDGE_OPENAPI}")
        set(bridge_dep "${ARG_BRIDGE_OPENAPI}")
    endif()

    file(MAKE_DIRECTORY "${ARG_OUTPUT}")
    set(stamp "${ARG_OUTPUT}/.${NAME}.stamp")
    file(GLOB sql_inputs CONFIGURE_DEPENDS "${ARG_DIR}/*.sql")
    add_custom_command(
        OUTPUT "${stamp}"
        COMMAND $<TARGET_FILE:${KATANA_GEN_TARGET}> sql -i "${ARG_DIR}" -o "${ARG_OUTPUT}"
                --emit ${ARG_EMIT} ${ns_args} ${bridge_args}
        COMMAND ${CMAKE_COMMAND} -E touch "${stamp}"
        DEPENDS ${KATANA_GEN_TARGET} ${sql_inputs} ${bridge_dep}
        COMMENT "katana_gen sql: ${NAME}"
        VERBATIM)
    add_custom_target(${NAME}_codegen DEPENDS "${stamp}")

    add_library(${NAME} INTERFACE)
    add_dependencies(${NAME} ${NAME}_codegen)
    target_include_directories(${NAME} INTERFACE "${ARG_OUTPUT}")
    target_link_libraries(${NAME} INTERFACE katana::core)
endfunction()
