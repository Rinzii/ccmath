##########################################
# Internal headers
##########################################

set(ccmath_internal_config_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/compiler.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/builtin_support.hpp
)

set(ccmath_internal_helpers_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/fpclassify_helper.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/exp_helpers.hpp
)

set(ccmath_internal_predef_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/compiler_warnings_and_errors_def.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/compiler_warnings_and_errors_undef.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/has_attribute.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/likely.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/unlikely.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/no_debug.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/suppress.hpp
)

set(ccmath_internal_support_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/bits.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/endian.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/floating_point_traits.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/fp_bits.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/is_constant_evaluated.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/meta_compare.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/unreachable.hpp
)

set(ccmath_internal_types_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/fp_types.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/int128.hpp
)


set(ccmath_internal_headers
        ${ccmath_internal_helpers_headers}
        ${ccmath_internal_predef_headers}
        ${ccmath_internal_config_headers}
        ${ccmath_internal_support_headers}
        ${ccmath_internal_types_headers}

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/setup.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/version.hpp
)


##########################################
# Detail headers
##########################################

set(ccmath_detail_basic_impl_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/impl/remquo_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/impl/remquo_double_impl.hpp
)

set(ccmath_detail_basic_headers
        ${ccmath_detail_basic_impl_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/abs.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/fdim.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/fma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/fmod.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/max.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/min.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/remainder.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/remquo.hpp
)

set(ccmath_detail_compare_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/fpclassify.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/isfinite.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/isgreater.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/isgreaterequal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/isinf.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/isless.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/islessequal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/islessgreater.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/isnan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/isnormal.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/isunordered.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare/signbit.hpp
)

set(ccmath_detail_exponential_impl_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_data.hpp
)

set(ccmath_detail_exponential_headers
        ${ccmath_detail_exponential_impl_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/exp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/exp2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/expm1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/log.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/log1p.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/log2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/log10.hpp
)

set(ccmath_detail_fmanip_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/copysign.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/frexp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/ilogb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/ldexp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/logb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/modf.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/nextafter.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/scalbn.hpp
)

set(ccmath_detail_hyperbolic_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/acosh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/asinh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/atanh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/cosh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/sinh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/tanh.hpp
)

set(ccmath_detail_nearest_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/ceil.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/floor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/nearbyint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/rint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/round.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/trunc.hpp
)

set(ccmath_detail_power_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/cbrt.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/hypot.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/pow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/sqrt.hpp
)

set(ccmath_detail_special_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/assoc_laguerre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/assoc_legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/beta.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/comp_ellint_1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/comp_ellint_2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/comp_ellint_3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/cyl_bessel_i.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/cyl_bessel_j.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/cyl_bessel_k.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/cyl_neumann.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/ellint_1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/ellint_2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/ellint_3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/expint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/hermite.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/laguerre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/riemann_zeta.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/sph_bessel.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/sph_legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special/sph_neumann.hpp
)

set(ccmath_detail_trig_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/acos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/asin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/atan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/atan2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/cos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/sin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/tan.hpp
)


set(ccmath_detail_root_headers
        # Functions without a specified category
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/gamma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/lerp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/lgamma.hpp
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


##########################################
# Root headers
##########################################

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
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/numbers.hpp
)

set(ccmath_headers
        ${ccmath_detail_headers}
        ${ccmath_internal_headers}
        ${ccmath_root_headers}
)
