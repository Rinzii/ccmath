macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/acos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/asin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/atan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/atan2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tan.hpp
)
