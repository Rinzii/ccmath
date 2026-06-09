# Oracle campaign registry

OracleCampaignRegistry.cmake lists MPFR and CORE-MATH executables and ctests. tests/rigorous/CMakeLists.txt calls ccmath_apply_oracle_registries from cmake/helpers/CcmOracleHelpers.cmake.

## New executable

Add a thin main (or a runner under tests/shared/oracle/runners/), then in the registry:

1. Append the ID to CCMATH_ORACLE_MPFR_EXECUTABLE_IDS or CCMATH_ORACLE_COREMATH_EXECUTABLE_IDS.
2. Set CCMATH_ORACLE_<BACKEND>_<id>_TARGET, _SOURCE, _CTESTS.
3. For each ctest ID set NAME, TARGET, TIMEOUT, LABELS, ARGS. Optional REQUIRES, LOG_OUTPUT, JSON_OUTPUT.

Pow uses runners/mpfr_pow_runner.hpp and runners/coremath_pow_runner.hpp.

## Configure

MPFR: CCMATH_ENABLE_MPFR_TESTS=ON. CORE-MATH: CCMATH_ENABLE_COREMATH_TESTS=ON.

Logs usually go under tests/rigorous/oracle_logs/.
