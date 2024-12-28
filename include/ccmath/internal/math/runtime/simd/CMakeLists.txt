ccm_add_headers(
        common.hpp
        pack.hpp
        simd.hpp
        simd_vectorize.hpp
        vector_size.hpp
)

add_subdirectory(func)
add_subdirectory(instructions)
