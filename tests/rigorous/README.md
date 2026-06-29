# Rigorous test tooling

tests/rigorous/CMakeLists.txt wires expensive GTest targets and oracle campaigns. CTest label: rigorous.

## What this file registers

| Target | Mechanism |
| --- | --- |
| ccmath-rigorous-conformance | ccmath_add_conformance_suite on tests/conformance/ |
| ccmath-rigorous-fma-software | Dedicated suite, CCM_CONFIG_DISABLE_RUNTIME_BUILTIN_FMA |
| ccmath-rigorous-power-boundaries | ccmath_add_rigorous_module_suite on power, filename patterns |
| ccmath-rigorous-powl-platform | Dedicated conformance source |
| ccmath-rigorous-mpfr-*, ccmath-rigorous-coremath-* | ccmath_apply_oracle_registries |
| ccmath-rigorous-pow-proof-smoke | Custom target when Python3, gappa, and sollya are found (see ../proofs/README.md) |

Conformance glob excludes fma_software_fenv_test.cpp and powl_platform_test.cpp because those need separate compile flags or APPLY_POW_VALIDATION wiring.

Oracle logs go to tests/rigorous/oracle_logs/ (CCMATH_ORACLE_LOG_DIR).

## Add coverage

New conformance GTest: drop a file in tests/conformance/. It joins ccmath-rigorous-conformance unless you add it to EXCLUDE and register a dedicated ccmath_add_rigorous_gtest_suite call here.

Module subset from tests/src/math/: add a ccmath_add_rigorous_module_suite block with PATTERNS (see power-boundaries).

Oracle campaign: register in cmake/config/OracleCampaignRegistry.cmake only. See tests/shared/oracle/README.md.

## Configure

MPFR: CCMATH_ENABLE_MPFR_TESTS=ON. CORE-MATH: CCMATH_ENABLE_COREMATH_TESTS=ON.

## Run

```bash
ctest -L rigorous --output-on-failure
```

Roll-up target: ccmath-ctest-rigorous in cmake/config/TestCtestTargets.cmake.

Parent overview: ../README.md
