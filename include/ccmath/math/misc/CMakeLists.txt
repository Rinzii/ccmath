macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/gamma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lgamma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lerp.hpp
)
