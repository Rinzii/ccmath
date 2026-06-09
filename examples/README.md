# CCMath examples

Small programs that show constexpr use, numeric behavior, and practical call patterns. The examples stay registry-driven in CMake, but the source files and CLion run configs are kept flat so moving between demos is fast.

## Quick start

```bash
cmake --preset clion-clang-debug-examples
cmake --build --preset build-clion-clang-debug-examples
out/clang-examples/examples/ccmath-example-unit_circle
```

CLion run configurations live under examples/item/.

## Layout

```text
examples/
  common/          # shared printing helpers
  src/             # one .cpp per example id
  item/            # flat CLion *.run.xml entries
```

CMake helpers: cmake/helpers/CcmExampleHelpers.cmake. Topic groupings remain in cmake/config/ExampleModuleRegistry.cmake.

## Modules

| Module | Examples | Topic |
| --- | --- | --- |
| smoke | constexpr_smoke | minimal compile and runtime check |
| basic | constexpr_abs, signed_magnitude | constexpr vs runtime, fdim and copysign |
| trig | unit_circle | sin and cos table, identity spot check |
| power | hypot_distance | planar and spatial distance |
| expo | decibel_gain | log10 in dB conversions |
| nearest | rounding_edge_cases | nearbyint, round, floor, ceil, trunc |
| compare | fp_classes | fpclassify and signbit tour |

Targets: ccmath-example-<id>. Aggregate: ccmath-example-all.

## Add an example

1. Add examples/src/my_demo.cpp with a short main and optional common/io.hpp helpers.
2. Append my_demo to CCMATH_EXAMPLE_MODULE_<module>_EXAMPLES in cmake/config/ExampleModuleRegistry.cmake.
3. Reconfigure with CCMATH_BUILD_EXAMPLES=ON and build ccmath-example-my_demo.
4. Optional: add examples/item/my_demo.run.xml for CLion.

## Build one target

```bash
cmake --preset clion-clang-debug-examples
cmake --build out/clang-examples --target ccmath-example-unit_circle
```

Compiler notes: examples/docs/compiler_support.md
