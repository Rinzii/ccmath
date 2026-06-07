# Discover MPFR and GMP for rigorous oracle validation.
#
# Linux and macOS typically use system pkg-config modules.
# Windows CI uses vcpkg (see cmake/vcpkg/vcpkg.json) with pkgconf and .pc files.

include_guard(GLOBAL)

set(CCMATH_MPFR_FOUND FALSE)

macro(_ccmath_configure_vcpkg_pkgconfig)
    if (NOT WIN32)
        return()
    endif ()
    if (NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
        return()
    endif ()
    if (NOT DEFINED VCPKG_INSTALLED_DIR OR NOT DEFINED VCPKG_TARGET_TRIPLET)
        return()
    endif ()

    set(_vcpkg_triplet_root "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
    find_program(CCMATH_PKGCONFIG_EXECUTABLE
            NAMES pkgconf pkg-config
            PATHS
            "${_vcpkg_triplet_root}/tools/pkgconf"
            "${_vcpkg_triplet_root}/tools/pkg-config"
            PATH_SUFFIXES bin
            NO_DEFAULT_PATH
    )
    if (CCMATH_PKGCONFIG_EXECUTABLE)
        set(PKG_CONFIG_EXECUTABLE "${CCMATH_PKGCONFIG_EXECUTABLE}" CACHE FILEPATH "" FORCE)
    endif ()

    set(ENV{PKG_CONFIG_PATH}
            "${_vcpkg_triplet_root}/lib/pkgconfig;${_vcpkg_triplet_root}/share/pkgconfig")
endmacro()

_ccmath_configure_vcpkg_pkgconfig()

find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
    pkg_check_modules(MPFR QUIET IMPORTED_TARGET mpfr)
    pkg_check_modules(GMP QUIET IMPORTED_TARGET gmp)
endif ()

if (TARGET PkgConfig::MPFR AND TARGET PkgConfig::GMP)
    if (NOT TARGET ccmath_mpfr_oracle)
        add_library(ccmath_mpfr_oracle INTERFACE IMPORTED GLOBAL)
        target_link_libraries(ccmath_mpfr_oracle INTERFACE PkgConfig::MPFR PkgConfig::GMP)
        add_library(ccmath::mpfr_oracle ALIAS ccmath_mpfr_oracle)
    endif ()
    set(CCMATH_MPFR_FOUND TRUE)
endif ()

if (CCMATH_ENABLE_MPFR_TESTS AND NOT CCMATH_MPFR_FOUND)
    message(STATUS "CCMath rigorous MPFR validation requested, but MPFR/GMP were not both found")
endif ()
