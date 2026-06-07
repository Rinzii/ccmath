# Discover CORE-MATH oracle library for rigorous validation.
#
# Resolution order:
#   1. pkg-config module coremath or crmath
#   2. CCMATH_COREMATH_ROOT or CMAKE_PREFIX_PATH (crmath.h + libcoremath_oracle.a / libcrmath.a)
#   3. CCMATH_FETCH_COREMATH=ON FetchContent fallback

include_guard(GLOBAL)

set(CCMATH_COREMATH_FOUND FALSE)

if (CCMATH_COREMATH_ROOT)
    list(APPEND CMAKE_PREFIX_PATH "${CCMATH_COREMATH_ROOT}")
endif ()

find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
    foreach (_pc_module IN ITEMS coremath crmath ccmath_coremath_oracle)
        pkg_check_modules(_COREMATH_PC QUIET IMPORTED_TARGET ${_pc_module})
        if (TARGET PkgConfig::_COREMATH_PC)
            if (NOT TARGET ccmath::coremath_oracle)
                add_library(ccmath::coremath_oracle ALIAS PkgConfig::_COREMATH_PC)
            endif ()
            set(CCMATH_COREMATH_FOUND TRUE)
            break()
        endif ()
        unset(_COREMATH_PC_LIBRARIES)
        unset(_COREMATH_PC_INCLUDE_DIRS)
        unset(_COREMATH_PC_CFLAGS)
        unset(_COREMATH_PC_CFLAGS_OTHER)
    endforeach ()
endif ()

if (NOT CCMATH_COREMATH_FOUND)
    find_path(CCMATH_COREMATH_INCLUDE_DIR
            NAMES crmath.h coremath.h
            HINTS ${CCMATH_COREMATH_ROOT}
            PATH_SUFFIXES include
    )
    find_library(CCMATH_COREMATH_LIBRARY
            NAMES coremath_oracle crmath coremath
            HINTS ${CCMATH_COREMATH_ROOT}
            PATH_SUFFIXES lib lib64
    )

    if (CCMATH_COREMATH_INCLUDE_DIR AND CCMATH_COREMATH_LIBRARY)
        if (NOT TARGET ccmath_coremath_oracle)
            add_library(ccmath_coremath_oracle UNKNOWN IMPORTED GLOBAL)
            set_target_properties(ccmath_coremath_oracle PROPERTIES
                    IMPORTED_LOCATION "${CCMATH_COREMATH_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${CCMATH_COREMATH_INCLUDE_DIR}"
            )
            add_library(ccmath::coremath_oracle ALIAS ccmath_coremath_oracle)
        endif ()
        set(CCMATH_COREMATH_FOUND TRUE)
    endif ()
endif ()

if (NOT CCMATH_COREMATH_FOUND AND CCMATH_FETCH_COREMATH)
    include(FetchContent)
    FetchContent_Declare(
            coremath_upstream
            GIT_REPOSITORY https://github.com/mockingbirdnest/core-math.git
            GIT_TAG master
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(coremath_upstream)

    set(CORE_MATH_SOURCE_DIR "${coremath_upstream_SOURCE_DIR}" CACHE PATH "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../coremath ${CMAKE_BINARY_DIR}/_deps/coremath_oracle_build EXCLUDE_FROM_ALL)

    if (TARGET coremath_oracle AND NOT TARGET ccmath::coremath_oracle)
        add_library(ccmath::coremath_oracle ALIAS coremath_oracle)
        set(CCMATH_COREMATH_FOUND TRUE)
    endif ()
endif ()

if (CCMATH_ENABLE_COREMATH_TESTS AND NOT CCMATH_COREMATH_FOUND)
    if (CCMATH_FETCH_COREMATH)
        message(STATUS "CCMath rigorous CORE-MATH validation requested, but CORE-MATH was not found and FetchContent did not produce a target")
    else ()
        message(STATUS "CCMath rigorous CORE-MATH validation requested, but CORE-MATH was not found (set CCMATH_COREMATH_ROOT or CCMATH_FETCH_COREMATH=ON)")
    endif ()
endif ()
