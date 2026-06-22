include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/TestOracleTargets.cmake)

set(CCMATH_ORACLE_LOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/oracle_logs)
file(MAKE_DIRECTORY ${CCMATH_ORACLE_LOG_DIR})

ccmath_add_conformance_suite(ccmath-rigorous-conformance
        APPLY_POW_VALIDATION
        EXCLUDE
        conformance/fenv_disabled_test.cpp
        conformance/fma_software_fenv_test.cpp
        conformance/powl_platform_test.cpp)

ccmath_add_rigorous_gtest_suite(ccmath-rigorous-fma-software
        SOURCES ../conformance/fma_software_fenv_test.cpp
        COMPILE_DEFINITIONS CCM_CONFIG_DISABLE_RUNTIME_BUILTIN_FMA)

ccmath_add_rigorous_gtest_suite(ccmath-rigorous-fenv-disabled
        SOURCES ../conformance/fenv_disabled_test.cpp
        COMPILE_DEFINITIONS CCM_CONFIG_DISABLE_FENV)

ccmath_add_rigorous_module_suite(ccmath-rigorous-power-boundaries
        MODULE power
        APPLY_POW_VALIDATION
        PATTERNS
        pow_integer_boundary_test.cpp
        powl_*_test.cpp)

ccmath_add_rigorous_gtest_suite(ccmath-rigorous-powl-platform
        SOURCES ../conformance/powl_platform_test.cpp
        APPLY_POW_VALIDATION)

if (CCMATH_ENABLE_MPFR_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/FindMpfrGmp.cmake)
    if (CCMATH_MPFR_FOUND)
        ccmath_add_rigorous_oracle_target(ccmath-rigorous-mpfr-big-int
                BACKEND MPFR
                SOURCES ../shared/oracle/big_int_gmp.cpp)
        ccmath_add_oracle_ctest(ccmath-rigorous-mpfr-big-int
                TARGET ccmath-rigorous-mpfr-big-int
                LABELS rigorous mpfr
                TIMEOUT 180)
    endif ()
endif ()

if (CCMATH_ENABLE_COREMATH_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/FindCoreMath.cmake)
endif ()

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/config/OracleCampaignRegistry.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/helpers/CcmOracleHelpers.cmake)
ccmath_apply_oracle_registries(${CCMATH_ORACLE_LOG_DIR})

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/helpers/CcmOptionalTooling.cmake)
ccmath_add_pow_proof_smoke_target()
ccmath_add_pow_proof_ulp_aspiration_target()
