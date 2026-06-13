cmake_minimum_required(VERSION 3.18)
project(ccmath_coremath_oracle LANGUAGES C)

if (NOT CORE_MATH_SOURCE_DIR)
    message(FATAL_ERROR "CORE_MATH_SOURCE_DIR must point at a CORE-MATH source tree")
endif ()

add_library(coremath_oracle STATIC
        ${CORE_MATH_SOURCE_DIR}/src/binary64/pow/pow.c
        ${CORE_MATH_SOURCE_DIR}/src/binary32/pow/powf.c
)

target_include_directories(coremath_oracle
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CORE_MATH_SOURCE_DIR}/src/binary64/pow
)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
    target_compile_options(coremath_oracle PRIVATE -march=x86-64-v3)
endif ()

if (MSVC)
    target_compile_options(coremath_oracle PRIVATE -O2 -fp:strict)
else ()
    target_compile_options(coremath_oracle PRIVATE -O2 -fno-fast-math -ffp-contract=off)
endif ()

include(GNUInstallDirs)

install(TARGETS coremath_oracle
        EXPORT ccmath_coremath_oracle_targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/crmath.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ccmath_coremath_oracle_targets
        FILE ccmath_coremath_oracle_targets.cmake
        NAMESPACE ccmath::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ccmath_coremath_oracle
)
