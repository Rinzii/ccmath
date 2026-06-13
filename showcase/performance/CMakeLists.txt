function(ccmath_add_showcase_bench TARGET SOURCE)
    add_executable(${TARGET} ${SOURCE})
    target_link_libraries(${TARGET} PRIVATE ccmath::ccmath benchmark::benchmark)
    ccmath_showcase_configure_target(${TARGET})
endfunction()

ccmath_add_showcase_bench(showcase_bench_sqrt sqrt.bench.cpp)
ccmath_add_showcase_bench(showcase_bench_sin sin.bench.cpp)
