if (NOT DEFINED CCMATH_ROOT_DIR)
    message(FATAL_ERROR "CCMATH_ROOT_DIR is not defined. Did you forget to include the main CMakeLists.txt?")
endif ()

if (NOT CCMATH_DISABLE_CMAKE_FEATURE_CHECKS)
    include(${CCMATH_ROOT_DIR}/cmake/features/CheckAllSupportedBuiltinFeatures.cmake)

    if (CCMATH_ENABLE_RUNTIME_SIMD)
        include(${CCMATH_ROOT_DIR}/cmake/features/CheckAllSupportedSimdFeatures.cmake)
    endif ()
else ()
    message(STATUS "CCMath: Requested disabled cmake feature checking. Be warned this might have undefined issues in the library.")
endif ()