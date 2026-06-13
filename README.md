<img src="docs/resources/ccmath_logo_white.png" style="width: 35%;" alt="CCMath Logo">

# CCMath

## A constexpr-first `<cmath>` library for C++17 and later

[![Release](https://img.shields.io/github/v/release/Rinzii/ccmath?sort=semver&label=release)](https://github.com/Rinzii/ccmath/releases/latest)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/Rinzii/ccmath/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Rinzii/ccmath)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/9402/badge)](https://www.bestpractices.dev/projects/9402)
[![License](https://img.shields.io/badge/License-Apache%202.0%20WITH%20LLVM--exception-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Discord](https://img.shields.io/discord/1286067628456284244?label=Discord)](https://discord.gg/p3mVxAbdmc)

| Branch          | Linux                                                                                                                                                                                                              | macOS                                                                                                                                                                                                              | Windows                                                                                                                                                                                                                  |
|-----------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `main`          | [![Linux main](https://github.com/Rinzii/ccmath/actions/workflows/ci-linux.yml/badge.svg?branch=main)](https://github.com/Rinzii/ccmath/actions/workflows/ci-linux.yml?query=branch%3Amain)                        | [![macOS main](https://github.com/Rinzii/ccmath/actions/workflows/ci-macos.yml/badge.svg?branch=main)](https://github.com/Rinzii/ccmath/actions/workflows/ci-macos.yml?query=branch%3Amain)                        | [![Windows main](https://github.com/Rinzii/ccmath/actions/workflows/ci-windows.yml/badge.svg?branch=main)](https://github.com/Rinzii/ccmath/actions/workflows/ci-windows.yml?query=branch%3Amain)                        |
| `release/0.3.x` | [![Linux release](https://github.com/Rinzii/ccmath/actions/workflows/ci-linux.yml/badge.svg?branch=release/0.3.x)](https://github.com/Rinzii/ccmath/actions/workflows/ci-linux.yml?query=branch%3Arelease%2F0.3.x) | [![macOS release](https://github.com/Rinzii/ccmath/actions/workflows/ci-macos.yml/badge.svg?branch=release/0.3.x)](https://github.com/Rinzii/ccmath/actions/workflows/ci-macos.yml?query=branch%3Arelease%2F0.3.x) | [![Windows release](https://github.com/Rinzii/ccmath/actions/workflows/ci-windows.yml/badge.svg?branch=release/0.3.x)](https://github.com/Rinzii/ccmath/actions/workflows/ci-windows.yml?query=branch%3Arelease%2F0.3.x) |

CCMath is a header-only library with a `<cmath>`-style API. The same entry points constexpr-evaluate where the standard and compiler allow, dispatch to portable runtime implementations elsewhere, and can use SIMD on selected functions when the target supports it.

This README describes what CCMath is working toward. Some claims here reflect goals still in progress. See [STATUS.md](docs/STATUS.md) for current implementation status by module and function.

For approximation-driven function work, see the implementation guide
in [APPROXIMATING_FUNCTIONS.pdf](docs/approximating_functions/APPROXIMATING_FUNCTIONS.pdf).

---

## Why CCMath?

Most `<cmath>` implementations prioritize different tradeoffs. Few combine standards-oriented semantics, constexpr evaluation, full runtime paths, SIMD, and portable in-tree code.

| Library           | Standards-Oriented | Constexpr | Runtime | SIMD             | Portable Implementation |
|-------------------|--------------------|-----------|---------|------------------|-------------------------|
| `std::cmath`      | Yes                | Partial   | Yes     | Vendor dependent | No                      |
| Compiler builtins | Partial            | Partial   | Yes     | No               | No                      |
| GCEM              | Partial            | Yes       | Limited | No               | Yes                     |
| OpenLibm          | Yes                | No        | Yes     | Limited          | Yes                     |
| SLEEF             | Partial            | No        | Yes     | Yes              | Yes                     |
| CCMath            | Yes                | Yes       | Yes     | Yes              | Yes                     |

CCMath targets:

- Standards conformance
- Compile-time evaluation
- Runtime execution
- SIMD acceleration
- Numerical correctness
- Cross-platform portability

These goals overlap with the comparison columns plus numerical validation. Per-function progress is tracked in [STATUS.md](docs/STATUS.md).

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

Implemented functions follow the rules the C and C++ standards set for `<cmath>`.

### Compile-Time Evaluation

```cpp
constexpr auto value = ccm::sin(0.5);

static_assert(value > 0.0);
```

There is no separate constexpr API. When constant evaluation is possible, the public symbol participates in it.

### Runtime Execution

```cpp
double result = ccm::exp(x);
```

When constant evaluation is not possible, the same symbols dispatch to runtime paths.

### SIMD Acceleration

Selected functions pick up SIMD on supported targets:

- SSE2
- SSE4
- AVX2
- ARM NEON

### Numerical Correctness

We aim for correct rounding under all four IEEE rounding modes. Coverage is function-by-function: some paths are validated today under all modes, while other functions still target round-to-nearest ties-to-even first.

ULP harnesses, all-mode rounding probes, worst-case grids, and cross-compiler CI matrices live in-tree. The validation section below describes how they run in practice.

---

## Function Coverage

Work spans trigonometric, exponential, power, rounding, classification, and utility categories from the C and C++ mathematical library. Several modules are still in flight. [STATUS.md](docs/STATUS.md) lists what ships today and what remains open.

---

## Validation and Verification

Validation follows patterns from LLVM-libc, CORE-MATH, and glibc. CI covers:

### Continuous Integration

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
- C++23
- C++26 / latest

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

Rigorous jobs compare against MPFR and CORE-MATH oracles.

#### MPFR Validation

Within a rigorous build, MPFR-backed tests use the `mpfr` label:

```bash
ctest -L mpfr
```

Includes:

- Extended function corpora
- Exceptional-value matrices
- Adversarial search

Linux and macOS use system MPFR/GMP.

Windows uses MPFR provided through vcpkg.

#### CORE-MATH Validation

Within a rigorous build, CORE-MATH-backed tests use the `coremath` label:

```bash
ctest -L coremath
```

Correctly-rounded reference comparisons under all four IEEE rounding modes on finite inputs.

#### Proof Verification

When available, CCMath uses:

- Gappa
- Sollya

to refresh and verify proof artifacts for supported implementations.

### Fuzz Testing

Fuzz smoke tests run in dedicated macOS and Linux CI jobs with full LLVM Clang (libFuzzer requires the fuzzer sanitizer runtime). Windows matrix jobs use MSVC and are outside libFuzzer coverage today.

### Static Analysis and Security

CodeQL, OpenSSF Scorecard, warning-as-error builds, and strict compiler warning gates run in CI.

### Style and Consistency

clang-format runs in CI on every platform matrix job and on pull requests that touch headers. clang-tidy is available locally via lint.sh and lint.bat.

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
    GIT_TAG v0.3.0
)

FetchContent_MakeAvailable(ccmath)

target_link_libraries(
    main
    PRIVATE
    ccmath::ccmath
)
```

You may also vendor the headers directly.

### Secondary build systems (Meson and Premake)

CMake is the primary and authoritative build system. Meson and Premake support library consumption only: include paths, compile definitions, the generated `version.hpp`, and the library-required feature validation that a consumer needs. They do not define examples, smoke binaries, tests, benchmarks, fuzzing targets, or third-party dependency fetches.

After configuring CMake, regenerate the secondary build files:

```bash
cmake --preset ninja-clang-debug -DCCMATH_BUILD_TESTS=OFF -DCCMATH_BUILD_EXAMPLES=OFF
cmake --build out/build/ninja/ninja-clang-debug --target ccmath-generate-secondary-builds
```

Meson consumer usage after regeneration:

```meson
ccmath_dep = dependency('ccmath', fallback : ['ccmath', 'ccmath_dep'])
executable('my-app', 'main.cpp', dependencies : ccmath_dep)
```

Premake consumer usage after regeneration:

```lua
include("../ccmath/premake5.lua")

project "my-app"
    kind "ConsoleApp"
    language "C++"
    files { "main.cpp" }
    ccmath.use()
```

The generated root `meson.build` exposes `ccmath_dep` and overrides the `ccmath` dependency name for Meson consumers. The generated root `premake5.lua` exposes `ccmath.use()`, `ccmath.include_dirs()`, and `ccmath.defines()` for Premake consumers. Library-facing options are declared once in `cmake/config/BuildManifest.cmake` and exported through `ccmath-build-manifest.json`.

### Vendored integration testing

[`integration/vendored-consumer`](integration/vendored-consumer) simulates an external project that vendors CCMath headers under `third_party/ccmath`. CI copies the public headers and generated `version.hpp`, then builds the consumer with CMake, Meson, and Premake.

```bash
cmake --preset ninja-clang-debug -DCCMATH_BUILD_TESTS=OFF -DCCMATH_BUILD_EXAMPLES=OFF
bash tools/integration/prepare_vendored_tree.sh out/build/ninja/ninja-clang-debug

cmake -S integration/vendored-consumer -B integration/vendored-consumer/build-cmake -G Ninja
cmake --build integration/vendored-consumer/build-cmake

meson setup integration/vendored-consumer/build-meson integration/vendored-consumer
meson compile -C integration/vendored-consumer/build-meson

cd integration/vendored-consumer && premake5 gmake && make config=debug
```

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

If your project uses CCMath and is publicly available, open a PR to add it to this list.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md), [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md), and [SECURITY.md](SECURITY.md). Bug reports and design questions go to GitHub Issues or Discord.

---

## License

Apache License v2.0 with LLVM Exceptions. See [LICENSE](LICENSE).

---

## Acknowledgments

Algorithm and validation ideas draw on LLVM-libc, GCC libm, CORE-MATH, glibc math, and OpenLibm.
