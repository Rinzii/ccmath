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

function(ccmath_add_pow_oracle_target TARGET)
    add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PRIVATE ccmath::ccmath ccmath::ext ccmath::mpfr_oracle)
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../..)
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    ccmath_apply_pow_validation_options(${TARGET})
endfunction()

function(ccmath_add_coremath_oracle_target TARGET)
    add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PRIVATE ccmath::ccmath ccmath::ext ccmath::coremath_oracle)
    if (NOT WIN32)
        target_link_libraries(${TARGET} PRIVATE m)
    endif ()
    target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../..)
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    ccmath_apply_pow_validation_options(${TARGET})
endfunction()

if (CCMATH_ENABLE_MPFR_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/FindMpfrGmp.cmake)

    if (CCMATH_MPFR_FOUND)
        ccmath_add_pow_oracle_target(ccmath-rigorous-mpfr-pow ../oracle/mpfr_pow_double.cpp)
        if (NOT CCMATH_TEST_DISABLE_RUNTIME_BUILTIN)
            add_test(NAME ccmath-rigorous-mpfr-pow COMMAND ccmath-rigorous-mpfr-pow --mode=quick --path=public_default)
            set_tests_properties(ccmath-rigorous-mpfr-pow PROPERTIES LABELS "rigorous;mpfr" TIMEOUT 600)
        endif ()

        ccmath_add_pow_oracle_target(ccmath-rigorous-mpfr-powf ../oracle/mpfr_pow_float.cpp)
        add_test(NAME ccmath-rigorous-mpfr-powf COMMAND ccmath-rigorous-mpfr-powf --mode=quick --path=public_default)
        set_tests_properties(ccmath-rigorous-mpfr-powf PROPERTIES LABELS "rigorous;mpfr" TIMEOUT 900)

        add_test(NAME ccmath-rigorous-mpfr-powf-reduced
                COMMAND ccmath-rigorous-mpfr-powf --mode=quick --path=public_default --domain=mantissa-sweep,subnormal-x)
        set_tests_properties(ccmath-rigorous-mpfr-powf-reduced PROPERTIES LABELS "rigorous;mpfr" TIMEOUT 900)

        if (CCMATH_TEST_DISABLE_RUNTIME_BUILTIN)
            add_test(NAME ccmath-rigorous-mpfr-pow-nobuiltin
                    COMMAND ccmath-rigorous-mpfr-pow --mode=quick --path=runtime_no_builtin,generic_modeled_domain)
            set_tests_properties(ccmath-rigorous-mpfr-pow-nobuiltin PROPERTIES LABELS "rigorous;mpfr" TIMEOUT 600)

            add_test(NAME ccmath-rigorous-mpfr-powf-nobuiltin
                    COMMAND ccmath-rigorous-mpfr-powf --mode=quick --path=runtime_no_builtin,generic_modeled_domain)
            set_tests_properties(ccmath-rigorous-mpfr-powf-nobuiltin PROPERTIES LABELS "rigorous;mpfr" TIMEOUT 900)
        endif ()

        if (CCMATH_ENABLE_POW_SEARCH_TOOLS)
            ccmath_add_pow_oracle_target(ccmath-rigorous-pow-search ../oracle/pow_adversarial_search.cpp)
            add_test(NAME ccmath-rigorous-pow-search-smoke
                    COMMAND ccmath-rigorous-pow-search --mode=near-one --count=256 --path=public_default)
            set_tests_properties(ccmath-rigorous-pow-search-smoke PROPERTIES LABELS "rigorous;mpfr" TIMEOUT 60)
        endif ()

        ccmath_add_pow_oracle_target(ccmath-rigorous-powl-characterization ../oracle/powl_platform_characterization.cpp)
        add_test(NAME ccmath-rigorous-powl-characterization
                COMMAND ccmath-rigorous-powl-characterization --json-output=powl-platform.json)
        set_tests_properties(ccmath-rigorous-powl-characterization PROPERTIES LABELS "rigorous;mpfr" TIMEOUT 120)

        ccmath_add_pow_oracle_target(ccmath-rigorous-mpfr-powl ../oracle/mpfr_powl.cpp)
        add_test(NAME ccmath-rigorous-mpfr-powl COMMAND ccmath-rigorous-mpfr-powl --mode=quick)
        set_tests_properties(ccmath-rigorous-mpfr-powl PROPERTIES LABELS "rigorous;mpfr" TIMEOUT 300)
    endif ()
endif ()

if (CCMATH_ENABLE_COREMATH_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/FindCoreMath.cmake)

    if (CCMATH_COREMATH_FOUND AND TARGET ccmath::coremath_oracle)
        ccmath_add_coremath_oracle_target(ccmath-rigorous-coremath-pow ../oracle/coremath_pow_double.cpp)
        add_test(NAME ccmath-rigorous-coremath-pow
                COMMAND ccmath-rigorous-coremath-pow --mode=quick --path=public_default --rounding-modes=all)
        set_tests_properties(ccmath-rigorous-coremath-pow PROPERTIES LABELS "rigorous;coremath" TIMEOUT 900)

        ccmath_add_coremath_oracle_target(ccmath-rigorous-coremath-powf ../oracle/coremath_pow_float.cpp)
        add_test(NAME ccmath-rigorous-coremath-powf
                COMMAND ccmath-rigorous-coremath-powf --mode=quick --path=public_default --rounding-modes=all)
        set_tests_properties(ccmath-rigorous-coremath-powf PROPERTIES LABELS "rigorous;coremath" TIMEOUT 1200)

        add_test(NAME ccmath-rigorous-coremath-powf-reduced
                COMMAND ccmath-rigorous-coremath-powf --mode=quick --path=public_default --rounding-modes=all --domain=mantissa-sweep,subnormal-x)
        set_tests_properties(ccmath-rigorous-coremath-powf-reduced PROPERTIES LABELS "rigorous;coremath" TIMEOUT 1200)
    endif ()
endif ()

if (CCMATH_ENABLE_CROSS_LIBM_TESTS AND CCMATH_ENABLE_MPFR_TESTS)
    if (CCMATH_MPFR_FOUND)
        ccmath_add_pow_oracle_target(ccmath-rigorous-cross-libm-pow ../oracle/cross_libm_pow.cpp)
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
