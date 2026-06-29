<!-- Please target the "main" branch when creating a pull request. Active development happens on main, and release/N.x branches receive only backported fixes. -->

## Summary

<!-- What changed and why. -->

## How tested

<!-- Which presets and platforms you ran. For numeric changes, the oracle used and the ULP or edge-case results. -->

## Checklist

- [ ] Targets `main` (or a `release/N.x` branch for a backport)
- [ ] Tests added or updated in this change
- [ ] No `<cmath>` include added under `include/`
- [ ] Formatting and lint pass locally

### For numeric or kernel changes

<!-- Delete this block if the PR does not change numeric behavior. -->

- [ ] Standard edge cases handled (NaN, signed zero, infinities, domain and range)
- [ ] Accuracy within the 4 ULP policy, validated against an oracle (MPFR, Sollya, or Gappa) with the source of truth noted
- [ ] Any accuracy or rounding-mode compromise is documented at the function with a `TODO` and a tracking issue
- [ ] Boundary inputs tested, not a single point (overflow, underflow, subnormals)
- [ ] The test reaches the changed kernel via `constexpr`/`static_assert` or a direct `ccm::gen::*_gen` call, not only a runtime call that may resolve to a builtin
- [ ] Every instantiated type covered (`float` and `double` at a minimum)
- [ ] Approach agreed on the issue first, for work labeled `D0`/`D1` or a numeric-rigor `A:` area

<!-- Area, size, and review-state labels are applied automatically. See CONTRIBUTING.md. -->
