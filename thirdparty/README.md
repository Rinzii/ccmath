# Third-party dependencies

The CCMath library under `include/ccmath/` is header-only and has no external
runtime dependencies. Consumers can link `ccmath::ccmath` alone. Nothing in this
directory ships with the library API.

This folder exists for **development tooling** only. [CMakeLists.txt](CMakeLists.txt)
uses CMake `FetchContent` to download dependencies when you turn on tests or
benchmarks. They land under your build tree, not in the repo.

## Fetched when tests are enabled

Set `CCMATH_BUILD_TESTS=ON` (the default for standalone builds) to pull in:

| Package | Role |
| --- | --- |
| [GoogleTest](https://github.com/google/googletest) | Unit and regression tests (`ccmath::test`) |
| [Abseil](https://github.com/abseil/abseil-cpp) | Test harness support |
| [RE2](https://github.com/google/re2) | Test harness support |

These link through the `ccmath::ext` interface target. That target is for tests
and benchmarks, not for applications that only need math headers.

Use `CCMATH_FIND_GTEST_PACKAGE=ON` if you prefer a system-installed GTest instead
of FetchContent.

## Fetched when benchmarks are enabled

Set `CCMATH_BUILD_BENCHMARKS=ON` to fetch
[Google Benchmark](https://github.com/google/benchmark). Targets under
`benchmarks/` link `benchmark::benchmark` through `ccmath::ext`.

## Optional tooling elsewhere in the tree

Rigorous validation, fuzzing, and proof checks use other optional dependencies
that are **not** vendored here:

| Tool | Purpose | How it is obtained |
| --- | --- | --- |
| MPFR / GMP | Rigorous oracle campaigns | System packages on Linux and macOS. [vcpkg manifest](../cmake/vcpkg/vcpkg.json) on Windows. |
| CORE-MATH | Correctly-rounded pow references | Prefix install via [tools/coremath/build_prefix.sh](../tools/coremath/build_prefix.sh) or `CCMATH_COREMATH_ROOT`. |
| Gappa / Sollya | Formal proof smoke for pow kernels | Installed on the host PATH when available. |
| LLVM libFuzzer | Fuzz targets (`CCMATH_BUILD_FUZZING`) | Compiler-rt from a full LLVM Clang toolchain. |

See the [Validation](../README.md#validation) section in the root README for how
those pieces fit into CI.

## Library headers stay dependency-free

Production code under `include/ccmath/` must not include `<cmath>` or call into
these third-party packages. Only `tests/`, `benchmarks/`, and `fuzzing/` may use
them, plus comparison against platform libm where the test policy allows it.
