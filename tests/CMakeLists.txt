cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(ccmath-tests LANGUAGES CXX)

if (NOT TARGET ccmath)
    find_package(ccmath CONFIG REQUIRED)
endif ()

add_library(ccmath-test STATIC ccmath_test_main.cpp)
add_library(ccmath::test ALIAS ccmath-test)

target_link_libraries(ccmath-test
        PUBLIC
        ccmath::ccmath
        ccmath::ext
)

target_include_directories(ccmath-test PUBLIC
        ${Abseil_INCLUDE_DIRS}
        .
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

function(ccmath_add_test TEST_TARGET)
    set(SOURCES ${ARGN})
    add_executable(${TEST_TARGET})
    target_sources(${TEST_TARGET} PRIVATE ${SOURCES})
    target_link_libraries(${TEST_TARGET} PRIVATE ccmath::test)
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endfunction()

function(ccmath_add_command_test TEST_TARGET)
    set(SOURCES ${ARGN})
    add_executable(${TEST_TARGET} ${SOURCES})
    target_link_libraries(${TEST_TARGET} PRIVATE ccmath::ccmath ccmath::ext)
    target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
    target_compile_features(${TEST_TARGET} PRIVATE cxx_std_17)
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endfunction()

if (CCMATH_BUILD_SIMPLE_TESTS)
    add_subdirectory(simple)
endif ()

if (CCMATH_BUILD_RIGOROUS_TESTS)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/config/TestValidationOptions.cmake)
    add_subdirectory(rigorous)
endif ()
