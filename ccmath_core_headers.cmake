




##############################################################################
# Math Section
##############################################################################


### Basic/Impl headers
##########################################
set(ccmath_math_basic_impl_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/impl/nan_float_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/impl/nan_double_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/impl/nan_ldouble_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/impl/remquo_float_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/impl/remquo_double_impl.hpp
)



#######################################
## Basic headers
#######################################

set(ccmath_math_basic_headers
        ${ccmath_math_basic_impl_headers}
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/abs.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/fdim.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/fma.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/fmod.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/max.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/min.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/remainder.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic/remquo.hpp
)



#######################################
## Compare headers
#######################################

set(ccmath_math_compare_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/fpclassify.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/isfinite.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/isgreater.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/isgreaterequal.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/isinf.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/isless.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/islessequal.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/islessgreater.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/isnan.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/isnormal.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/isunordered.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare/signbit.hpp
)


### Exponential/Impl headers
##########################################
set(ccmath_math_exponential_impl_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_float_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_double_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_data.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp2_float_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp2_double_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp2_data.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_float_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_double_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_data.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_float_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_double_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_data.hpp
)



#######################################
## Exponential headers
#######################################

set(ccmath_math_exponential_headers
        ${ccmath_math_exponential_impl_headers}
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/exp.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/exp2.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/expm1.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/log.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/log1p.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/log2.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential/log10.hpp
)


### Fmanip/Impl headers
##########################################
set(ccmath_math_fmanip_impl_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/impl/modf_float_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/impl/modf_double_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/impl/scalbn_float_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/impl/scalbn_double_impl.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/impl/scalbn_ldouble_impl.hpp
)



#######################################
## Fmanip headers
#######################################

set(ccmath_math_fmanip_headers
        ${ccmath_math_fmanip_impl_headers}
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/copysign.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/frexp.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/ilogb.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/ldexp.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/logb.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/modf.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/nextafter.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/nexttoward.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip/scalbn.hpp
)



#######################################
## Hyperbolic headers
#######################################

set(ccmath_math_hyperbolic_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/hyperbolic/acosh.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/hyperbolic/asinh.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/hyperbolic/atanh.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/hyperbolic/cosh.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/hyperbolic/sinh.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/hyperbolic/tanh.hpp
)



#######################################
## Nearest headers
#######################################

set(ccmath_detail_nearest_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/nearest/ceil.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/nearest/floor.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/nearest/nearbyint.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/nearest/rint.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/nearest/round.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/nearest/trunc.hpp
)

### Power/Impl headers - TODO: Remove this an instead use the generic power headers
##########################################
set(ccmath_math_power_impl_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/power/impl/pow_impl.hpp
)



#######################################
## Power headers
#######################################

set(ccmath_math_power_headers
        ${ccmath_math_power_impl_headers}
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/power/cbrt.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/power/hypot.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/power/pow.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/power/sqrt.hpp
)



#######################################
## Special headers
#######################################

set(ccmath_math_special_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/assoc_laguerre.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/assoc_legendre.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/beta.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/comp_ellint_1.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/comp_ellint_2.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/comp_ellint_3.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/cyl_bessel_i.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/cyl_bessel_j.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/cyl_bessel_k.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/cyl_neumann.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/ellint_1.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/ellint_2.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/ellint_3.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/expint.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/hermite.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/laguerre.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/legendre.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/riemann_zeta.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/sph_bessel.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/sph_legendre.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special/sph_neumann.hpp
)



#######################################
## Trig headers
#######################################

set(ccmath_math_trig_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/trig/acos.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/trig/asin.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/trig/atan.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/trig/atan2.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/trig/cos.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/trig/sin.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/trig/tan.hpp
)



#######################################
## Math headers (root)
#######################################
set(ccmath_math_misc_headers
        # func without a specified category
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/misc/gamma.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/misc/lerp.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/misc/lgamma.hpp
)

set(ccmath_math_headers
        ${ccmath_math_basic_headers}
        ${ccmath_math_compare_headers}
        ${ccmath_math_exponential_headers}
        ${ccmath_math_fmanip_headers}
        ${ccmath_math_hyperbolic_headers}
        ${ccmath_math_nearest_headers}
        ${ccmath_math_power_headers}
        ${ccmath_math_trig_headers}
        ${ccmath_math_misc_headers}
)




##############################################################################
# Root Section
##############################################################################



#######################################
## CCMath headers Monolithic Headers
#######################################

set(ccmath_monolithic_headers
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/basic.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/compare.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/exponential.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/fmanip.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/hyperbolic.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/nearest.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/power.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/special.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/trig.hpp
        ${CCMATH_SOURCE_DIR}/include/ccmath/math/numbers.hpp

        ${CCMATH_SOURCE_DIR}/include/ccmath/ccmath.hpp
)



#######################################
## CCMath headers (base) - all headers
#######################################

set(ccmath_core_headers
        ${ccmath_math_headers}
        ${ccmath_monolithic_headers}
)
