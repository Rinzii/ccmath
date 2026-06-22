<img src="docs/resources/ccmath_logo_white.png" style="width: 35%;" alt="CCMath Logo">

# CCMath

**A constexpr-first `<cmath>` for C++17 and later**

[![Release](https://img.shields.io/github/v/release/Rinzii/ccmath?sort=semver&label=release)](https://github.com/Rinzii/ccmath/releases/latest)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/Rinzii/ccmath/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Rinzii/ccmath)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/9402/badge)](https://www.bestpractices.dev/projects/9402)
[![License](https://img.shields.io/badge/License-Apache%202.0%20WITH%20LLVM--exception-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Discord](https://img.shields.io/discord/1286067628456284244?label=Discord)](https://discord.gg/p3mVxAbdmc)

| Branch          | Linux                                                                                                                                                                                                              | macOS                                                                                                                                                                                                              | Windows                                                                                                                                                                                                                  |
|-----------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `main`          | [![Linux main](https://github.com/Rinzii/ccmath/actions/workflows/ci-linux.yml/badge.svg?branch=main)](https://github.com/Rinzii/ccmath/actions/workflows/ci-linux.yml?query=branch%3Amain)                        | [![macOS main](https://github.com/Rinzii/ccmath/actions/workflows/ci-macos.yml/badge.svg?branch=main)](https://github.com/Rinzii/ccmath/actions/workflows/ci-macos.yml?query=branch%3Amain)                        | [![Windows main](https://github.com/Rinzii/ccmath/actions/workflows/ci-windows.yml/badge.svg?branch=main)](https://github.com/Rinzii/ccmath/actions/workflows/ci-windows.yml?query=branch%3Amain)                        |
| `release/0.3.x` | [![Linux release](https://github.com/Rinzii/ccmath/actions/workflows/ci-linux.yml/badge.svg?branch=release/0.3.x)](https://github.com/Rinzii/ccmath/actions/workflows/ci-linux.yml?query=branch%3Arelease%2F0.3.x) | [![macOS release](https://github.com/Rinzii/ccmath/actions/workflows/ci-macos.yml/badge.svg?branch=release/0.3.x)](https://github.com/Rinzii/ccmath/actions/workflows/ci-macos.yml?query=branch%3Arelease%2F0.3.x) | [![Windows release](https://github.com/Rinzii/ccmath/actions/workflows/ci-windows.yml/badge.svg?branch=release/0.3.x)](https://github.com/Rinzii/ccmath/actions/workflows/ci-windows.yml?query=branch%3Arelease%2F0.3.x) |

CCMath is a header-only library with a `<cmath>`-style API. The same entry points constexpr-evaluate where the standard
and the compiler allow, dispatch to portable runtime implementations everywhere else, and pick up SIMD on selected
functions when the target supports it.

This README also describes what CCMath is working toward, so some of these claims are goals rather than finished
work. [STATUS.md](docs/STATUS.md) tracks where each module and function actually stands today. For the
approximation-driven function work, the implementation guide lives
in [APPROXIMATING_FUNCTIONS.pdf](docs/approximating_functions/APPROXIMATING_FUNCTIONS.pdf). Questions and design
discussion are welcome on [Discord](https://discord.gg/p3mVxAbdmc).

---

## Contents

- [Why CCMath](#why-ccmath)
- [Quick start](#quick-start)
- [Accuracy and conformance](#accuracy-and-conformance)
- [Performance](#performance)
- [Supported platforms and compilers](#supported-platforms-and-compilers)
- [Installation](#installation)
- [Validation](#validation)
- [Contributing](#contributing)
- [Projects using CCMath](#projects-using-ccmath)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## Why CCMath

Most `<cmath>` implementations are built around different tradeoffs, and few of them combine standards-oriented
semantics, constexpr evaluation, full runtime execution, SIMD, and portable in-tree code in one place. That combination
is what CCMath is after.

| Library           | Standards-Oriented | Constexpr | Runtime | SIMD             | Portable Implementation |
|-------------------|--------------------|-----------|---------|------------------|-------------------------|
| `std::cmath`      | Yes                | Partial   | Yes     | Vendor dependent | No                      |
| Compiler builtins | Partial            | Partial   | Yes     | No               | No                      |
| GCEM              | Partial            | Yes       | Limited | No               | Yes                     |
| OpenLibm          | Yes                | No        | Yes     | Limited          | Yes                     |
| SLEEF             | Partial            | No        | Yes     | Yes              | Yes                     |
| CCMath            | Yes                | Yes       | Yes     | Yes              | Yes                     |

Numerical correctness and performance are not columns in the table, and each gets its own section below. Per-function
progress is tracked in [STATUS.md](docs/STATUS.md).

---

## Quick start

There is no separate constexpr API. When constant evaluation is possible the public symbol participates in it, and when
it is not, the same symbol dispatches to a runtime implementation. You write `ccm::sqrt` once and it works in both
contexts.

The monolithic header pulls in everything:

```cpp
#include <ccmath/ccmath.hpp>
#include <iostream>

int main() {
    constexpr double result = ccm::sqrt(25.0);

    std::cout << result << '\n';
}
```

You can also include a single function when that is all you need:

```cpp
#include <ccmath/math/expo/log.hpp>
#include <iostream>

int main() {
    constexpr double result = ccm::log(12.0);

    std::cout << result << '\n';
}
```

Both the compile-time and the runtime forms go through the same entry point:

```cpp
// Compile time: usable in a constant expression.
constexpr double a = ccm::sin(0.5);
static_assert(a > 0.0);

// Runtime: the same call on a value the compiler cannot fold.
double exp_at(double x) { return ccm::exp(x); }
```

---

## Accuracy and conformance

Accuracy is the first priority, ahead of performance. The goal for every function is a correctly rounded result under
all four IEEE-754 rounding modes. Coverage is function-by-function, so some already meet that bar today while others are
still getting there.

| Status                       | What it means                                                                      |
|------------------------------|------------------------------------------------------------------------------------|
| Correctly rounded, all modes | Bit-exact under all four IEEE-754 rounding modes                                   |
| Correctly rounded, nearest   | Bit-exact under round-to-nearest ties-to-even, other modes not yet covered         |
| Bounded error                | Within a documented bound of no more than 4 ULP until correct rounding is finished |

Where each function sits on that scale is listed in [STATUS.md](docs/STATUS.md). A gap that is not written down there is
a bug.

Alongside accuracy, CCMath is designed to follow the behavior the C and C++ standards pin down for `<cmath>`. That
covers function semantics, domain and range handling, NaN propagation, infinity behavior, floating-point classification,
rounding behavior, and the edge cases the standards specify. Implemented functions follow those rules exactly.

We cover the trigonometric, exponential, power, rounding, classification, and utility categories of the C and C++
mathematical library, though several functions are not finished yet.

---

## Performance

After accuracy, performance is the next thing we care about. The idea is that once a function is correct it should still
keep pace with the platform libm, so the common case stays quick and we only fall back to the fully accurate version
when we have to. A lot of functions also pick up SIMD where the hardware supports it, currently SSE2, SSE4, AVX2, and
ARM NEON. We keep benchmarks against the platform libm in the tree, so a regression is easy to catch.

---

## Supported platforms and compilers

CCMath builds and is tested across Linux, macOS, and Windows, under GCC, Clang, Apple Clang, and MSVC, on C++17 through
C++26 in both Debug and Release. The CI matrix that enforces this is described under [validation](#validation).

| Compiler       | Minimum version |
|----------------|-----------------|
| GCC            | 11.1            |
| Clang          | 9.0.0           |
| Apple Clang    | 14.0.3          |
| MSVC           | 19.26           |
| Intel DPC++    | 2022.0.0        |
| NVIDIA HPC SDK | 22.7            |

> [!NOTE]
> Minimum compiler versions are not final yet, so treat them as guidance for now.

---

## Installation

The library itself has no required dependencies, so consuming it is just a matter of putting the headers where your
compiler can find them.

### CMake

CMake is the primary build system.

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

You can also vendor the headers directly.

### Secondary build systems (Meson and Premake)

CMake is the primary build system, but the repository also ships ready-to-use `meson.build` and `premake5.lua`. Both are
self-contained, since each generates `ccmath/version.hpp` itself, so a Meson or Premake consumer never needs a CMake
configure first. They expose include directories and compile definitions only, not examples, tests, benchmarks, or
fuzzing targets.

Meson, with ccmath as a subproject:

```meson
ccmath_dep = dependency('ccmath', fallback : ['ccmath', 'ccmath_dep'])
executable('my-app', 'main.cpp', dependencies : ccmath_dep)
```

Premake:

```lua
include("path/to/ccmath/premake5.lua")

project "my-app"
    kind "ConsoleApp"
    language "C++"
    files { "main.cpp" }
    ccmath.use()
```

---

## Validation

CCMath is validated thoroughly. The approach borrows from LLVM-libc, CORE-MATH, and glibc: every public entry point is
checked for standards edge cases and ULP error against the platform libm. Beyond that, a rigorous suite cross-checks the
harder cases against MPFR and CORE-MATH oracles under all four IEEE rounding modes, Gappa and Sollya back the proof
artifacts, and differential fuzzing catches kernel disagreements. All of it runs in CI across Linux, macOS, and Windows,
under GCC, Clang, Apple Clang, and MSVC, on C++17 through C++26, alongside CodeQL, OpenSSF Scorecard, and
warning-as-error builds.

How to run any of these suites locally is documented in [CONTRIBUTING.md](CONTRIBUTING.md), and per-function accuracy
status is tracked in [STATUS.md](docs/STATUS.md).

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md), [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md), and [SECURITY.md](SECURITY.md). Bug
reports and design questions go to GitHub Issues or Discord.

---

## Projects using CCMath

The following projects publicly use CCMath:

- [Fornani](https://github.com/swagween/fornani)

If your project uses CCMath and is publicly available, open a PR to add it to this list.

---

## License

Apache License v2.0 with LLVM Exceptions. See [LICENSE](LICENSE).

---

## Acknowledgments

Algorithm and validation ideas draw on LLVM-libc, GCC libm, CORE-MATH, glibc math, and OpenLibm.
