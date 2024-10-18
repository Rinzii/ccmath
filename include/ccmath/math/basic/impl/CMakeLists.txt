macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/nan_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nan_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nan_ldouble_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/remquo_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/remquo_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/remquo_ldouble_impl.hpp
)

