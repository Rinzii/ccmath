include(${CCMATH_SOURCE_DIR}/cmake/config/features/CheckAllSupportedBuiltinFeatures.cmake)

if (CCMATH_ENABLE_RUNTIME_SIMD)
  include(${CCMATH_SOURCE_DIR}/cmake/config/features/CheckAllSupportedSimdFeatures.cmake)
endif ()
