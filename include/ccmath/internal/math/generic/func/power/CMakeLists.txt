ccm_add_headers(
        cbrt_gen.hpp
        hypot_gen.hpp
        pow_gen.hpp
        sqrt_gen.hpp
)

# TODO: Move this to its own folder at some point
ccm_add_headers(
        pow_impl.hpp
        powf_impl.hpp
)
