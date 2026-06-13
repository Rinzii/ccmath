cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(ccmath-benchmark LANGUAGES CXX)

option(CCM_BENCH_BASIC "Enable basic benchmarks" OFF)
option(CCM_BENCH_COMPARE "Enable comparison benchmarks" OFF)
option(CCM_BENCH_POWER "Enable power benchmarks" OFF)
option(CCM_BENCH_NEAREST "Enable nearest benchmarks" ON)
option(CCM_BENCH_ALL "Enable all benchmarks" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif ()

if (NOT TARGET ccmath)
    find_package(ccmath CONFIG REQUIRED)
endif ()

set(CCMATH_BENCH_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/config/BenchmarkModuleRegistry.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/helpers/CcmBenchHelpers.cmake)

ccmath_apply_bench_registry()
