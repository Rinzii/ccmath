BENCHMARK(BM_power_trunc_ccm)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_rand_int_std)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_rand_int_ccm)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_rand_double_std)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_rand_double_ccm)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();

#endif

#ifndef CCM_BM_CONFIG_NO_RT

BENCHMARK(BM_power_trunc_std_rt)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_ccm_rt)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_rand_int_std_rt)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_rand_int_ccm_rt)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_rand_double_std_rt)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_power_trunc_rand_double_ccm_rt)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();

#endif

BENCHMARK_MAIN();

// NOLINTEND
