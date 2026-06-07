<img src="docs/resources/ccmath_logo_white.png" style="width: 35%;" alt="CCMath Logo">

# CCMath

[![image](https://github.com/Rinzii/ccmath/workflows/ci-windows/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-windows)
[![image](https://github.com/Rinzii/ccmath/workflows/ci-linux/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-linux)
[![image](https://github.com/Rinzii/ccmath/workflows/ci-macos/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-macos)
[![image](https://api.securityscorecards.dev/projects/github.com/Rinzii/ccmath/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Rinzii/ccmath)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/9402/badge)](https://www.bestpractices.dev/projects/9402)
[![License](https://img.shields.io/badge/License-Apache%202.0%20WITH%20LLVM--exception-blue.svg)](
https://opensource.org/licenses/Apache-2.0)
[![Discord](https://img.shields.io/discord/1286067628456284244?label=Discord)](https://discord.gg/p3mVxAbdmc)

Header-only C++17 math library with a `<cmath>`-shaped API. Public entry points
dispatch through constexpr-friendly generic paths and runtime paths where the
standard requires runtime behavior or the compiler cannot constexpr-evaluate the
kernel.

See [docs/STATUS.md](docs/STATUS.md) for what is actually implemented today.

## Usage

Monolithic include:

```cpp
#include <ccmath/ccmath.hpp>
#include <iostream>

int main() {
    constexpr double result = ccm::sqrt(25.0);
    std::cout << "Square root of 25: " << result << std::endl;
    return 0;
}
```

Per-function headers:

```cpp
#include <ccmath/math/expo/log.hpp>
#include <iostream>

int main() {
    constexpr double result = ccm::log(12.0);
    std::cout << "Log of 12: " << result << std::endl;
    return 0;
}
```

### CMake

```cmake
include(FetchContent)
FetchContent_Declare(
        ccmath
        GIT_REPOSITORY https://github.com/Rinzii/ccmath.git
        GIT_TAG main
)
FetchContent_MakeAvailable(ccmath)

target_link_libraries(main PRIVATE ccmath::ccmath)
```

You can also vendor the headers directly.

## Supported compilers

- GCC 11.1+
- Clang 9.0.0+
- AppleClang 14.0.3+
- MSVC 19.26+
- Intel DPC++ 2022.0.0+
- Nvidia HPC SDK 22.7+

> [!ATTENTION]
> Minimum compiler versions are still being narrowed. Treat this list as guidance,
> not a finished matrix.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). Open issues on GitHub or ask in
[Discord](https://discord.gg/p3mVxAbdmc).

## Projects using CCMath

- [Fornani](https://github.com/swagween/fornani)

Tell us if you ship CCMath in something public.

## License

Apache-2.0 WITH LLVM-exception. See LICENSE.

## Acknowledgments

LLVM-libc, GCC libm, and other public libm implementations informed several kernels
in this tree.
