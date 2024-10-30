if (NOT DEFINED CCMATH_SOURCE_DIR)
    message(FATAL_ERROR "CCMATH_SOURCE_DIR is not defined. Did you forget to include the main CMakeLists.txt?")
endif ()

include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckFMASupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckAVXSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckAVX2Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckAVX512Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckSSE2Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckSSE3Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckSSSE3Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckSSE4Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckSVMLSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/simd/CheckNEONSupport.cmake)



