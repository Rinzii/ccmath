# ULP status

ccmath aims for correctly rounded results in all four IEEE 754 rounding modes for every function.
This page records how close each function is, measured in ULP. ROUNDING_STATUS.md covers which
modes each function handles at runtime.

## How to read this

Every function falls into one of five classes.

| Class                   | Meaning                                                                                                                                                    |
|-------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Exact                   | Produces the exact representable result, so 0 ULP in all four modes by construction. The nearest-integer, sign, and scaling families are here.             |
| Correctly rounded       | The unique nearest representable value, 0.5 ULP, confirmed by an all-modes campaign. sqrt is correctly rounded by IEEE 754, and fma is exact-then-rounded. |
| Bounded                 | Measured to be within a small number of ULP of the correctly rounded value. Correct rounding is the goal, and these functions are not there yet.           |
| Not a rounding function | Returns a bool, an int, or a classification, so ULP does not apply. The comparison family is here.                                                         |
| Not implemented         | The header exists but there is no implementation yet. The hyperbolic and special-function families are here.                                               |

The numbers measure ccmath's generic kernels (--impl=gen) in round-to-nearest, not the public function. On clang and gcc the public function hands runtime
round-to-nearest to a compiler builtin, the system libm, which is often more accurate than the figures here. The generic kernel is what runs at compile time, in
the directed modes, in deterministic mode and anywhere no builtin is available. These are the portable worst case. The harness can also measure --impl=public or
--impl=libm, but this page does not report those.

Max ULP is the largest error observed between the kernel result and the correctly rounded value over the measured corpus. 0 means every result was the nearest
representable value. The reference is MPFR at high precision, rounded once to the target type. That is the true answer and not another library's result.

The Exponential, Power, Trigonometric and Miscellaneous families split each function into its float, double and long double variant (expf, exp, expl). The rest
are exact or not implemented at every type so they stay one row each.

Only binary32 is measured. Its sweep is exhaustive over all bit patterns so the max is a proven worst case. binary64 and long double are pending.

The Tested kernel column names the symbol that runs. Generic kernels are templates so one symbol instantiates at each type (ccm::gen::pow_gen<float>, ccm::gen::
pow_gen<double>). asin, acos, atan, tgamma and lgamma have separate float and double functions with no long double kernel so their long double variant uses the
double one.

To refresh the tables, run the sweep and then the generator.

```
tools/measure_ulp.sh --exhaustive-float
python3 tools/ulp_status.py
```

Function classes and the measured-function list live in tools/ulp_status_registry.json. A function with no completed campaign summary shows up as pending.

## Accuracy by function

<!-- BEGIN GENERATED: tools/ulp_status.py -->

> ccmath generic kernels (--impl=gen), round-to-nearest. MPFR correctly-rounded reference. The accuracy families break
> each function into its float, double, and long double variant (expf, exp, expl). binary32 is measured by an exhaustive
> sweep over all bit patterns, so its max is a proven worst case. binary64 and long double are pending. The Tested kernel
> column names the exact symbol that runs. Measured on apple with clang 17.0.0 (clang-1700.4.4.1). Last refreshed
> 2026-06-18T12:49:45Z.

### Basic

| Function  | Class                   | Max ULP | Avg ULP | Notes                                                                                                     |
|-----------|-------------------------|---------|---------|-----------------------------------------------------------------------------------------------------------|
| fabs      | Exact                   | 0       | 0       |                                                                                                           |
| fdim      | Exact                   | 0       | 0       |                                                                                                           |
| fma       | Correctly rounded       | 0       | 0       | Native FMA where trusted, exact software fallback otherwise. Validated by the MPFR fma all-mode campaign. |
| fmax      | Exact                   | 0       | 0       |                                                                                                           |
| fmin      | Exact                   | 0       | 0       |                                                                                                           |
| fmod      | Exact                   | 0       | 0       | Exact remainder by construction.                                                                          |
| remainder | Exact                   | 0       | 0       |                                                                                                           |
| remquo    | Exact                   | 0       | 0       |                                                                                                           |
| nan       | Not a rounding function | n/a     | n/a     | Constructs a NaN payload. No rounded result.                                                              |

