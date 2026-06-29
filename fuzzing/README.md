# Fuzz tooling

FuzzModuleRegistry.cmake lists modules. CcmFuzzHelpers.cmake registers each ccmath-fuzz-<module> executable, configure-time seeds, smoke CTest, and campaign target.

Needs LLVM Clang with libFuzzer (CCMATH_BUILD_FUZZING=ON, CMAKE_CXX_COMPILER pointing at that clang++).

## Targets per module

| Artifact | Name |
| --- | --- |
| Executable | ccmath-fuzz-<module> |
| Smoke | ccmath-fuzz-<module>-smoke |
| Campaign | ccmath-fuzz-<module>-campaign |
| All modules | ccmath-fuzz-all, ccmath-fuzz-smoke |

## Driver

CCMATH_FUZZ_DRIVER in fuzzing/shared/entry.hpp runs the template for float and double. Pair with Inputs loaders in input.hpp and fuzz_*_vs_std in harness.hpp.

## New module

Add fuzzing/src/math/foo_fuzz.cpp, append foo to CCMATH_FUZZ_MODULE_ORDER, reconfigure, build ccmath-fuzz-foo-smoke.

## Smoke

```bash
cmake --preset ninja-fuzz-debug -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++
ctest -C Debug -R 'ccmath-fuzz-.*-smoke' --output-on-failure
```

Seeds: fuzzing/scripts/generate_seeds.py at configure time. Runners: fuzzing/cmake/run_fuzz_smoke.cmake and run_fuzz_campaign.cmake. Dictionary entries in fuzzing/dicts/ccmath.dict use \xNN hex escapes.
