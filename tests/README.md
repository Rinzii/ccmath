# Test tooling

Two CMake entry points register tests from registries. Sources live under tests/src and tests/conformance and are picked up by globs or registry SOURCE paths.

## Suites

| CMake entry | Registry | Label | Targets |
| --- | --- | --- | --- |
| tests/unit/CMakeLists.txt | cmake/config/TestModuleRegistry.cmake | simple | ccmath-simple-* |
| tests/rigorous/CMakeLists.txt | cmake/config/OracleCampaignRegistry.cmake, plus CcmTestHelpers conformance calls | rigorous | ccmath-rigorous-* |

Unit helpers: cmake/helpers/CcmTestHelpers.cmake. Oracle helpers: cmake/helpers/CcmOracleHelpers.cmake. Full registry index: cmake/README.md.

## Unit coverage

Drop tests/src/math/<module>/*_test.cpp into a registered module and rebuild the matching ccmath-simple-* target.

New module: set CCMATH_UNIT_MODULE_foo and append foo to CCMATH_UNIT_MATH_MODULE_ORDER in TestModuleRegistry.cmake. Custom compile flags need CCMATH_UNIT_MODULE_foo_EXCLUDE and a separate target in tests/unit/CMakeLists.txt (see basic and fma_constexpr_upward).

## Rigorous coverage

See tests/rigorous/README.md for conformance globs, dedicated targets, and oracle wiring in tests/rigorous/CMakeLists.txt. Oracle campaign fields: tests/shared/oracle/README.md.

## Run

```bash
ctest -L simple --output-on-failure
ctest -L rigorous --output-on-failure
```

ctest labels roll up through ccmath-ctest-simple and ccmath-ctest-rigorous in cmake/config/TestCtestTargets.cmake.

Oracle backends need CCMATH_ENABLE_MPFR_TESTS=ON and/or CCMATH_ENABLE_COREMATH_TESTS=ON at configure time.

Proof freshness smoke: tests/proofs/README.md (wired as ccmath-rigorous-pow-proof-smoke from tests/rigorous/CMakeLists.txt).
