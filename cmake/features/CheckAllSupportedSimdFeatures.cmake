if (NOT DEFINED CCMATH_ROOT_DIR)
    message(FATAL_ERROR "CCMATH_ROOT_DIR is not defined. Did you forget to include the main CMakeLists.txt?")
endif ()

include(${CCMATH_ROOT_DIR}/cmake/features/simd/CheckFMASupport.cmake)

if (NOT CCMATH_DISABLE_SVML_USAGE)
    include(${CCMATH_ROOT_DIR}/cmake/features/simd/CheckSVMLSupport.cmake)
endif ()
