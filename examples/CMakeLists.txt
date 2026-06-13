cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(ccmath-examples LANGUAGES CXX)

if (NOT TARGET ccmath)
    find_package(ccmath CONFIG REQUIRED)
endif ()

set(CCMATH_EXAMPLE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/config/ExampleModuleRegistry.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/helpers/CcmExampleHelpers.cmake)

ccmath_apply_example_registry()
