include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/TestModuleRegistry.cmake)

foreach (_module IN LISTS CCMATH_UNIT_MATH_MODULE_ORDER)
    set(_module_args)
    set(_exclude_var CCMATH_UNIT_MODULE_${_module}_EXCLUDE)
    if (DEFINED ${_exclude_var})
        list(APPEND _module_args EXCLUDE ${${_exclude_var}})
    endif ()
    ccmath_add_unit_module(
            TARGET ${CCMATH_UNIT_MODULE_${_module}}
            MODULE ${_module}
            LABELS simple
            ${_module_args})
endforeach ()

ccmath_add_unit_module(
        TARGET ${CCMATH_UNIT_MODULE_ext}
        MODULE ext
        LABELS simple)

ccmath_add_unit_module(
        TARGET ccmath-simple-internal-types
        MODULE internal/types
        LABELS simple)

ccmath_add_unit_module(
        TARGET ccmath-simple-internal-support
        MODULE internal/support
        LABELS simple)

ccmath_add_unit_module(
        TARGET ccmath-simple-internal-simd
        MODULE internal/simd
        LABELS simple)

ccmath_add_command_gtest(ccmath-simple-constexpr-fmaf-upward
        SOURCES ../src/math/basic/fma_constexpr_upward_config_test.cpp
        LABELS simple
        COMPILE_DEFINITIONS CCM_CONSTEXPR_ROUNDING_MODE=FE_UPWARD)

ccmath_add_gtest_suite(ccmath-simple-math-constexpr
        SOURCES ../src/math/constexpr_smoke_test.cpp
        LABELS simple)

ccmath_add_gtest_suite(ccmath-simple-ext-constexpr
        SOURCES ../src/ext/constexpr_smoke_test.cpp
        LABELS simple)

ccmath_add_gtest_suite(ccmath-simple-system-math-coexistence
        SOURCES ../src/math/system_math_header_coexistence_test.cpp
        LABELS simple)

if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
    set(_lerp_source ../src/math/misc/lerp_test_std.cpp)
else ()
    set(_lerp_source ../src/math/misc/lerp_test_no_std.cpp)
endif ()

ccmath_add_unit_module(
        TARGET ccmath-simple-misc
        MODULE misc
        LABELS simple
        EXCLUDE
        src/math/misc/lerp_test_std.cpp
        src/math/misc/lerp_test_no_std.cpp
        EXTRA_SOURCES ${_lerp_source})

find_package(Python3 COMPONENTS Interpreter QUIET)
ccmath_add_python_script_test(asmlab-powf-impl-bench-wiring
        OPTIONAL
        SCRIPT ${CMAKE_SOURCE_DIR}/tools/asmlab/scripts/test_bench_wiring.py
        LABELS simple)
ccmath_add_python_script_test(asmlab-golden-quick-wiring
        OPTIONAL
        SCRIPT ${CMAKE_SOURCE_DIR}/tools/asmlab/scripts/test_golden_quick.py
        LABELS simple
        ARGS GoldenWiringTest)
ccmath_add_python_script_test(asmlab-golden-quick
        OPTIONAL
        SCRIPT ${CMAKE_SOURCE_DIR}/tools/asmlab/scripts/run_golden_analysis.py
        LABELS simple
        ARGS --quick)
