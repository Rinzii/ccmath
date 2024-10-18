macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/acosh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/asinh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/atanh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cosh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sinh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tanh_gen.hpp
)
