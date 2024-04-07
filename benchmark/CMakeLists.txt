if (NOT DEFINED INTERNAL_PROJ_DEFAULT_NAME)
  message(FATAL_ERROR "INTERNAL_PROJ_DEFAULT_NAME is not defined. Did you somehow include this file directly without the main CMakeLists.txt being called first?")
endif ()

cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(${INTERNAL_PROJ_DEFAULT_NAME}-benchmark)

if(NOT TARGET ${INTERNAL_PROJ_DEFAULT_NAME})
  find_package(${INTERNAL_PROJ_DEFAULT_NAME} CONFIG REQUIRED)
endif()

add_executable(${PROJECT_NAME})
add_executable(${INTERNAL_PROJ_DEFAULT_NAME}::benchmark ALIAS ${PROJECT_NAME})
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
