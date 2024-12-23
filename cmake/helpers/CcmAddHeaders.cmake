# Helper function to add headers to the global source list
function(ccm_add_headers)
  # Append all the passed arguments (headers) to the target's sources
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${ARGV}>")
endfunction()
