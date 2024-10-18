macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/acosh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/asinh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/atanh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cosh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sinh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tanh.hpp
)
