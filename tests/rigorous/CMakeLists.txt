include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/TestOracleTargets.cmake)

set(CCMATH_ORACLE_LOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/oracle_logs)
file(MAKE_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})

ccmath_add_test(ccmath-rigorous-conformance
        ../math/conformance/fenv_exception_test.cpp
        ../math/conformance/fenv_probe_test.cpp
        ../math/conformance/fma_rounding_test.cpp
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

ccmath_add_test(ccmath-rigorous-fma-software
        ../math/conformance/fma_software_fenv_test.cpp
)
target_compile_definitions(ccmath-rigorous-fma-software PRIVATE CCM_CONFIG_DISABLE_RUNTIME_BUILTIN_FMA)
set_tests_properties(ccmath-rigorous-fma-software PROPERTIES LABELS rigorous)

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
        ccmath_ensure_mpfr_oracle_target(ccmath-rigorous-mpfr-pow SOURCES ../oracle/mpfr_pow_double.cpp)
        if (NOT CCMATH_TEST_DISABLE_RUNTIME_BUILTIN)
            ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-pow
                    TARGET ccmath-rigorous-mpfr-pow
                    LABELS rigorous mpfr
                    TIMEOUT 600
                    ARGS --mode=quick --path=public_default
                    LOG_OUTPUT mpfr-pow-double-public_default.json
                    WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})
        endif ()

        ccmath_ensure_mpfr_oracle_target(ccmath-rigorous-mpfr-powf SOURCES ../oracle/mpfr_pow_float.cpp)
        ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-powf
                TARGET ccmath-rigorous-mpfr-powf
                LABELS rigorous mpfr
                TIMEOUT 900
                ARGS --mode=quick --path=public_default
                LOG_OUTPUT mpfr-powf-public_default.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})
        ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-powf-reduced
                TARGET ccmath-rigorous-mpfr-powf
                LABELS rigorous mpfr
                TIMEOUT 900
                ARGS --mode=quick --path=public_default --domain=mantissa-sweep,subnormal-x
                LOG_OUTPUT mpfr-powf-reduced-domains.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})

        ccmath_ensure_mpfr_oracle_target(ccmath-rigorous-mpfr-fma SOURCES ../oracle/mpfr_fma_double.cpp)
        ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-fma
                TARGET ccmath-rigorous-mpfr-fma
                LABELS rigorous mpfr
                TIMEOUT 600
                ARGS --mode=quick --path=public_default,software_fallback,quiet_generic --rounding-modes=all
                LOG_OUTPUT mpfr-fma-double.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})

        ccmath_ensure_mpfr_oracle_target(ccmath-rigorous-mpfr-fmaf SOURCES ../oracle/mpfr_fma_float.cpp)
        ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-fmaf
                TARGET ccmath-rigorous-mpfr-fmaf
                LABELS rigorous mpfr
                TIMEOUT 600
                ARGS --mode=quick --path=public_default,software_fallback,quiet_generic --rounding-modes=all
                LOG_OUTPUT mpfr-fmaf.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})

        if (CCMATH_TEST_DISABLE_RUNTIME_BUILTIN)
            ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-pow-nobuiltin
                    TARGET ccmath-rigorous-mpfr-pow
                    LABELS rigorous mpfr
                    TIMEOUT 600
                    ARGS --mode=quick --path=runtime_no_builtin,generic_modeled_domain
                    LOG_OUTPUT mpfr-pow-double-nobuiltin.json
                    WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})
            ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-powf-nobuiltin
                    TARGET ccmath-rigorous-mpfr-powf
                    LABELS rigorous mpfr
                    TIMEOUT 900
                    ARGS --mode=quick --path=runtime_no_builtin,generic_modeled_domain
                    LOG_OUTPUT mpfr-powf-nobuiltin.json
                    WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})
        endif ()

        if (CCMATH_ENABLE_POW_SEARCH_TOOLS)
            ccmath_ensure_mpfr_oracle_target(ccmath-rigorous-pow-search SOURCES ../oracle/pow_adversarial_search.cpp)
            ccmath_add_oracle_ctest(ccmath-rigorous-pow-search-smoke
                    TARGET ccmath-rigorous-pow-search
                    LABELS rigorous mpfr
                    TIMEOUT 60
                    ARGS --mode=near-one --count=256 --path=public_default)
        endif ()

        ccmath_ensure_mpfr_oracle_target(ccmath-rigorous-powl-characterization SOURCES ../oracle/powl_platform_characterization.cpp)
        ccmath_add_oracle_ctest(ccmath-rigorous-powl-characterization
                TARGET ccmath-rigorous-powl-characterization
                LABELS rigorous mpfr
                TIMEOUT 120
                JSON_OUTPUT powl-platform.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})

        ccmath_ensure_mpfr_oracle_target(ccmath-rigorous-mpfr-powl SOURCES ../oracle/mpfr_powl.cpp)
        ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-powl
                TARGET ccmath-rigorous-mpfr-powl
                LABELS rigorous mpfr
                TIMEOUT 300
                ARGS --mode=quick
                JSON_OUTPUT mpfr-powl.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})

        if (CCMATH_ENABLE_CROSS_LIBM_TESTS)
            ccmath_ensure_mpfr_oracle_target(ccmath-rigorous-cross-libm-pow SOURCES ../oracle/cross_libm_pow.cpp)
            ccmath_add_oracle_ctest(ccmath-rigorous-cross-libm-pow-smoke
                    TARGET ccmath-rigorous-cross-libm-pow
                    LABELS rigorous mpfr
                    TIMEOUT 60
                    ARGS --corpus=quick --format=json --output=cross-libm-pow-quick.json
                    WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})
        endif ()
    endif ()
endif ()

if (CCMATH_ENABLE_COREMATH_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/FindCoreMath.cmake)

    if (CCMATH_COREMATH_FOUND AND TARGET ccmath::coremath_oracle)
        ccmath_ensure_coremath_oracle_target(ccmath-rigorous-coremath-pow SOURCES ../oracle/coremath_pow_double.cpp)
        # CORE-MATH is a correctly-rounded oracle, so it validates ccmath's own kernel rather than
        # the platform libm reached through the public builtin path.
        ccmath_add_oracle_ctest(ccmath-rigorous-coremath-pow
                TARGET ccmath-rigorous-coremath-pow
                LABELS rigorous coremath
                TIMEOUT 900
                ARGS --mode=quick --path=generic_runtime --rounding-modes=all
                LOG_OUTPUT coremath-pow-double-generic_runtime.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})

        ccmath_ensure_coremath_oracle_target(ccmath-rigorous-coremath-powf SOURCES ../oracle/coremath_pow_float.cpp)
        ccmath_add_oracle_ctest(ccmath-rigorous-coremath-powf
                TARGET ccmath-rigorous-coremath-powf
                LABELS rigorous coremath
                TIMEOUT 1200
                ARGS --mode=quick --path=generic_runtime --rounding-modes=all
                LOG_OUTPUT coremath-powf-generic_runtime.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})
        ccmath_add_oracle_ctest(ccmath-rigorous-coremath-powf-reduced
                TARGET ccmath-rigorous-coremath-powf
                LABELS rigorous coremath
                TIMEOUT 1200
                ARGS --mode=quick --path=generic_runtime --rounding-modes=all --domain=mantissa-sweep,subnormal-x
                LOG_OUTPUT coremath-powf-reduced-domains.json
                WORKING_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})
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
