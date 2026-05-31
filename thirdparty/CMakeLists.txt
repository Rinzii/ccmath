cmake_minimum_required(VERSION 3.18)
project(ccmath-ext LANGUAGES CXX)

include(FetchContent)
#
# Versions
#
set(CCMATH_BENCHMARK_VERSION v1.9.1)
set(CCMATH_GTEST_VERSION v1.15.2)
set(CCMATH_ABSEIL_VERSION 20240722.0)
set(CCMATH_RE2_VERSION 2024-07-02)


#
# Fetch Google Benchmark if building benchmarks
#
if (CCMATH_BUILD_BENCHMARKS)
    # Disable benchmark’s own tests
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG ${CCMATH_BENCHMARK_VERSION}
    )
    FetchContent_MakeAvailable(benchmark)
endif ()

#
# Fetch Abseil, RE2, GTest if building tests
#
if (CCMATH_BUILD_TESTS)
    # Abseil config
    set(ABSL_PROPAGATE_CXX_STD ON)
    set(ABSL_ENABLE_INSTALL ON)

    # GTest config
    if (WIN32)
        set(gtest_force_shared_crt ON)
        set(gtest_disable_pthreads ON)
    endif ()
    set(BUILD_GMOCK OFF)
    set(INSTALL_GTEST OFF)
    set(gtest_build_samples OFF)
    set(gtest_build_tests OFF)

    # --- Abseil ---
    FetchContent_Declare(
            abseil
            GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
            GIT_TAG ${CCMATH_ABSEIL_VERSION}
    )
    FetchContent_MakeAvailable(abseil)

    # --- RE2 ---
    FetchContent_Declare(
            re2
            GIT_REPOSITORY https://github.com/google/re2.git
            GIT_TAG ${CCMATH_RE2_VERSION}
    )
    FetchContent_MakeAvailable(re2)

    # --- GTest ---
    if (CCMATH_FIND_GTEST_PACKAGE)
        # Use a pre-installed GTest if requested
        find_package(GTest REQUIRED)
    else ()
        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG ${CCMATH_GTEST_VERSION}
        )
        FetchContent_MakeAvailable(googletest)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
            set(HAVE_STD_REGEX CACHE BOOL ON "")
        endif ()

        # Provide an ALIAS target if needed
        if (NOT TARGET gtest::gtest)
            add_library(gtest::gtest ALIAS gtest)
        endif ()
    endif ()
endif ()

#
# Main library
#
add_library(ccmath-ext INTERFACE)
add_library(ccmath::ext ALIAS ccmath-ext)

#
# Link dependencies conditionally
#
if (CCMATH_BUILD_TESTS)
    target_link_libraries(ccmath-ext INTERFACE
            gtest::gtest
            absl::base
            absl::debugging
            re2::re2
    )

    # For Clang, silence some warnings in GTest itself
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(gtest INTERFACE
                -Wno-everything
                -Wno-sign-conversion
        )
    endif ()
endif ()

if (CCMATH_BUILD_BENCHMARKS)
    target_link_libraries(ccmath-ext INTERFACE benchmark::benchmark)
endif ()
