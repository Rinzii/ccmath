ccm_add_headers(
        compiler.hpp
        runtime_detection.hpp
        type_support.hpp
)

add_subdirectory(arch)
add_subdirectory(builtin)
add_subdirectory(platform)
