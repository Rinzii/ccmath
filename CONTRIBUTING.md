# Contributing to CCMath

For help understanding how to implement elementary functions in CCMath, see
[Approximating Functions: A Practical Guide to Sollya and Function Implementation](docs/approximating_functions/APPROXIMATING_FUNCTIONS.pdf).

PRs go to the `main` branch. Bug reports and design questions belong in
[Issues](https://github.com/Rinzii/ccmath/issues) or
[Discord](https://discord.gg/p3mVxAbdmc).

By contributing you confirm the change is yours to license and compatible with
Apache-2.0 WITH LLVM-exception.

## Core tenets

1. Prefer constexpr evaluation where the standard allows it (`gen` paths, `static_assert` smoke).
2. Match `<cmath>` observable behavior. Correctly rounded in all four rounding modes is the goal, with documented compromises up to 4 ULP or fewer modes allowed to ship sooner. See [Accuracy and rounding goals](#accuracy-and-rounding-goals).
3. Keep runtime performance in the same ballpark as the platform libm on covered paths.
4. Follow the existing dispatch layout (`builtin` / `gen` / `rt`, or impl-direct where the family already does).
5. Prefer portable generic kernels over platform forks when both satisfy the standard.
6. Mirror supported compiler behavior where the standard leaves details unspecified (for example `fpclassify` payload bits).
7. Add tests in the same change. Cover standards-mandated edge cases and ULP checks on public APIs.
8. Doxygen on exported entry points (`@brief`, `@param`, `@return`).
9. Keep the public header layout aligned with [cppreference `<cmath>`](https://en.cppreference.com/w/cpp/header/cmath).

## Accuracy and rounding goals

The end goal for every function is a correctly rounded result, bit-for-bit the
infinitely precise value rounded to the destination type, in all four IEEE
rounding modes (`FE_TONEAREST`, `FE_DOWNWARD`, `FE_UPWARD`, `FE_TOWARDZERO`). That
is the bar the library is held to over time.

We accept documented compromises when they get working, in-contract functionality
out sooner:

- A function that is not yet correctly rounded may land with a stated, tested
  error bound of no more than 4 ULP. The ULP suites under `tests/shared/utils`
  check against the platform libm, and an MPFR oracle checks against the correctly
  rounded value where one is targeted.
- A function may support fewer than the four rounding modes. Landing
  `FE_TONEAREST` first, with the directed modes to follow, is fine.

A compromise only counts if it is written down. State the current ULP bound and
the supported rounding modes at the function, say what is missing, and leave a
`TODO(<your-handle>):` plus a tracking issue so the gap stays visible. An
undocumented shortfall is a bug. A documented one is a known limitation we can
close later.

Accuracy and standards conformance are the top priority. Performance is the next
priority after that. Once a function is correct it should stay in the same
ballpark as the platform libm on covered paths, and the usual shape is a fast
path with an accurate fallback rather than a single slow path. When the two
genuinely conflict, correctness wins, but reach for a layered design before
trading away speed.

## First PR

```bash
git clone https://github.com/<your-username>/ccmath.git
cd ccmath
git checkout main
git checkout -b <topic-branch>
```

Configure, build, and run the simple test preset:

```bash
cmake --preset=ninja-gcc-debug
cmake --build --preset=build-ninja-gcc-debug
ctest --preset=test-ninja-gcc-debug --output-on-failure
```

Commit with a one-line imperative subject (no trailing period). Open a PR against
`main` with a short summary of what changed and how you tested it.

Implementation work should stay under `include/ccmath/` using the patterns already
present in the function family you touch. Do not add `<cmath>` includes under
`include/`.

## Issue and pull request workflow

### Branches

`main` is the development trunk and the target for almost all PRs. `release/N.x`
branches carry stabilization for a given minor series. Open fixes against `main`
first. A maintainer backports to a release branch when it applies. Never
force-push `main`.

### Releases and tags

Releases are annotated tags in the form `vMAJOR.MINOR.PATCH`, for example
`v0.2.0`. Release candidates use the semver pre-release form
`vMAJOR.MINOR.PATCH-rc.N`, for example `v0.3.0-rc.1`. While the project is below
`1.0`, a minor bump may carry breaking API changes.

### Labels

Issues and PRs use a prefixed label taxonomy. Maintainers apply most of these
during triage. The groups are:

- `T:` type of work (`T: Bugfix`, `T: New Feature`, `T: Performance`, `T: Refactor`, `T: Cleanup`, `T: Documentation`, `T: Tests`, `T: Chore`)
- `A:` area, covering both function families (`A: Trig`, `A: Power`, `A: Expo`, and the rest) and numeric or implementation axes (`A: Correctly Rounded`, `A: Accuracy/ULP`, `A: Range Reduction`, `A: Rounding Modes`, `A: Edge Cases`, `A: Constexpr`, `A: Runtime Kernel`, `A: SIMD`, `A: Deterministic`, `A: Numeric Proof`, `A: Build`, `A: Tooling`)
- `D:` difficulty, from `D3: Low` for approachable work up to `D0: Irrational` for hard-to-round problems that may take unreasonable effort
- `P:` priority, from `P1: Critical` to `P4: Low`
- `S:` status through review (`S: Untriaged`, `S: Needs Discussion`, `S: Needs Review`, `S: Approved`, `S: Awaiting Changes`, `S: Blocked`, `S: Merge Conflict`, `S: Stale`, `S: DO NOT MERGE`, `S: Won't Fix`)
- `Bug:` reproduction state (`Bug: Needs Replicating`, `Bug: Replicated`)
- `Platform:`, `Arch:`, and `Compiler:` for environment-specific reports
- `Branch:` for the branch a PR targets (`Branch: main`, `Branch: release`)
- `Changes:` for the surface a PR touches (`Changes: Public API`, `Changes: Headers Only`)
- `size/` for PR size, from `size/XS` to `size/XL`

A contributor looking for a starting point should filter on `D3: Low` and
`good first issue`.

### Filing an issue

Bug reports use the form under `.github/ISSUE_TEMPLATE`. For a numeric defect,
give the exact function, the affected code path (constexpr, runtime, SIMD, or
deterministic mode), the scalar type, and the exact input, expected, and actual
values. Hex float literals such as `0x1.0p+2` avoid precision loss. State the
active rounding mode and the source of truth for the expected result (MPFR, the
C++ standard, or a reference libm). New bug reports start as `T: Bugfix` and
`S: Untriaged`.

### Agree the approach first

Hard numeric work is design-reviewed on the issue before it is implemented. If an
issue sits at the hard end of the `D:` scale (`D0` or `D1`) or carries
`A: Correctly Rounded`, `A: Accuracy/ULP`, `A: Rounding Modes`, or
`A: Range Reduction`, comment on the issue with your intended approach and the
exact inputs you plan to validate against, and wait for a maintainer to ack it
before opening a non-draft PR. A draft PR holding only the acceptance tests and a
validation plan is a good way to start that conversation, and a maintainer may
move the issue to `S: Needs Discussion` while it is settled. The point is to
catch a wrong method in a short comment rather than in a finished kernel, so you
do not pour effort into an approach that has to be redone.

### Opening a pull request

PRs target `main` unless the fix is specific to a release branch. Keep the
change minimal and aligned with the function family you touch. Add tests in the
same change. Use a one-line imperative subject with no trailing period.

Before you request review, read your own diff once outside the editor, make sure
the warning-as-error build and the test presets are green, and write a short
summary of what changed and why. Small focused PRs get reviewed faster and more
carefully than large ones, so split unrelated work and leave drive-by refactors
to their own change. Once review starts, answer every comment, even if only to
say it is done. When a reviewer misreads the code, prefer making the code or a
why comment clearer over explaining only in the thread, since the next reader
sees the code and not the discussion.

### Testing math changes

Tests land in the same change as the code. A few specifics matter for math
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

### Reviewing a pull request

A review is a dialogue, not a verdict. Ask rather than demand, assume the author
acted in good faith, and talk about the code as ours rather than yours. Give real
credit when something is done well.

Mark each comment as blocking or a nit so the author knows what gates the merge.
A wrong result, a standards violation, a red build, and a claim the change does
not actually deliver are blocking. Style, naming, and scoping are nits, and any
reviewer can raise those, while contract and architecture calls are left to a
maintainer.

Back a numeric claim with evidence. A failing input and its correctly-rounded
value from MPFR, a worst-case grid result, or a link to the algorithm reference
carries more than an assertion. Read the tests first, since the most common gap
here is a test that never reaches the kernel or only checks a single point. Do
not gatekeep past the accuracy policy. In-contract and honest is the bar, not
sub-ULP perfection everywhere. If a thread keeps going back and forth, move it to
a synchronous chat and write up the outcome.

### Automation

Several labels are applied for you, so you do not need to set them by hand:

- `A:` area labels from the files a PR changes, plus `Changes: Public API`
- `size/` from the number of lines changed
- `Branch: main` or `Branch: release` from the base branch
- `S: Untriaged` on new issues and PRs that open without labels, cleared once any other label is added
- `S: Merge Conflict` when a PR develops conflicts
- `S: Needs Review`, `S: Approved`, and `S: Awaiting Changes` as a maintainer review progresses
- `S: Stale` after 60 days of inactivity, removed on the next update

PRs opened from a fork's `main`, `master`, or `develop` branch are closed
automatically with a note to move the work to a topic branch.

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

## More detail

- [docs/STATUS.md](docs/STATUS.md) for module completion
- [SECURITY.md](SECURITY.md) for vulnerability reports
