# `pow` Validation Policy

This repository separates lightweight math testing from heavier validation.

## Test Projects

- `tests/simple`
  Contains ordinary unit tests, regression cases, and lightweight comparisons that are suitable for every CI matrix run.
- `tests/rigorous`
  Contains heavier conformance checks, integer/parity boundary tests, MPFR-backed oracle campaigns, proof-smoke hooks, and adversarial-search tooling.

Build them separately:

```sh
cmake -S . -B build-simple -DCCMATH_BUILD_TESTS=ON -DCCMATH_BUILD_SIMPLE_TESTS=ON -DCCMATH_BUILD_RIGOROUS_TESTS=OFF
ctest --test-dir build-simple -L simple --output-on-failure

cmake -S . -B build-rigorous -DCCMATH_BUILD_TESTS=ON -DCCMATH_BUILD_SIMPLE_TESTS=OFF -DCCMATH_BUILD_RIGOROUS_TESTS=ON -DCCMATH_ENABLE_MPFR_TESTS=ON -DCCMATH_ENABLE_POW_SEARCH_TOOLS=ON
ctest --test-dir build-rigorous -L rigorous --output-on-failure
```

## Path Forcing Options

Validation builds may set these CMake options. FMA on/off is controlled through the global `CCMATH_TARGET_CPU_HAS_FMA` switch via compiler flags, not a separate pow-specific override.

| CMake option | Effect |
| ---- | ---- |
| `CCMATH_TEST_DISABLE_RUNTIME_BUILTIN` | Skips runtime `__builtin_pow` in `pow_rt` for validation targets |
| `CCMATH_TEST_DISABLE_FMA` | Adds `-fno-fma` on x86 validation targets when supported |
| `CCMATH_TEST_FORCE_FMA` | Adds `-mfma` on x86 validation targets when supported |
| `CCMATH_DISABLE_RUNTIME_SIMD` | Disables runtime SIMD dispatch globally |
| `CCMATH_ENABLE_REDUCED_PRECISION_POWL` | Allows incomplete powl tiers to use double `pow_impl` fallback (default OFF) |

Harness path names (CLI `--path=`):

| Path | Meaning |
| ---- | ---- |
| `public_default` | Public `ccm::pow` / `ccm::powf` at runtime |
| `generic_runtime` | Direct `gen::pow_gen` |
| `generic_modeled_domain` | `gen::pow_gen` on finite positive-base modeled cases |
| `runtime_no_builtin` | `pow_rt` with runtime builtin disabled at configure time |
| `runtime_simd` | Direct `simd_impl::pow_simd_impl` when compiled |
| `runtime_builtin` | Direct `builtin::runtime_pow` when available |

Unsupported paths are skipped with an explicit reason in `pow_path_config_test` and oracle tools.

## Function And Path Scope

| Function | Path | Rigorous CI evidence | Not supported by that evidence |
| ---- | ---- | ---- | ---- |
| `ccm::pow` | `public_default` | MPFR quick campaign on structured binary64 pairs | Exhaustive binary64 validation, end-to-end proof, every internal dispatch path |
| `ccm::pow` | `runtime_no_builtin`, `generic_modeled_domain` | No-builtin rigorous build (`ccmath-rigorous-mpfr-pow-nobuiltin`, `rigorous` label) | Quick MPFR path-matrix campaign under kernel oracle policy; not exhaustive binary64 validation |
| `ccm::powf` | `public_default` | MPFR quick campaign on structured binary32 pairs | Full ordered-pair binary32 validation |
| `ccm::powf` | Reduced domains | `ccmath-rigorous-mpfr-powf-reduced` quick campaign | Release-mode exhaustive domains in ordinary CI |
| `ccm::powl` | ld64 (`LongDoubleFormat::Double`) | `powl_ld64_alias_test`, `ccmath-rigorous-mpfr-powl` conservative grid | Independent extended-precision validation |
| `ccm::powl` | ld80 special cases, bounded integer, general finite | `powl_ld80_*` rigorous tests, `ccmath-rigorous-mpfr-powl` ld80 corpora | Global ld80 ULP bound, errno/fenv proof |
| `ccm::powl` | ld128 / unknown detection | `powl_format_detection_test`, `powl_unsupported_tier_test` | Independent binary128 powl, IBM double-double powl |
| `ccm::powl` | Platform characterized | `powl_platform_characterization`, `ccmath-rigorous-powl-platform` | errno/fenv proof for powl |
| Proof kernels | `generic_non_fma_kernel` | Proof-smoke when Sollya/Gappa available | Public API end-to-end proof |

The default MPFR harness uses `--path=public_default`. Generic and no-builtin paths are opt-in and may report exceptional-case disagreements outside the modeled kernel scope.

## Campaign Modes

| Mode | Use |
| ---- | ---- |
| `quick` | Ordinary rigorous CI |
| `extended` | Nightly or manual validation |
| `release` | Pre-release manual validation (alias `full` accepted) |

Powf domain filters (`--domain=`): `mantissa-sweep`, `all-x-for-y`, `all-y-for-x`, `exponent-field-sweep`, `subnormal-x`, `unit-interval`, `negative-base`, `overflow-threshold`, `underflow-threshold`, `structured-corpus`.

Example commands:

