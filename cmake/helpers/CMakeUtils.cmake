set(GRAPHVIZ_OUTPUT OFF)
set(DEPENDENCY_DIAGNOSTICS OFF)

function(run_active_cmake_diagnostics)
    if(DEPENDENCY_DIAGNOSTICS MATCHES ON)
        # prints a dependency hierarchy for all targets in project
        set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE ON)
    endif()

    # enabled with -D GRAPHVIZ_OUTPUT=ON
    if(GRAPHVIZ_OUTPUT MATCHES ON)
        # Outputs graphviz dot files and generates png images showing dependency
        # relationships for top level project and all targets it contains.
        # All files will be generated in src/build/graphviz_output by default.
        set(GRAPHVIZ_GRAPH_NAME "Dependency tree")
        add_custom_target(graphviz ALL
            COMMAND ${CMAKE_COMMAND} "--graphviz=${CURRENT_CMAKE_BINARY_DIR}/graphviz_output/${PROJECT_NAME}.dot" .
            WORKING_DIRECTORY ${CURRENT_CMAKE_BINARY_DIR}
        )
        # Generate PNG from graphviz `${PROJECT_NAME}.dot` file...
        # Note: png image graph generation requires graphviz (dot executable specifically) to be installed
        execute_process(
            COMMAND dot -Tpng -o "./graphviz_output/${PROJECT_NAME}.png" "./graphviz_output/${PROJECT_NAME}.dot"
            WORKING_DIRECTORY "${CURRENT_CMAKE_BINARY_DIR}"
        )
    endif()
endfunction(run_active_cmake_diagnostics)

# function to output all CMAKE variables along with their
# values using a case insentive regex match
#
# examples:
# 1. print all cmake variables:
#    > dump_cmake_variables(".*")
# 2. print all boolt cmake variables:
#    > dump_cmake_variables("^boost.*")
function(dump_cmake_variables)
    get_cmake_property(_vars VARIABLES)
    list(SORT _vars)

    foreach(_var ${_vars})
        if(ARGV0)
            unset(MATCHED)

            # case insenstitive match
            string(TOLOWER "${ARGV0}" ARGV0_lower)
            string(TOLOWER "${_var}" _var_lower)

            string(REGEX MATCH ${ARGV0_lower} MATCHED ${_var_lower})

            if(NOT MATCHED)
                continue()
            endif()
        endif()


        set(_value ${${_var}})
        list(LENGTH _value _val_list_len)
        if(_val_list_len GREATER 1)
            message(DEBUG "    [${_var}] =>")
            foreach(_val ${_value})
                message(DEBUG "        - ${_val}")
            endforeach()
        else()
            message(DEBUG "    [${_var}] => ${_value}")
        endif()
    endforeach()
endfunction()

