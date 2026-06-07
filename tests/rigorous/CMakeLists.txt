include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/TestOracleTargets.cmake)

ccmath_add_test(ccmath-rigorous-conformance
        ../math/conformance/fenv_exception_test.cpp
        ../math/conformance/fenv_probe_test.cpp
        ../math/conformance/include_policy_test.cpp
        ../math/conformance/pow_errno_fenv_test.cpp
        ../math/conformance/pow_path_config_test.cpp
        ../math/conformance/rounding_conformance_test.cpp
        ../math/conformance/ulp_all_modes_test.cpp
        ../math/conformance/ulp_distance_test.cpp
        ../math/conformance/worst_case_ulp_test.cpp
)
set_tests_properties(ccmath-rigorous-conformance PROPERTIES LABELS rigorous)
ccmath_apply_pow_validation_options(ccmath-rigorous-conformance)

ccmath_add_test(ccmath-rigorous-power-boundaries
        ../math/power/pow_integer_boundary_test.cpp
        ../math/power/powl_ld80_integer_class_test.cpp
        ../math/power/powl_ld80_special_cases_test.cpp
        ../math/power/powl_ld80_bounded_int_test.cpp
        ../math/power/powl_ld80_general_finite_test.cpp
        ../math/power/powl_unsupported_tier_test.cpp
)
set_tests_properties(ccmath-rigorous-power-boundaries PROPERTIES LABELS rigorous)
ccmath_apply_pow_validation_options(ccmath-rigorous-power-boundaries)

ccmath_add_test(ccmath-rigorous-powl-platform
        ../math/conformance/powl_platform_test.cpp
)
set_tests_properties(ccmath-rigorous-powl-platform PROPERTIES LABELS rigorous)
ccmath_apply_pow_validation_options(ccmath-rigorous-powl-platform)

if (CCMATH_ENABLE_MPFR_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/FindMpfrGmp.cmake)

    if (CCMATH_MPFR_FOUND)
        ccmath_add_rigorous_oracle_target(ccmath-rigorous-mpfr-pow BACKEND MPFR SOURCES ../oracle/mpfr_pow_double.cpp)
        if (NOT CCMATH_TEST_DISABLE_RUNTIME_BUILTIN)
            ccmath_add_rigorous_oracle_test(ccmath-rigorous-mpfr-pow ccmath-rigorous-mpfr-pow
                    LABELS rigorous mpfr
                    TIMEOUT 600
                    ARGS --mode=quick --path=public_default)
        endif ()

        ccmath_add_rigorous_oracle_target(ccmath-rigorous-mpfr-powf BACKEND MPFR SOURCES ../oracle/mpfr_pow_float.cpp)
        ccmath_add_rigorous_oracle_test(ccmath-rigorous-mpfr-powf ccmath-rigorous-mpfr-powf
                LABELS rigorous mpfr
                TIMEOUT 900
                ARGS --mode=quick --path=public_default)

        ccmath_add_rigorous_oracle_test(ccmath-rigorous-mpfr-powf-reduced ccmath-rigorous-mpfr-powf
                LABELS rigorous mpfr
                TIMEOUT 900
                ARGS --mode=quick --path=public_default --domain=mantissa-sweep,subnormal-x)

        if (CCMATH_TEST_DISABLE_RUNTIME_BUILTIN)
            ccmath_add_rigorous_oracle_test(ccmath-rigorous-mpfr-pow-nobuiltin ccmath-rigorous-mpfr-pow
                    LABELS rigorous mpfr
                    TIMEOUT 600
                    ARGS --mode=quick --path=runtime_no_builtin,generic_modeled_domain)

            ccmath_add_rigorous_oracle_test(ccmath-rigorous-mpfr-powf-nobuiltin ccmath-rigorous-mpfr-powf
                    LABELS rigorous mpfr
                    TIMEOUT 900
                    ARGS --mode=quick --path=runtime_no_builtin,generic_modeled_domain)
        endif ()

        if (CCMATH_ENABLE_POW_SEARCH_TOOLS)
            ccmath_add_rigorous_oracle_target(ccmath-rigorous-pow-search BACKEND MPFR SOURCES ../oracle/pow_adversarial_search.cpp)
            ccmath_add_rigorous_oracle_test(ccmath-rigorous-pow-search-smoke ccmath-rigorous-pow-search
                    LABELS rigorous mpfr
                    TIMEOUT 60
                    ARGS --mode=near-one --count=256 --path=public_default)
        endif ()

        ccmath_add_rigorous_oracle_target(ccmath-rigorous-powl-characterization BACKEND MPFR SOURCES ../oracle/powl_platform_characterization.cpp)
        ccmath_add_rigorous_oracle_test(ccmath-rigorous-powl-characterization ccmath-rigorous-powl-characterization
                LABELS rigorous mpfr
                TIMEOUT 120
                ARGS --json-output=powl-platform.json)

        ccmath_add_rigorous_oracle_target(ccmath-rigorous-mpfr-powl BACKEND MPFR SOURCES ../oracle/mpfr_powl.cpp)
        ccmath_add_rigorous_oracle_test(ccmath-rigorous-mpfr-powl ccmath-rigorous-mpfr-powl
                LABELS rigorous mpfr
                TIMEOUT 300
                ARGS --mode=quick)
    endif ()
endif ()

if (CCMATH_ENABLE_COREMATH_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/FindCoreMath.cmake)

    if (CCMATH_COREMATH_FOUND AND TARGET ccmath::coremath_oracle)
        ccmath_add_rigorous_oracle_target(ccmath-rigorous-coremath-pow BACKEND COREMATH SOURCES ../oracle/coremath_pow_double.cpp)
        ccmath_add_rigorous_oracle_test(ccmath-rigorous-coremath-pow ccmath-rigorous-coremath-pow
                LABELS rigorous coremath
                TIMEOUT 900
                ARGS --mode=quick --path=public_default --rounding-modes=all)

        ccmath_add_rigorous_oracle_target(ccmath-rigorous-coremath-powf BACKEND COREMATH SOURCES ../oracle/coremath_pow_float.cpp)
        ccmath_add_rigorous_oracle_test(ccmath-rigorous-coremath-powf ccmath-rigorous-coremath-powf
                LABELS rigorous coremath
                TIMEOUT 1200
                ARGS --mode=quick --path=public_default --rounding-modes=all)

        ccmath_add_rigorous_oracle_test(ccmath-rigorous-coremath-powf-reduced ccmath-rigorous-coremath-powf
                LABELS rigorous coremath
                TIMEOUT 1200
                ARGS --mode=quick --path=public_default --rounding-modes=all --domain=mantissa-sweep,subnormal-x)
    endif ()
endif ()

if (CCMATH_ENABLE_CROSS_LIBM_TESTS AND CCMATH_ENABLE_MPFR_TESTS)
    if (CCMATH_MPFR_FOUND)
        ccmath_add_rigorous_oracle_target(ccmath-rigorous-cross-libm-pow BACKEND MPFR SOURCES ../oracle/cross_libm_pow.cpp)
    endif ()
endif ()

find_package(Python3 COMPONENTS Interpreter QUIET)
find_program(GAPPA_EXECUTABLE gappa)
find_program(SOLLYA_EXECUTABLE sollya)

if (Python3_Interpreter_FOUND AND GAPPA_EXECUTABLE AND SOLLYA_EXECUTABLE)
    add_custom_target(ccmath-rigorous-pow-proof-smoke
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../proofs/math/power/proof_freshness_check.py --smoke-only
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
            COMMENT "Running pow proof smoke checks against current source constants")
endif ()
