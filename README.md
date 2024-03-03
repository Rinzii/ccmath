# ccmath - A C++17 constexpr-Compatible CMath Library

ccmath is a C++17 library that provides a re-implementation of the standard `<cmath>` library with all features made `constexpr`. This enables compile-time evaluation of mathematical functions, improving performance and allowing for more efficient code in scenarios where constant expressions are required.

## Features

- **constexpr Compatibility**: All functions provided by ccmath are implemented as `constexpr`, allowing them to be evaluated at compile time when used with constant expressions.

- **Standard Math Functions**: ccmath provides a comprehensive set of mathematical functions similar to those in the standard `<cmath>` library, including trigonometric, exponential, logarithmic, and other common mathematical operations. If `<cmath>` has it then it is likely ccmath has implemented it.

- **Performance Optimization**: By leveraging constexpr, ccmath aims to optimize performance by computing mathematical expressions at compile time where possible, reducing runtime overhead.

- **No External Dependencies**: ccmath has no external dependencies and only requires a C++17-compliant compiler.

## Usage

To use ccmath in your projects, simply include the ccmath.hpp header file and start using the provided functions. Here's a basic example:

```cpp

#include <ccmath/ccmath.hpp> // Monolithic header file that includes all functions. Similar to including <cmath>
#include <iostream> // For std::cout

int main() {
    constexpr double x = ccmath::sqrt(25.0);  // Compile-time evaluation of square root
    std::cout << "Square root of 25: " << x << std::endl;
    return 0;
}
```

## Adding ccmath to your project

ccmath has a comprehensive cmake setup and can be easily included in your project using fetchcontent like so:

```cmake
include(FetchContent)
FetchContent_Declare(
        ccmath
        GIT_REPOSITORY https://github.com/Rinzii/ccmath.git
        GIT_TAG main
)
FetchContent_MakeAvailable(ccmath)
```

## Compatability

ccmath is designed to be compatible with any C++17-compliant compiler. It should work seamlessly with popular compilers such as GCC, Clang, and MSVC.

## Contributing

Contributions to ccmath are welcome! If you encounter any bugs, have suggestions for improvements, or would like to contribute new features, feel free to open an issue or submit a pull request!

## Implementation Progress (Sub Sections)
| Section            | % done | In Progress? | Notes?                                                                    | Planned Completion Version |
|--------------------|--------|--------------|---------------------------------------------------------------------------|----------------------------|
| Basic              | 91     |              | Remquo is being pushed back to a later release due to technical problems. | v0.1.0 (Released)          |
| Compare            | 90     |              |                                                                           | v0.2.0                     |
| Exponential        | 25     | ✓            |                                                                           | v0.2.0                     |              
| Float Manipulation | 0      |              |                                                                           |
| Hyperbolic         | 0      |              |                                                                           |
| Nearest            | 15     |              |                                                                           |
| Power              | 5      |              |                                                                           |
| Special Functions  | 0      |              |                                                                           |
| Trigonometric      | 0      |              |                                                                           |
| Misc Functions     | 0      |              |                                                                           |

> Last Updated: Mar 03, 2024

## Implementation Progress (All Functions)

