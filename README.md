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

#include <ccmath/ccmath.hpp>
#include <iostream>

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

## License

ccmath is distributed under the MIT License. See the LICENSE file for more information.