### Comparison

| Function       | Class                   | Max ULP | Avg ULP | Notes |
|----------------|-------------------------|---------|---------|-------|
| fpclassify     | Not a rounding function | n/a     | n/a     |       |
| isfinite       | Not a rounding function | n/a     | n/a     |       |
| isgreater      | Not a rounding function | n/a     | n/a     |       |
| isgreaterequal | Not a rounding function | n/a     | n/a     |       |
| isinf          | Not a rounding function | n/a     | n/a     |       |
| isless         | Not a rounding function | n/a     | n/a     |       |
| islessequal    | Not a rounding function | n/a     | n/a     |       |
| islessgreater  | Not a rounding function | n/a     | n/a     |       |
| isnan          | Not a rounding function | n/a     | n/a     |       |
| isnormal       | Not a rounding function | n/a     | n/a     |       |
| isunordered    | Not a rounding function | n/a     | n/a     |       |
| signbit        | Not a rounding function | n/a     | n/a     |       |

### Exponential

| Variant              | Class   | Max ULP | Avg ULP  | Tested kernel                    | Notes |
|----------------------|---------|---------|----------|----------------------------------|-------|
| expf (binary32)      | Bounded | 1       | 7.62e-05 | ccm::gen::exp_gen<float>         |       |
| exp (binary64)       | Bounded | pending | pending  | ccm::gen::exp_gen<double>        |       |
| expl (long double)   | Bounded | pending | pending  | ccm::gen::exp_gen<long double>   |       |
| exp2f (binary32)     | Bounded | pending | pending  | ccm::gen::exp2_gen<float>        |       |
| exp2 (binary64)      | Bounded | pending | pending  | ccm::gen::exp2_gen<double>       |       |
| exp2l (long double)  | Bounded | pending | pending  | ccm::gen::exp2_gen<long double>  |       |
| expm1f (binary32)    | Bounded | pending | pending  | ccm::gen::expm1_gen<float>       |       |
| expm1 (binary64)     | Bounded | pending | pending  | ccm::gen::expm1_gen<double>      |       |
| expm1l (long double) | Bounded | pending | pending  | ccm::gen::expm1_gen<long double> |       |
| logf (binary32)      | Bounded | pending | pending  | ccm::gen::log_gen<float>         |       |
| log (binary64)       | Bounded | pending | pending  | ccm::gen::log_gen<double>        |       |
| logl (long double)   | Bounded | pending | pending  | ccm::gen::log_gen<long double>   |       |
| log1pf (binary32)    | Bounded | 2       | 0.00453  | ccm::gen::log1p_gen<float>       |       |
| log1p (binary64)     | Bounded | pending | pending  | ccm::gen::log1p_gen<double>      |       |
| log1pl (long double) | Bounded | pending | pending  | ccm::gen::log1p_gen<long double> |       |
| log2f (binary32)     | Bounded | pending | pending  | ccm::gen::log2_gen<float>        |       |
| log2 (binary64)      | Bounded | pending | pending  | ccm::gen::log2_gen<double>       |       |
| log2l (long double)  | Bounded | pending | pending  | ccm::gen::log2_gen<long double>  |       |
| log10f (binary32)    | Bounded | pending | pending  | ccm::gen::log10_gen<float>       |       |
| log10 (binary64)     | Bounded | pending | pending  | ccm::gen::log10_gen<double>      |       |
| log10l (long double) | Bounded | pending | pending  | ccm::gen::log10_gen<long double> |       |

### Float manipulation

| Function   | Class                   | Max ULP | Avg ULP | Notes                          |
|------------|-------------------------|---------|---------|--------------------------------|
| copysign   | Exact                   | 0       | 0       |                                |
| frexp      | Exact                   | 0       | 0       |                                |
| ilogb      | Not a rounding function | n/a     | n/a     | Returns an integer exponent.   |
| ldexp      | Exact                   | 0       | 0       |                                |
| logb       | Exact                   | 0       | 0       |                                |
| modf       | Exact                   | 0       | 0       |                                |
| nextafter  | Exact                   | 0       | 0       |                                |
| nexttoward | Exact                   | 0       | 0       |                                |
| nextup     | Exact                   | 0       | 0       |                                |
| nextdown   | Exact                   | 0       | 0       |                                |
| scalbn     | Exact                   | 0       | 0       | long double is slow but exact. |

