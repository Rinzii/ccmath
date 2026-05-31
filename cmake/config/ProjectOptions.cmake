# -- Internal variables used by CCMath --

set(CCMATH_DESIRED_CXX_STANDARD cxx_std_17 CACHE STRING "")
mark_as_advanced(CCMATH_DESIRED_CXX_STANDARD)

if (NOT DEFINED CCMATH_ROOT_DIR)
    set(CCMATH_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()
mark_as_advanced(CCMATH_ROOT_DIR)