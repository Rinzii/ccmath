cmake_minimum_required(VERSION 3.18.0)

set(CCMATH_BUILD_VERSION 0.2.0)

project(
        ccmath
        VERSION ${CCMATH_BUILD_VERSION}
        DESCRIPTION "A C++17 Compile Time <cmath> Library"
        HOMEPAGE_URL "https://github.com/Rinzii/ccmath"
        LANGUAGES CXX
)

# Determine if this is the root project or a subproject.
set(is_root_project OFF)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(is_root_project ON)
endif ()
if (NOT CCMATH_SOURCE_DIR)
    set(CCMATH_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

# TODO: Possibly change this to instead use cmakes more modern target_compile_features
# TODO: Changing this to use target_compile_features will require adjustments of the CI
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "CCMath: Version: ${PROJECT_VERSION}")

#
# User-facing options
#
include(cmake/config/UserOptions.cmake) # To see the options, look at this file

add_library(ccmath-compile-options INTERFACE)
add_library(ccmath::ccmath-compile-options ALIAS ccmath-compile-options)

#
# Compiler Flags and Conditions
#

# MSVC Compiler Options
if (MSVC)
    # Set MSVC-specific compilation flags
    target_compile_options(ccmath-compile-options INTERFACE
            /W4              # Enable high warning level
            /permissive-     # Enforce standard C++ conformance
            /Zc:__cplusplus  # Properly define __cplusplus macro for the compiler version
            /EHsc            # Enable standard exception handling
    )

    # Define NOMINMAX on Windows platforms to avoid macro conflicts with std::min/std::max
    if (WIN32)
        target_compile_definitions(ccmath-compile-options INTERFACE NOMINMAX)
    endif ()

    # Treat warnings as errors if CCMATH_STRICT_WARNINGS is set
    if (CCMATH_STRICT_WARNINGS)
        target_compile_options(ccmath-compile-options INTERFACE /WX) # Warnings as errors
    endif ()

    # Clang, GCC, and IntelLLVM Compiler Options
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|IntelLLVM")


    # Generic options for Clang, GCC, and IntelLLVM
    target_compile_options(ccmath-compile-options INTERFACE
            -Wall           # Enable common warnings
            -Wextra         # Enable additional warnings
            -Wconversion    # Warn on implicit type conversions
            -Wpedantic      # Enforce strict standard compliance
            # Define NOMINMAX on Windows to avoid macro conflicts with min/max
            $<$<BOOL:${WIN32}>:-DNOMINMAX>
    )

    # Add Clang-specific options
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(ccmath-compile-options INTERFACE
                -Wpedantic-macros  # Warn about non-standard macro usage
        )
    endif ()

    # Treat specific warnings as errors if CCMATH_STRICT_WARNINGS is set
    if (CCMATH_STRICT_WARNINGS)
        target_compile_options(ccmath-compile-options INTERFACE
                -Werror=return-type  # Treat missing return type warnings as errors
        )
    endif ()

    # Add aggressive debug options for developers if CCMATH_DEV_AGGRESSIVE_DEBUG is set
    if (CCMATH_DEV_AGGRESIVE_DEBUG)
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|IntelLLVM")

            target_compile_options(ccmath-compile-options INTERFACE
                    -Wwrite-strings  # Warn about writeable string literals
                    -g3              # Maximum debug information
            )
        elseif (MSVC)
            target_compile_options(ccmath-compile-options INTERFACE
                    /Zi  # Generate full debugging information
            )
        endif ()
    endif ()

    # TODO: Decide if we plan to delete this or not
    #  if (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
    #    target_compile_options(ccmath-compile-options INTERFACE
    #            -Wno-tautological-constant-compare
    #    )
    #  endif()

else ()
    message(WARNING "CCMath: Unknown compiler. No specific flags applied.")
endif ()

add_library(ccmath INTERFACE)
add_library(ccmath::ccmath ALIAS ccmath)

include(cmake/helpers/CcmAddHeaders.cmake)

# Add public headers through a directory-level CMakeLists that sets variables, etc.
add_subdirectory(include/ccmath)


# CCMath configuration and detection files
include(cmake/config/features/GetAllSupportedFeatures.cmake)


# Ensure proper include directories for consumers
target_include_directories(ccmath INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(ccmath INTERFACE
        ccmath::ccmath-compile-options
)

if (CCMATH_ENABLE_RUNTIME_SIMD)
    target_compile_definitions(ccmath INTERFACE CCM_CONFIG_USE_RT_SIMD)
endif ()

if (NOT CCMATH_ENABLE_USER_DEFINED_OPTIMIZATION_MACROS)
    target_compile_definitions(ccmath INTERFACE
            $<$<CONFIG:Debug>:CCM_CONFIG_DEBUG>
            $<$<CONFIG:RelWithDebInfo>:CCM_CONFIG_OPTIMIZE>
            $<$<CONFIG:Release>:CCM_CONFIG_AGGRESSIVELY_OPTIMIZE>
            $<$<CONFIG:MinSizeRel>:CCM_CONFIG_MINSIZE>
    )
endif ()

if (CCMATH_DISABLE_ERRNO)
    target_compile_definitions(ccmath INTERFACE CCM_CONFIG_DISABLE_ERRNO)
endif ()

# Generate version header
configure_file(cmake/version.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/include/ccmath/version.hpp" @ONLY)

# Add optional subdirectories only if requested
if (CCMATH_BUILD_EXAMPLES OR CCMATH_BUILD_BENCHMARKS OR CCMATH_BUILD_TESTS)
    # Use SYSTEM to suppress warnings from thirdparty code if supported by the CMake version.
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
        add_subdirectory(thirdparty SYSTEM)
    else ()
        add_subdirectory(thirdparty)
    endif ()
endif ()

if (CCMATH_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()

if (CCMATH_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif ()

if (CCMATH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()

# Installation and Packaging
if (CCMATH_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS
            ccmath
            ccmath-compile-options
            EXPORT ccmath-targets
    )

    install(DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/include/"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            FILES_MATCHING PATTERN "*.hpp"
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/include/ccmath/version.hpp"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/ccmath"
    )

    install(EXPORT ccmath-targets
            FILE ccmath-targets.cmake
            NAMESPACE ccmath::
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ccmath"
    )

    configure_package_config_file(
            cmake/ccmath-config.cmake.in
            "${CMAKE_CURRENT_BINARY_DIR}/ccmath-config.cmake"
            INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ccmath"
    )

    # Write a version file for strict version checking
    write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/ccmath-config-version.cmake"
            COMPATIBILITY SameMajorVersion
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/ccmath-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/ccmath-config-version.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ccmath"
    )
endif ()
