set(ccmath_internal_helpers_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/endian.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/narrow_cast.hpp
)

set(ccmath_internal_typetraits_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/type_traits/is_constant_evaluated.hpp
)

set(ccmath_internal_utility_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/utility/unreachable.hpp
)


set(ccmath_internal_headers
        ${ccmath_internal_helpers_headers}
        ${ccmath_internal_typetraits_headers}
        ${ccmath_internal_utility_headers}

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/setup.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/version.hpp
)

set(ccmath_detail_basic_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/basic/abs.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/basic/fdim.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/basic/fma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/basic/fmod.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/basic/max.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/basic/min.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/basic/remainder.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/basic/remquo.hpp
)

set(ccmath_detail_compare_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/fpclassify.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/isfinite.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/isgreater.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/isgreaterequal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/isinf.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/isless.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/islessequal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/islessgreater.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/isnan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/isnormal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/isunordered.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/compare/signbit.hpp
)

set(ccmath_detail_exponential_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/exponential/exp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/exponential/exp2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/exponential/expm1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/exponential/log.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/exponential/log1p.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/exponential/log2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/exponential/log10.hpp
)

set(ccmath_detail_fmanip_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/fmanip/copysign.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/fmanip/frexp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/fmanip/ilogb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/fmanip/ldexp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/fmanip/logb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/fmanip/modf.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/fmanip/nextafter.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/fmanip/scalbn.hpp
)

set(ccmath_detail_hyperbolic_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/hyperbolic/acosh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/hyperbolic/asinh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/hyperbolic/atanh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/hyperbolic/cosh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/hyperbolic/sinh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/hyperbolic/tanh.hpp
)

set(ccmath_detail_nearest_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/nearest/ceil.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/nearest/floor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/nearest/nearbyint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/nearest/rint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/nearest/round.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/nearest/trunc.hpp
)

set(ccmath_detail_power_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/power/cbrt.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/power/hypot.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/power/pow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/power/sqrt.hpp
)

set(ccmath_detail_special_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/assoc_laguerre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/assoc_legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/beta.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/comp_ellint_1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/comp_ellint_2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/comp_ellint_3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/cyl_bessel_i.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/cyl_bessel_j.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/cyl_bessel_k.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/cyl_neumann.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/ellint_1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/ellint_2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/ellint_3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/expint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/hermite.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/laguerre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/riemann_zeta.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/sph_bessel.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/sph_legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/special/sph_neumann.hpp
)

set(ccmath_detail_trig_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/trig/acos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/trig/asin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/trig/atan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/trig/atan2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/trig/cos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/trig/sin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/trig/tan.hpp
)


set(ccmath_detail_root_headers
        # Functions without a specified category
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/gamma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/lerp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/detail/lgamma.hpp
)

set(ccmath_detail_headers
        ${ccmath_detail_basic_headers}
        ${ccmath_detail_compare_headers}
        ${ccmath_detail_exponential_headers}
        ${ccmath_detail_fmanip_headers}
        ${ccmath_detail_hyperbolic_headers}
        ${ccmath_detail_nearest_headers}
        ${ccmath_detail_power_headers}
        ${ccmath_detail_trig_headers}
        ${ccmath_detail_root_headers}
)

set(ccmath_root_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/basic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/compare.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/exponential.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/fmanip.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/hyperbolic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/nearest.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/power.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/special.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/trig.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/ccmath.hpp
)

set(ccmath_headers
        ${ccmath_detail_headers}
        ${ccmath_internal_headers}
        ${ccmath_root_headers}
)
