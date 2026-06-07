ccmath_add_test(ccmath-simple-basic
        ../math/basic/abs_test.cpp
        ../math/basic/fdim_test.cpp
        ../math/basic/fma_test.cpp
        ../math/basic/fmod_test.cpp
        ../math/basic/max_test.cpp
        ../math/basic/min_test.cpp
        ../math/basic/nan_test.cpp
        ../math/basic/remainder_test.cpp
        ../math/basic/remquo_test.cpp
)
set_tests_properties(ccmath-simple-basic PROPERTIES LABELS simple)

ccmath_add_test(ccmath-simple-compare
        ../math/compare/fpclassify_test.cpp
        ../math/compare/isfinite_test.cpp
        ../math/compare/isgreater_test.cpp
        ../math/compare/isgreaterequal_test.cpp
        ../math/compare/isinf_test.cpp
        ../math/compare/isless_test.cpp
        ../math/compare/islessequal_test.cpp
        ../math/compare/islessgreater_test.cpp
        ../math/compare/isnan_test.cpp
        ../math/compare/isnormal_test.cpp
        ../math/compare/isunordered_test.cpp
        ../math/compare/signbit_test.cpp
)
set_tests_properties(ccmath-simple-compare PROPERTIES LABELS simple)

ccmath_add_test(ccmath-simple-exponential
        ../math/exponential/exp2_test.cpp
        ../math/exponential/exp_test.cpp
        ../math/exponential/expm1_test.cpp
        ../math/exponential/log1p_test.cpp
        ../math/exponential/log2_test.cpp
        ../math/exponential/log10_test.cpp
        ../math/exponential/log_test.cpp
        ../math/exponential/ulp_accuracy_test.cpp
)
set_tests_properties(ccmath-simple-exponential PROPERTIES LABELS simple)

ccmath_add_test(ccmath-simple-fmanip
        ../math/fmanip/copysign_test.cpp
        ../math/fmanip/frexp_test.cpp
        ../math/fmanip/ilogb_test.cpp
        ../math/fmanip/ldexp_test.cpp
        ../math/fmanip/logb_test.cpp
        ../math/fmanip/modf_test.cpp
        ../math/fmanip/nextafter_test.cpp
        ../math/fmanip/nexttoward_test.cpp
        ../math/fmanip/scalbn_test.cpp
)
set_tests_properties(ccmath-simple-fmanip PROPERTIES LABELS simple)

ccmath_add_test(ccmath-simple-nearest
        ../math/nearest/ceil_round_rint_test.cpp
        ../math/nearest/floor_test.cpp
        ../math/nearest/nearbyint_test.cpp
        ../math/nearest/trunc_test.cpp
)
set_tests_properties(ccmath-simple-nearest PROPERTIES LABELS simple)

ccmath_add_test(ccmath-simple-power
        ../math/power/hypot_cbrt_test.cpp
        ../math/power/pow_test.cpp
        ../math/power/powl_format_detection_test.cpp
        ../math/power/powl_fallback_policy_test.cpp
        ../math/power/powl_ld64_alias_test.cpp
        ../math/power/sqrt_test.cpp
        ../math/power/ulp_accuracy_test.cpp
)
set_tests_properties(ccmath-simple-power PROPERTIES LABELS simple)

ccmath_add_test(ccmath-simple-trigonometric
        ../math/trigonometric/inverse_trig_test.cpp
        ../math/trigonometric/sin_test.cpp
        ../math/trigonometric/ulp_accuracy_test.cpp
)
set_tests_properties(ccmath-simple-trigonometric PROPERTIES LABELS simple)

if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
    ccmath_add_test(ccmath-simple-misc
            ../math/misc/lerp_test_std.cpp
            ../math/misc/gamma_test.cpp
            ../math/misc/lgamma_test.cpp
    )
else ()
    ccmath_add_test(ccmath-simple-misc
            ../math/misc/lerp_test_no_std.cpp
            ../math/misc/gamma_test.cpp
            ../math/misc/lgamma_test.cpp
    )
endif ()
set_tests_properties(ccmath-simple-misc PROPERTIES LABELS simple)

ccmath_add_test(ccmath-simple-internal-types
        ../math/internal/types/big_int_test.cpp
        ../math/internal/types/dyadic_float_test.cpp
)
set_tests_properties(ccmath-simple-internal-types PROPERTIES LABELS simple)

ccmath_add_test(ccmath-simple-ext
        ../ext/ext_helpers_test.cpp
)
set_tests_properties(ccmath-simple-ext PROPERTIES LABELS simple)

find_package(Python3 COMPONENTS Interpreter QUIET)
if (Python3_FOUND)
    add_test(NAME asmlab-powf-impl-bench-wiring
            COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/tools/asmlab/scripts/test_bench_wiring.py
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_tests_properties(asmlab-powf-impl-bench-wiring PROPERTIES LABELS simple)
    add_test(NAME asmlab-golden-quick-wiring
            COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/tools/asmlab/scripts/test_golden_quick.py
            GoldenWiringTest
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_tests_properties(asmlab-golden-quick-wiring PROPERTIES LABELS simple)
    add_test(NAME asmlab-golden-quick
            COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/tools/asmlab/scripts/run_golden_analysis.py
            --quick
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_tests_properties(asmlab-golden-quick PROPERTIES LABELS simple)
endif ()
