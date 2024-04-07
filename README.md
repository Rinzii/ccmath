# CCMath - A C++17 constexpr-Compatible CMath Library

CCMath is a C++17 library that provides a re-implementation of the standard `<cmath>` library with all features made `constexpr`.
This enables compile-time evaluation of mathematical functions,
improving performance and allows for more efficient code in scenarios where constant expressions are required.

## Features

- **Full constexpr Compatibility**: All functions provided by CCMath are implemented as `constexpr` along with an active effort made to ensure all functions work within `static_assert`. The primary goal is to ensure every function can be evaluated at compile time.

- **Drop-in Replacement for the Standard Math Library**: CCMath provides a comprehensive set of mathematical functions that are 1:1 compatible with the C++ standard library `<cmath>`. The goal of CCMath is to effectively be a drop-in replacement for `<cmath>` with little to no discernible difference between the two. This includes trigonometric, exponential, logarithmic, and other common mathematical operations. If `<cmath>` has it then it is likely CCMath has implemented it.

- **Performance Optimization**: Besides all the functions being able to be evaluated at compile time, CCMath was also built with speed in mind. We strive to have speeds nearly as fast as the standard implementation.

- **No External Dependencies**: CCMath has no external dependencies and only requires a modern C++17-compliant compiler.

## Usage

To use CCMath in your projects, include the `<ccmath/ccmath.hpp>` header file and start using the provided functions.
Here's a basic example:

```cpp

#include <ccmath/ccmath.hpp> // Monolithic header file that includes all functions. Similar to including <cmath>
#include <iostream> // For std::cout

int main() {
    constexpr double x = ccmath::sqrt(25.0);  // Compile-time evaluation of square root
    std::cout << "Square root of 25: " << x << std::endl;
    return 0;
}
```

## Adding CCMath to your project

CCMath has a comprehensive cmake setup and can be easily included in your project using fetchcontent like so:

```cmake
cmake_minimum_required(VERSION 3.18)

include(FetchContent)
FetchContent_Declare(
        ccmath
        GIT_REPOSITORY https://github.com/Rinzii/ccmath.git
        GIT_TAG v0.1.0 # Replace with the version you want to use
)
FetchContent_MakeAvailable(ccmath)

target_link_libraries(main PRIVATE ccmath::ccmath)
```

## Compiler Support
* GCC 11.1+
* Clang 9.0.0+
* AppleClang 14.0.3+ (Lowest tested version)
* MSVC 19.26+
* Intel DPC++ 2022.0.0+
* Nvidia HPC SDK 22.7+ (Lowest tested version)

> [!NOTE]
> Currently working on finding manners to lower these requirements.

## Contributing

CCmath is an open-source project, and it needs your help to go on growing and improving. If you want to get involved and suggest some additional features, file a bug report or submit a patch, please have a look at the [contribution guidelines](CONTRIBUTING.md)!

## Implementation Progress (Modules)
| Module             | % done | In Progress? | Notes? | Planned Completion Version |
|--------------------|--------|--------------|--------|----------------------------|
| Basic              | 100    |              |        | v0.1.0 (Released)          |
| Compare            | 100    |              |        | v0.2.0                     |
| Exponential        | 33     | ✓            |        | v0.2.0                     |              
| Float Manipulation | 20     |              |        |
| Hyperbolic         | 0      |              |        |
| Nearest            | 33     |              |        |
| Power              | 5      |              |        |
| Special Functions  | 0      |              |        |
| Trigonometric      | 0      |              |        |
| Misc Functions     | 30     |              |        |

> Last Updated: Apr 2, 2024

## Implementation Progress (Functions)

