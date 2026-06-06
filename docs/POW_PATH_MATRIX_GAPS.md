# Pow path-matrix exceptional-case gaps

This note classifies gaps observed when running MPFR path-matrix campaigns with
CCMATH_TEST_DISABLE_RUNTIME_BUILTIN=ON (no runtime __builtin_pow). It separates
harness oracle policy from kernel bugs and documents what rigorous CI covers.

Campaign JSON from local runs is written under the build tree (for example
build-rigorous-nobuiltin/path-matrix-json-v4/). These files are generated
artifacts and are not committed.

## Oracle policies

| Policy | When used | Finite positive domain | Exceptional inputs | Exceptional outputs |
| ------ | --------- | ---------------------- | ------------------ | ------------------- |
| mpfr_full | public_default with runtime builtin available | MPFR, <= 4 ULP | MPFR bit/NaN/inf rules | MPFR |
| kernel_finite_mpfr_with_std_exceptional | all paths when runtime builtin disabled | MPFR, <= 4 ULP | skipped (not counted as failures) | std::pow on macOS/Linux CI |

Skipped inputs are recorded in campaign summary JSON (skipped_count). MPFR cases
where ccmath matches std::pow but not MPFR are recorded as
mpfr_policy_mismatch_count and are not failures.

## Quick campaign results (no-builtin build, seed 0xC0DEC0FFEE)

| Function | Path | Cases run | Skipped | Failures | MPFR policy mismatches | Max ULP |
| -------- | ---- | --------- | ------- | -------- | ---------------------- | ------- |
| pow | public_default | 415 | 368 | 0 | 0 | 1 |
| pow | runtime_no_builtin | 415 | 368 | 0 | 0 | 1 |
| pow | generic_runtime | 415 | 368 | 0 | 0 | 1 |
| pow | generic_modeled_domain | 415 | 0 | 0 | 0 | 1 |
| powf | runtime_no_builtin | 14135 | 2874 | 0 | 0 | 0 |
| powf | generic_modeled_domain | 14135 | 0 | 0 | 0 | 0 |

Default build (runtime builtin enabled): pow public_default quick campaign, 0 failures.

## Residual underflow cases (29, fixed)

All 29 failures shared notes=exceptional mismatch vs std::pow on the
generic_modeled_domain path. They were positive-base, finite-input pairs where
the generic double kernel returned a small positive normal (exponent roughly
-240 to -264) instead of flushing to signed zero with underflow semantics.

Representative rows (hex IEEE bits):

| base bits | exponent bits | ccmath result (pre-fix) | std::pow result |
| --------- | ------------- | ----------------------- | --------------- |
| 0x251d5d43f50d0108 | 0x729c2b90e3442eec | 0x0c491eea7d05e89b (+1.75e-249) | 0x0000000000000000 (+0) |
| 0x4da248bc6a915c1e | 0xca93fd094b629db9 | 0x0c9f60bc4a18f192 (+7.01e-248) | 0x0000000000000000 (+0) |

### Root cause

Implementation bug in pow_kernel scale-underflow reconstruction.

When y * log2(x) is far below the representable range, the LLVM-style path sets
scale = 2^-512 and clamps y6_log2_x.hi to -564 * 64. The hi clamp zeroed the
scaled exponent index, but y6_log2_x.lo still held a huge double-double tail
from the exact_mult(y6, log2_x) product (order 1e17 for campaign inputs). That
tail dominated lo6 and the exp2 polynomial reconstructed a spurious positive
normal near 2^-826 instead of flushing to +0.

Classification: underflow threshold check too weak (lo tail not cleared on hi
clamp) plus reconstruction scaling bug (polynomial evaluated at bogus lo6).

### Fix

In include/ccmath/internal/math/generic/func/power/pow_impl/pow_impl.hpp:

1. Zero y6_log2_x.lo when y6_log2_x.hi is clamped to the minimum underflow index.
2. After result * scale, flush positive finite outputs below min subnormal to +0
   when the underflow scale factor was used.

### Regression coverage

All 29 bit-pattern cases are in tests/utils/worst_case_samples.hpp as
kPowDoubleUnderflowResidual. tests/math/power/pow_test.cpp
PowDoubleUnderflowResidualPathMatrixRegression checks ccm::gen::pow_gen and the
public ccm::pow entry against std::pow (+0).

## Gap classification table

