macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/cbrt_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/hypot_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/pow_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sqrt_gen.hpp
)

# TODO: Move this to its own folder at some point
ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/pow_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/powf_impl.hpp
)
