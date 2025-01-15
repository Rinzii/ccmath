<img src="docs/resources/ccmath_logo_white.png" style="width: 35%;" alt="CCMath Logo">

# CCMath - A Modern, Compile-Time `<cmath>` Alternative

[![image](https://github.com/Rinzii/ccmath/workflows/ci-windows/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-windows)
[![image](https://github.com/Rinzii/ccmath/workflows/ci-linux/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-linux)
[![image](https://github.com/Rinzii/ccmath/workflows/ci-macos/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-macos)
[![image](https://api.securityscorecards.dev/projects/github.com/Rinzii/ccmath/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Rinzii/ccmath)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/9402/badge)](https://www.bestpractices.dev/projects/9402)
[![License](https://img.shields.io/badge/License-Apache%202.0%20WITH%20LLVM--exception-blue.svg)](
https://opensource.org/licenses/Apache-2.0)
[![Discord](https://img.shields.io/discord/1286067628456284244?label=Discord)](https://discord.gg/p3mVxAbdmc)

**CCMath** is a C++17 library designed to provide a reimplementation of the standard <cmath> library, emphasizing
comprehensive constexpr support.

## Key Features

- **Full `constexpr` Compatibility**: Every function in CCMath is implemented as `constexpr`, ensuring compatibility
  with `static_assert` and enabling robust compile-time evaluations.
- **Seamless `<cmath>` Replacement**: CCMath replicates the functionality of the standard `<cmath>` library, offering 1:
  1 API compatibility. CCMath covers the vast majority of mathematical operations, including trigonometric, exponential,
  logarithmic, and more that are provided by the standard library.
- **Performance-Focused Design**: All implemented functions are optimized for both compile-time and runtime performance,
  providing
  comparable or faster execution than standard library implementations.
- **Cross-Platform Support**: Tested on major platforms (Windows, macOS, Linux) with GCC, Clang, MSVC, and more, CCMath
  ensures compatibility across diverse environments.
- **No External Dependencies**: The library requires only a modern C++17-compliant compiler, with no additional
  dependencies.
- **Header-Only Library**: Integration is straightforward—just include the headers in your project and start using
  CCMath.

## Usage

Include CCMath in your project by adding the `<ccmath/ccmath.hpp>` header file. Here's a simple example:

```cpp
#include <ccmath/ccmath.hpp> // Monolithic header that includes all functions like <cmath>
#include <iostream>

int main() {
    constexpr double result = ccm::sqrt(25.0); // Compile-time evaluation
    std::cout << "Square root of 25: " << result << std::endl;
    return 0;
}
```

or if you prefer to include only specific functions:

```cpp
#include <ccmath/expo/log.hpp> // Function-specific header for free-standing functions
#include <iostream>

int main() {
    constexpr double result = ccm::log(12.0); // Compile-time evaluation
    std::cout << "Log of 12: " << result << std::endl;
    return 0;
}
```

### Adding CCMath via CMake

CCMath includes a comprehensive CMake setup for easy integration. Use `FetchContent` as shown below:

```cmake
include(FetchContent)
FetchContent_Declare(
        ccmath
        GIT_REPOSITORY https://github.com/Rinzii/ccmath.git
        GIT_TAG main # Or replace with a specific version (e.g., v0.2.0)
)
FetchContent_MakeAvailable(ccmath)

target_link_libraries(main PRIVATE ccmath::ccmath)
```

Alternatively, you can directly include the headers in your project.

## Supported Compilers

- GCC 11.1+
- Clang 9.0.0+
- AppleClang 14.0.3+
- MSVC 19.26+
- Intel DPC++ 2022.0.0+
- Nvidia HPC SDK 22.7+

> [!ATTENTION]
> Listed compilers may not be the actual supported versions as development is still ongoing.

> [!NOTE]
> Currently working on finding manners to lower these requirements.

## Contributing

CCMath is an open-source project, and contributions are always welcome! Whether you want to report bugs, suggest
features, or contribute code, check out our [contribution guidelines](CONTRIBUTING.md).

## Join Our Discord!

Have questions, ideas, or just want to chat? Join our Discord server—we’d love to hear from you!

[![Discord Banner](https://discord.com/api/guilds/1286067628456284244/widget.png?style=banner2)](https://discord.gg/p3mVxAbdmc)

## Implementation Status

For details on the current progress of the library, including completed features and planned additions, refer to
the [implementation status](docs/STATUS.md) document.

## Projects Using CCMath

- [Fornani](https://github.com/swagween/fornani) - A 2D action-adventure game in development.
- [KittyOS](https://github.com/kitty-os/kitty-os/) - A whimsical, non-Unix operating system for cat lovers.

_(Are you using CCMath? Let us know!)_

## License

CCMath is licensed under the Apache-2.0 WITH LLVM-exception. See the LICENSE file for more details.

## Special Thanks

A special thanks to LLVM, GCC, and the many public LibM's for providing inspiration and insights during the development
of CCMath.
