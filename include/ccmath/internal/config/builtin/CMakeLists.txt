# TODO(IanP): Trim legacy builtin macro probes now that dispatch avoids most of them.

ccm_add_headers(
        bit_cast_support.hpp
        copysign_support.hpp
        exp2_support.hpp
        fma_support.hpp
        ldexp_support.hpp
        signbit_support.hpp
)
