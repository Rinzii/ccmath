ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/exp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/exp2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/expm1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log1p.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/log10.hpp
)

add_subdirectory(impl)
