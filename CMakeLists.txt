# CCMath

cmake_minimum_required(VERSION 3.18)

# Set global build version for CCMath

set(CCMATH_BUILD_VERSION 0.2.0)

# Project Configuration

set(CCMATH_PROJ_NAME "ccmath")

project(
        ${CCMATH_PROJ_NAME}
        VERSION ${CCMATH_BUILD_VERSION}
        DESCRIPTION "A C++17 Compile Time <cmath> Library"
        HOMEPAGE_URL "https://github.com/Rinzii/ccmath"
        LANGUAGES CXX
)

if(NOT CCMATH_SOURCE_DIR)
    set(CCMATH_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()

set(is_root_project OFF) # Identifies if this is the root project
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(is_root_project ON)
endif ()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

configure_file(cmake/input/version.hpp.in "${CCMATH_SOURCE_DIR}/include/${PROJECT_NAME}/version.hpp" @ONLY)

message(VERBOSE "*")
message(VERBOSE "* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message(VERBOSE "* Copyright (c) 2024-Present Ian Pike <ianpike98@gmail.com>")
message(VERBOSE "* Copyright (c) 2024-Present CCMath contributors <https://github.com/Rinzii/ccmath>")
message(VERBOSE "*")

# Bring in cmake modules

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

# Compiler Configuration

option(CCMATH_USE_SANITIZERS "Enable sanitizers by adding -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined flags if available" OFF)
option(CCMATH_USE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

if(CCMATH_USE_SANITIZER)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(CCMATH_HAS_SANITIZER TRUE CACHE BOOL "" FORCE)
        mark_as_advanced(CCMATH_HAS_SANITIZER)
    endif()

    if(NOT CCMATH_HAS_SANITIZER)
        message(VERBOSE "The option CCMATH_USE_SANITIZER is set but sanitizer support is not available!")
    endif()
endif()

if(CCMATH_USE_CLANG_TIDY)
    find_program(CCMATH_CLANG_TIDY_EXECUTABLE "clang-tidy")

    if(NOT CCMATH_CLANG_TIDY_EXECUTABLE)
        message(VERBOSE "The option CCMATH_USE_CLANG_TIDY is set but clang-tidy executable is not available!")
    endif()
endif()

# Add CCMath targets

option(CCMATH_INSTALL "Setup install and package steps" ${is_root_project})

include(GNUInstallDirs)

include(ccmath_internal_headers.cmake)
include(ccmath_core_headers.cmake)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set(CCMATH_CONFIG_DESIRED_CPP_VER 17)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_${CCMATH_CONFIG_DESIRED_CPP_VER})

target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ccmath_internal_headers}>")
target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ccmath_core_headers}>")

option(CCMATH_ENABLE_EXTENSIONS "Enable the extended ccmath library that adds helpful additional methods that are not defined by the standard" ${is_root_project})

if (CCMATH_ENABLE_EXTENSIONS)
    include(ccmath_extensions_headers.cmake)
    target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ccmath_extensions_headers}>")
endif ()

# include the global configuration file
include(cmake/config/GlobalConfig.cmake)

add_library(${PROJECT_NAME}-compile-options INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME}-compile-options ALIAS ${PROJECT_NAME}-compile-options)


target_include_directories(${PROJECT_NAME}-compile-options INTERFACE $<BUILD_INTERFACE:${CCMATH_SOURCE_DIR}/include/>)
target_include_directories(${PROJECT_NAME}-compile-options SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

if(CCMATH_HAS_SANITIZER)
    target_compile_options(${PROJECT_NAME}-compile-options INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined>)
    target_link_libraries(${PROJECT_NAME}-compile-options INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined>)
endif()

option(CCMATH_ENABLE_AGGRESSIVE_COMPILER_FLAGS "Enforce aggressive flags for error checking and static analysis" ${is_root_project})

if(CCMATH_ENABLE_AGGRESSIVE_COMPILER_FLAGS)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|IntelLLVM")
        target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
                -Wall
                -Wextra
                -Wconversion
                -Werror=return-type

                -Wno-pedantic

                # Allow FMA instructions
                -ffp-contract=fast

                # Macros to define
                -DNOMINMAX
        )

        # Disable intel specific warnings that don't apply to us.
        if (CMAKE_CXX_COMPILER_ID STREQUAL IntelLLVM)
            target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
                    -Wno-tautological-constant-compare
            )
        endif ()

        if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
            target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
                    /W3
                    /WX
                    /permissive-

                    /Zc:__cplusplus

                    # Macros to define
                    /DNOMINMAX
                    /D_ENABLE_EXTENDED_ALIGNED_STORAGE
            )
        endif ()

        # TODO: Remove this later.
        #       Some variables have been provided but are not currently being used, but it would not atm make sense to remove them.
        #       So to clean up the warnings we are just silencing these specific cases.
        target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
                -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-parameter
        )
    endif ()