| Feature        | % done | TODO                                                                                     |
|----------------|--------|------------------------------------------------------------------------------------------|
| abs            | 100    |                                                                                          |
| fdim           | 100    |                                                                                          |
| fma            | 100    | Functional but more fallbacks are desired.                                               |
| (f)max         | 100    |                                                                                          |
| (f)min         | 100    |                                                                                          |
| remainder      | 100    |                                                                                          |
| remquo         | 40     | Partially implemented, but being pushed back to later release due to technical problems. |
| fpclassify     | 100    |                                                                                          |
| isfinite       | 100    |                                                                                          |
| isgreater      | 100    |                                                                                          |
| isgreaterequal | 100    |                                                                                          |
| isinf          | 98     | Improve documentation                                                                    |
| isless         | 100    |                                                                                          |
| islessequal    | 100    |                                                                                          |
| islessgreater  | 100    |                                                                                          |
| isnan          | 95     | Add more support for built-in functions and improve documentation                        |
| isnormal       | 100    |                                                                                          |
| isunordered    | 100    |                                                                                          |
| signbit        | 90     | Add more fallbacks and builtin support if possible and improve reliability with MSVC     |
| exp            | 35     | Continue implementation process and add documentation and tests                          |
| exp2           | 0      | Implement function                                                                       |
| expm1          | 0      | Implement function                                                                       |
| log            | 95     | Add more test cases that cover a larger subset of edge cases.                            |
| log1p          | 0      | Implement function                                                                       |
| log2           | 0      | Implement function                                                                       |
| log10          | 0      | Implement function                                                                       |
| copysign       | 0      | Implement function                                                                       |
| frexp          | 0      | Implement function                                                                       |
| ilogb          | 0      | Implement function                                                                       |
| ldexp          | 0      | Implement function                                                                       |
| logb           | 0      | Implement function                                                                       |
| modf           | 0      | Implement function                                                                       |
| nextafter      | 0      | Implement function                                                                       |
| scalbn         | 0      | Implement function                                                                       |
| acosh          | 0      | Implement function                                                                       |
| asinh          | 0      | Implement function                                                                       |
| atanh          | 0      | Implement function                                                                       |
| cosh           | 0      | Implement function                                                                       |
| sinh           | 0      | Implement function                                                                       |
| tanh           | 0      | Implement function                                                                       |
| ceil           | 0      | Implement function                                                                       |
| floor          | 0      | Implement function                                                                       |
| nearbyint      | 0      | Implement function                                                                       |
| rint           | 0      | Implement function                                                                       |
| round          | 0      | Implement function                                                                       |
| trunc          | 80     | Continue refinement and solve all issues                                                 |
| cbrt           | 0      | Implement function                                                                       |
| hypot          | 0      | Implement function                                                                       |
| pow            | 20     | Continue implementation process and add documentation and tests                          |
| sqrt           | 0      | Implement function                                                                       |
| assoc_laguerre | 0      | Implement function                                                                       |
| assoc_legendre | 0      | Implement function                                                                       |
| beta           | 0      | Implement function                                                                       |
| comp_ellint_1  | 0      | Implement function                                                                       |
| comp_ellint_2  | 0      | Implement function                                                                       |
| comp_ellint_3  | 0      | Implement function                                                                       |
| cyl_bessel_i   | 0      | Implement function                                                                       |
| cyl_bessel_j   | 0      | Implement function                                                                       |
| cyl_bessel_k   | 0      | Implement function                                                                       |
| cyl_neumann    | 0      | Implement function                                                                       |
| ellint_1       | 0      | Implement function                                                                       |
| ellint_2       | 0      | Implement function                                                                       |
| ellint_3       | 0      | Implement function                                                                       |
| expint         | 0      | Implement function                                                                       |
| hermite        | 0      | Implement function                                                                       |
| laguerre       | 0      | Implement function                                                                       |
| legendre       | 0      | Implement function                                                                       |
| riemann_zeta   | 0      | Implement function                                                                       |
| sph_bessel     | 0      | Implement function                                                                       |
| sph_legendre   | 0      | Implement function                                                                       |
| sph_neumann    | 0      | Implement function                                                                       |
| acos           | 0      | Implement function                                                                       |
| asin           | 0      | Implement function                                                                       |
| atan           | 0      | Implement function                                                                       |
| atan2          | 0      | Implement function                                                                       |
| cos            | 0      | Implement function                                                                       |
| sin            | 0      | Implement function                                                                       |
| tan            | 0      | Implement function                                                                       |
| gamma          | 0      | Implement function                                                                       |
| lerp           | 0      | Implement function                                                                       |
| lgamma         | 0      | Implement function                                                                       |

> Last Updated: Mar 03, 2024


## License

ccmath is distributed under the MIT License. See the LICENSE file for more information.
