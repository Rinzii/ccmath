ccm_add_headers(
        always_false.hpp
        bits.hpp
        ctz.hpp
        endian.hpp
        floating_point_traits.hpp
        integer_literals.hpp
        is_constant_evaluated.hpp
        limits.hpp
        math_support.hpp
        meta_compare.hpp
        multiply_add.hpp
        poly_eval.hpp
        type_traits.hpp
        unreachable.hpp
)

add_subdirectory(fenv)
add_subdirectory(fp)
add_subdirectory(helpers)
