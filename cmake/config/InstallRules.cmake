if (NOT DEFINED CCMATH_ROOT_DIR)
    message(FATAL_ERROR "CCMATH_ROOT_DIR not defined in cmake for install rules!")
endif ()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY
        "${CCMATH_ROOT_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.hpp"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/include/ccmath/version.hpp"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/ccmath"
)

install(
        TARGETS ${CCMATH_TARGET_NAME}
        EXPORT ${CCMATH_TARGET_NAME}-targets
)

install(EXPORT ${CCMATH_TARGET_NAME}-targets
        FILE ${CCMATH_TARGET_NAME}-targets.cmake
        NAMESPACE ${CCMATH_TARGET_NAME}::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ccmath"
)

configure_package_config_file(
        "${CCMATH_ROOT_DIR}/cmake/in/ccmath-config.cmake.in"
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
