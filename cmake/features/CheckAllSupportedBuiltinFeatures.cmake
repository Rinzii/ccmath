# Monolithic feature checking script
#
# All of these included cmake files perform compilation tests to determine if we have support for
# internally used builtin functions. If the tests pass, we add a compile definition to the project
# to indicate that the feature is supported. We should only use cmake to perform these checks if
# and only if we are actually using the feature in the codebase. If not then don't bother checking.
#
# NOTE: Be aware these scripts WILL create macros definitions that the project may use internally.
# NOTE: The check for __builtin_bit_cast will cause a cmake error as the library has a hard dependency on it.

if (NOT DEFINED CCMATH_ROOT_DIR)
    message(FATAL_ERROR "CCMATH_ROOT_DIR is not defined. Did you forget to include the main CMakeLists.txt?")
endif ()

# Common Support Builtins
include(${CCMATH_ROOT_DIR}/cmake/features/builtin/support/CheckBuiltinBitCastSupport.cmake)
include(${CCMATH_ROOT_DIR}/cmake/features/builtin/support/CheckBuiltinFmaSupport.cmake)
