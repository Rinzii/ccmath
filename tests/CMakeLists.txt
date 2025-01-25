cmake_minimum_required(VERSION 3.20 FATAL_ERROR)


project(ccmath-tests LANGUAGES CXX)

# If ccmath is the target then don't search for it
if (NOT TARGET ccmath)
    find_package(ccmath CONFIG REQUIRED)
endif ()

# --------------------------------------------------------------------------
# Set up the main test library that includes test main (ccmath_test_main.cpp)
# --------------------------------------------------------------------------
add_library(ccmath-test STATIC
        ccmath_test_main.cpp
)

# Create an ALIAS so we can link with ccmath::test from anywhere
add_library(ccmath::test ALIAS ccmath-test)

# Link test library dependencies
target_link_libraries(ccmath-test
        PUBLIC
        ccmath::ccmath # target library to test e.g. CCMath :)
        ccmath::ext
)

target_include_directories(ccmath-test PUBLIC
        ${Abseil_INCLUDE_DIRS}
        .
)

# Only supported compilers are MSVC, GNU, and Clang for specific compiler flags
# TODO: Add additional flags for all supported compilers
if (MSVC)
    # Force shared CRT if desired (on Windows)
    # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    target_compile_options(ccmath-test PUBLIC
            /W4 /WX
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
        CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(ccmath-test PUBLIC
            -Wall -Wextra -Wno-pedantic -Wno-unused-function -Wno-sign-conversion
    )
endif ()

# --------------------------------------------------------------------------
# Helper function to avoid repeating the same lines for each test
# --------------------------------------------------------------------------
function(ccmath_add_test TEST_TARGET)
    # The rest of the arguments after the target name are source files
    set(SOURCES ${ARGN})

    add_executable(${TEST_TARGET})
    target_sources(${TEST_TARGET} PRIVATE ${SOURCES})

    # Link to our shared ccmath-test library with the main + all test deps
    target_link_libraries(${TEST_TARGET} PRIVATE ccmath::test)

    # Register with CTest
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endfunction()

# --------------------------------------------------------------------------
# Add test targets for each category
# --------------------------------------------------------------------------

# Basic tests
ccmath_add_test(ccmath-test-basic
        basic/abs_test.cpp
        basic/fdim_test.cpp
        basic/fma_test.cpp
        basic/fmod_test.cpp
        basic/max_test.cpp
        basic/min_test.cpp
        basic/nan_test.cpp
        basic/remainder_test.cpp
        basic/remquo_test.cpp
)

# Compare tests
ccmath_add_test(ccmath-test-compare
        compare/fpclassify_test.cpp
        compare/isfinite_test.cpp
        compare/isgreater_test.cpp
        compare/isgreaterequal_test.cpp
        compare/isinf_test.cpp
        compare/isless_test.cpp
        compare/islessequal_test.cpp
        compare/islessgreater_test.cpp
        compare/isnan_test.cpp
        compare/isnormal_test.cpp
        compare/isunordered_test.cpp
        compare/signbit_test.cpp
)

# Exponential tests
ccmath_add_test(ccmath-test-exponential
        exponential/exp2_test.cpp
        exponential/exp_test.cpp
        exponential/expm1_test.cpp
        exponential/log1p_test.cpp
        exponential/log2_test.cpp
        exponential/log10_test.cpp
        exponential/log_test.cpp
)

# Floating manipulation tests
ccmath_add_test(ccmath-test-fmanip
        fmanip/copysign_test.cpp
        fmanip/frexp_test.cpp
        fmanip/ilogb_test.cpp
        fmanip/ldexp_test.cpp
        fmanip/logb_test.cpp
        fmanip/modf_test.cpp
        fmanip/nextafter_test.cpp
        fmanip/nexttoward_test.cpp
        fmanip/scalbn_test.cpp
)

# Nearest tests
ccmath_add_test(ccmath-test-nearest
        nearest/floor_test.cpp
        nearest/nearbyint_test.cpp
        nearest/trunc_test.cpp
)

# Power tests
ccmath_add_test(ccmath-test-power
        power/pow_test.cpp
        power/sqrt_test.cpp
)

# Misc tests (handling std::lerp, version dependent)
if (CMAKE_CXX_STANDARD GREATER_EQUAL 20)
    ccmath_add_test(ccmath-test-misc
            misc/lerp_test_std.cpp
    )
else ()
    ccmath_add_test(ccmath-test-misc
            misc/lerp_test_no_std.cpp
    )
endif ()

# Internal test(s)
ccmath_add_test(ccmath-test-internal-types
        internal/types/big_int_test.cpp
)
