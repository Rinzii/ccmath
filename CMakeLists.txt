# CCMath

cmake_minimum_required(VERSION 3.18)

# Set global build version for CCMath

set(CCMATH_BUILD_VERSION 0.2.0)

# Project Configuration

project(
        ccmath
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

configure_file(cmake/version.hpp.in "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/version.hpp" @ONLY)

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

if (CCMATH_ENABLE_EXTENSIONS)
    include(ccmath_extensions_headers.cmake)
    target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ccmath_extensions_headers}>")
endif ()

target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CCMATH_SOURCE_DIR}/include/>)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

if(CCMATH_HAS_SANITIZER)
    target_compile_options(${PROJECT_NAME} INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined>)
    target_link_libraries(${PROJECT_NAME} INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined>)
endif()

option(CCMATH_ENABLE_AGGRESSIVE_COMPILER_FLAGS "Enforce aggressive flags for error checking and static analysis" ${is_root_project})

if(CCMATH_ENABLE_AGGRESSIVE_COMPILER_FLAGS)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|IntelLLVM")
        target_compile_options(${PROJECT_NAME} INTERFACE
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
            target_compile_options(${PROJECT_NAME} INTERFACE
                    -Wno-tautological-constant-compare
            )
        endif ()

        if (CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
            target_compile_options(${PROJECT_NAME} INTERFACE
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
        target_compile_options(${PROJECT_NAME} INTERFACE
                -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-parameter
        )
    endif ()
endif ()

# TODO: Implement pkgconfig

# Install CCMath


if (CCMATH_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(
            TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    write_basic_package_version_file(
            ${PROJECT_NAME}ConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY AnyNewerVersion
    )

    configure_package_config_file(
            ${CCMATH_SOURCE_DIR}/cmake/input/${PROJECT_NAME}Config.cmake.in
            ${PROJECT_NAME}Config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake
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

    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}-targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    configure_package_config_file(
            cmake/${PROJECT_NAME}-config.cmake.in
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
            INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

endif ()

