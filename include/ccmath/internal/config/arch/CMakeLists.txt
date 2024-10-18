macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/check_arch_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/check_simd_support.hpp
)

add_subdirectory(targets)
