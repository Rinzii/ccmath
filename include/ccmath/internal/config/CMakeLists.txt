ccm_add_headers(
        compiler.hpp
        freestanding.hpp
        type_support.hpp
)

add_subdirectory(arch)
add_subdirectory(builtin)
add_subdirectory(platform)
