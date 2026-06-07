# Contributing to CCMath

PRs go to the `dev` branch. Bug reports and design questions belong in
[Issues](https://github.com/Rinzii/ccmath/issues) or
[Discord](https://discord.gg/p3mVxAbdmc).

By contributing you confirm the change is yours to license and compatible with
Apache-2.0 WITH LLVM-exception.

## Core tenets

1. Prefer constexpr evaluation where the standard allows it (`gen` paths, `static_assert` smoke).
2. Match `<cmath>` observable behavior. Accuracy targets are documented in tests, typically within 4 ULP of the platform libm unless noted.
3. Keep runtime performance in the same ballpark as the platform libm on covered paths.
4. Follow the existing dispatch layout (`builtin` / `gen` / `rt`, or impl-direct where the family already does).
5. Prefer portable generic kernels over platform forks when both satisfy the standard.
6. Mirror supported compiler behavior where the standard leaves details unspecified (for example `fpclassify` payload bits).
7. Add tests in the same change. Cover standards-mandated edge cases and ULP checks on public APIs.
8. Doxygen on exported entry points (`@brief`, `@param`, `@return`).
9. Keep the public header layout aligned with [cppreference `<cmath>`](https://en.cppreference.com/w/cpp/header/cmath).

## First PR

```bash
git clone https://github.com/<your-username>/ccmath.git
cd ccmath
git checkout dev
git checkout -b <topic-branch>
```

Configure, build, and run the simple test preset:

```bash
cmake --preset=ninja-gcc-debug
cmake --build --preset=build-ninja-gcc-debug
ctest --preset=test-ninja-gcc-debug --output-on-failure
```

Commit with a one-line imperative subject (no trailing period). Open a PR against
`dev` with a short summary of what changed and how you tested it.

Implementation work should stay under `include/ccmath/` using the patterns already
present in the function family you touch. Do not add `<cmath>` includes under
`include/`.

## Build from the command line

### Prerequisites

- CMake 3.18+
- Git
- Ninja (Linux and macOS)
- Clang or GCC (Linux and macOS)
- Visual Studio 2022 (Windows)

### Linux and macOS

```bash
git clone https://github.com/Rinzii/ccmath.git
cd ccmath
git checkout dev
cmake --preset=ninja-gcc-debug
cmake --build --preset=build-ninja-gcc-debug
ctest --preset=test-ninja-gcc-debug --output-on-failure
```

Other configure presets are listed in `CMakePresets.json` (`ninja-clang-debug`,
`ninja-clang-release`, and similar). Pass `-DCMAKE_CXX_STANDARD=17` or `20` on the
configure line when you need a specific standard.

### Windows (Visual Studio 2022)

```powershell
git clone https://github.com/Rinzii/ccmath.git
cd ccmath
git checkout dev
cmake --preset=vs22-debug
cmake --build --preset=build-vs22-debug
ctest --preset=test-vs22-debug --output-on-failure
```

Preset names vary. Read `CMakePresets.json` before assuming a name exists locally.

## More detail

- [docs/STATUS.md](docs/STATUS.md) for module completion
- [SECURITY.md](SECURITY.md) for vulnerability reports
