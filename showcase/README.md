# CCMath showcase

Publish-quality comparisons of CCMath against other constexpr math libraries. Phase 1 covers double `sqrt` and `sin` against GCEM, with `std` as a runtime baseline in performance benches only.

## Quick start

```bash
./showcase/run build    # configure + build (CLion preset when available)
./showcase/run doctor   # verify binaries
./showcase/run all      # full suite, reports in showcase/reports/
```

Run `./showcase/run help` for every command.

### CLion

1. Enable CMake profile **CLion Clang Release + showcase** (`clion-clang-release-showcase`).
2. Open the Run menu. Configurations live under **showcase/** (from `showcase/item/*.run.xml`).
3. Run **showcase / build** once, then **showcase / suite / all** or any feature-specific config.

See `showcase/item/README.md` for the full configuration list.

## Measurement axes

1. **Runtime performance** via Google Benchmark
2. **Accuracy** vs MPFR at 256-bit precision
3. **Compile-time cost** via isolated probe translation units

## CCMath paths under test

| Path                     | What it measures                                           |
|--------------------------|------------------------------------------------------------|
| `ccmath/public_default`  | Production entry point (`ccm::sqrt`, `ccm::sin`)           |
| `ccmath/generic_gen`     | Generic kernel (`ccm::gen::sqrt_gen`, `ccm::gen::sin_gen`) |
| `ccmath/runtime_rt`      | Runtime dispatcher (`ccm::rt::sqrt_rt`, `ccm::rt::sin_rt`) |
| `ccmath/runtime_simd`    | SIMD fast path when compiled in                            |
| `ccmath/runtime_builtin` | Direct compiler builtin when available                     |

External backends in Phase 1: **GCEM** (accuracy + perf + compile), **std** (perf only).

The generic_gen path calls the raw generic kernel directly. For sin this bypasses runtime range reduction, so large-magnitude inputs are expected to diverge from MPFR while still exercising the kernel under test.

## Prerequisites

- CMake 3.18+, Ninja, C++20 compiler
- MPFR and GMP for accuracy executables
- Python 3 for compile probes and the suite orchestrator

## CMake presets

| Preset                         | Use                                         |
|--------------------------------|---------------------------------------------|
| `clion-clang-release-showcase` | CLion (single-config, `out/clang-showcase`) |
| `ninja-clang-release-showcase` | Terminal / CI-style (multi-config Release)  |

```bash
cmake --preset clion-clang-release-showcase
cmake --build --preset build-clion-clang-release-showcase
```

Or use `./showcase/run build`, which picks the CLion preset when present.

## Reports

Output lands under `showcase/reports/` (gitignored):

- `perf/`: Google Benchmark JSON
- `accuracy/`: per-path failure logs
- `compile/`: probe timings and object sizes
- `summary.json`: merged suite output

## Fairness notes

- GCEM is header-only. Runtime benches measure inlined code, not a separate shared library.
- CCMath public and runtime paths may use libm builtins or SIMD. That is intentional.
- Compile probes use static_assert instantiations as a footprint proxy.

## Extending

- New backend: adapter under `showcase/common/backends/`, register in `showcase/config/backends.json`
- New function: entry in `showcase/config/functions.json`, extend `ccmath_paths.hpp`, add bench + accuracy support
- New CLion run: add a `*.run.xml` under `showcase/item/` (CLion scans the whole project tree)