# prints a collection of useful C++ project configuration values
function(print_project_variables)
    message(DEBUG "")
    message(DEBUG "DEBUG CMake Cache Variable Dump")
    message(DEBUG "=============================================")
    message(DEBUG "")
    dump_cmake_variables(".*")

    message(NOTICE "")
    message(NOTICE "Project Configuration Settigs: ${PROJECT_NAME} v${CCMATH_BUILD_VERSION}")
    message(NOTICE "=============================================")
    message(NOTICE "")
    message(NOTICE "Build Configuration")
    message(NOTICE "    CMAKE_SYSTEM_PROCESSOR:..................: " ${CMAKE_SYSTEM_PROCESSOR})
    message(NOTICE "    CMAKE_HOST_SYSTEM_NAME:..................: " ${CMAKE_HOST_SYSTEM_NAME})
    message(NOTICE "    CMAKE_BUILD_TYPE:........................: " ${CMAKE_BUILD_TYPE})
    message(NOTICE "    CMAKE_CXX_COMPILER_ARCHITECTURE_ID:......: " ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID})
    message(NOTICE "    CMAKE_CXX_STANDARD:......................: " ${CMAKE_CXX_STANDARD})
    message(NOTICE "    CMAKE_CXX_COMPILER_VERSION:..............: " ${CMAKE_CXX_COMPILER_VERSION})
    message(NOTICE "    CMAKE_GENERATOR:.........................: " ${CMAKE_GENERATOR})
    message(NOTICE "    CMAKE_VERSION:...........................: " ${CMAKE_VERSION})
    message(NOTICE "    CMAKE_MINIMUM_REQUIRED_VERSION:..........: " ${CMAKE_MINIMUM_REQUIRED_VERSION})
    message(NOTICE "    CMAKE_CXX_SIZEOF_DATA_PTR:...............: " ${CMAKE_CXX_SIZEOF_DATA_PTR})
    message(NOTICE "    VCPKG_TARGET_TRIPLET.....................: " ${VCPKG_TARGET_TRIPLET})
    message(NOTICE "    CMAKE_DEBUG_POSTFIX......................: " ${CMAKE_DEBUG_POSTFIX})
    message(NOTICE "")
    message(NOTICE "CMake Paths")
    message(NOTICE "    CMAKE_CURRENT_SOURCE_DIR.................: " ${CMAKE_CURRENT_SOURCE_DIR})
    message(NOTICE "    CMAKE_TOOLCHAIN_FILE:....................: " ${CMAKE_TOOLCHAIN_FILE})
    message(NOTICE "    CMAKE_SOURCE_DIR:........................: " ${CMAKE_SOURCE_DIR})
    message(NOTICE "    CMAKE_COMMAND:...........................: " ${CMAKE_COMMAND})
    message(NOTICE "    CLANG_FORMAT_PROGRAM:....................: " ${CLANG_FORMAT_PROGRAM})
    message(NOTICE "    CMAKE_CXX_COMPILER:......................: " ${CMAKE_CXX_COMPILER})
    message(NOTICE "    CMAKE_LINKER:............................: " ${CMAKE_LINKER})
    message(NOTICE "    CMAKE_BUILD_TOOL:........................: " ${CMAKE_BUILD_TOOL})
    message(NOTICE "    VCPKG_PROGRAM:...........................: " ${VCPKG_PROGRAM})
    message(NOTICE "    CMAKE_INSTALL_PREFIX:....................: " ${CMAKE_INSTALL_PREFIX})
    message(NOTICE "    CMAKE_BINARY_DIR:........................: " ${CMAKE_BINARY_DIR})
    message(NOTICE "")
    message(NOTICE "CCMath Project Options")
    message(NOTICE "  > General")
    message(NOTICE "    - CCMATH_INSTALL...........................: " ${CCMATH_INSTALL})
    message(NOTICE "    - CCMATH_BUILD_TESTS:......................: " ${CCMATH_BUILD_TESTS})
    message(NOTICE "    - CCMATH_FIND_GTEST_PACKAGE:...............: " ${CCMATH_FIND_GTEST_PACKAGE})
    message(NOTICE "    - CCMATH_BUILD_BENCHMARKS:.................: " ${CCMATH_BUILD_BENCHMARKS})
    message(NOTICE "    - CCMATH_FIND_GBENCH_PACKAGE:..............: " ${CCMATH_FIND_GBENCH_PACKAGE})
    message(NOTICE "    - CCMATH_BUILD_EXAMPLES:...................: " ${CCMATH_BUILD_EXAMPLES})
    message(NOTICE "    - CCMATH_ENABLE_USER_DEFINED_MACROS:.......: " ${CCMATH_ENABLE_USER_DEFINED_MACROS})
    message(NOTICE "    - CCMATH_ENABLE_EXTENSIONS:................: " ${CCMATH_ENABLE_EXTENSIONS})
    message(NOTICE "    - CCMATH_DISABLE_ERRNO:....................: " ${CCMATH_DISABLE_ERRNO})
    message(NOTICE "    - CCMATH_ENABLE_RUNTIME_SIMD:..............: " ${CCMATH_ENABLE_RUNTIME_SIMD})
    message(NOTICE "    - CCMATH_DISABLE_SVML_USAGE:...............: " ${CCMATH_DISABLE_SVML_USAGE})
    message(NOTICE "    - CCMATH_DISABLE_CMAKE_FEATURE_CHECKS:.....: " ${CCMATH_DISABLE_CMAKE_FEATURE_CHECKS})
    message(NOTICE "    - CCMATH_DISABLE_CMAKE_SIMD_CHECKS:........: " ${CCMATH_DISABLE_CMAKE_SIMD_CHECKS})
    message(NOTICE "    - CCMATH_DISABLE_CMAKE_BUILTIN_CHECKS:.....: " ${CCMATH_DISABLE_CMAKE_BUILTIN_CHECKS})
    message(NOTICE "    - CCMATH_STRICT_WARNINGS:..................: " ${CCMATH_STRICT_WARNINGS})
    message(NOTICE "    - CCMATH_ENABLE_AUTOFORMAT_SRC:............: " ${CCMATH_ENABLE_AUTOFORMAT_SRC})
    message(NOTICE "")

    if (CCMATH_BUILD_BENCHMARKS)
        message(NOTICE "  > Benchmarks")
        message(NOTICE "    - CCM_BENCH_BASIC:.........................: " ${CCMATH_FIND_GTEST_PACKAGE})
        message(NOTICE "    - CCM_BENCH_COMPARE:.......................: " ${CCMATH_FIND_GTEST_PACKAGE})
        message(NOTICE "    - CCM_BENCH_POWER:.........................: " ${CCMATH_FIND_GTEST_PACKAGE})
        message(NOTICE "    - CCM_BENCH_BASIC:.........................: " ${CCMATH_FIND_GTEST_PACKAGE})
        message(NOTICE "    - CCM_BENCH_NEAREST:.......................: " ${CCMATH_FIND_GTEST_PACKAGE})
        message(NOTICE "    - CCM_BENCH_ALL:...........................: " ${CCMATH_FIND_GTEST_PACKAGE})
    endif()

endfunction(print_project_variables)

print_project_variables()
