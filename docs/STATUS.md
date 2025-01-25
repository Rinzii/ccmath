## Implementation Progress (Modules)

| Module                   | % done | In Progress? | Notes? | Planned Completion Version |
|--------------------------|--------|--------------|--------|----------------------------|
| Basic                    | 100    |              |        | v0.1.0 (Released)          |
| Compare                  | 100    |              |        | v0.1.0 (Released)          |
| Exponential              | 57     |              |        | maybe v0.2.0 or v0.3.0     |              
| Float Manipulation       | 25     |              |        |                            |
| Hyperbolic               | 0      |              |        |                            |
| Nearest                  | 33     |              |        |                            |
| Power                    | 5      | ✓            |        | maybe v0.2.0 or v0.3.0     |
| Special Functions        | 0      |              |        |                            |
| Trigonometric            | 0      |              |        |                            |
| Misc Functions           | 30     |              |        |                            |
| Total Library Completion | 37     |              |        |                            |

> Last Updated: Aug 31, 2024

## Implementation Progress (Functions)

| Feature        | % done | TODO                                                                                                        |
|----------------|--------|-------------------------------------------------------------------------------------------------------------|
| abs            | 100    |                                                                                                             |
| fdim           | 100    |                                                                                                             |
| fma            | 100    | Functional. Need to add proper runtime specific speed up.                                                   |
| (f)max         | 100    |                                                                                                             |
| (f)min         | 100    |                                                                                                             |
| remainder      | 100    |                                                                                                             |
| remquo         | 100    |                                                                                                             |
| nan            | 100    | The nan function is still quite brittle, but it has reached a point of functionality.                       |
| fpclassify     | 100    |                                                                                                             |
| isfinite       | 100    |                                                                                                             |
| isgreater      | 100    |                                                                                                             |
| isgreaterequal | 100    |                                                                                                             |
| isinf          | 100    |                                                                                                             |
| isless         | 100    |                                                                                                             |
| islessequal    | 100    |                                                                                                             |
| islessgreater  | 100    |                                                                                                             |
| isnan          | 100    |                                                                                                             |
| isnormal       | 100    |                                                                                                             |
| isunordered    | 100    |                                                                                                             |
| signbit        | 100    | Desire to find a manner of implementing signbit on lower versions of MSVC.                                  |
| exp            | 100    | Needs to be modernized to current CCMath standards                                                          |
| exp2           | 100    | Needs to be modernized to current CCMath standards                                                          |
| expm1          | 0      | Implement function                                                                                          |
| log            | 100    | Needs to be modernized to current CCMath standards                                                          |
| log1p          | 0      | Implement function                                                                                          |
| log2           | 100    | Needs to be modernized to current CCMath standards                                                          |
| log10          | 0      | Implement function                                                                                          |
| copysign       | 100    |                                                                                                             |
| frexp          | 0      | Implement function                                                                                          |
| ilogb          | 0      | Implement function                                                                                          |
| ldexp          | 100    |                                                                                                             |
| logb           | 0      | Implement function                                                                                          |
| modf           | 0      | Implement function                                                                                          |
| nextafter      | 0      | Implement function                                                                                          |
| scalbn         | 100    | Needs to be modernized to current CCMath standards also long double implementation is slow, but functional. |
| acosh          | 0      | Implement function                                                                                          |
| asinh          | 0      | Implement function                                                                                          |
| atanh          | 0      | Implement function                                                                                          |
| cosh           | 0      | Implement function                                                                                          |
| sinh           | 0      | Implement function                                                                                          |
| tanh           | 0      | Implement function                                                                                          |
| ceil           | 0      | Implement function                                                                                          |
| floor          | 100    |                                                                                                             |
| nearbyint      | 0      | Implement function                                                                                          |
| rint           | 0      | Implement function                                                                                          |
| round          | 0      | Implement function                                                                                          |
| trunc          | 100    |                                                                                                             |
| cbrt           | 0      | Implement function                                                                                          |
| hypot          | 0      | Implement function                                                                                          |
| pow            | 30     | Continue implementation process and add documentation and tests                                             |
| sqrt           | 100    |                                                                                                             |
| assoc_laguerre | 0      | Implement function                                                                                          |
| assoc_legendre | 0      | Implement function                                                                                          |
| beta           | 0      | Implement function                                                                                          |
| comp_ellint_1  | 0      | Implement function                                                                                          |
| comp_ellint_2  | 0      | Implement function                                                                                          |
| comp_ellint_3  | 0      | Implement function                                                                                          |
| cyl_bessel_i   | 0      | Implement function                                                                                          |
| cyl_bessel_j   | 0      | Implement function                                                                                          |
| cyl_bessel_k   | 0      | Implement function                                                                                          |
| cyl_neumann    | 0      | Implement function                                                                                          |
| ellint_1       | 0      | Implement function                                                                                          |
| ellint_2       | 0      | Implement function                                                                                          |
| ellint_3       | 0      | Implement function                                                                                          |
| expint         | 0      | Implement function                                                                                          |
| hermite        | 0      | Implement function                                                                                          |
| laguerre       | 0      | Implement function                                                                                          |
| legendre       | 0      | Implement function                                                                                          |
| riemann_zeta   | 0      | Implement function                                                                                          |
| sph_bessel     | 0      | Implement function                                                                                          |
| sph_legendre   | 0      | Implement function                                                                                          |
| sph_neumann    | 0      | Implement function                                                                                          |
| acos           | 0      | Implement function                                                                                          |
| asin           | 0      | Implement function                                                                                          |
| atan           | 0      | Implement function                                                                                          |
| atan2          | 0      | Implement function                                                                                          |
| cos            | 0      | Implement function                                                                                          |
| sin            | 0      | Implement function                                                                                          |
| tan            | 0      | Implement function                                                                                          |
| gamma          | 0      | Implement function                                                                                          |
| lerp           | 90     | Nearly finished, just need to test the function further and validate full compliance.                       |
| lgamma         | 0      | Implement function                                                                                          |

> Last Updated: Aug 31, 2024