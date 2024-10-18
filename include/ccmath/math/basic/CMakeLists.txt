macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/abs.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fdim.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fmod.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/max.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/min.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/remainder.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/remquo.hpp
)

add_subdirectory(impl)
