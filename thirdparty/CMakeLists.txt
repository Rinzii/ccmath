project(ccmath-ext)

set(CCMATH_BENCHMARK_VERSION  v1.9.1)
set(CCMATH_GTEST_VERSION      v1.15.2)
set(CCMATH_ABSEIL_VERSION     20240722.0)
set(CCMATH_RE2_VERSION        2024-07-02)

if (CCMATH_BUILD_BENCHMARKS)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(HAVE_STD_REGEX ON)
    endif()

    set(BENCHMARK_ENABLE_TESTING NO)
endif()

if (CCMATH_BUILD_TESTS)
    set(ABSL_PROPAGATE_CXX_STD ON)
    set(ABSL_ENABLE_INSTALL ON)
    set(GTEST_HAS_ABSL ON)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(HAVE_STD_REGEX ON)
    endif ()

    if (WIN32)
        set(gtest_force_shared_crt ON)
        set(gtest_disable_pthreads ON)
    endif()

    set(BUILD_GMOCK OFF)
    set(INSTALL_GTEST OFF)
    set(gtest_build_samples OFF)
    set(gtest_build_tests OFF)
endif()

# Create the INTERFACE library
add_library(${PROJECT_NAME} INTERFACE)
add_library(ccmath::ext ALIAS ${PROJECT_NAME})

# Link GTest only if tests are enabled
if (CCMATH_BUILD_TESTS)
    find_package(PkgConfig REQUIRED)
    find_package(absl CONFIG REQUIRED)
    find_package(re2 CONFIG REQUIRED)
    find_package(GTest CONFIG REQUIRED)

    target_link_libraries(${PROJECT_NAME} INTERFACE GTest::gtest)
    target_link_libraries(${PROJECT_NAME} INTERFACE absl::base absl::debugging re2::re2)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(GTest::gtest INTERFACE -Wno-everything)
    endif()
endif()

# Link Google Benchmark only if benchmarks are enabled
if (CCMATH_BUILD_BENCHMARKS)
    find_package(benchmark CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} INTERFACE benchmark::benchmark)
endif()
