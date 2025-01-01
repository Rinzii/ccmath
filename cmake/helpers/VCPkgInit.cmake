# |=======================================================================|
# |                  VCPKG submodule init/update                          |
# |=======================================================================|

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/extern/vcpkg/ports")
    message(NOTICE "VCPKG package manager sources not found")
    message(NOTICE "initializing/updating the vcpkg submodule...")
    execute_process(
        COMMAND git submodule update --init extern/vcpkg
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

# VCPKG toolchain file
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(toolchain_file_path "${CMAKE_SOURCE_DIR}/extern/vcpkg/scripts/buildsystems/vcpkg.cmake")
    if (EXISTS "${toolchain_file_path}")
        set(CMAKE_TOOLCHAIN_FILE "${toolchain_file_path}")
    else()
        message(WARNING "VCPKG toolchain file not found: ${toolchain_file_path}")
    endif()
endif()

# |=======================================================================|
# |               VCPKG bootstrap / initialization                        |
# |=======================================================================|

if (WIN32)
    set(EXECUTABLE_EXTENSION ".exe")
endif()
set(VCPKG_PROGRAM "${CMAKE_SOURCE_DIR}/extern/vcpkg/vcpkg${EXECUTABLE_EXTENSION}")

if(EXISTS "${VCPKG_PROGRAM}")
    message(NOTICE "Found VCPKG Executable: ${VCPKG_PROGRAM}")
else()
    message(NOTICE "Could not find VCPKG Executable: ${VCPKG_PROGRAM}")
    message(NOTICE "Calling VCPKG bootstrap scripts.")

    if(WIN32)
        set(VCPKG_INSTALL_COMMAND
            powershell
            -c
            "${CMAKE_SOURCE_DIR}/extern/vcpkg/bootstrap-vcpkg.bat"
        )
    else()
        set(VCPKG_INSTALL_COMMAND
            bash
            "${CMAKE_SOURCE_DIR}/extern/vcpkg/bootstrap-vcpkg.sh"
        )
    endif()

    execute_process(
        COMMAND ${VCPKG_INSTALL_COMMAND}
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMAND_ERROR_IS_FATAL ANY
    )

    if(NOT EXISTS "${VCPKG_PROGRAM}")
        message(FATAL_ERROR "ERROR: '${VCPKG_PROGRAM}' not found!")
    endif()
endif()
