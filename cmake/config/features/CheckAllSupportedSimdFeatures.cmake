if (NOT DEFINED CCMATH_SOURCE_DIR)
    message(FATAL_ERROR "CCMATH_SOURCE_DIR is not defined. Did you forget to include the main CMakeLists.txt?")
endif ()

include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckFMASupport.cmake)

if (NOT CCMATH_DISABLE_SVML_USAGE)
    include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckSVMLSupport.cmake)
endif ()