```sh
./build-rigorous/tests/rigorous/ccmath-rigorous-mpfr-pow --mode=quick --path=public_default
./build-rigorous/tests/rigorous/ccmath-rigorous-mpfr-powf --mode=extended --domain=mantissa-sweep,subnormal-x
./build-rigorous/tests/rigorous/ccmath-rigorous-mpfr-powl --mode=quick
./build-rigorous/tests/rigorous/ccmath-rigorous-pow-search --mode=near-one --count=256 --path=public_default
cmake --build build-rigorous --target ccmath-rigorous-pow-proof-smoke
```

Campaign summaries are written as `mpfr-pow*-summary.json` and `mpfr-powl-summary.json`. Adversarial search writes `pow-search-results.json` plus a `.meta.json` sidecar.

## Powl Fallback Policy

`CCMATH_ENABLE_REDUCED_PRECISION_POWL` controls whether incomplete powl tiers may delegate to the double `pow_impl` path.

| Setting | Behavior |
| ---- | ---- |
| OFF (default) | Strict validation. ld128/unknown tiers return quiet NaN from `gen::pow_gen`. ld80 general finite uses the native kernel. |
| ON | Compatibility mode. ld128/unknown may use reduced-precision double fallback. Must not be reported as native long-double support. |

Validation configuration names include `powl-strict` or `powl-fallback`.

Dispatch-path summary:

| Format | Detection | Native Phase 1 behavior | When fallback OFF | When fallback ON |
| ---- | ---- | ---- | ---- | ---- |
| ld64 | digits=53, max_exp=1024 | double alias via `pow_impl` | same | same |
| ld80 | digits=64, max_exp=16384 | special cases, bounded integer exponents, general finite kernel | ld128/unknown return NaN | ld128/unknown use double fallback |
| ld128 | digits=113, max_exp=16384 | none | all inputs return NaN from `gen::pow_gen` | double fallback |
| unknown | other | none | all inputs return NaN from `gen::pow_gen` | double fallback |

The MPFR powl harness skips reduced-precision and unsupported paths rather than counting them as native validation passes. Each case records `path`, `reduced_precision`, and `disposition` in `mpfr-powl-summary.json`.

## Promoting Adversarial Findings

1. Copy the failing pair from search JSON into `tests/utils/worst_case_samples.hpp` with a provenance string naming the search mode and seed.
2. Re-run the MPFR quick campaign on `public_default`.
3. Add a GTest case if the disagreement involves exceptional-case policy rather than ordinary ULP error.

## Cross-Libm Differential Testing

Optional tool: `ccmath-rigorous-cross-libm-pow` when `CCMATH_ENABLE_CROSS_LIBM_TESTS=ON`.

```sh
cmake -S . -B build-cross-libm -DCCMATH_BUILD_RIGOROUS_TESTS=ON -DCCMATH_ENABLE_MPFR_TESTS=ON -DCCMATH_ENABLE_CROSS_LIBM_TESTS=ON
./build-cross-libm/tests/rigorous/ccmath-rigorous-cross-libm-pow --corpus=quick --format=json
```

This reports disagreements between available libm backends and MPFR. It is bug-finding infrastructure, not correctness proof.

## Exceptional Cases

- Quiet-NaN and signaling-NaN behavior is tested in the rigorous conformance suite to the extent the platform exposes it.
- Floating-point exception flags are tested where the repository claims flag behavior.
- `errno` is only checked when the active build advertises errno-setting math error handling.

## Release Gates

| Tier | Required checks |
| ---- | ---- |
| Every commit | `tests/simple` |
| Pull request | Full `tests/simple` matrix plus Linux rigorous label |
| Nightly | Extended MPFR, path-matrix builds, proof-smoke when tools exist |
| Release | Multi-platform MPFR, proof freshness, worst-case replay, documentation audit |
| Research only | Cross-libm, release-mode powf domains, broad adversarial search |

## Claims That Remain Unsupported

- A complete end-to-end binary64 proof for `ccm::pow`
- A complete ordered-pair binary32 validation for `ccm::powf`
- Complete x87 80-bit `ccm::powl` ULP guarantee for all inputs
- Reduced-precision double fallback results counted as native powl validation (unless fallback explicitly enabled and labeled)
- Independent binary128 or IBM double-double `ccm::powl`
- Global `ccm::powl` correctness or errno/fenv proof
- A blanket statement that every builtin, SIMD, fallback, FMA, and generic path is equally validated

## Supported `ccm::powl` Claims

- `ccm::powl` is platform-characterized via `LongDoubleFormat` detection (`digits` and `max_exponent`).
- On ld64 platforms, `gen::pow_gen<long double>` is a documented alias to the double `pow_impl` path.
- On ld80 platforms, `powl_gen` delegates to `bit80::powl_calc_80bits` for special cases, bounded integer exponents (`|exp| <= 2^62 - 1`), and general positive finite inputs via `powl_ld80_general_finite`.
- ld128 and unknown formats are detection-only unless fallback is explicitly enabled.
- MPFR harness `ccmath-rigorous-mpfr-powl` validates native paths (including ld80 general finite on x87 hosts) and reports fallback policy state in JSON output.

Prefer language such as:

- "observed max ULP over this MPFR-backed quick campaign on `public_default`"
- "proof-smoke verified kernel constants for the modeled generic path"
- "compared against platform `std::pow`, not an oracle"
