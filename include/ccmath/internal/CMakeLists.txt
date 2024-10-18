macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/setup.hpp
)

add_subdirectory(config)
add_subdirectory(math)
#add_subdirectory(predef)
#add_subdirectory(support)
#add_subdirectory(types)
