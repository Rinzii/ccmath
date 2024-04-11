

cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(ccmath-benchmark)

# Force cmake to use Release if debug is detected
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_BUILD_TYPE "Release" ON CACHE STRING "Build type" FORCE)
endif()

if(NOT TARGET ccmath)
  find_package(ccmath CONFIG REQUIRED)
endif()

add_executable(${PROJECT_NAME})
add_executable(ccmath::benchmark ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC
        ccmath::ccmath
        benchmark::benchmark
)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_sources(${PROJECT_NAME} PRIVATE
        helpers/randomizers.h
        ccmath_benchmark_main.cpp
)

# Only supported compilers currently are MSVC, GNU and Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC
          /W4 /WX
  )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)
  target_compile_options(${PROJECT_NAME} PUBLIC
          -Wall -Wextra -Wno-pedantic -Wno-unused-function
  )
endif()
