if (NOT DEFINED INTERNAL_PROJ_DEFAULT_NAME)
  message(FATAL_ERROR "INTERNAL_PROJ_DEFAULT_NAME is not defined. Did you somehow include this file directly without the main CMakeLists.txt being called first?")
endif ()

project(${INTERNAL_PROJ_DEFAULT_NAME}-ext)

include(FetchContent)

if (CCMATH_BUILD_BENCHMARKS)
  set(BENCHMARK_ENABLE_TESTING NO)
  FetchContent_Declare(
          benchmark
          GIT_REPOSITORY https://github.com/google/benchmark.git
          GIT_TAG v1.8.3
  )
  FetchContent_MakeAvailable(benchmark)
endif ()



if (CCMATH_BUILD_TEST)
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


  FetchContent_Declare(
          googletest
          GIT_REPOSITORY https://github.com/google/googletest.git
          GIT_TAG v1.14.0
  )
  FetchContent_MakeAvailable(googletest)

  add_library(gtest::gtest ALIAS gtest)
endif()


add_library(${PROJECT_NAME} INTERFACE)
add_library(${INTERNAL_PROJ_DEFAULT_NAME}::ext ALIAS ${PROJECT_NAME})

if(CCMATH_BUILD_TEST)
  target_link_libraries(${PROJECT_NAME} INTERFACE
          gtest::gtest
          )
endif ()

if(CCMATH_BUILD_BENCHMARKS)
  target_link_libraries(${PROJECT_NAME} INTERFACE
          benchmark::benchmark
          )
endif ()

