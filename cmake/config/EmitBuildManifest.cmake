include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/BuildManifest.cmake")

function(_ccmath_manifest_json_escape input output_var)
    string(REPLACE "\\" "\\\\" _ccmath_escaped "${input}")
    string(REPLACE "\"" "\\\"" _ccmath_escaped "${_ccmath_escaped}")
    set("${output_var}" "${_ccmath_escaped}" PARENT_SCOPE)
endfunction()

function(_ccmath_manifest_append_option_json_lines out_var)
    set(_ccmath_json_lines "")
    set(_ccmath_comma "")

    foreach (_ccmath_manifest_key IN LISTS CCMATH_LIBRARY_MANIFEST_OPTION_KEYS)
        set(_ccmath_cmake_var "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_CMAKE_VAR}")
        set(_ccmath_meson_option "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_MESON_OPTION}")
        set(_ccmath_define "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_DEFINE}")
        set(_ccmath_description "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_DESCRIPTION}")
        set(_ccmath_cmake_default "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_CMAKE_DEFAULT}")
        set(_ccmath_invert "${CCMATH_LIBRARY_MANIFEST_OPTION_${_ccmath_manifest_key}_CMAKE_INVERT}")

        ccmath_manifest_meson_default("${_ccmath_manifest_key}" _ccmath_meson_default)
        ccmath_manifest_option_enabled("${_ccmath_manifest_key}" _ccmath_enabled)

        _ccmath_manifest_json_escape("${_ccmath_description}" _ccmath_description_json)

        if (_ccmath_cmake_default)
            set(_ccmath_cmake_default_json "true")
        else ()
            set(_ccmath_cmake_default_json "false")
        endif ()

        if (_ccmath_meson_default)
            set(_ccmath_meson_default_json "true")
        else ()
            set(_ccmath_meson_default_json "false")
        endif ()

        if (_ccmath_invert)
            set(_ccmath_invert_json "true")
        else ()
            set(_ccmath_invert_json "false")
        endif ()

        if (_ccmath_enabled)
            set(_ccmath_enabled_json "true")
        else ()
            set(_ccmath_enabled_json "false")
        endif ()

        string(APPEND _ccmath_json_lines
                "${_ccmath_comma}    {\n"
                "      \"key\": \"${_ccmath_manifest_key}\",\n"
                "      \"cmake_option\": \"${_ccmath_cmake_var}\",\n"
                "      \"meson_option\": \"${_ccmath_meson_option}\",\n"
                "      \"define\": \"${_ccmath_define}\",\n"
                "      \"description\": \"${_ccmath_description_json}\",\n"
                "      \"cmake_default\": ${_ccmath_cmake_default_json},\n"
                "      \"meson_default\": ${_ccmath_meson_default_json},\n"
                "      \"cmake_invert\": ${_ccmath_invert_json},\n"
                "      \"enabled\": ${_ccmath_enabled_json}\n"
                "    }")
        set(_ccmath_comma ",\n")
    endforeach ()

    set("${out_var}" "${_ccmath_json_lines}" PARENT_SCOPE)
endfunction()

function(ccmath_emit_build_manifest output_path)
    _ccmath_manifest_append_option_json_lines(_ccmath_option_json)

    set(_ccmath_generated_headers_json "")
    set(_ccmath_header_comma "")
    foreach (_ccmath_header IN LISTS CCMATH_LIBRARY_MANIFEST_GENERATED_HEADERS)
        string(APPEND _ccmath_generated_headers_json
                "${_ccmath_header_comma}    \"${_ccmath_header}\"")
        set(_ccmath_header_comma ",\n")
    endforeach ()

    _ccmath_manifest_json_escape("${CMAKE_CURRENT_SOURCE_DIR}" _ccmath_source_dir_json)
    _ccmath_manifest_json_escape("${CMAKE_CURRENT_BINARY_DIR}" _ccmath_build_dir_json)
    _ccmath_manifest_json_escape("${CCMATH_LIBRARY_MANIFEST_VERSION_TEMPLATE}" _ccmath_version_template_json)

    set(_ccmath_manifest_json "")
    string(APPEND _ccmath_manifest_json
            "{\n"
            "  \"name\": \"${CCMATH_TARGET_NAME}\",\n"
            "  \"version\": \"${CCMATH_BUILD_VERSION}\",\n"
            "  \"cpp_standard\": ${CCMATH_LIBRARY_MANIFEST_CPP_STANDARD},\n"
            "  \"source_dir\": \"${_ccmath_source_dir_json}\",\n"
            "  \"build_dir\": \"${_ccmath_build_dir_json}\",\n"
            "  \"include_dirs\": [\n"
            "    \"include\",\n"
            "    \"${CMAKE_CURRENT_BINARY_DIR}/include\"\n"
            "  ],\n"
            "  \"version_template\": \"${_ccmath_version_template_json}\",\n"
            "  \"generated_headers\": [\n"
            "${_ccmath_generated_headers_json}\n"
            "  ],\n"
            "  \"options\": [\n"
            "${_ccmath_option_json}\n"
            "  ]\n"
            "}\n")

    file(WRITE "${output_path}" "${_ccmath_manifest_json}")
endfunction()

function(ccmath_setup_secondary_build_generation)
    set(_ccmath_manifest_path "${CMAKE_CURRENT_BINARY_DIR}/ccmath-build-manifest.json")
    set(_ccmath_gen_dir "${CMAKE_CURRENT_BINARY_DIR}/gen-secondary")

    ccmath_emit_build_manifest("${_ccmath_manifest_path}")

    find_package(Python3 COMPONENTS Interpreter QUIET)
    if (NOT Python3_Interpreter_FOUND)
        message(WARNING "CCMath: Python3 not found. Secondary build generation will be skipped.")
        return()
    endif ()

    add_custom_command(
            OUTPUT
            "${_ccmath_gen_dir}/meson.build"
            "${_ccmath_gen_dir}/meson_options.txt"
            "${_ccmath_gen_dir}/premake5.lua"
            "${_ccmath_gen_dir}/BUILD_STAMP"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_ccmath_gen_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy "${_ccmath_manifest_path}" "${_ccmath_gen_dir}/ccmath-build-manifest.json"
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_secondary_builds.py"
            --manifest "${_ccmath_manifest_path}"
            --output-dir "${_ccmath_gen_dir}"
            --source-dir "${CMAKE_CURRENT_SOURCE_DIR}"
            --update-root
            COMMAND ${CMAKE_COMMAND} -E touch "${_ccmath_gen_dir}/BUILD_STAMP"
            DEPENDS
            "${_ccmath_manifest_path}"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_secondary_builds.py"
            COMMENT "Generating Meson and Premake secondary build files"
            VERBATIM
    )

    add_custom_target(
            ccmath-generate-secondary-builds
            ALL
            DEPENDS
            "${_ccmath_gen_dir}/BUILD_STAMP"
    )
endfunction()
