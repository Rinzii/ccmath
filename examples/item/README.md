# Example run configurations (CLion)

CLion picks up any `*.run.xml` file under the project tree. The example configs live directly under `examples/item/` so they show up in one `examples` folder instead of being split across nested groups.

## One-time setup

1. **CMake profile:** Settings > CMake > Profiles > add preset **CLion Clang Debug + examples** (
   `clion-clang-debug-examples`). Output directory: `out/clang-examples`.
2. **Build:** Run `cmake --build --preset build-clion-clang-debug-examples` once, or let CLion build on first launch.
3. **Run:** Open the Run menu and choose any entry under **examples/**.

## Run configurations

Each current example has a matching CMake run configuration:

- `examples / smoke / constexpr_smoke`
- `examples / basic / constexpr_abs`
- `examples / basic / signed_magnitude`
- `examples / trig / unit_circle`
- `examples / power / hypot_distance`
- `examples / expo / decibel_gain`
- `examples / nearest / rounding_edge_cases`
- `examples / compare / fp_classes`
