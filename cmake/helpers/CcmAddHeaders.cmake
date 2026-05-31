# Helper functions for adding headers to the interfaces of ccmath

function(ccm_add_headers)
    foreach (header IN LISTS ARGV)
        target_sources(${CCMATH_TARGET_NAME} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${header}>")
    endforeach ()
endfunction()

# TODO: Verify this is correct
function(ccm_add_target_interface target)
    foreach (header IN LISTS ARGV)
        if (NOT header STREQUAL ${target})
            target_sources(${target} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${header}>")
        endif ()
    endforeach ()
endfunction()