### Hyperbolic

| Function | Class           | Max ULP  | Avg ULP  | Notes |
|----------|-----------------|----------|----------|-------|
| acosh    | Not implemented | not impl | not impl |       |
| asinh    | Not implemented | not impl | not impl |       |
| atanh    | Not implemented | not impl | not impl |       |
| cosh     | Not implemented | not impl | not impl |       |
| sinh     | Not implemented | not impl | not impl |       |
| tanh     | Not implemented | not impl | not impl |       |

### Nearest integer

| Function  | Class | Max ULP | Avg ULP | Notes |
|-----------|-------|---------|---------|-------|
| ceil      | Exact | 0       | 0       |       |
| floor     | Exact | 0       | 0       |       |
| nearbyint | Exact | 0       | 0       |       |
| rint      | Exact | 0       | 0       |       |
| round     | Exact | 0       | 0       |       |
| trunc     | Exact | 0       | 0       |       |

### Power

| Variant              | Class             | Max ULP | Avg ULP | Tested kernel                   | Notes                                                                                                                              |
|----------------------|-------------------|---------|---------|---------------------------------|------------------------------------------------------------------------------------------------------------------------------------|
| cbrtf (binary32)     | Bounded           | 1       | 0.296   | ccm::gen::cbrt_gen<float>       |                                                                                                                                    |
| cbrt (binary64)      | Bounded           | pending | pending | ccm::gen::cbrt_gen<double>      |                                                                                                                                    |
| cbrtl (long double)  | Bounded           | pending | pending | ccm::gen::cbrt_gen<long double> |                                                                                                                                    |
| hypotf (binary32)    | Bounded           | pending | pending |                                 | Binary argument. Measurement is pending a binary sweep harness.                                                                    |
| hypot (binary64)     | Bounded           | pending | pending |                                 |                                                                                                                                    |
| hypotl (long double) | Bounded           | pending | pending |                                 |                                                                                                                                    |
| powf (binary32)      | Bounded           | 0       | 0       | ccm::gen::pow_gen<float>        | Binary, so sampled over the finite-result band and broad bit patterns rather than exhaustive. See the pow campaign appendix below. |
| pow (binary64)       | Bounded           | pending | pending | ccm::gen::pow_gen<double>       |                                                                                                                                    |
| powl (long double)   | Bounded           | pending | pending | ccm::gen::pow_gen<long double>  |                                                                                                                                    |
| sqrtf (binary32)     | Correctly rounded | 0       | 0       | ccm::gen::sqrt_gen<float>       | Correctly rounded by IEEE 754.                                                                                                     |
| sqrt (binary64)      | Correctly rounded | pending | pending | ccm::gen::sqrt_gen<double>      |                                                                                                                                    |
| sqrtl (long double)  | Correctly rounded | pending | pending | ccm::gen::sqrt_gen<long double> |                                                                                                                                    |

### Special functions

| Function       | Class           | Max ULP  | Avg ULP  | Notes |
|----------------|-----------------|----------|----------|-------|
| assoc_laguerre | Not implemented | not impl | not impl |       |
| assoc_legendre | Not implemented | not impl | not impl |       |
| beta           | Not implemented | not impl | not impl |       |
| comp_ellint_1  | Not implemented | not impl | not impl |       |
| comp_ellint_2  | Not implemented | not impl | not impl |       |
| comp_ellint_3  | Not implemented | not impl | not impl |       |
| cyl_bessel_i   | Not implemented | not impl | not impl |       |
| cyl_bessel_j   | Not implemented | not impl | not impl |       |
| cyl_bessel_k   | Not implemented | not impl | not impl |       |
| cyl_neumann    | Not implemented | not impl | not impl |       |
| ellint_1       | Not implemented | not impl | not impl |       |
| ellint_2       | Not implemented | not impl | not impl |       |
| ellint_3       | Not implemented | not impl | not impl |       |
| expint         | Not implemented | not impl | not impl |       |
| hermite        | Not implemented | not impl | not impl |       |
| laguerre       | Not implemented | not impl | not impl |       |
| legendre       | Not implemented | not impl | not impl |       |
| riemann_zeta   | Not implemented | not impl | not impl |       |
| sph_bessel     | Not implemented | not impl | not impl |       |
| sph_legendre   | Not implemented | not impl | not impl |       |
| sph_neumann    | Not implemented | not impl | not impl |       |

