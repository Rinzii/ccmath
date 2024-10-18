macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/android.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/darwin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/linux.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/windows.hpp
)
