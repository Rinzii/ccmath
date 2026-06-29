# ULP status

The goal for ccmath is correctly rounded results in all four IEEE 754 rounding modes for every
function. We are not there yet, and this page is meant to be an honest picture of where each
function actually stands rather than a promise. It reports what was measured, how it was measured,
and what is still left to do.

The companion page ROUNDING_STATUS.md tracks which rounding modes each function handles at runtime.
This page is about accuracy, the distance between the ccm result and the true correctly rounded
value.

## How to read this

Every function falls into one of five classes.

| Class                   | Meaning                                                                                                                                                    |
|-------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Exact                   | Produces the exact representable result, so 0 ULP in all four modes by construction. The nearest-integer, sign, and scaling families are here.             |
| Correctly rounded       | The unique nearest representable value, 0.5 ULP, confirmed by an all-modes campaign. sqrt is correctly rounded by IEEE 754, and fma is exact-then-rounded. |
| Bounded                 | Measured to be within a small number of ULP of the correctly rounded value. Correct rounding is the goal, and these functions are not there yet.           |
| Not a rounding function | Returns a bool, an int, or a classification, so ULP does not apply. The comparison family is here.                                                         |
| Not implemented         | The header exists but there is no implementation yet. The hyperbolic and special-function families are here.                                               |

A few notes on the numbers in the tables below.

These numbers are for ccmath's own generic kernels, not for whatever the public function lowers to
at runtime. On a clang or gcc build the public functions hand the runtime round-to-nearest case to
a compiler builtin, which is the system libm and is often more accurate than the figures here. We
measure the generic kernel on purpose, because it is ccmath's own code and it is what runs at
compile time, in the directed rounding modes, in deterministic mode, and anywhere a builtin is not
available. So treat these as the portable worst case ccmath is responsible for, not as what a clang
or gcc runtime build delivers in round-to-nearest.

Max ULP is the largest error observed between the kernel result and the correctly rounded value
over the measured corpus. A value of 0 means every result was the nearest representable value. The
reference is MPFR evaluated at high precision and rounded once to the target type, so it is the
true correctly rounded answer and not another library's result.

The accuracy families, which are Exponential, Power, Trigonometric, and Miscellaneous, break each
function into its three type variants: the float f-suffix variant (expf, powf, sinf), the plain double
variant (exp, pow, sin), and the long double variant (expl, powl, sinl). The other families are exact
or not implemented, which holds for every type, so they stay one row each.

Only binary32 is measured so far. Every binary32 number comes from an exhaustive sweep over all bit
patterns, which makes its max a proven worst case in round-to-nearest. binary64 is a larger job and is
measured next, and long double after that, so both show up as pending until then.

Each variant row names the exact kernel it runs in the Tested kernel column. The gen kernels are
templates, so the same symbol instantiates at each type, for example ccm::gen::pow_gen<float> and
ccm::gen::pow_gen<double>. The asin, acos, atan, tgamma, and lgamma kernels come as a separate float
and double function with no dedicated long double kernel, so their long double variant uses the double
kernel. These are ccmath's own generic kernels under --impl=gen, the implementation the public function
falls back to in the directed modes, at compile time, and in deterministic mode. The same harness can
point at the public path or the system libm with --impl=public or --impl=libm, but those are not what
this page reports.

The numbers are measured in round-to-nearest, the default mode. Which rounding modes each function
handles at runtime is tracked separately in ROUNDING_STATUS.md.

To refresh these tables, run the sweep and then the generator.

```
tools/measure_ulp.sh --exhaustive-float
python3 tools/ulp_status.py
```

The function classes and the list of measured functions live in tools/ulp_status_registry.json.
A function with no completed campaign summary shows up as pending, so the tables never claim a
number that was not actually measured.

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

This pass measures binary32. binary64 is the next job, and long double after that, so both show up as
pending. The accuracy families list all three variants so the gaps are explicit rather than implied.

The atan2, hypot, and lerp functions take more than one argument, so they wait on a binary and
ternary sweep and are marked pending. pow is measured separately, and its detailed campaign is in
the appendix below.

Near a zero or a pole, ULP error is governed by conditioning rather than by rounding, because a
tiny absolute error sits across many representable values when the result itself is near zero or
very large. Functions with poles or zeros in range, such as tan near its poles and lgamma near its
zeros at 1 and 2, can show a large max and average that reflect how close the corpus landed to
those spots rather than the kernel's typical accuracy.

## Appendix: pow campaign detail

Rigorous oracle quick campaigns (apple aarch64, clang 17, June 7 2026). Not proofs. Max ULP is the
same in all four IEEE rounding modes on the corpora below.

**Failure columns**

| Column | Meaning |
|--------|---------|
| MPFR hard failures | Cases that exceed the campaign 4 ULP ceiling, or disagree with MPFR on NaN, infinity, or signed zero. Within-ceiling results that miss the 0.5 ULP correctly-rounded target are tracked separately and are not counted here. |
| CORE-MATH bit mismatches | Cases where ccm bits differ from the CORE-MATH cr_* reference in the active rounding mode. This is not a ULP threshold. Cases where a higher-precision cross-check shows ccm is correct and the CORE-MATH oracle is wrong are excluded. |

Per-case logs with base, exponent, actual, and expected bit patterns live under
[tests/rigorous/oracle_logs/README.md](../tests/rigorous/oracle_logs/README.md). Re-run the
rigorous oracle ctest targets to refresh them.

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
