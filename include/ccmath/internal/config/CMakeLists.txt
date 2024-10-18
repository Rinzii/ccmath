ccm_add_headers(
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime_detection.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/type_support.hpp
)

add_subdirectory(arch)
add_subdirectory(builtin)
add_subdirectory(platform)