### Trigonometric

| Variant              | Class   | Max ULP | Avg ULP  | Tested kernel                    | Notes                                                           |
|----------------------|---------|---------|----------|----------------------------------|-----------------------------------------------------------------|
| acosf (binary32)     | Bounded | pending | pending  | ccm::internal::impl::acos_float  |                                                                 |
| acos (binary64)      | Bounded | pending | pending  | ccm::internal::impl::acos_double |                                                                 |
| acosl (long double)  | Bounded | pending | pending  | ccm::internal::impl::acos_double |                                                                 |
| asinf (binary32)     | Bounded | pending | pending  | ccm::internal::impl::asin_float  |                                                                 |
| asin (binary64)      | Bounded | pending | pending  | ccm::internal::impl::asin_double |                                                                 |
| asinl (long double)  | Bounded | pending | pending  | ccm::internal::impl::asin_double |                                                                 |
| atanf (binary32)     | Bounded | 1       | 4.67e-10 | ccm::internal::impl::atan_float  |                                                                 |
| atan (binary64)      | Bounded | pending | pending  | ccm::internal::impl::atan_double |                                                                 |
| atanl (long double)  | Bounded | pending | pending  | ccm::internal::impl::atan_double |                                                                 |
| atan2f (binary32)    | Bounded | pending | pending  |                                  | Binary argument. Measurement is pending a binary sweep harness. |
| atan2 (binary64)     | Bounded | pending | pending  |                                  |                                                                 |
| atan2l (long double) | Bounded | pending | pending  |                                  |                                                                 |
| cosf (binary32)      | Bounded | pending | pending  | ccm::gen::cos_gen<float>         |                                                                 |
| cos (binary64)       | Bounded | pending | pending  | ccm::gen::cos_gen<double>        |                                                                 |
| cosl (long double)   | Bounded | pending | pending  | ccm::gen::cos_gen<long double>   |                                                                 |
| sinf (binary32)      | Bounded | pending | pending  | ccm::gen::sin_gen<float>         |                                                                 |
| sin (binary64)       | Bounded | pending | pending  | ccm::gen::sin_gen<double>        |                                                                 |
| sinl (long double)   | Bounded | pending | pending  | ccm::gen::sin_gen<long double>   |                                                                 |
| tanf (binary32)      | Bounded | pending | pending  | ccm::gen::tan_gen<float>         |                                                                 |
| tan (binary64)       | Bounded | pending | pending  | ccm::gen::tan_gen<double>        |                                                                 |
| tanl (long double)   | Bounded | pending | pending  | ccm::gen::tan_gen<long double>   |                                                                 |

### Miscellaneous

| Variant               | Class   | Max ULP | Avg ULP | Tested kernel                | Notes                                                                                                               |
|-----------------------|---------|---------|---------|------------------------------|---------------------------------------------------------------------------------------------------------------------|
| tgammaf (binary32)    | Bounded | pending | pending | ccm::internal::gamma_float   | Listed as gamma in earlier status docs.                                                                             |
| tgamma (binary64)     | Bounded | pending | pending | ccm::internal::gamma_double  |                                                                                                                     |
| tgammal (long double) | Bounded | pending | pending | ccm::internal::gamma_double  |                                                                                                                     |
| lerp (binary32)       | Bounded | pending | pending |                              | Ternary argument, and a C++ template with no f or l suffix variant. Measurement is pending a ternary sweep harness. |
| lerp (binary64)       | Bounded | pending | pending |                              |                                                                                                                     |
| lerp (long double)    | Bounded | pending | pending |                              |                                                                                                                     |
| lgammaf (binary32)    | Bounded | pending | pending | ccm::internal::lgamma_float  |                                                                                                                     |
| lgamma (binary64)     | Bounded | pending | pending | ccm::internal::lgamma_double |                                                                                                                     |
| lgammal (long double) | Bounded | pending | pending | ccm::internal::lgamma_double |                                                                                                                     |

