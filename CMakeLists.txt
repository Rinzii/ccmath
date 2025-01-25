cmake_minimum_required(VERSION 3.18.0)


# ----------------------------------------------------------------------
# Project Setup and Metadata
# ----------------------------------------------------------------------
set(CCMATH_TARGET_NAME "ccmath")
set(CCMATH_BUILD_VERSION 0.2.0)

project(
        ${CCMATH_TARGET_NAME}
        VERSION ${CCMATH_BUILD_VERSION}
        DESCRIPTION "A C++17 Compile Time <cmath> Library"
        HOMEPAGE_URL "https://github.com/Rinzii/ccmath"
        LANGUAGES CXX
)

# Include helper CMake files for project configuration
include(cmake/config/ProjectIsTopLevel.cmake)
include(cmake/config/ProjectOptions.cmake)


# ----------------------------------------------------------------------
# Color Diagnostics (Only if at top-level with Ninja)
# ----------------------------------------------------------------------
if (CCMATH_PROJECT_IS_TOP_LEVEL)
    if (CMAKE_GENERATOR STREQUAL "Ninja")
        # Enable colored output for C++ compilers
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
        endif ()
        # Enable colored output for C compilers
        if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
            SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
        elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
            SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
        endif ()
    endif ()
endif ()

message(STATUS "CCMath: Version: ${CCMATH_BUILD_VERSION}")

# Load user-configurable options (e.g., BUILD_EXAMPLES, BUILD_TESTS, etc.)
include(cmake/config/UserOptions.cmake)


# ----------------------------------------------------------------------
# Compiler Warning Options
# ----------------------------------------------------------------------
# Collect common warning flags for various compilers and conditions.
set(
        ccmath_warning_options
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wconversion -Wpedantic -Werror=return-type -Wundef -Wshadow -Wnull-dereference>
        $<$<CXX_COMPILER_ID:Clang>:-Wpedantic-macros>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /we4715>
)

# Turn on -Werror or /WX if requested
if (CCMATH_ENABLE_WARNINGS_AS_ERRORS)
    set(
            ccmath_warning_options
            ${ccmath_warning_options}
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
            $<$<CXX_COMPILER_ID:MSVC>:/WX>
    )
endif ()

# Add extra debug info flags if requested
if (CCMATH_ENABLE_DEBUG_INFO)
    set(
            ccmath_warning_options
            ${ccmath_warning_options}
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wwrite-strings -g3>
            $<$<CXX_COMPILER_ID:MSVC>:/Zi /analyze>
    )
endif ()


# ----------------------------------------------------------------------
# Library Definition
# ----------------------------------------------------------------------
add_library(${CCMATH_TARGET_NAME} INTERFACE)
add_library(${CCMATH_TARGET_NAME}::${CCMATH_TARGET_NAME} ALIAS ${CCMATH_TARGET_NAME})

# Collect all header files for installation or packaging
include(cmake/helpers/CcmAddHeaders.cmake) # Helper for adding headers to interface
add_subdirectory(include/ccmath)

# Detect supported compiler features
include(cmake/features/GetAllSupportedFeatures.cmake)

target_include_directories(${CCMATH_TARGET_NAME} INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Specify minimum C++ standard for library (likely C++17)
target_compile_features(
        ${CCMATH_TARGET_NAME}
        INTERFACE
        ${CCMATH_DESIRED_CXX_STANDARD}
)

# Enable aggressive warning options only if requested
if (CCMATH_ENABLE_AGGRESSIVE_WARNINGS)
    target_compile_options(
            ${CCMATH_TARGET_NAME}
            INTERFACE
            ${ccmath_warning_options}
    )
endif ()

# Enable AddressSanitizer if requested
if (CCMATH_ENABLE_SANITIZER_BUILD)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif ()

# Setup project specific compile definitions that must continue with the interface
include(cmake/config/ProjectCompileDefinitions.cmake)

# Setup the version header for the library
set(CCMATH_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(CCMATH_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(CCMATH_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
configure_file("${PROJECT_SOURCE_DIR}/cmake/in/version.hpp.in" "${PROJECT_BINARY_DIR}/include/ccmath/version.hpp")


# ----------------------------------------------------------------------
# Installation Rules
# ----------------------------------------------------------------------
if (CCMATH_INSTALL)
    include(cmake/config/InstallRules.cmake)
endif ()


# ----------------------------------------------------------------------
# Subdirectories for Examples, Benchmarks, and Tests
# ----------------------------------------------------------------------
# Only add these if the user explicitly asked for them in the options.
if (CCMATH_BUILD_EXAMPLES OR CCMATH_BUILD_BENCHMARKS OR CCMATH_BUILD_TESTS)
    # Use SYSTEM to suppress warnings from external code if supported by cmake
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
        add_subdirectory(thirdparty SYSTEM)
    else ()
        add_subdirectory(thirdparty)
    endif ()

    if (CCMATH_BUILD_EXAMPLES)
        # Use SYSTEM to suppress warnings from external code if supported by cmake
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
            add_subdirectory(examples SYSTEM)
        else ()
            add_subdirectory(examples)
        endif ()
    endif ()

    if (CCMATH_BUILD_BENCHMARKS)
        # Use SYSTEM to suppress warnings from external code if supported by cmake
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
            add_subdirectory(benchmarks SYSTEM)
        else ()
            add_subdirectory(benchmarks)
        endif ()
    endif ()

    if (CCMATH_BUILD_TESTS)
        include(CTest)
        enable_testing()
        # Use SYSTEM to suppress warnings from external code if supported by cmake
        if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
            add_subdirectory(tests SYSTEM)
        else ()
            add_subdirectory(tests)
        endif ()
    endif ()
endif ()
