cmake_minimum_required(VERSION 3.18)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif ()

include(${CMAKE_SOURCE_DIR}/cmake/config/ShowcaseDeps.cmake)
ccmath_showcase_fetch_gcem()

if (CCMATH_SHOWCASE_ENABLE_MPFR)
    include(${CMAKE_SOURCE_DIR}/cmake/config/FindMpfrGmp.cmake)
endif ()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/reports)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/compile_time/probes)

add_subdirectory(performance)

if (CCMATH_SHOWCASE_ENABLE_MPFR AND CCMATH_MPFR_FOUND)
    ccmath_showcase_fetch_gtest()
    add_subdirectory(accuracy)
endif ()