endif ()

target_link_libraries(${PROJECT_NAME} INTERFACE
        ${PROJECT_NAME}::${PROJECT_NAME}-compile-options
)

# SIMD Configuration

option(CCMATH_ENABLE_RUNTIME_SIMD "Enable SIMD optimization for runtime evaluation (does not effect compile time)" ON)

if (CCMATH_ENABLE_RUNTIME_SIMD)
    # TODO: Add checking for simd support through cmake using check_cxx_source_compiles
    # Example of how checking would work:
    #        cmake_push_check_state()
    #
    #        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -stdlib=libc++")
    #
    #        check_cxx_source_compiles("
    #            #include<type_traits>
    #            int main() { return std::is_same_v<int, char>; }
    #        " CCMATH_CONFIG_HAS_LIBCPP)
    #
    #        cmake_pop_check_state()

    target_compile_definitions(${PROJECT_NAME} INTERFACE CCM_CONFIG_USE_RT_SIMD)
endif ()

# Decide if we allow the user to define there own macros or not

option(CCMATH_ENABLE_USER_DEFINED_OPTIMIZATION_MACROS "Enable user defined optimization macros instead of having ccmath define its own internal ones in cmake" OFF)

if (NOT CCMATH_ENABLE_USER_DEFINED_OPTIMIZATION_MACROS)
    # Set these definitions based on the build type to aid in identifying the optimization level.
    # These are essential when dealing with compiler specific outcomes that are based on the optimization level.
    target_compile_definitions(${PROJECT_NAME}
            INTERFACE
            $<$<CONFIG:Debug>:CCM_CONFIG_DEBUG> # -O0
            $<$<CONFIG:RelWithDebInfo>:CCM_CONFIG_OPTIMIZE> # -O2
            $<$<CONFIG:Release>:CCM_CONFIG_AGGRESSIVELY_OPTIMIZE> # -O3
            $<$<CONFIG:MinSizeRel>:CCM_CONFIG_MINSIZE> # -Os
    )
endif ()

# Let the user decide if they want to disable errno checking and setting

option(CCMATH_DISABLE_ERRNO "Disable the use of errno in ccmath during runtime" OFF)

if (CCMATH_DISABLE_ERRNO)
    target_compile_definitions(${PROJECT_NAME} INTERFACE CCM_CONFIG_DISABLE_ERRNO)
endif ()

# TODO: Implement pkgconfig

# Install CCMath


if (CCMATH_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS
            ${PROJECT_NAME}
            ${PROJECT_NAME}-compile-options
            EXPORT ${PROJECT_NAME}Targets
    )

    # Manually specify the headers to install
    install(DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/include/"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            FILES_MATCHING PATTERN "*.hpp"
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.hpp"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
    )

    install(EXPORT ${PROJECT_NAME}Targets
            FILE ${PROJECT_NAME}Targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    configure_package_config_file(
            cmake/input/${PROJECT_NAME}Config.cmake.in
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")


    export(PACKAGE ${PROJECT_NAME})
endif ()

# Tests

option(CCMATH_BUILD_TESTS "Build ccmath tests" OFF)

if (CCMATH_BUILD_TESTS)
    option(CCMATH_FIND_GTEST_PACKAGE "Enable finding of gtest package" OFF)

    #include(CTest)
    enable_testing()
    add_subdirectory(test)
endif ()

# Benchmarks

option(CCMATH_BUILD_BENCHMARKS "Build ccmath benchmarks" OFF)

if (CCMATH_BUILD_BENCHMARKS)
    option(CCMATH_FIND_GBENCH_PACKAGE "Enable finding of google benchmark package" OFF)

    add_subdirectory(benchmark)
endif ()

# Examples

option(CCMATH_BUILD_EXAMPLES "Build ccmath examples" OFF)

if (CCMATH_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()




