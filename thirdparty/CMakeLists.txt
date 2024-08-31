project(ccmath-ext)

include(FetchContent)

if (CCMATH_BUILD_BENCHMARKS)
  # Intel does implement the regex library but google test is unable to detect this.
  # Lets help them out and set the var ourselves.
  if (CCMATH_COMPILER_INTEL)
    set(HAVE_STD_REGEX ON CACHE BOOL "" FORCE)
  endif ()
  set(BENCHMARK_ENABLE_TESTING NO)
  FetchContent_Declare(
          benchmark
          GIT_REPOSITORY https://github.com/google/benchmark.git
          GIT_TAG v1.8.3
  )
  FetchContent_MakeAvailable(benchmark)
endif ()

if (CCMATH_BUILD_TESTS)
  # Intel does implement the regex library but google test is unable to detect this.
  # Lets help them out and set the var ourselves.
  if (CCMATH_COMPILER_INTEL)
    set(HAVE_STD_REGEX ON CACHE BOOL "" FORCE)
  endif ()

  # PThreads are not available on Windows
  # So tell gtest to not use them.
  if(CCMATH_WINDOWS)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
  endif()

  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_build_samples OFF CACHE BOOL "" FORCE)
  set(gtest_build_tests OFF CACHE BOOL "" FORCE)

  if(CCMATH_FIND_GTEST_PACKAGE)
    find_package(GTest REQUIRED)
  else()
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )
  FetchContent_MakeAvailable(googletest)
  endif ()

  add_library(gtest::gtest ALIAS gtest)
endif()


add_library(${PROJECT_NAME} INTERFACE)
add_library(ccmath::ext ALIAS ${PROJECT_NAME})

if(CCMATH_BUILD_TESTS)
  target_link_libraries(${PROJECT_NAME} INTERFACE
          gtest::gtest
          )
endif ()

if(CCMATH_BUILD_BENCHMARKS)
  target_link_libraries(${PROJECT_NAME} INTERFACE
          benchmark::benchmark
          )
endif ()

