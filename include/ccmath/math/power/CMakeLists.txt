macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/cbrt.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/hypot.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/pow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sqrt.hpp
)
