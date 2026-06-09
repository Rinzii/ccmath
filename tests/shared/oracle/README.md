# Oracle Harness Notes

The rigorous oracle layer is split into two parts:

- `cmake/config/TestOracleTargets.cmake`
  Registers MPFR and CORE-MATH executables and their `ctest` variants without re-writing the backend link and include logic in each suite file.
- `tests/oracle/mpfr_oracle_harness.hpp` and `tests/oracle/coremath_oracle_harness.hpp`
  Provide shared binary-function campaign evaluators for the common runner loop: rounding-mode handling, skip accounting, failure records, and summary bookkeeping.

## Adding a New Binary Oracle Campaign

1. Add a corpus builder that produces `binary_case<T>` records for the function inputs you want to validate.
2. Implement the function-specific reference callback.
   MPFR campaigns provide an `(x, y, precision, rounding)` callback.
   CORE-MATH campaigns provide an `(x, y)` callback and, when needed, an optional higher-precision truth cross-check for known oracle defects.
3. Call the shared evaluator from the executable.
   Use `evaluate_binary_mpfr_case(...)` or `evaluate_binary_coremath_case_all_modes(...)`.
4. Register the executable in [tests/rigorous/CMakeLists.txt](/Users/ianpike/Dev/ccmath/tests/rigorous/CMakeLists.txt) with:

```cmake
ccmath_add_rigorous_oracle_target(my-target BACKEND MPFR SOURCES ../oracle/my_file.cpp)
ccmath_add_rigorous_oracle_test(my-test my-target LABELS rigorous mpfr TIMEOUT 300 ARGS --mode=quick)
```

## Event logs

Pass `--log-output=<file>.json` (or `--json-output=`) to write a running log of mismatches and failures. Rigorous ctest targets under `tests/rigorous/CMakeLists.txt` write into [tests/rigorous/oracle_logs/README.md](../rigorous/oracle_logs/README.md).

Each record carries `event_kind`, bit patterns for base/exponent/actual/expected, rounding mode, and provenance. MPFR campaigns also log `mpfr_above_target` cases separately from hard failures.

## Scope

The shared backend helpers currently target binary scalar campaigns, which covers the current `pow`/`powf` path-matrix work and other two-argument functions that will want the same infrastructure. Unary campaigns can add a parallel adapter later without changing the CMake registration layer.
