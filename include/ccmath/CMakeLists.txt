macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/ccmath.hpp
)

add_subdirectory(ext)
add_subdirectory(internal)
add_subdirectory(math)
