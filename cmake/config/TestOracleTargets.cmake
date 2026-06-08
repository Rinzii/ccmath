# Helper functions for rigorous MPFR and CORE-MATH oracle executables.

include_guard(GLOBAL)

function(ccmath_add_rigorous_oracle_target TARGET)
    set(options)
    set(oneValueArgs BACKEND)
    set(multiValueArgs SOURCES LIBRARIES INCLUDE_DIRECTORIES)
    cmake_parse_arguments(CCM_ORACLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_ORACLE_BACKEND)
        message(FATAL_ERROR "ccmath_add_rigorous_oracle_target requires BACKEND MPFR or COREMATH")
    endif ()
    if (NOT CCM_ORACLE_SOURCES)
        message(FATAL_ERROR "ccmath_add_rigorous_oracle_target requires at least one source file")
    endif ()

    add_executable(${TARGET})
    target_sources(${TARGET} PRIVATE ${CCM_ORACLE_SOURCES})
    target_link_libraries(${TARGET} PRIVATE ccmath::ccmath ccmath::ext)

    if (CCM_ORACLE_BACKEND STREQUAL "MPFR")
        if (NOT TARGET ccmath::mpfr_oracle)
            message(FATAL_ERROR "ccmath_add_rigorous_oracle_target(${TARGET}): ccmath::mpfr_oracle target is unavailable")
        endif ()
        target_link_libraries(${TARGET} PRIVATE ccmath::mpfr_oracle)
    elseif (CCM_ORACLE_BACKEND STREQUAL "COREMATH")
        if (NOT TARGET ccmath::coremath_oracle)
            message(FATAL_ERROR "ccmath_add_rigorous_oracle_target(${TARGET}): ccmath::coremath_oracle target is unavailable")
        endif ()
        target_link_libraries(${TARGET} PRIVATE ccmath::coremath_oracle)
        if (NOT WIN32)
            target_link_libraries(${TARGET} PRIVATE m)
        endif ()
    else ()
        message(FATAL_ERROR "ccmath_add_rigorous_oracle_target(${TARGET}): unknown BACKEND ${CCM_ORACLE_BACKEND}")
    endif ()

    if (CCM_ORACLE_LIBRARIES)
        target_link_libraries(${TARGET} PRIVATE ${CCM_ORACLE_LIBRARIES})
    endif ()

    target_include_directories(${TARGET} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/..
            ${CMAKE_CURRENT_SOURCE_DIR}/../..
            ${CCM_ORACLE_INCLUDE_DIRECTORIES}
    )
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    ccmath_apply_pow_validation_options(${TARGET})
endfunction()

function(ccmath_add_rigorous_oracle_test TEST_NAME TARGET)
    set(options)
    set(oneValueArgs TIMEOUT WORKING_DIRECTORY)
    set(multiValueArgs LABELS ARGS)
    cmake_parse_arguments(CCM_ORACLE_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT TARGET ${TARGET})
        message(FATAL_ERROR "ccmath_add_rigorous_oracle_test(${TEST_NAME}): target ${TARGET} does not exist")
    endif ()

    set(_command ${TARGET})
    if (CCM_ORACLE_TEST_ARGS)
        list(APPEND _command ${CCM_ORACLE_TEST_ARGS})
    endif ()

    add_test(NAME ${TEST_NAME} COMMAND ${_command})

    if (CCM_ORACLE_TEST_LABELS)
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "${CCM_ORACLE_TEST_LABELS}")
    endif ()
    if (CCM_ORACLE_TEST_TIMEOUT)
        set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT ${CCM_ORACLE_TEST_TIMEOUT})
    endif ()
    if (CCM_ORACLE_TEST_WORKING_DIRECTORY)
        set_tests_properties(${TEST_NAME} PROPERTIES WORKING_DIRECTORY "${CCM_ORACLE_TEST_WORKING_DIRECTORY}")
    endif ()
endfunction()

function(ccmath_ensure_mpfr_oracle_target TARGET)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(CCM_ENSURE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (TARGET ${TARGET})
        return()
    endif ()
    if (NOT CCM_ENSURE_SOURCES)
        message(FATAL_ERROR "ccmath_ensure_mpfr_oracle_target(${TARGET}): SOURCES is required")
    endif ()
    ccmath_add_rigorous_oracle_target(${TARGET} BACKEND MPFR SOURCES ${CCM_ENSURE_SOURCES})
endfunction()

function(ccmath_ensure_coremath_oracle_target TARGET)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(CCM_ENSURE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (TARGET ${TARGET})
        return()
    endif ()
    if (NOT CCM_ENSURE_SOURCES)
        message(FATAL_ERROR "ccmath_ensure_coremath_oracle_target(${TARGET}): SOURCES is required")
    endif ()
    ccmath_add_rigorous_oracle_target(${TARGET} BACKEND COREMATH SOURCES ${CCM_ENSURE_SOURCES})
endfunction()

function(ccmath_add_oracle_ctest TEST_NAME)
    set(options)
    set(oneValueArgs TARGET TIMEOUT WORKING_DIRECTORY LOG_OUTPUT JSON_OUTPUT)
    set(multiValueArgs LABELS ARGS)
    cmake_parse_arguments(CCM_CTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT CCM_CTEST_TARGET)
        message(FATAL_ERROR "ccmath_add_oracle_ctest(${TEST_NAME}): TARGET is required")
    endif ()

    set(_command_args ${CCM_CTEST_ARGS})
    if (CCM_CTEST_LOG_OUTPUT)
        list(APPEND _command_args --log-output=${CCM_CTEST_LOG_OUTPUT})
    endif ()
    if (CCM_CTEST_JSON_OUTPUT)
        list(APPEND _command_args --json-output=${CCM_CTEST_JSON_OUTPUT})
    endif ()

    ccmath_add_rigorous_oracle_test(${TEST_NAME} ${CCM_CTEST_TARGET}
            LABELS ${CCM_CTEST_LABELS}
            TIMEOUT ${CCM_CTEST_TIMEOUT}
            ARGS ${_command_args}
            WORKING_DIRECTORY ${CCM_CTEST_WORKING_DIRECTORY})
endfunction()
