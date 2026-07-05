# Contributing to CCMath

For help understanding how to implement elementary functions in CCMath, see
[Approximating Functions: A Practical Guide to Sollya and Function Implementation](docs/approximating_functions/APPROXIMATING_FUNCTIONS.pdf).

PRs go to the `main` branch. Bug reports and design questions belong in
[Issues](https://github.com/Rinzii/ccmath/issues).

By contributing you confirm the change is yours to license and compatible with
Apache-2.0 WITH LLVM-exception.

## Project layout

A quick map of the repository:

- `include/ccmath/` is the header-only library. Each function sits under its family
  (`math/power/`, `math/trig/`, and so on), and the internals, including the
  `builtin`, `gen`, and `rt` kernels, sit under `include/ccmath/internal/`.
- `tests/` holds the test suites, with the ULP and conformance harnesses under
  `tests/shared/utils` and the MPFR oracles under `tests/shared/oracle`.
- `docs/` has the longer guides, and `cmake/`, `CMakeLists.txt`, and `CMakePresets.json`
  drive the build.

## Core tenets

1. Always follow the C and C++ standard requirements for `<cmath>`. Match the specified semantics, domain and range
   handling, special values, and error reporting (errno and the floating-point exception flags).
2. Aim for a correctly rounded result in all four rounding modes, with documented compromises up to 4 ULP or fewer modes
   allowed to ship sooner. See [Accuracy and rounding goals](#accuracy-and-rounding-goals).
3. Prefer constexpr evaluation where the standard allows it (`gen` kernels, `static_assert` smoke).
4. Keep runtime performance in the same ballpark as the platform libm on the cases it covers.
5. Follow the existing dispatch layout (`builtin` / `gen` / `rt`, or impl-direct where the family already does).
6. Prefer portable generic kernels over platform forks when both satisfy the standard.
7. Mirror supported compiler behavior where the standard leaves details unspecified (for example `fpclassify` payload
   bits).
8. Add tests in the same change. Cover standards-mandated edge cases and ULP checks on public APIs.
9. Doxygen on exported entry points (`@brief`, `@param`, `@return`).
10. Keep the public header layout and naming conventions aligned with [cppreference
    `<cmath>`](https://en.cppreference.com/w/cpp/header/cmath).

## Accuracy and rounding goals

The end goal for every function is a correctly rounded result, bit-for-bit the
infinitely precise value rounded to the destination type, in all four IEEE
rounding modes (`FE_TONEAREST`, `FE_DOWNWARD`, `FE_UPWARD`, `FE_TOWARDZERO`). That
is the bar the library is held to.

We accept documented compromises that ship in-contract functionality
sooner:

- A function that is not yet correctly rounded may ship with a stated, tested
  error bound of no more than 4 ULP. The ULP suites under `tests/shared/utils`
  check against the platform libm, and an MPFR oracle checks against the correctly
  rounded value where one is targeted.
- A function may support fewer than the four rounding modes. Shipping
  `FE_TONEAREST` first, with the directed modes to follow, is fine.

A compromise only counts if it is written down. State the current ULP bound and
the supported rounding modes at the function, say what is missing, and leave a
`TODO:` plus a tracking issue so the gap stays visible. An undocumented one
is a bug. A documented one is a known limitation we can close later.

Accuracy and standards conformance are the top priority. Performance is the next
priority after that. Once a function is correct it should stay in the same ballpark
as the platform libm where it is implemented, and the usual shape is a fast common
case backed by an accurate fallback rather than one slow implementation. When the
two genuinely conflict, correctness wins, but reach for a layered design before
trading away speed.

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
git checkout main
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
git checkout main
cmake --preset=vs22-debug
cmake --build --preset=build-vs22-debug
ctest --preset=test-vs22-debug --output-on-failure
```

Preset names vary. Read `CMakePresets.json` before assuming a name exists locally.

## First PR

Fork the repo, then branch off `main`:

```bash
git clone https://github.com/<your-username>/ccmath.git
cd ccmath
git checkout main
git checkout -b <topic-branch>
```

Build and test with the presets above. Implementation work stays under
`include/ccmath/`, follows the patterns already present in the function family you
touch, and does not add `<cmath>` includes. When the change is ready, open a PR
against `main` as described in [Opening a pull request](#opening-a-pull-request).

## Issue and pull request workflow

### Branches

`main` is the development trunk and the target for almost all PRs. `release/N.x`
branches carry stabilization for a given minor series. Open fixes against `main`
first. A maintainer backports to a release branch when it applies. Never
force-push `main`.

### Releases and tags

Releases are annotated tags `vMAJOR.MINOR.PATCH`, with release candidates as
`vMAJOR.MINOR.PATCH-rc.N`. Below `1.0`, a minor bump may carry breaking API changes.

### Filing an issue

Bug reports use the form under `.github/ISSUE_TEMPLATE`. For a numeric defect,
give the exact function, the affected evaluation (constexpr, runtime, SIMD, or
deterministic mode), the scalar type, and the exact input, expected, and actual
values. Hex float literals such as `0x1.0p+2` avoid precision loss. State the
active rounding mode and the source of truth for the expected result (MPFR, the
C++ standard, or a reference libm).

### Agree on the approach first

Hard numeric work gets a design review on the issue before you implement it. For a
correctly-rounded function, or work on accuracy and ULP, rounding modes, or range
reduction, comment on the issue with your intended approach and the exact inputs you
plan to validate against, and wait for a maintainer to sign off before opening a
non-draft PR. A draft PR holding only the acceptance tests and a validation plan is a
good way to start that conversation. The point is to catch a wrong method in a short
comment rather than in a finished kernel, so you do not pour effort into an approach
that has to be redone. Likewise, hold off on implementing a new feature until a
maintainer approves the request.

### Opening a pull request

PRs target `main` unless the fix is specific to a release branch. Keep the change
minimal and aligned with the function family you touch. Use a one-line imperative
subject with no trailing period. Open the PR from a topic branch, since a PR from
your fork's `main`, `master`, or `develop` is closed automatically.

Before you request review, read your own diff once outside the editor, make sure
the warning-as-error build and the test presets are green, and write a short
summary of what changed and why. Small focused PRs get reviewed faster and more
carefully than large ones, so split unrelated work and leave drive-by refactors
to their own change. Once review starts, answer every comment, even if only to
say it is done. When a reviewer misreads the code, prefer making the code or a
why comment clearer over explaining only in the thread, since the next reader
sees the code and not the discussion.

### Cite your sources

Most numeric work builds on existing research, so share what you drew on. In the
issue or PR, name the algorithm and the reference behind it, whether that is a paper,
a worked derivation, or a reference implementation you studied for its behavior. Link
any analysis or proof artifact too, a Sollya script, a Gappa proof, or the MPFR
comparison you validated against. A reviewer can check a method far faster when its
source is named than when they have to reconstruct it from the code, and a future
maintainer can follow the reasoning rather than guess at it.

Also, if any code is adapted from another source rather than written from scratch, name
that source in a comment on the code itself, not only in the PR. Only adapt from a source
whose license is compatible with Apache-2.0 WITH LLVM-exception.

### Testing math changes

Tests go in the same change as the code. A few specifics matter for math
kernels and are easy to get wrong:

- Exercise the code you changed. A public call such as `ccm::pow` can resolve to
  a compiler builtin at runtime, so a plain runtime test may never reach the
  ccmath kernel. Drive the kernel through a `constexpr` context (a
  `static_assert`) or by calling the generic kernel directly (`ccm::gen::*_gen`).
- Validate numeric changes against an oracle. Use MPFR, Sollya, or Gappa and note
  the source of truth rather than eyeballing a value or trusting a system libm at
  a boundary. The MPFR oracle harnesses under `tests/shared/oracle` are the
  pattern to follow.
- Test boundaries, not a single point. Cover overflow, underflow, subnormals, and
  signed zero, along with the worst-case inputs for the function, not just one
  representative value.
- Cover every type the function is instantiated for, `float` and `double` at a
  minimum.
- For exception or errno behavior, use the fenv fixture at
  `tests/shared/utils/fenv_fixture.hpp`. The ULP and conformance suites live under
  `tests/shared/utils`.

## More detail

- [CONVENTIONS.md](CONVENTIONS.md) for the code-level conventions
- [docs/STATUS.md](docs/STATUS.md) for module completion
- [SECURITY.md](SECURITY.md) for vulnerability reports
