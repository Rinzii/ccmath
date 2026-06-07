# Path classification

Every asmlab report attempts to classify the isolated assembly region. Classification
is conservative. When evidence is insufficient, the category is `unknown`.

## Categories

| Category | Meaning |
| --- | --- |
| `ccmath_public_wrapper` | Public API wrapper with dispatch, may call internal kernel |
| `ccmath_internal_kernel` | Inlined or direct internal ccmath kernel |
| `ccmath_constexpr_path` | Constexpr probe harness |
| `ccmath_runtime_path` | Substantial runtime body without external calls |
| `compiler_builtin_or_intrinsic` | Compiler builtin or LLVM intrinsic lowering |
| `hardware_instruction_lowering` | Hardware instruction (e.g. fsqrt) |
| `external_libm_call` | External libm call; **kernel not measured** |
| `special_case_path` | Branchy special-case handling |
| `finite_normal_hot_path` | Normal finite input hot path |
| `bounded_integer_path` | Integer-exponent or similar fast path |
| `long_double_ld64_alias_path` | long double via double alias |
| `long_double_ld80_kernel_path` | Native ld80 kernel |
| `long_double_ld128_fallback_path` | ld128 fallback |
| `unknown` | Insufficient evidence |

## Red flags

If the region contains `pow`, `_pow`, `pow@PLT`, or similar external symbols, the
report states that the internal ccmath kernel was **not** measured. Optimize
`pow_impl` or `powf_impl` entries instead of public API when tuning kernels.

Hardware sqrt (`fsqrt`, `vsqrtsd`) is often the intended fast path for `sqrt`. It is
still classified explicitly as `hardware_instruction_lowering`.

## Confidence

Each classification includes `high`, `medium`, or `low` confidence and a list of
evidence strings. Registry `intended_path` is cross-checked but does not override
strong contradictory assembly evidence.

## Harness modes affect classification

| Mode | Effect |
| --- | --- |
| `runtime_flatten` | Inlines callees; good for `*_impl` / `*_rt` kernels |
| `runtime_no_flatten` | Observes wrapper shape and out-of-line calls |
| `constexpr_probe` | Compile-time only; no runtime assembly |
| `path_direct` | Same as runtime_flatten for direct path entries |
