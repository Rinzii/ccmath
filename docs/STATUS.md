## Implementation Progress (Modules)

| Module                       | % done | In Progress? | Notes?                                                                                   | Planned Completion Version |
|------------------------------|--------|--------------|------------------------------------------------------------------------------------------|----------------------------|
| Basic                        | 100    |              | remquo `long double` delegates to double                                                 | v0.1.0 (Released)          |
| Compare                      | 100    |              |                                                                                          | v0.1.0 (Released)          |
| Exponential                  | 98     |              |                                                                                          | v0.2.0 (Released)          |
| Float Manipulation           | 100    |              | scalbn `long double` is slow                                                             | v0.2.0 (Released)          |
| Hyperbolic                   | 0      |              | Empty headers only                                                                       |                            |
| Nearest                      | 100    |              |                                                                                          | v0.2.0 (Released)          |
| Power                        | 96     |              | Path forcing, MPFR path-matrix, powf reduced domains, powl characterization, proof-smoke | v0.2.0 (Released)          |
| Special Functions            | 0      |              | Empty headers only                                                                       |                            |
| Trigonometric                | 100    |              |                                                                                          | v0.2.0 (Released)          |
| Misc Functions               | 97     |              |                                                                                          | v0.2.0 (Released)          |
| **Total Library Completion** | **68** |              |                                                                                          | N/a                        |

> Last Updated: June 7, 2026

## Implementation Progress (Functions)

Per-function rounding mode status: [ROUNDING_STATUS.md](ROUNDING_STATUS.md).

| Feature        | % done | TODO                                                                                                                                                                                             |
|----------------|--------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| fabs           | 100    |                                                                                                                                                                                                  |
| fdim           | 100    |                                                                                                                                                                                                  |
| fma            | 100    |                                                                                                                                                                                                  |
| (f)max         | 100    |                                                                                                                                                                                                  |
| (f)min         | 100    |                                                                                                                                                                                                  |
| remainder      | 100    |                                                                                                                                                                                                  |
| remquo         | 100    | remquol delegates through double                                                                                                                                                                 |
| nan            | 100    | Brittle on non-IEC559 platforms                                                                                                                                                                  |
| fpclassify     | 100    |                                                                                                                                                                                                  |
| isfinite       | 100    |                                                                                                                                                                                                  |
| isgreater      | 100    |                                                                                                                                                                                                  |
| isgreaterequal | 100    |                                                                                                                                                                                                  |
| isinf          | 100    |                                                                                                                                                                                                  |
| isless         | 100    |                                                                                                                                                                                                  |
| islessequal    | 100    |                                                                                                                                                                                                  |
| islessgreater  | 100    |                                                                                                                                                                                                  |
| isnan          | 100    |                                                                                                                                                                                                  |
| isnormal       | 100    |                                                                                                                                                                                                  |
| isunordered    | 100    |                                                                                                                                                                                                  |
| signbit        | 100    |                                                                                                                                                                                                  |
| exp            | 98     |                                                                                                                                                                                                  |
| exp2           | 98     |                                                                                                                                                                                                  |
| expm1          | 98     |                                                                                                                                                                                                  |
| log            | 98     |                                                                                                                                                                                                  |
| log1p          | 98     |                                                                                                                                                                                                  |
| log2           | 98     |                                                                                                                                                                                                  |
| log10          | 98     |                                                                                                                                                                                                  |
| copysign       | 100    |                                                                                                                                                                                                  |
| frexp          | 100    |                                                                                                                                                                                                  |
| ilogb          | 100    |                                                                                                                                                                                                  |
| ldexp          | 100    |                                                                                                                                                                                                  |
| logb           | 100    |                                                                                                                                                                                                  |
| modf           | 100    |                                                                                                                                                                                                  |
| nextafter      | 100    |                                                                                                                                                                                                  |
| nexttoward     | 100    |                                                                                                                                                                                                  |
| scalbn         | 100    | Long double is slow.                                                                                                                                                                             |
| acosh          | 0      | Empty header                                                                                                                                                                                     |
| asinh          | 0      | Empty header                                                                                                                                                                                     |
| atanh          | 0      | Empty header                                                                                                                                                                                     |
| cosh           | 0      | Empty header                                                                                                                                                                                     |
| sinh           | 0      | Empty header                                                                                                                                                                                     |
| tanh           | 0      | Empty header                                                                                                                                                                                     |
| ceil           | 100    |                                                                                                                                                                                                  |
| floor          | 100    |                                                                                                                                                                                                  |
| nearbyint      | 100    | Runtime rounding mode via `fenv`                                                                                                                                                                 |
| rint           | 100    |                                                                                                                                                                                                  |
| round          | 100    |                                                                                                                                                                                                  |
| trunc          | 100    |                                                                                                                                                                                                  |
| cbrt           | 98     |                                                                                                                                                                                                  |
| hypot          | 100    |                                                                                                                                                                                                  |
| pow            | 88     | Rigorous MPFR `public_default` campaigns, path-matrix tooling, powf reduced domains, powl characterization. Full binary64 proof and independent extended-precision `powl` validation remain open |
| sqrt           | 100    |                                                                                                                                                                                                  |
| assoc_laguerre | 0      | Empty header                                                                                                                                                                                     |
| assoc_legendre | 0      | Empty header                                                                                                                                                                                     |
| beta           | 0      | Empty header                                                                                                                                                                                     |
| comp_ellint_1  | 0      | Empty header                                                                                                                                                                                     |
| comp_ellint_2  | 0      | Empty header                                                                                                                                                                                     |
| comp_ellint_3  | 0      | Empty header                                                                                                                                                                                     |
| cyl_bessel_i   | 0      | Empty header                                                                                                                                                                                     |
| cyl_bessel_j   | 0      | Empty header                                                                                                                                                                                     |
| cyl_bessel_k   | 0      | Empty header                                                                                                                                                                                     |
| cyl_neumann    | 0      | Empty header                                                                                                                                                                                     |
| ellint_1       | 0      | Empty header                                                                                                                                                                                     |
| ellint_2       | 0      | Empty header                                                                                                                                                                                     |
| ellint_3       | 0      | Empty header                                                                                                                                                                                     |
| expint         | 0      | Empty header                                                                                                                                                                                     |
| hermite        | 0      | Empty header                                                                                                                                                                                     |
| laguerre       | 0      | Empty header                                                                                                                                                                                     |
| legendre       | 0      | Empty header                                                                                                                                                                                     |
| riemann_zeta   | 0      | Empty header                                                                                                                                                                                     |
| sph_bessel     | 0      | Empty header                                                                                                                                                                                     |
| sph_legendre   | 0      | Empty header                                                                                                                                                                                     |
| sph_neumann    | 0      | Empty header                                                                                                                                                                                     |
| acos           | 100    |                                                                                                                                                                                                  |
| asin           | 100    |                                                                                                                                                                                                  |
| atan           | 100    |                                                                                                                                                                                                  |
| atan2          | 100    |                                                                                                                                                                                                  |
| cos            | 100    |                                                                                                                                                                                                  |
| sin            | 100    |                                                                                                                                                                                                  |
| tan            | 100    |                                                                                                                                                                                                  |
| gamma          | 100    |                                                                                                                                                                                                  |
| lerp           | 90     |                                                                                                                                                                                                  |
| lgamma         | 100    |                                                                                                                                                                                                  |
> Last Updated: June 7, 2026
