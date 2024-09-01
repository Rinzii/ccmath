




##############################################################################
# Math Section
##############################################################################

# TODO: Remove all impls from here into the generic headers.

### Basic/Impl headers
##########################################
set(ccmath_math_basic_impl_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/impl/nan_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/impl/nan_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/impl/nan_ldouble_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/impl/remquo_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/impl/remquo_double_impl.hpp
)



#######################################
## Basic headers
#######################################

set(ccmath_math_basic_headers
        ${ccmath_math_basic_impl_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/abs.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/fdim.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/fma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/fmod.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/max.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/min.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/remainder.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic/remquo.hpp
)



#######################################
## Compare headers
#######################################

set(ccmath_math_compare_headers
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


### Exponential/Impl headers
##########################################
set(ccmath_math_exponential_impl_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp2_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp2_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/exp2_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/impl/log2_data.hpp
)



#######################################
## Exponential headers
#######################################

set(ccmath_math_exponential_headers
        ${ccmath_math_exponential_impl_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/exp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/exp2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/expm1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/log.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/log1p.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/log2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential/log10.hpp
)


### Fmanip/Impl headers
##########################################
set(ccmath_math_fmanip_impl_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/impl/modf_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/impl/modf_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/impl/scalbn_float_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/impl/scalbn_double_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/impl/scalbn_ldouble_impl.hpp
)



#######################################
## Fmanip headers
#######################################

set(ccmath_math_fmanip_headers
        ${ccmath_math_fmanip_impl_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/copysign.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/frexp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/ilogb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/ldexp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/logb.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/modf.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/nextafter.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/nexttoward.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip/scalbn.hpp
)



#######################################
## Hyperbolic headers
#######################################

set(ccmath_math_hyperbolic_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/acosh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/asinh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/atanh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/cosh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/sinh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic/tanh.hpp
)



#######################################
## Nearest headers
#######################################

set(ccmath_detail_nearest_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/ceil.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/floor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/nearbyint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/rint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/round.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest/trunc.hpp
)


#######################################
## Power headers
#######################################

set(ccmath_math_power_headers
        ${ccmath_math_power_impl_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/cbrt.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/hypot.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/pow.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/sqrt.hpp
)



#######################################
## Special headers
#######################################

set(ccmath_math_special_headers
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



#######################################
## Trig headers
#######################################

set(ccmath_math_trig_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/acos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/asin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/atan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/atan2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/cos.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/sin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig/tan.hpp
)



#######################################
## Math headers (root)
#######################################
set(ccmath_math_misc_headers
        # func without a specified category
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/misc/gamma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/misc/lerp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/misc/lgamma.hpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/basic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/compare.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/exponential.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/fmanip.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/hyperbolic.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/nearest.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/special.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/trig.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/numbers.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/ccmath.hpp
)



#######################################
## CCMath headers (base) - all headers
#######################################

set(ccmath_core_headers
        ${ccmath_math_headers}
        ${ccmath_monolithic_headers}
)
