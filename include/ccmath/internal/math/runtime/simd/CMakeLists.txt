ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/common.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/pack.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simd.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simd_vectorize.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/vector_size.hpp
)

add_subdirectory(func)
add_subdirectory(instructions)
