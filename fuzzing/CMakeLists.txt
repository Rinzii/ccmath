cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(ccmath-fuzzing LANGUAGES CXX)

if (NOT TARGET ccmath)
    find_package(ccmath CONFIG REQUIRED)
endif ()

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "CCMath fuzzing requires Clang with libFuzzer (-fsanitize=fuzzer).")
endif ()

include(CTest)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules")
include(CcmLibFuzzer)

ccmath_detect_libfuzzer()

if (NOT CCMATH_LIBFUZZER_AVAILABLE)
    message(FATAL_ERROR
            "libFuzzer link probe failed with ${CMAKE_CXX_COMPILER}. "
            "Use a Clang toolchain that ships libFuzzer (full LLVM Clang, not Apple Clang), "
            "or set LIB_FUZZING_ENGINE for OSS-Fuzz-style builds. "
            "Reconfigure with -DCMAKE_CXX_COMPILER=... pointing at that toolchain.")
endif ()

option(CCMATH_FUZZ_GENERIC "Fuzz the generic kernels instead of the public dispatch" OFF)

set(CCMATH_FUZZ_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/helpers/CcmFuzzHelpers.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/config/FuzzModuleRegistry.cmake)

foreach (_module IN LISTS CCMATH_FUZZ_MODULE_ORDER)
    ccmath_add_fuzz_module(ccmath-fuzz-${_module} MODULE ${_module})
endforeach ()

add_custom_target(ccmath-fuzz-all)
add_custom_target(ccmath-fuzz-campaign-all)
foreach (_module IN LISTS CCMATH_FUZZ_MODULE_ORDER)
    add_dependencies(ccmath-fuzz-all ccmath-fuzz-${_module})
    add_dependencies(ccmath-fuzz-campaign-all ccmath-fuzz-${_module}-campaign)
endforeach ()

set(_ccmath_fuzz_smoke_ctest_args --output-on-failure -R "ccmath-fuzz-.*-smoke")
if (CMAKE_CONFIGURATION_TYPES)
    list(APPEND _ccmath_fuzz_smoke_ctest_args -C $<CONFIG>)
endif ()
add_custom_target(ccmath-fuzz-smoke
        COMMAND ${CMAKE_CTEST_COMMAND} ${_ccmath_fuzz_smoke_ctest_args}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
        COMMENT "Running all fuzz smoke tests")
