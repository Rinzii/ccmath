macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/ceil_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/floor_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nearbyint_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rint_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/round_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/trunc_gen.hpp
)
