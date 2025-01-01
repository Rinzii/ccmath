

cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(ccmath-benchmark)

option(CCM_BENCH_BASIC   "Enable basic benchmarks"      OFF)
option(CCM_BENCH_COMPARE "Enable comparison benchmarks" OFF)
option(CCM_BENCH_POWER   "Enable power benchmarks"      OFF)
option(CCM_BENCH_NEAREST "Enable nearest benchmarks"    ON)
option(CCM_BENCH_ALL     "Enable all benchmarks"        OFF)

# Force cmake to use Release if debug is detected
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(status "ccmath : Changing build type from Debug to Release for benchmarks")
    set(CMAKE_BUILD_TYPE "Release" ON CACHE STRING "Build type" FORCE)
endif()

if(NOT TARGET ccmath)
    message(info "ccmath : Searching or ccmath package")
    find_package(ccmath CONFIG REQUIRED)
endif()

function(add_benchmark function_name source_files)
    find_package(PkgConfig REQUIRED)
    find_package(absl CONFIG REQUIRED)
    find_package(re2 CONFIG REQUIRED)
    find_package(GTest CONFIG REQUIRED)
    find_package(benchmark CONFIG REQUIRED)

    add_executable(ccm_benchmark_${function_name} helpers/randomizers.hpp ${source_files})
    target_link_libraries(ccm_benchmark_${function_name} PRIVATE ccmath::ccmath benchmark::benchmark)
    target_compile_features(ccm_benchmark_${function_name} PRIVATE cxx_std_17)
endfunction()

if(CCM_BENCH_BASIC)
    message(info "ccmath : Adding basic benchmarks")
    add_benchmark( abs  benchmarks/basic/abs.bench.cpp  benchmarks/basic/abs.bench.hpp  )
    add_benchmark( fdim benchmarks/basic/fdim.bench.cpp benchmarks/basic/fdim.bench.hpp )
    add_benchmark( fma  benchmarks/basic/fma.bench.cpp  benchmarks/basic/fma.bench.hpp  )
endif ()

if(CCM_BENCH_POWER)
    message(info "ccmath : Adding power benchmarks")
    add_benchmark(sqrt benchmarks/power/sqrt.bench.cpp benchmarks/power/sqrt.bench.hpp)
endif ()

if(CCM_BENCH_NEAREST)
    message(status "ccmath : Adding nearest benchmarks")
    #add_benchmark(trunc benchmarks/nearest/trunc.bench.cpp benchmarks/nearest/trunc.bench.hpp)
endif ()
