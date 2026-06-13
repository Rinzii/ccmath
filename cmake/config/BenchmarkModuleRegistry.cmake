# Declarative benchmark registry.
# Add a function: drop benchmarks/src/math/<module>/<fn>.bench.cpp and append below.

set(CCMATH_BENCH_MODULE_ORDER basic power nearest compare)

set(CCMATH_BENCH_MODULE_basic_FUNCTIONS abs fdim fma)
set(CCMATH_BENCH_MODULE_basic_OPTION CCM_BENCH_BASIC)

set(CCMATH_BENCH_MODULE_power_FUNCTIONS sqrt)
set(CCMATH_BENCH_MODULE_power_OPTION CCM_BENCH_POWER)

set(CCMATH_BENCH_MODULE_nearest_FUNCTIONS trunc)
set(CCMATH_BENCH_MODULE_nearest_OPTION CCM_BENCH_NEAREST)

set(CCMATH_BENCH_MODULE_compare_FUNCTIONS)
set(CCMATH_BENCH_MODULE_compare_OPTION CCM_BENCH_COMPARE)
