# CMake tooling

Helpers in cmake/helpers/ and module lists in cmake/config/. Family CMakeLists.txt files include a registry and call ccmath_add_* helpers instead of listing every source file.

## Registries

| Registry | Helper | Wired from |
| --- | --- | --- |
| TestModuleRegistry.cmake | CcmTestHelpers.cmake | tests/unit/CMakeLists.txt |
| OracleCampaignRegistry.cmake | CcmOracleHelpers.cmake | tests/rigorous/CMakeLists.txt |
| FuzzModuleRegistry.cmake | CcmFuzzHelpers.cmake | fuzzing/CMakeLists.txt |
| BenchmarkModuleRegistry.cmake | CcmBenchHelpers.cmake | benchmarks/CMakeLists.txt |
| ExampleModuleRegistry.cmake | CcmExampleHelpers.cmake | examples/CMakeLists.txt |

TestCtestTargets.cmake defines ccmath-ctest-simple and ccmath-ctest-rigorous. TestOracleTargets.cmake links MPFR and CORE-MATH. cmake/modules/CcmLibFuzzer.cmake probes libFuzzer.

## Family docs

| Area | README |
| --- | --- |
| GTest suites | tests/README.md |
| Rigorous wiring | tests/rigorous/README.md |
| Proof smoke | tests/proofs/README.md |
| Oracle campaigns | tests/shared/oracle/README.md |
| Fuzz | fuzzing/README.md |
| Benchmarks | benchmarks/README.md |
| Examples | examples/README.md |

## Helpers (quick ref)

CcmTestHelpers.cmake needs CCMATH_TESTS_ROOT set first (tests/CMakeLists.txt does this). ccmath_add_unit_module globs tests/src/math/<module>/. ccmath_add_conformance_suite and ccmath_add_rigorous_module_suite cover rigorous GTest subsets.

CcmFuzzHelpers.cmake needs CCMATH_FUZZ_ROOT. ccmath_add_fuzz_module builds the executable, seeds, smoke CTest, and campaign target.

CcmBenchHelpers.cmake needs CCMATH_BENCH_ROOT. ccmath_apply_bench_registry loops BenchmarkModuleRegistry.cmake.

CcmExampleHelpers.cmake needs CCMATH_EXAMPLE_ROOT. ccmath_apply_example_registry loops ExampleModuleRegistry.cmake.

CcmOracleHelpers.cmake: ccmath_apply_oracle_registries(LOG_DIR) reads OracleCampaignRegistry.cmake.

Dedicated targets stay in the family CMakeLists.txt when a source file needs its own COMPILE_DEFINITIONS or must be excluded from a glob.
