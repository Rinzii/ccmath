cmake_minimum_required(VERSION 3.18)

enable_language(CXX)

# We will hit 1.0.0 when we have a stable API
# We will hit 0.1.0 when we have finished the basic API
set(CCMATH_BUILD_VERSION 0.0.1)
set(INTERNAL_PROJ_DEFAULT_NAME ccmath)

project(${INTERNAL_PROJ_DEFAULT_NAME} VERSION ${CCMATH_BUILD_VERSION})

set(is_root_project OFF) # Identifies if this is the root project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(is_root_project ON)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# TO THE IMPLEMENTOR: If CCMATH_BUILD_TEST is set to OFF then googletest can be deleted from the ext folder.
option(CCMATH_BUILD_TEST "Build ccmath tests" ${is_root_project})
option(CCMATH_BUILD_EXAMPLES "Build ccmath examples" ${is_root_project})
option(CCMATH_BUILD_BENCHMARKS "Build ccmath benchmarks" ${is_root_project})
option(CCMATH_INSTALL "Setup install and package steps" ${is_root_project})
option(CCMATH_USE_SIMD "Use SIMD instructions" OFF)

# include the global configuration file
include(cmake/GlobalConfig.cmake)

add_library(${PROJECT_NAME}-compile-options INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME}-compile-options ALIAS ${PROJECT_NAME}-compile-options)

if(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
          -Wall -Wextra -Wpedantic -Wconversion -Werror=return-type
  )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
  target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
          /W4 /Wall /WX /permissive- /Zc:__cplusplus
  )
endif()

include(ccmath_headers.cmake)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ccmath_headers}>")

target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_link_libraries(${PROJECT_NAME} INTERFACE
        ${PROJECT_NAME}::${PROJECT_NAME}-compile-options
)




configure_file(cmake/version.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.hpp" @ONLY)

if (CCMATH_BUILD_EXAMPLES)
  add_subdirectory(example)
endif()

if (CCMATH_BUILD_EXAMPLES OR CCMATH_BUILD_BENCHMARKS OR CCMATH_BUILD_TEST)
  add_subdirectory(ext)
endif()

if (CCMATH_BUILD_TEST)
  enable_testing()
  add_subdirectory(test)
endif()

if (CCMATH_USE_SIMD)
  # TODO: Add a better way to handle enabling simd internally.
  add_compile_definitions(INTERNAL_ENABLE_CHECK_FOR_SIMD)
endif ()


if(CCMATH_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(TARGETS
          ${PROJECT_NAME}
          ${PROJECT_NAME}-compile-options
          EXPORT ${PROJECT_NAME}-targets
          FILE_SET HEADERS
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

endif()
