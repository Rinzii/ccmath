cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(ccmath-test)

if(NOT TARGET ccmath)
    find_package(ccmath CONFIG REQUIRED)
endif()


add_library(${PROJECT_NAME} STATIC)
add_library(ccmath::test ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PUBLIC
        ccmath::ccmath
        gtest::gtest
        )
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_sources(${PROJECT_NAME} PRIVATE
        ccmath_test_main.cpp
)


add_executable(${PROJECT_NAME}-basic)
target_sources(${PROJECT_NAME}-basic PRIVATE
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
target_link_libraries(${PROJECT_NAME}-basic PRIVATE
        ccmath::test
        gtest::gtest
        )

add_executable(${PROJECT_NAME}-compare)
target_sources(${PROJECT_NAME}-compare PRIVATE
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
target_link_libraries(${PROJECT_NAME}-compare PRIVATE
        ccmath::test
        gtest::gtest
)

add_executable(${PROJECT_NAME}-exponential)
target_sources(${PROJECT_NAME}-exponential PRIVATE
        exponential/exp2_test.cpp
        exponential/exp_test.cpp
        exponential/expm1_test.cpp
        exponential/log1p_test.cpp
        exponential/log2_test.cpp
        exponential/log10_test.cpp
        exponential/log_test.cpp


)
target_link_libraries(${PROJECT_NAME}-exponential PRIVATE
        ccmath::test
        gtest::gtest
)

add_executable(${PROJECT_NAME}-fmanip)
target_sources(${PROJECT_NAME}-fmanip PRIVATE
        fmanip/copysign_test.cpp
        fmanip/frexp_test.cpp
        fmanip/ilogb_test.cpp
        fmanip/ldexp_test.cpp
        fmanip/logb_test.cpp
        fmanip/modf_test.cpp
        fmanip/nextafter_test.cpp
        fmanip/scalbn_test.cpp
)
target_link_libraries(${PROJECT_NAME}-fmanip PRIVATE
        ccmath::test
        gtest::gtest
)

add_executable(${PROJECT_NAME}-nearest)
target_sources(${PROJECT_NAME}-nearest PRIVATE
        nearest/trunc_test.cpp
        nearest/floor_test.cpp
        nearest/ceil_test.cpp

)
target_link_libraries(${PROJECT_NAME}-nearest PRIVATE
        ccmath::test
        gtest::gtest
)

add_executable(${PROJECT_NAME}-power)
target_sources(${PROJECT_NAME}-power PRIVATE
        power/pow_test.cpp

)
target_link_libraries(${PROJECT_NAME}-power PRIVATE
        ccmath::test
        gtest::gtest
)

add_executable(${PROJECT_NAME}-misc)

# This is required as std::lerp is only available in versions of C++20 or greater.
if(CMAKE_CXX_STANDARD GREATER_EQUAL 20)
  target_sources(${PROJECT_NAME}-misc PRIVATE
          misc/lerp_test_std.cpp

  )
else()
  target_sources(${PROJECT_NAME}-misc PRIVATE
          misc/lerp_test_no_std.cpp
  )
endif()

target_link_libraries(${PROJECT_NAME}-misc PRIVATE
        ccmath::test
        gtest::gtest
)


# Tests for internal items
add_executable(${PROJECT_NAME}-internal-types)
target_sources(${PROJECT_NAME}-internal-types PRIVATE
        internal/types/big_int_test.cpp

)
target_link_libraries(${PROJECT_NAME}-internal-types PRIVATE
        ccmath::test
        gtest::gtest
)


if(CCMATH_OS_WINDOWS)
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

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


add_test(NAME ${PROJECT_NAME}-basic COMMAND ${PROJECT_NAME}-basic)
add_test(NAME ${PROJECT_NAME}-compare COMMAND ${PROJECT_NAME}-compare)
add_test(NAME ${PROJECT_NAME}-exponential COMMAND ${PROJECT_NAME}-exponential)
add_test(NAME ${PROJECT_NAME}-fmanip COMMAND ${PROJECT_NAME}-fmanip)
add_test(NAME ${PROJECT_NAME}-nearest COMMAND ${PROJECT_NAME}-nearest)
add_test(NAME ${PROJECT_NAME}-power COMMAND ${PROJECT_NAME}-power)
add_test(NAME ${PROJECT_NAME}-misc COMMAND ${PROJECT_NAME}-misc)

# Internal tests
add_test(NAME ${PROJECT_NAME}-internal-types COMMAND ${PROJECT_NAME}-internal-types)

