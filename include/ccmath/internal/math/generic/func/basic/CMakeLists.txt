macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/abs_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fdim_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fma_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fmod_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/max_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/min_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/remainder_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/remquo_gen.hpp
)
