# Common Support Builtins
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/support/CheckBuiltinBitCastSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/support/CheckBuiltinFmaSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/compare/CheckBuiltinSignbitSupport.cmake)


# TODO: Revisit this in a manner that is better. The current approach takes up too much time in the cmake.
#include(${CCMATH_SOURCE_DIR}/cmake/config/features/CheckAllSupportedBuiltinFeatures.cmake)

if (CCMATH_ENABLE_RUNTIME_SIMD)
  include(${CCMATH_SOURCE_DIR}/cmake/config/features/CheckAllSupportedSimdFeatures.cmake)
endif ()
