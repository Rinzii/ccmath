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

add_library(${PROJECT_NAME}-compile-options INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME}-compile-options ALIAS ${PROJECT_NAME}-compile-options)

#
# Compiler Flags and Conditions
#
if (MSVC)
  # MSVC-specific options
  target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
          /W4
          /permissive-
          /Zc:__cplusplus
          /D_ENABLE_EXTENDED_ALIGNED_STORAGE
          /D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
  )
  # Only define NOMINMAX on Windows platforms
  if (WIN32)
    target_compile_definitions(${PROJECT_NAME}-compile-options INTERFACE NOMINMAX)
  endif()

  if (CCMATH_STRICT_WARNINGS)
    target_compile_options(${PROJECT_NAME}-compile-options INTERFACE /WX)
  endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
        OR CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")

  # Generic clang/gcc/intel options
  # Consider making some of these suppressions configurable if requested by users.
  target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
          -Wall
          -Wextra
          -Wconversion
          -Wpedantic
          # Define NOMINMAX only on Windows to avoid conflicts with min/max macros
          $<$<BOOL:${WIN32}>:-DNOMINMAX>
          #-Wno-unused-but-set-variable
          #-Wno-unused-value
          #-Wno-unused-parameter
  )

  if (CCMATH_STRICT_WARNINGS)
    target_compile_options(${PROJECT_NAME}-compile-options INTERFACE -Werror=return-type)
  endif ()

#  if (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
#    target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
#            -Wno-tautological-constant-compare
#    )
#  endif()

else()
  message(WARNING "CCMath: Unknown compiler. No specific flags applied.")
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(cmake/helpers/CcmAddHeaders.cmake)

# Add public headers through a directory-level CMakeLists that sets variables, etc.
add_subdirectory(include/ccmath)


# CCMath configuration and detection files
include(cmake/config/features/GetAllSupportedFeatures.cmake)


# Ensure proper include directories for consumers
target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME} INTERFACE
        ${PROJECT_NAME}::${PROJECT_NAME}-compile-options
)

if (CCMATH_ENABLE_RUNTIME_SIMD)
  target_compile_definitions(${PROJECT_NAME} INTERFACE CCM_CONFIG_USE_RT_SIMD)
endif ()

if (NOT CCMATH_ENABLE_USER_DEFINED_OPTIMIZATION_MACROS)
  target_compile_definitions(${PROJECT_NAME} INTERFACE
          $<$<CONFIG:Debug>:CCM_CONFIG_DEBUG>
          $<$<CONFIG:RelWithDebInfo>:CCM_CONFIG_OPTIMIZE>
          $<$<CONFIG:Release>:CCM_CONFIG_AGGRESSIVELY_OPTIMIZE>
          $<$<CONFIG:MinSizeRel>:CCM_CONFIG_MINSIZE>
  )
endif ()

if (CCMATH_DISABLE_ERRNO)
  target_compile_definitions(${PROJECT_NAME} INTERFACE CCM_CONFIG_DISABLE_ERRNO)
endif ()

# Generate version header
configure_file(cmake/version.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.hpp" @ONLY)

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
          ${PROJECT_NAME}
          ${PROJECT_NAME}-compile-options
          EXPORT ${PROJECT_NAME}-targets
  )

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

  # Write a version file for strict version checking
  write_basic_package_version_file(
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
          COMPATIBILITY SameMajorVersion
  )

  install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
          DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  )
endif ()
