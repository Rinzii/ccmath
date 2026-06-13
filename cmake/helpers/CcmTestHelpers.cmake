# Helpers for registering ccmath GTest suites, oracle executables, and script tests.
# Layout follows LLVM-libc style: sources under tests/src mirror include/ccmath,
# shared harness under tests/shared, wiring under tests/unit and tests/rigorous.

include_guard(GLOBAL)

if (NOT DEFINED CCMATH_TESTS_ROOT)
    message(FATAL_ERROR "CCMATH_TESTS_ROOT must be set before including CcmTestHelpers.cmake")
endif ()

function(ccmath_module_rel_dir MODULE OUT_VAR)
    if (MODULE STREQUAL "ext")
        set(_rel src/ext)
    elseif (MODULE MATCHES "^internal/")
        set(_rel src/math/${MODULE})
    else ()
        set(_rel src/math/${MODULE})
    endif ()
    set(${OUT_VAR} ${_rel} PARENT_SCOPE)
endfunction()

function(ccmath_collect_test_sources SOURCE_DIR OUT_VAR)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs EXCLUDE PATTERNS)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (IS_ABSOLUTE "${SOURCE_DIR}")
        set(_abs_dir "${SOURCE_DIR}")
    else ()
        set(_abs_dir "${CCMATH_TESTS_ROOT}/${SOURCE_DIR}")
    endif ()

    if (NOT EXISTS "${_abs_dir}")
        message(FATAL_ERROR "ccmath_collect_test_sources: directory does not exist: ${_abs_dir}")
    endif ()

    if (NOT CCM_PATTERNS)
        set(CCM_PATTERNS "*_test.cpp" "lerp_test_*.cpp")
    endif ()

    set(_candidates "")
    foreach (_pattern IN LISTS CCM_PATTERNS)
        file(GLOB _matches
                LIST_DIRECTORIES false
                RELATIVE "${CCMATH_TESTS_ROOT}"
                "${_abs_dir}/${_pattern}")
        list(APPEND _candidates ${_matches})
    endforeach ()

    if (CCM_EXCLUDE)
        foreach (_path IN LISTS CCM_EXCLUDE)
            list(REMOVE_ITEM _candidates "${_path}")
        endforeach ()
    endif ()

    list(REMOVE_DUPLICATES _candidates)
    list(SORT _candidates)
    set(${OUT_VAR} ${_candidates} PARENT_SCOPE)
endfunction()

function(ccmath_prefix_test_sources REL_PREFIX SOURCES OUT_VAR)
    set(_prefixed "")
    foreach (_source IN LISTS SOURCES)
        list(APPEND _prefixed "${REL_PREFIX}${_source}")
    endforeach ()
    set(${OUT_VAR} ${_prefixed} PARENT_SCOPE)
endfunction()

function(ccmath_add_gtest_suite TARGET)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES LABELS COMPILE_DEFINITIONS COMPILE_OPTIONS)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_SOURCES)
        message(FATAL_ERROR "ccmath_add_gtest_suite(${TARGET}): SOURCES is required")
    endif ()

    add_executable(${TARGET})
    target_sources(${TARGET} PRIVATE ${CCM_SOURCES})
    target_link_libraries(${TARGET} PRIVATE ccmath::test)
    add_test(NAME ${TARGET} COMMAND ${TARGET})

    if (CCM_LABELS)
        set_tests_properties(${TARGET} PROPERTIES LABELS "${CCM_LABELS}")
    endif ()
    if (CCM_COMPILE_DEFINITIONS)
        target_compile_definitions(${TARGET} PRIVATE ${CCM_COMPILE_DEFINITIONS})
    endif ()
    if (CCM_COMPILE_OPTIONS)
        target_compile_options(${TARGET} PRIVATE ${CCM_COMPILE_OPTIONS})
    endif ()
endfunction()