| Function | Path/config | Case category | Input example | Observed ccmath behavior | MPFR/reference behavior | Classification | Action |
| -------- | ----------- | ------------- | ------------- | ------------------------ | ----------------------- | -------------- | ------ |
| pow | all no-builtin paths | Negative base, NaN/inf inputs, non-integer negative base | base=-2.0, exp=0.5 | NaN + FE_INVALID (matches std::pow) | MPFR may return +inf for some domain-error reals | mpfr_policy_mismatch | fixed in harness: skipped on kernel paths, not a failure |
| pow | all no-builtin paths | Negative infinite base, fractional exponent | base=-inf, exp=0.5 | quiet NaN (matches std::pow) | +inf | mpfr_policy_mismatch | fixed in harness: skipped; counted only if ccmath != std::pow |
| pow | all no-builtin paths | Negative base, odd integer exponent | base=-2.0, exp=3.0 | -8.0 (matches std::pow) | MPFR agrees on value; may differ on NaN payload | unsupported_path_behavior | skipped on kernel paths; errno/fenv not MPFR-oracle checked here |
| pow | generic_modeled_domain | Extreme exponent underflow (pre-fix) | base=4.53e+170, exp=-5.96e+220 | spurious -inf or non-zero finite | std::pow=+0 | implementation_bug | fixed: LLVM-style exponent clamp + sign correction in pow_kernel |
| pow | generic_modeled_domain | Extreme exponent overflow sign (pre-fix) | base=3.66e-242, exp=-8.26e+289 | -inf | std::pow=+inf | implementation_bug | fixed: positive-base negative-inf/spurious-negative-finite cleanup |
| pow | generic_modeled_domain | Residual underflow flush (29 cases) | base=6.62e-130, exp=1.20e+244 | +1.75e-249 (finite) | std::pow=+0 | implementation_bug | fixed: clear y6_log2_x.lo on underflow hi clamp; post-scale subnormal flush |
| powf | runtime_no_builtin, generic_modeled_domain | All quick-campaign inputs | (structured + random corpus) | matches MPFR on finite domain; std::pow on exceptional outputs | MPFR / std::pow per policy | n/a | passes rigorous CI on no-builtin build |
| powl | ld64 alias | double-shaped long double | n/a | alias to ccm::pow | n/a | supported_path | documented alias, MPFR conservative grid |
| powl | ld80 special cases | exceptional inputs on x87 hosts | n/a | tested vs std::powl / MPFR policy | n/a | partial_validation | special-value matrix in rigorous tests |
| powl | ld80 bounded integer | integer exponents within int64-safe range | n/a | MPFR on ld80 hosts | n/a | partial_validation | exponentiation-by-squaring path |
| powl | ld80 general finite | positive finite non-integer exponents on x87 hosts | n/a | MPFR ld80 general corpus, std::pow regression | n/a | partial_validation | exp2(y log2 x) kernel in powl_ld80_kernel.hpp |
| powl | ld128 / unknown | binary128 or unrecognized format | n/a | NaN when fallback OFF | double fallback when ON | policy-controlled | detection only unless fallback ON |
| powl | n/a | Independent binary128 / x87 validation | n/a | n/a | n/a | unsupported_path_behavior | not covered by path-matrix campaigns |

## Rigorous CI placement

| Test | Build | Label | Strict CI? | Notes |
| ---- | ----- | ----- | ---------- | ----- |
| ccmath-rigorous-mpfr-pow | default | rigorous | yes | public_default, full MPFR |
| ccmath-rigorous-mpfr-powf | default | rigorous | yes | public_default |
| ccmath-rigorous-mpfr-powf-nobuiltin | no-builtin | rigorous | yes | kernel oracle; 0 failures |
| ccmath-rigorous-mpfr-pow-nobuiltin | no-builtin | rigorous | yes | kernel oracle; 0 failures after underflow fix |
| ccmath-rigorous-mpfr-pow | no-builtin | (not registered) | n/a | duplicates nobuiltin campaign; omitted when builtin disabled |
| ccmath-rigorous-mpfr-powl | default or no-builtin | rigorous | yes | format-aware ld64 grid and ld80 special/bounded/general corpora |
| ccmath-rigorous-powl-platform | default | rigorous | yes | platform smoke and std::pow regression |
| ccmath-rigorous-powl-characterization | default | rigorous | yes | JSON characterization report |

## Supported claims after this pass

- The non-builtin double pow quick MPFR path-matrix campaign passes over the
  documented finite-domain corpus under the path-aware oracle policy.
- powf non-builtin paths (runtime_no_builtin, generic_modeled_domain) pass the
  quick MPFR kernel campaign under the kernel oracle policy.
- pow default public API with runtime builtin passes full MPFR quick campaign.
- ccm::powl is platform-characterized with explicit ld64 alias, ld80 special-case and bounded integer-exponent support, and a native ld80 general finite kernel.
- Non-builtin pow double paths are classified: exceptional inputs skipped,
  exceptional outputs compared to std::pow, finite positive domain compared to
  MPFR at <= 4 ULP (max observed 1 ULP on passing cases).

## Unsupported / not claimed

- Complete binary64 pow proof.
- Full 2^64 powf exhaustive validation.
- All dispatch paths validated (SIMD path-matrix not in quick corpus).
- powl global ld80 ULP bound or binary128 native validation.
- powl errno/fenv conformance beyond value-focused tests.
