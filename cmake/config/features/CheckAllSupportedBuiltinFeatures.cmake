# Monolithic feature checking script
#
# All of these included cmake files perform compilation tests to determine if we have support for
# internally used builtin functions. If the tests pass, we add a compile definition to the project
# to indicate that the feature is supported. We should only use cmake to perform these checks if
# and only if we are actually using the feature in the codebase. If not then don't bother checking.
#
# NOTE: Be aware these scripts WILL create macros definitions that the project may use internally.
# NOTE: The check for __builtin_bit_cast will cause a cmake error as the library has a hard dependency on it.


# Common Support Builtins
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/support/CheckBuiltinBitCastSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/support/CheckBuiltinFmaSupport.cmake)


# Basic
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/basic/CheckBuiltinNanSupport.cmake)


# Compare
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/compare/CheckBuiltinIsInfSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/compare/CheckBuiltinIsNanSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/compare/CheckBuiltinSignbitSupport.cmake)


# Exponential
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/expo/CheckBuiltinExpSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/expo/CheckBuiltinExp2Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/expo/CheckBuiltinExpm1Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/expo/CheckBuiltinLogSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/expo/CheckBuiltinLog10Support.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/expo/CheckBuiltinLog1pSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/expo/CheckBuiltinLog2Support.cmake)


# Fmanip
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/fmanip/CheckBuiltinCopysignSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/fmanip/CheckBuiltinLdexpSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/fmanip/CheckBuiltinFrexpSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/fmanip/CheckBuiltinModfSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/fmanip/CheckBuiltinLogbSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/fmanip/CheckBuiltinIlogbSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/fmanip/CheckBuiltinScalbnSupport.cmake)


# Hyperbolic
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/hyper/CheckBuiltinSinhSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/hyper/CheckBuiltinCoshSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/hyper/CheckBuiltinTanhSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/hyper/CheckBuiltinAsinhSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/hyper/CheckBuiltinAcoshSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/hyper/CheckBuiltinAtanhSupport.cmake)


# Power
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/power/CheckBuiltinCbrtSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/power/CheckBuiltinHypotSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/power/CheckBuiltinPowSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/power/CheckBuiltinSqrtSupport.cmake)


# Trigonometric
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/trig/CheckBuiltinSinSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/trig/CheckBuiltinCosSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/trig/CheckBuiltinTanSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/trig/CheckBuiltinAsinSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/trig/CheckBuiltinAcosSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/trig/CheckBuiltinAtanSupport.cmake)
include(${CCMATH_SOURCE_DIR}/cmake/config/features/builtin/math/trig/CheckBuiltinAtan2Support.cmake)
