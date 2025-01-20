project(ccmath-ext)

include(FetchContent)

# TODO: Cleanup all of the logic of handling deps

set(CCMATH_BENCHMARK_VERSION v1.9.1)
set(CCMATH_GTEST_VERSION v1.15.2)
set(CCMATH_ABSEIL_VERSION 20240722.0)
set(CCMATH_RE2_VERSION 2024-07-02)

if (CCMATH_BUILD_BENCHMARKS)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(HAVE_STD_REGEX ON)
    endif ()
    set(BENCHMARK_ENABLE_TESTING NO)
    FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG ${CCMATH_BENCHMARK_VERSION}
    )
endif ()

if (CCMATH_BUILD_TESTS)
    set(ABSL_PROPAGATE_CXX_STD ON)
    set(ABSL_ENABLE_INSTALL ON)
    set(GTEST_HAS_ABSL ON)
    FetchContent_Declare(
            abseil
            GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
            GIT_TAG ${CCMATH_ABSEIL_VERSION}
    )
    FetchContent_MakeAvailable(abseil)

    FetchContent_Declare(
            re2
            GIT_REPOSITORY https://github.com/google/re2.git
            GIT_TAG ${CCMATH_RE2_VERSION}
    )
    FetchContent_MakeAvailable(re2)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(HAVE_STD_REGEX ON)
    endif ()
    if (WIN32)
        set(gtest_force_shared_crt ON)
        set(gtest_disable_pthreads ON)
    endif ()
    set(BUILD_GMOCK OFF)
    set(INSTALL_GTEST OFF)
    set(gtest_build_samples OFF)
    set(gtest_build_tests OFF)

    if (CCMATH_FIND_GTEST_PACKAGE)
        find_package(GTest REQUIRED)
    else ()
        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG ${CCMATH_GTEST_VERSION}
        )
        FetchContent_MakeAvailable(googletest)
        if (NOT TARGET gtest::gtest)
            add_library(gtest::gtest ALIAS gtest)
        endif ()
    endif ()

endif ()

FetchContent_MakeAvailable(benchmark)

# Create the INTERFACE library
add_library(ccmath-ext INTERFACE)
add_library(ccmath::ext ALIAS ccmath-ext)

# Link GTest only if tests are enabled
if (CCMATH_BUILD_TESTS)
    target_link_libraries(ccmath-ext INTERFACE gtest::gtest)
    target_link_libraries(ccmath-ext INTERFACE absl::base absl::debugging re2::re2)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(gtest INTERFACE -Wno-everything -Wno-sign-conversion)
    endif ()

endif ()

# Link Google Benchmark only if benchmarks are enabled
if (CCMATH_BUILD_BENCHMARKS)
    target_link_libraries(ccmath-ext INTERFACE benchmark::benchmark)
endif ()


