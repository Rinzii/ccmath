<img src="docs/resources/ccmath_logo_white.png" style="width: 35%;" alt="CCMath Logo">

# CCMath

## A modern C++ implementation of the C and C++ mathematical library

[![Windows CI](https://github.com/Rinzii/ccmath/workflows/ci-windows/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-windows)
[![Linux CI](https://github.com/Rinzii/ccmath/workflows/ci-linux/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-linux)
[![macOS CI](https://github.com/Rinzii/ccmath/workflows/ci-macos/badge.svg)](https://github.com/Rinzii/ccmath/actions?query=workflow%3Aci-macos)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/Rinzii/ccmath/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Rinzii/ccmath)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/9402/badge)](https://www.bestpractices.dev/projects/9402)
[![License](https://img.shields.io/badge/License-Apache%202.0%20WITH%20LLVM--exception-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Discord](https://img.shields.io/discord/1286067628456284244?label=Discord)](https://discord.gg/p3mVxAbdmc)

CCMath is a header-only C++17 mathematical library that provides standards-oriented implementations of the C and C++ mathematical library.

The project aims to closely follow the behavior specified by the C and C++ standards while supporting:

- Compile-time evaluation
- Runtime execution
- SIMD acceleration
- Cross-platform portability
- Extensive numerical validation

through a familiar `<cmath>`-style API.

Public entry points dispatch through constexpr-friendly generic paths and runtime paths where required by the standard or where the compiler cannot perform constant evaluation.

For current implementation status and function coverage, see [STATUS.md](docs/STATUS.md).

---

## At a Glance

- Standards-oriented `<cmath>` implementation
- Header-only
- C++17 and newer
- Compile-time and runtime execution
- SIMD acceleration
- Cross-platform support
- Extensive numerical validation
- Apache License 2.0 with LLVM Exceptions

---

## Why CCMath?

Many mathematical libraries focus on a specific use case.

| Library | Standards-Oriented | Constexpr | Runtime | SIMD | Portable Implementation |
|----------|----------|----------|----------|----------|----------|
| `std::cmath` | Yes | Partial | Yes | Vendor dependent | No |
| Compiler builtins | Partial | Partial | Yes | No | No |
| GCEM | Partial | Yes | Limited | No | Yes |
| OpenLibm | Yes | No | Yes | Limited | Yes |
| SLEEF | Partial | No | Yes | Yes | Yes |
| CCMath | Yes | Yes | Yes | Yes | Yes |

CCMath attempts to unify:

- Standards conformance
- Compile-time evaluation
- Runtime execution
- SIMD acceleration
- Numerical correctness
- Cross-platform portability

under a single API.

---

## Design Principles

### Standards Conformance

CCMath is designed to closely follow the behavior specified by the C and C++ standards for mathematical functions.

This includes:

- Function semantics
- Domain handling
- Range handling
- NaN propagation
- Infinity behavior
- Floating-point classification
- Rounding behavior
- Edge-case handling

The goal is not merely to produce mathematically reasonable results, but to behave like a conforming implementation of `<cmath>`.

### Compile-Time Evaluation

```cpp
constexpr auto value = ccm::sin(0.5);

static_assert(value > 0.0);
```

Functions are designed to participate naturally in constant evaluation without requiring separate APIs.

### Runtime Execution

```cpp
double result = ccm::exp(x);
```

The same API can be used efficiently at runtime.

### SIMD Acceleration

CCMath includes SIMD implementations for supported architectures, including:

- SSE2
- SSE4
- AVX2
- AVX512
- ARM NEON

### Numerical Correctness

Numerical behavior is a first-class design concern.

CCMath includes dedicated infrastructure for:

- ULP validation
- Worst-case input testing
- Floating-point conformance testing
- Rounding-mode verification
- Cross-platform verification
- Cross-compiler verification

---

## Function Coverage

CCMath provides implementations across the major categories of the C and C++ mathematical library:

- Trigonometric functions
- Exponential and logarithmic functions
- Power and root functions
- Rounding functions
- Floating-point classification
- Utility and floating-point operations

See [STATUS.md](docs/STATUS.md) for complete coverage information and implementation status.

---

## Validation and Verification

CCMath uses a multi-layer validation strategy inspired by projects such as LLVM-libc, CORE-MATH, and glibc.

### Continuous Integration

CCMath is continuously validated on:

#### Platforms

- Linux
- macOS
- Windows

#### Compilers

- GCC
- Clang
- Apple Clang
- MSVC

#### Language Standards

- C++17
- C++20

#### Build Configurations

- Debug
- Release

### Simple Validation Suite

The `simple` suite runs on every CI job:

```bash
ctest -L simple
```

This suite exercises public `ccm::*` entry points and includes:

- Smoke testing
- Standards edge cases
- ULP checks against the active platform libm
- Regression testing

### Rigorous Validation Suite

The `rigorous` suite runs in dedicated CI jobs:

```bash
ctest -L rigorous
```

Validation is performed using multiple independent oracles.

#### MPFR Validation

`rigorous-mpfr`

Includes:

- Extended `pow` and `powf` corpora
- Exceptional-value matrices
- `powl` characterization
- Adversarial search

Linux and macOS use system MPFR/GMP.

Windows uses MPFR provided through vcpkg.

#### CORE-MATH Validation

`rigorous-coremath`

Uses correctly-rounded:

- `cr_pow`
- `cr_powf`

under all four IEEE rounding modes on finite inputs.

#### Proof Verification

When available, CCMath uses:

- Gappa
- Sollya

to refresh and verify proof artifacts for supported implementations.

### Fuzz Testing

CCMath includes fuzzing infrastructure to help identify:

- Numerical edge cases
- Unexpected inputs
- Undefined behavior
- Stability issues

that are difficult to discover through conventional testing.

### Static Analysis and Security

Continuous validation includes:

- CodeQL analysis
- OpenSSF Scorecard analysis
- Warning-as-error builds
- Strict compiler warning validation

### Style and Consistency

The project enforces:

- clang-format
- clang-tidy
- Automated style validation

through continuous integration.

---

## Usage

### Monolithic Include

```cpp
#include <ccmath/ccmath.hpp>
#include <iostream>

int main() {
    constexpr double result = ccm::sqrt(25.0);

    std::cout << result << '\n';
}
```

### Per-Function Include

```cpp
#include <ccmath/math/expo/log.hpp>
#include <iostream>

int main() {
    constexpr double result = ccm::log(12.0);

    std::cout << result << '\n';
}
```

---

## Installation

### CMake

```cmake
include(FetchContent)

FetchContent_Declare(
    ccmath
    GIT_REPOSITORY https://github.com/Rinzii/ccmath.git
    GIT_TAG main
)

FetchContent_MakeAvailable(ccmath)

target_link_libraries(
    main
    PRIVATE
    ccmath::ccmath
)
```

You may also vendor the headers directly.

---

## Rigorous Testing Setup

Typical local configuration:

```bash
cmake -B build-rigorous -G Ninja \
  -DCCMATH_BUILD_RIGOROUS_TESTS=ON \
  -DCCMATH_ENABLE_MPFR_TESTS=ON \
  -DCCMATH_ENABLE_COREMATH_TESTS=ON \
  -DCCMATH_COREMATH_ROOT=/path/to/coremath-prefix
```

Build a CORE-MATH prefix using:

```bash
tools/coremath/build_prefix.sh
```

On Windows, pass:

```bash
-DVCPKG_MANIFEST_DIR=cmake/vcpkg
```

along with a vcpkg toolchain file to obtain MPFR.

See the workflow definitions in [`.github/workflows`](.github/workflows) for the exact CI configurations.

The library itself has no required dependencies.

Optional packages used for testing, benchmarking, and fuzzing are documented in [thirdparty/README.md](thirdparty/README.md).

---

## Supported Compilers

- GCC 11.1+
- Clang 9.0.0+
- Apple Clang 14.0.3+
- MSVC 19.26+
- Intel DPC++ 2022.0.0+
- NVIDIA HPC SDK 22.7+

> [!NOTE]
> Minimum compiler versions are still being refined and should be considered guidance rather than a finalized support matrix.

---

## Projects Using CCMath

The following projects publicly use CCMath:

- [Fornani](https://github.com/swagween/fornani)

If your project uses CCMath and is publicly available, feel free to open a pull request and add it to this list.

---

## Contributing

Contributions are welcome.

See:

- [CONTRIBUTING.md](CONTRIBUTING.md)
- [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)
- [SECURITY.md](SECURITY.md)

Questions, bug reports, and discussions are welcome through GitHub Issues or the Discord community.

---

## License

CCMath is licensed under the Apache License v2.0 with LLVM Exceptions.

This license combines the protections of Apache 2.0, including an explicit patent grant, with the LLVM exception that simplifies integration into both open-source and proprietary software.

See:

- [LICENSE](LICENSE)
- [NOTICE](NOTICE)

for complete licensing information.

---

## Acknowledgments

CCMath has been heavily influenced by the work of many numerical software projects and mathematical libraries, including:

- LLVM-libc
- GCC libm
- CORE-MATH
- glibc math
- OpenLibm

Their research, implementations, testing methodologies, and public documentation have been invaluable resources throughout CCMath's development.