<!-- END GENERATED -->

## Scope and caveats

atan2, hypot and lerp take more than one argument so they wait on a binary and ternary sweep and are marked pending. pow is measured separately, with its
campaign in the appendix.

Near a zero or a pole, conditioning governs ULP error, not rounding, because a tiny absolute error spans many representable values when the result is near zero
or very large. Functions with poles or zeros in range, such as tan at its poles and lgamma at its zeros at 1 and 2, can show a large max and average that
reflect where the corpus landed, not the kernel's typical accuracy.

## Appendix: pow campaign detail

Rigorous oracle quick campaigns (apple aarch64, clang 17, June 7 2026). Not proofs. Max ULP is the
same in all four IEEE rounding modes on the corpora below.

**Failure columns**

| Column | Meaning |
|--------|---------|
| MPFR hard failures | Cases that exceed the campaign 4 ULP ceiling, or disagree with MPFR on NaN, infinity, or signed zero. Within-ceiling results that miss the 0.5 ULP correctly-rounded target are tracked separately and are not counted here. |
| CORE-MATH bit mismatches | Cases where ccm bits differ from the CORE-MATH cr_* reference in the active rounding mode. This is not a ULP threshold. Cases where a higher-precision cross-check shows ccm is correct and the CORE-MATH oracle is wrong are excluded. |

Per-case logs (base, exponent, actual and expected bit patterns) live under
[tests/rigorous/oracle_logs/README.md](../tests/rigorous/oracle_logs/README.md). Re-run the rigorous oracle ctest targets to refresh them.

| Function  | Configuration                               | MPFR cases | MPFR Max ULP | MPFR hard failures | CORE-MATH cases | CORE-MATH Max ULP | CORE-MATH bit mismatches | Notes                                                                             |
|-----------|---------------------------------------------|------------|--------------|--------------------|-----------------|-------------------|--------------------------|-----------------------------------------------------------------------------------|
| ccm::pow  | public_default                              | 3132       | 1            | 0                  | 2068            | 1                 | 59                       | MPFR structured binary64 corpus. CORE-MATH finite regular corpus.                 |
| ccm::pow  | runtime_no_builtin, generic_modeled_domain  | N/a        | N/a          | 0                  | N/a             | N/a               | N/a                      | Configuration-matrix quick campaign, no-builtin build only.                       |
| ccm::powf | public_default                              | 68036      | 1            | 2                  | 62372           | 1                 | 2050                     | Structured binary32 corpus. Directional modes carry most CORE-MATH mismatches.    |
| ccm::powf | Reduced domains                             | 6144       | 1            | 0                  | 6144            | 1                 | 396                      | Mantissa/subnormal finite domains. Not evidence for NaN, infinity, or zero cases. |
| ccm::powl | ld64 alias                                  | 144        | 0            | 0                  | N/a             | N/a               | N/a                      | Double-shaped platform. ld64_conservative corpus.                                 |
| ccm::powl | ld80 special + bounded int + general finite | N/a        | N/a          | N/a                | N/a             | N/a               | N/a                      | Not exercised on double-shaped platforms.                                         |
| ccm::powl | ld128 / unknown strict                      | N/a        | N/a          | N/a                | N/a             | N/a               | N/a                      | Detection and NaN policy tests only.                                              |
| ccm::powl | reduced-precision fallback                  | N/a        | N/a          | N/a                | N/a             | N/a               | N/a                      | Only when CCMATH_ENABLE_REDUCED_PRECISION_POWL=ON.                                |

> Last Updated: June 17, 2026
