# Oracle campaign registry

OracleCampaignRegistry.cmake lists MPFR and CORE-MATH executables and ctests. tests/rigorous/CMakeLists.txt calls ccmath_apply_oracle_registries from cmake/helpers/CcmOracleHelpers.cmake.

## New executable

Add a thin main (or a runner under tests/shared/oracle/runners/), then in the registry:

1. Append the ID to CCMATH_ORACLE_MPFR_EXECUTABLE_IDS or CCMATH_ORACLE_COREMATH_EXECUTABLE_IDS.
2. Set CCMATH_ORACLE_<BACKEND>_<id>_TARGET, _SOURCE, _CTESTS.
3. For each ctest ID set NAME, TARGET, TIMEOUT, LABELS, ARGS. Optional REQUIRES, LOG_OUTPUT, JSON_OUTPUT.

Pow uses runners/mpfr_pow_runner.hpp and runners/coremath_pow_runner.hpp.

## Unary elementary functions

mpfr_unary.cpp is one executable that measures every unary elementary function against a
correctly-rounded MPFR reference, selected with --function and --type. mpfr_unary_common.hpp holds
the shared run_campaign, which samples binary64 over a per-function domain and can sweep binary32
exhaustively with --exhaustive. Adding a function is one case in the dispatch in mpfr_unary.cpp.
tools/measure_ulp.sh drives the full sweep and tools/ulp_status.py folds the summaries into
docs/ULP_STATUS.md.

Every run prints a ulp_histogram line bucketing the finite scored cases by ULP distance (0, 1, 2, 3,
4, then 5 and beyond), so the summary shows how close to correctly rounded a function is, not only its
worst case. The same buckets are written to the summary JSON.

--impl picks which implementation is measured: gen for the generic kernel (the default and the thing
ccmath owns), public for the ccm:: public path, or libm for the system math library. The public path
lowers to libm under FE_TONEAREST on clang and gcc, so it matches libm in round-to-nearest and shows
its own behaviour in the directed modes. The choice is recorded in the output line and the summary
JSON, so gen and libm numbers are never confused.

To aim a deep check at one region, clip any argument and the corpus switches to a confined region
sample. The bounds are --lo and --hi for a value range, or --scale=log with --min-exp and --max-exp
to sample evenly across binades. An unsuffixed flag applies to every argument and a suffix picks one
argument, so for pow --lo0 and --hi0 clip the base while --min-exp1 and --max-exp1 clip the exponent.
Add --confine to drop the structured edges and sample the region only, and --progress for a live
percent, throughput, and ETA line on stderr during long runs. A sampled sweep drops exact-bit repeat
inputs by default, so no value is measured twice and the average is not double counted. Pass
--allow-duplicates to keep them, which skips the dedup sort on a very large sample over a vast space
such as pow, where repeats are statistically absent anyway.

Exhaustive binary32 can cover a sub-range instead of all 2^32 patterns. A finite --lo and --hi with
--exhaustive sweeps every float in that range, and --exhaustive-range=START:END sweeps an explicit
inclusive interval of bit patterns (decimal or 0x hex). This makes a full sweep of a single binade,
such as every float in [1, 2], a sub-second check.

## Configure

MPFR: CCMATH_ENABLE_MPFR_TESTS=ON. CORE-MATH: CCMATH_ENABLE_COREMATH_TESTS=ON.

Logs usually go under tests/rigorous/oracle_logs/.
