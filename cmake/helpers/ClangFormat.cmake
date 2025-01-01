find_program(CLANG_FORMAT_PROGRAM NAMES clang-format)

if (CLANG_FORMAT_PROGRAM)
    execute_process(
        COMMAND "${CLANG_FORMAT_PROGRAM}" --version
        OUTPUT_VARIABLE CLANG_FORMAT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    message("Using clang-format: ${CLANG_FORMAT_PROGRAM} (${CLANG_FORMAT_VERSION})")

    file(GLOB_RECURSE
        format_src_list RELATIVE
        "${CMAKE_SOURCE_DIR}"
            "include/*.[hc]"
            "include/*.[hc]pp"
            "test/*.[hc]"
            "test/*.[hc]pp"
            "example/*.[hc]"
            "example/*.[hc]pp"
    )

    set(skip_pattern "include/ccmath/internal/math/(generic|runtime)/(func|simd)/.*")

    foreach(_src_file ${format_src_list})
        unset(MATCHED_SKIP)

        string(REGEX MATCH ${skip_pattern} MATCHED_SKIP ${_src_file})

        if (MATCHED_SKIP)
            message("      skipping => ${_src_file}")
        else()
            message("    formatting => ${_src_file}")
                execute_process(
                COMMAND "${CLANG_FORMAT_PROGRAM}" --style=file -i "${_src_file}"
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            )
        endif()
    endforeach()

    unset(CLANG_FORMAT_VERSION)
endif()
