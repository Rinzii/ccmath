cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(ccmath-tests LANGUAGES CXX)

if (NOT TARGET ccmath)
    find_package(ccmath CONFIG REQUIRED)
endif ()

set(CCMATH_TESTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/helpers/CcmTestHelpers.cmake)

add_library(ccmath-test STATIC ccmath_test_main.cpp)
add_library(ccmath::test ALIAS ccmath-test)

target_link_libraries(ccmath-test
        PUBLIC
        ccmath::ccmath
        ccmath::ext
)

target_include_directories(ccmath-test PUBLIC
        ${Abseil_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/shared
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
    target_compile_options(ccmath-test PRIVATE /W3)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|IntelLLVM")
    target_compile_options(ccmath-test PRIVATE
            -Wno-all
            -Wno-extra
            -Wno-conversion
            -Wno-pedantic
            -Wno-sign-conversion
            -Wno-error
    )

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(ccmath-test PRIVATE -Wno-everything)
    endif ()
endif ()

if (CCMATH_BUILD_SIMPLE_TESTS)
    add_subdirectory(unit)
endif ()

if (CCMATH_BUILD_RIGOROUS_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/config/TestValidationOptions.cmake)
    add_subdirectory(rigorous)
endif ()