function(ccmath_add_command_gtest TARGET)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES LABELS COMPILE_DEFINITIONS COMPILE_OPTIONS)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_SOURCES)
        message(FATAL_ERROR "ccmath_add_command_gtest(${TARGET}): SOURCES is required")
    endif ()

    add_executable(${TARGET} ${CCM_SOURCES})
    target_link_libraries(${TARGET} PRIVATE ccmath::ccmath ccmath::ext)
    target_include_directories(${TARGET} PRIVATE
            ${CCMATH_TESTS_ROOT}
            ${CCMATH_TESTS_ROOT}/shared
            ${CCMATH_TESTS_ROOT}/..)
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    add_test(NAME ${TARGET} COMMAND ${TARGET})

    if (CCM_LABELS)
        set_tests_properties(${TARGET} PROPERTIES LABELS "${CCM_LABELS}")
    endif ()
    if (CCM_COMPILE_DEFINITIONS)
        target_compile_definitions(${TARGET} PRIVATE ${CCM_COMPILE_DEFINITIONS})
    endif ()
    if (CCM_COMPILE_OPTIONS)
        target_compile_options(${TARGET} PRIVATE ${CCM_COMPILE_OPTIONS})
    endif ()
endfunction()

function(ccmath_add_unit_module)
    set(options)
    set(oneValueArgs TARGET MODULE)
    set(multiValueArgs LABELS EXTRA_SOURCES EXCLUDE PATTERNS)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_TARGET OR NOT CCM_MODULE)
        message(FATAL_ERROR "ccmath_add_unit_module requires TARGET and MODULE")
    endif ()

    ccmath_module_rel_dir(${CCM_MODULE} _rel_dir)
    ccmath_collect_test_sources(${_rel_dir} _sources
            EXCLUDE ${CCM_EXCLUDE}
            PATTERNS ${CCM_PATTERNS})
    ccmath_prefix_test_sources("../" "${_sources}" _prefixed)

    if (CCM_EXTRA_SOURCES)
        list(APPEND _prefixed ${CCM_EXTRA_SOURCES})
    endif ()

    if (NOT _prefixed)
        message(FATAL_ERROR "ccmath_add_unit_module(${CCM_TARGET}): no test sources in ${_rel_dir}")
    endif ()

    ccmath_add_gtest_suite(${CCM_TARGET} SOURCES ${_prefixed} LABELS ${CCM_LABELS})
endfunction()

function(ccmath_add_rigorous_gtest_suite TARGET)
    set(options APPLY_POW_VALIDATION)
    set(oneValueArgs)
    set(multiValueArgs SOURCES LABELS COMPILE_DEFINITIONS COMPILE_OPTIONS)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_SOURCES)
        message(FATAL_ERROR "ccmath_add_rigorous_gtest_suite(${TARGET}): SOURCES is required")
    endif ()

    if (NOT CCM_LABELS)
        set(CCM_LABELS rigorous)
    endif ()

    ccmath_add_gtest_suite(${TARGET}
            SOURCES ${CCM_SOURCES}
            LABELS ${CCM_LABELS}
            COMPILE_DEFINITIONS ${CCM_COMPILE_DEFINITIONS}
            COMPILE_OPTIONS ${CCM_COMPILE_OPTIONS})

    if (CCM_APPLY_POW_VALIDATION)
        if (NOT COMMAND ccmath_apply_pow_validation_options)
            message(FATAL_ERROR "ccmath_add_rigorous_gtest_suite(${TARGET}): include TestValidationOptions.cmake first")
        endif ()
        ccmath_apply_pow_validation_options(${TARGET})
    endif ()
endfunction()

