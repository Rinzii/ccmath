# Benchmark tooling

BenchmarkModuleRegistry.cmake maps modules to function lists. CcmBenchHelpers.cmake builds ccm_benchmark_<function> executables via ccmath_apply_bench_registry.

## Options

| Option | Default |
| --- | --- |
| CCM_BENCH_BASIC | OFF |
| CCM_BENCH_POWER | OFF |
| CCM_BENCH_NEAREST | ON |
| CCM_BENCH_ALL | OFF |

Aggregate target: ccm_benchmark_all.

## Macros (benchmarks/shared/register.hpp)

| Macro | Registers |
| --- | --- |
| CCMATH_BENCH_UNARY_COMPARE | std vs ccm on scalar and random vectors, CT path |
| CCMATH_BENCH_*_COMPARE_PATHS | Same with _rt suites when the ccm body forces runtime |
| CCMATH_BENCH_*_PROFILE | Fixed literal, named for asmlab profiles |
| CCMATH_BENCH_*_PROFILE_PATHS | Profile with CT and RT |

Per-target path opt-out: CCM_BM_CONFIG_NO_CT or CCM_BM_CONFIG_NO_RT through ccmath_add_bench_target COMPILE_DEFINITIONS.

## New function

Add benchmarks/src/math/<module>/foo.bench.cpp, append foo to CCMATH_BENCH_MODULE_<module>_FUNCTIONS, enable the module option at configure.

## Run

```bash
cmake --preset <preset> -DCCM_BENCH_POWER=ON
cmake --build out/<preset> --target ccm_benchmark_sqrt
```

powf_impl and other impl-kernel benches are asmlab targets under tools/asmlab/bench/, added when CCM_BENCH_POWER is on.
