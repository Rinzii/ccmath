macro(ccm_add_headers headers)
  target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${headers}>")
endmacro()

ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/fpclassify.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/isfinite.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/isgreater.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/isgreaterequal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/isinf.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/isless.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/islessequal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/islessgreater.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/isnan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/isnormal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/isunordered.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/signbit.hpp
)