function(ccmath_add_rigorous_module_suite TARGET)
    set(options APPLY_POW_VALIDATION)
    set(oneValueArgs MODULE)
    set(multiValueArgs LABELS EXCLUDE PATTERNS EXTRA_SOURCES)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_MODULE)
        message(FATAL_ERROR "ccmath_add_rigorous_module_suite(${TARGET}): MODULE is required")
    endif ()

    ccmath_module_rel_dir(${CCM_MODULE} _rel_dir)
    ccmath_collect_test_sources(${_rel_dir} _sources
            EXCLUDE ${CCM_EXCLUDE}
            PATTERNS ${CCM_PATTERNS})
    ccmath_prefix_test_sources("../" "${_sources}" _prefixed)

    if (CCM_EXTRA_SOURCES)
        list(APPEND _prefixed ${CCM_EXTRA_SOURCES})
    endif ()

    if (NOT _prefixed)
        message(FATAL_ERROR "ccmath_add_rigorous_module_suite(${TARGET}): no test sources in ${_rel_dir}")
    endif ()

    if (CCM_APPLY_POW_VALIDATION)
        ccmath_add_rigorous_gtest_suite(${TARGET}
                SOURCES ${_prefixed}
                LABELS ${CCM_LABELS}
                APPLY_POW_VALIDATION)
    else ()
        ccmath_add_rigorous_gtest_suite(${TARGET}
                SOURCES ${_prefixed}
                LABELS ${CCM_LABELS})
    endif ()
endfunction()

function(ccmath_add_conformance_suite TARGET)
    set(options APPLY_POW_VALIDATION)
    set(oneValueArgs)
    set(multiValueArgs EXCLUDE EXTRA_SOURCES)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    ccmath_collect_test_sources(conformance _sources EXCLUDE ${CCM_EXCLUDE})
    ccmath_prefix_test_sources("../" "${_sources}" _prefixed)

    if (CCM_EXTRA_SOURCES)
        list(APPEND _prefixed ${CCM_EXTRA_SOURCES})
    endif ()

    if (CCM_APPLY_POW_VALIDATION)
        ccmath_add_rigorous_gtest_suite(${TARGET}
                SOURCES ${_prefixed}
                APPLY_POW_VALIDATION)
    else ()
        ccmath_add_rigorous_gtest_suite(${TARGET} SOURCES ${_prefixed})
    endif ()
endfunction()

function(ccmath_add_python_script_test TEST_NAME)
    set(options OPTIONAL)
    set(oneValueArgs SCRIPT WORKING_DIRECTORY TIMEOUT)
    set(multiValueArgs LABELS ARGS)
    cmake_parse_arguments(CCM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_SCRIPT)
        message(FATAL_ERROR "ccmath_add_python_script_test(${TEST_NAME}): SCRIPT is required")
    endif ()
    if (NOT Python3_EXECUTABLE)
        if (CCM_OPTIONAL)
            message(WARNING "CCMath: Python3 not found; ${TEST_NAME} skipped")
            return ()
        endif ()
        message(FATAL_ERROR "ccmath_add_python_script_test(${TEST_NAME}): Python3 not available")
    endif ()

    set(_command ${Python3_EXECUTABLE} ${CCM_SCRIPT})
    if (CCM_ARGS)
        list(APPEND _command ${CCM_ARGS})
    endif ()

    if (NOT CCM_WORKING_DIRECTORY)
        set(CCM_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif ()

    add_test(NAME ${TEST_NAME} COMMAND ${_command})
    set_tests_properties(${TEST_NAME} PROPERTIES WORKING_DIRECTORY "${CCM_WORKING_DIRECTORY}")

    if (CCM_LABELS)
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "${CCM_LABELS}")
    endif ()
    if (CCM_TIMEOUT)
        set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT ${CCM_TIMEOUT})
    endif ()
endfunction()

# Back-compat aliases used by older CMakeLists fragments.
function(ccmath_add_test TEST_TARGET)
    ccmath_add_gtest_suite(${TEST_TARGET} SOURCES ${ARGN})
endfunction()

function(ccmath_add_command_test TEST_TARGET)
    ccmath_add_command_gtest(${TEST_TARGET} SOURCES ${ARGN})
endfunction()
