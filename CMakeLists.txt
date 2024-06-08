cmake_minimum_required(VERSION 3.18)

set(CCMATH_BUILD_VERSION 0.2.0)

project(ccmath VERSION ${CCMATH_BUILD_VERSION})

set(is_root_project OFF) # Identifies if this is the root project
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(is_root_project ON)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TO THE IMPLEMENTOR: If CCMATH_BUILD_TESTS is set to OFF then googletest can be deleted from the ext folder.
option(CCMATH_BUILD_TESTS "Build ccmath tests" ${is_root_project})
option(CCMATH_BUILD_EXAMPLES "Build ccmath examples" ${is_root_project})
option(CCMATH_BUILD_BENCHMARKS "Build ccmath benchmarks" ON)
option(CCMATH_INSTALL "Setup install and package steps" ${is_root_project})
option(CCMATH_ENABLE_EXTENSIONS "Enable the extended ccmath library that adds helpful additional methods that are not defined by the standard" ON)
option(CCMATH_ENABLE_RUNTIME_SIMD "Enable SIMD optimization for runtime evaluation (does not effect compile time)" ON)
option(CCMATH_ENABLE_USER_DEFINED_OPTIMIZATION_MACROS "Enable user defined optimization macros instead of having ccmath define its own internal ones in cmake" ON)
option(CCMATH_DISABLE_ERRNO "Disable the use of errno in ccmath during runtime" OFF)

# include the global configuration file
include(cmake/GlobalConfig.cmake)

add_library(${PROJECT_NAME}-compile-options INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME}-compile-options ALIAS ${PROJECT_NAME}-compile-options)


##### ccmath specified compiler options

# Generic clang/gcc compiler options
if (CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL IntelLLVM)
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

    # Disable intel specific warnings that don't apply to us.
    if (CMAKE_CXX_COMPILER_ID STREQUAL IntelLLVM)
        target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
                -Wno-tautological-constant-compare
        )
    endif ()


    # TODO: Remove this later.
    #       Some variables have been provided but are not currently being used, but it would not atm make sense to remove them.
    #       So to clean up the warnings we are just silencing these specific cases.
    target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
            -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-parameter
    )
endif ()

include(ccmath_headers.cmake)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ccmath_headers}>")

if (CCMATH_ENABLE_EXTENSIONS)
    include(ccmath_extensions_headers.cmake)
    target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ccmath_extensions_headers}>")
endif ()

target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

target_link_libraries(${PROJECT_NAME} INTERFACE
        ${PROJECT_NAME}::${PROJECT_NAME}-compile-options
)

if (CCMATH_ENABLE_RUNTIME_SIMD)
    target_compile_definitions(${PROJECT_NAME} INTERFACE CCM_CONFIG_USE_RT_SIMD)
endif ()

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

if (CCMATH_DISABLE_ERRNO)
    target_compile_definitions(${PROJECT_NAME} INTERFACE CCM_CONFIG_DISABLE_ERRNO)
endif ()

configure_file(cmake/version.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.hpp" @ONLY)

if (CCMATH_BUILD_EXAMPLES OR CCMATH_BUILD_BENCHMARKS OR CCMATH_BUILD_TESTS)
    add_subdirectory(thirdparty)
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


if (CCMATH_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS
            ${PROJECT_NAME}
            ${PROJECT_NAME}-compile-options
            EXPORT ${PROJECT_NAME}-targets
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