| Feature        | % done | TODO                                                                                  |
|----------------|--------|---------------------------------------------------------------------------------------|
| abs            | 100    |                                                                                       |
| fdim           | 100    |                                                                                       |
| fma            | 100    | Functional. Faster implementations desired.                                           |
| (f)max         | 100    |                                                                                       |
| (f)min         | 100    |                                                                                       |
| remainder      | 100    |                                                                                       |
| remquo         | 100    |                                                                                       |
| nan            | 100    | The nan function is still quite brittle, but it has reached a point of functionality. |
| fpclassify     | 100    |                                                                                       |
| isfinite       | 100    |                                                                                       |
| isgreater      | 100    |                                                                                       |
| isgreaterequal | 100    |                                                                                       |
| isinf          | 100    |                                                                                       |
| isless         | 100    |                                                                                       |
| islessequal    | 100    |                                                                                       |
| islessgreater  | 100    |                                                                                       |
| isnan          | 100    |                                                                                       |
| isnormal       | 100    |                                                                                       |
| isunordered    | 100    |                                                                                       |
| signbit        | 100    | Desire to find a manner of implementing signbit on lower versions of MSVC.            |
| exp            | 90     | Nearly finished. Just need some minor adjustments and better test coverage.           |
| exp2           | 0      | Implement function                                                                    |
| expm1          | 0      | Implement function                                                                    |
| log            | 100    | Functional, but fallbacks without requiring recent compiler versions is desired.      |
| log1p          | 0      | Implement function                                                                    |
| log2           | 100    | Functional, but fallbacks without requiring recent compiler versions is desired.      |
| log10          | 0      | Implement function                                                                    |
| copysign       | 100    |                                                                                       |
| frexp          | 0      | Implement function                                                                    |
| ilogb          | 0      | Implement function                                                                    |
| ldexp          | 0      | Implement function                                                                    |
| logb           | 0      | Implement function                                                                    |
| modf           | 0      | Implement function                                                                    |
| nextafter      | 0      | Implement function                                                                    |
| scalbn         | 80     | Still need to implement the long double version of scalbn.                            |
| acosh          | 0      | Implement function                                                                    |
| asinh          | 0      | Implement function                                                                    |
| atanh          | 0      | Implement function                                                                    |
| cosh           | 0      | Implement function                                                                    |
| sinh           | 0      | Implement function                                                                    |
| tanh           | 0      | Implement function                                                                    |
| ceil           | 0      | Implement function                                                                    |
| floor          | 100    |                                                                                       |
| nearbyint      | 0      | Implement function                                                                    |
| rint           | 0      | Implement function                                                                    |
| round          | 0      | Implement function                                                                    |
| trunc          | 100    |                                                                                       |
| cbrt           | 0      | Implement function                                                                    |
| hypot          | 0      | Implement function                                                                    |
| pow            | 20     | Continue implementation process and add documentation and tests                       |
| sqrt           | 0      | Implement function                                                                    |
| assoc_laguerre | 0      | Implement function                                                                    |
| assoc_legendre | 0      | Implement function                                                                    |
| beta           | 0      | Implement function                                                                    |
| comp_ellint_1  | 0      | Implement function                                                                    |
| comp_ellint_2  | 0      | Implement function                                                                    |
| comp_ellint_3  | 0      | Implement function                                                                    |
| cyl_bessel_i   | 0      | Implement function                                                                    |
| cyl_bessel_j   | 0      | Implement function                                                                    |
| cyl_bessel_k   | 0      | Implement function                                                                    |
| cyl_neumann    | 0      | Implement function                                                                    |
| ellint_1       | 0      | Implement function                                                                    |
| ellint_2       | 0      | Implement function                                                                    |
| ellint_3       | 0      | Implement function                                                                    |
| expint         | 0      | Implement function                                                                    |
| hermite        | 0      | Implement function                                                                    |
| laguerre       | 0      | Implement function                                                                    |
| legendre       | 0      | Implement function                                                                    |
| riemann_zeta   | 0      | Implement function                                                                    |
| sph_bessel     | 0      | Implement function                                                                    |
| sph_legendre   | 0      | Implement function                                                                    |
| sph_neumann    | 0      | Implement function                                                                    |
| acos           | 0      | Implement function                                                                    |
| asin           | 0      | Implement function                                                                    |
| atan           | 0      | Implement function                                                                    |
| atan2          | 0      | Implement function                                                                    |
| cos            | 0      | Implement function                                                                    |
| sin            | 0      | Implement function                                                                    |
| tan            | 0      | Implement function                                                                    |
| gamma          | 0      | Implement function                                                                    |
| lerp           | 90     | Nearly finished, just need to test the function further and validate full compliance. |
| lgamma         | 0      | Implement function                                                                    |

> Last Updated: Apr 2, 2024


## License

CCMath is distributed under the MIT License. See the LICENSE file for more information.


## Special Thanks

Thanks a lot to LLVM, GCC,
and LibM for a lot of insights and inspiration for many different tough issues
encountered through out the development of ccmath!
