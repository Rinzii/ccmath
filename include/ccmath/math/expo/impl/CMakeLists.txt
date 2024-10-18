macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/exp2_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exp2_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exp2_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exp_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exp_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exp_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log2_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log2_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log2_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log_float_impl.hpp
)
