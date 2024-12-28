# Helper function to add headers to the global source list
#function(ccm_add_headers)
#  # Append all the passed arguments (headers) to the target's sources
#  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ARGV}>")
#endfunction()

function(ccm_add_headers)
  foreach(header IN LISTS ARGV)
    target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${header}>")
  endforeach()
endfunction()

