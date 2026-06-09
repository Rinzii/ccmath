# Convenience targets that run labeled ctest suites from the build tree root.

include_guard(GLOBAL)

if (NOT CCMATH_BUILD_TESTS)
    return ()
endif ()

include(ProcessorCount)
ProcessorCount(_ccmath_ctest_job_count)
if (_ccmath_ctest_job_count EQUAL 0)
    set(_ccmath_ctest_job_count 1)
endif ()

function(_ccmath_add_ctest_label_target TARGET_NAME TEST_LABEL)
    set(_ctest_args --output-on-failure -j ${_ccmath_ctest_job_count})

    if (CMAKE_CONFIGURATION_TYPES)
        list(APPEND _ctest_args -C $<CONFIG>)
    endif ()

    if (TEST_LABEL)
        list(APPEND _ctest_args -L ${TEST_LABEL})
    endif ()

    if (TEST_LABEL)
        set(_ctest_comment "Running ctest label ${TEST_LABEL}")
    else ()
        set(_ctest_comment "Running all ctest tests")
    endif ()

    add_custom_target(${TARGET_NAME}
            COMMAND ${CMAKE_CTEST_COMMAND} ${_ctest_args}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            USES_TERMINAL
            COMMENT ${_ctest_comment}
    )
endfunction()

if (CCMATH_BUILD_SIMPLE_TESTS)
    _ccmath_add_ctest_label_target(ccmath-ctest-simple simple)
endif ()

if (CCMATH_BUILD_RIGOROUS_TESTS)
    _ccmath_add_ctest_label_target(ccmath-ctest-rigorous rigorous)
    _ccmath_add_ctest_label_target(ccmath-ctest-aspirational aspirational)

    if (CCMATH_ENABLE_MPFR_TESTS)
        _ccmath_add_ctest_label_target(ccmath-ctest-rigorous-mpfr mpfr)
    endif ()

    if (CCMATH_ENABLE_COREMATH_TESTS)
        _ccmath_add_ctest_label_target(ccmath-ctest-rigorous-coremath coremath)
    endif ()
endif ()

_ccmath_add_ctest_label_target(ccmath-ctest-all "")
