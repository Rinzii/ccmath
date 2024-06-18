##############################################################################
# Internal Section
##############################################################################

### Config/Arch/Targets headers
##########################################
set(ccmath_internal_config_arch_targets_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/arch/targets/aarch64.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/arch/targets/x86_64.hpp
)


### Config/Arch headers
##########################################
set(ccmath_internal_config_arch_headers
        ${ccmath_internal_config_arch_targets_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/arch/check_arch_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/arch/check_simd_support.hpp
)


### Config/Builtin headers
##########################################
set(ccmath_internal_config_builtin_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/builtin/bit_cast_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/builtin/copysign_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/builtin/exp2_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/builtin/fma_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/builtin/ldexp_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/builtin/signbit_support.hpp
)


### Config/Platform headers
##########################################
set(ccmath_internal_config_platform_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/platform/android.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/platform/darwin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/platform/linux.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/platform/windows.hpp
)



##########################################
## Config headers
##########################################

set(ccmath_internal_config_headers
        ${ccmath_internal_config_arch_headers}
        ${ccmath_internal_config_builtin_headers}
        ${ccmath_internal_config_platform_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/compiler.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/type_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/runtime_detection.hpp
)



### Generic/Functions/Expo headers
##########################################
set(ccmath_internal_generic_functions_expo_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/generic/functions/expo/exp2_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/generic/functions/expo/exp_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/generic/functions/expo/log2_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/generic/functions/expo/log_gen.hpp
)


### Generic/Functions/Power headers
##########################################
set(ccmath_internal_generic_functions_power_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/generic/functions/power/pow_gen_old.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/generic/functions/power/pow_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/generic/functions/power/sqrt_gen.hpp
)


### Generic/Functions headers
##########################################
set(ccmath_internal_generic_functions_headers
        ${ccmath_internal_generic_functions_expo_headers}
        ${ccmath_internal_generic_functions_power_headers}
)



##########################################
## Generic headers
##########################################

set(ccmath_internal_generic_headers
        ${ccmath_internal_generic_functions_headers}
)



##########################################
## Helpers headers
##########################################
# TODO: The helpers folder is likely to be deprecated at some point and moved into the support headers

set(ccmath_internal_helpers_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/digit_to_int.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/fpclassify_helper.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/exp_helpers.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/exp10.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/helpers/internal_ldexp.hpp
)


### Predef/Attributes headers
##########################################
set(ccmath_internal_predef_attributes_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/attributes/always_inline.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/attributes/never_inline.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/attributes/no_debug.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/attributes/optnone.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/attributes/gsl_suppress.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/attributes/gpu_device.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/attributes/gpu_host_device.hpp
)


### Predef/Compiler_Suppression headers
##########################################
set(ccmath_internal_predef_compiler_suppression_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/compiler_suppression/clang_compiler_suppression.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/compiler_suppression/gcc_compiler_suppression.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp
)


### Predef/Versioning headers
##########################################
set(ccmath_internal_predef_versioning_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/versioning/arm_version.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/versioning/clang_version.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/versioning/gcc_version.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/versioning/msvc_version.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/versioning/msvc_version.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/versioning/version_encode.hpp
)



##########################################
## Predef headers
##########################################

set(ccmath_internal_predef_headers
        ${ccmath_internal_predef_attributes_headers}
        ${ccmath_internal_predef_compiler_suppression_headers}
        ${ccmath_internal_predef_versioning_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/expects_bool_condition.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/has_attribute.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/likely.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/has_builtin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/has_const_builtin.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/predef/assume.hpp
)


### Runtime/Functions/Power headers
##########################################
set(ccmath_internal_runtime_functions_power_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/functions/power/sqrt_rt.hpp
)


### Runtime/Functions headers
##########################################
set(ccmath_internal_runtime_functions_headers
        ${ccmath_internal_runtime_functions_power_headers}
)


### Runtime/Simd/Functions/Impl/Avx headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_avx_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/avx/sqrt.hpp
)


### Runtime/Simd/Functions/Impl/Avx2 headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_avx2_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/avx2/sqrt.hpp
)


### Runtime/Simd/Functions/Impl/Avx512 headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_avx512_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/avx512/sqrt.hpp

)


### Runtime/Simd/Functions/Impl/Neon headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_neon_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/neon/sqrt.hpp

)


### Runtime/Simd/Functions/Impl/Scalar headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_scalar_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/scalar/sqrt.hpp

)


### Runtime/Simd/Functions/Impl/Sse2 headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_sse2_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/sse2/sqrt.hpp
)


### Runtime/Simd/Functions/Impl/Sse3 headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_sse3_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/sse3/sqrt.hpp
)


### Runtime/Simd/Functions/Impl/Sse4 headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_sse4_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/sse4/sqrt.hpp
)


### Runtime/Simd/Functions/Impl/Ssse3 headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_ssse3_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/ssse3/sqrt.hpp
)


### Runtime/Simd/Functions/Impl/Vector_Size headers
##########################################
set(ccmath_internal_runtime_simd_functions_impl_vector_size_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/impl/vector_size/sqrt.hpp
)


### Runtime/Simd/Functions headers
##########################################
set(ccmath_internal_runtime_simd_functions_headers
        ${ccmath_internal_runtime_simd_functions_impl_avx_headers}
        ${ccmath_internal_runtime_simd_functions_impl_avx512_headers}
        ${ccmath_internal_runtime_simd_functions_impl_neon_headers}
        ${ccmath_internal_runtime_simd_functions_impl_scalar_headers}
        ${ccmath_internal_runtime_simd_functions_impl_sse2_headers}
        ${ccmath_internal_runtime_simd_functions_impl_sse3_headers}
        ${ccmath_internal_runtime_simd_functions_impl_ssse3_headers}
        ${ccmath_internal_runtime_simd_functions_impl_sse4_headers}
        ${ccmath_internal_runtime_simd_functions_impl_vector_size_headers}

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/functions/sqrt.hpp
)


### Runtime/Simd/Instructions headers
##########################################
set(ccmath_internal_runtime_simd_instructions_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/scalar.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/sse2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/sse3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/ssse3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/sse4.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/avx.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/avx2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/avx512.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/instructions/neon.hpp
)


### Runtime/Simd headers
##########################################
set(ccmath_internal_runtime_simd_headers
        ${ccmath_internal_runtime_simd_functions_headers}
        ${ccmath_internal_runtime_simd_instructions_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/common.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/pack.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/simd_vectorize.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/vector_size.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/runtime/simd/simd.hpp
)



#######################################
# Runtime headers
#######################################

set(ccmath_internal_runtime_headers
        ${ccmath_internal_runtime_functions_headers}
        ${ccmath_internal_runtime_simd_headers}
)


### Support/Fenv headers
##########################################
set(ccmath_internal_support_fenv_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/fenv/fenv_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/fenv/rounding_mode.hpp
)


### Support/FP headers
##########################################
set(ccmath_internal_support_fp_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/fp/bit_mask_traits.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/fp/directional_rounding_utils.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/fp/fp_bits.hpp
)



#######################################
## Support headers
#######################################

set(ccmath_internal_support_headers
        ${ccmath_internal_support_fenv_headers}
        ${ccmath_internal_support_fp_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/always_false.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/bits.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/ctz.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/endian.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/floating_point_traits.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/integer_literals.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/is_constant_evaluated.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/limits.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/math_support.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/meta_compare.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/multiply_add.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/poly_eval.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/type_traits.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/unreachable.hpp
)



#######################################
## Types headers
#######################################

set(ccmath_internal_types_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/fp_types.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/number_pair.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/sign.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/float128.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/int128_types.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/double_double.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/triple_double.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/big_int.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/dyadic_float.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/types/normalized_float.hpp

)



##############################################################################
# Internal headers
##############################################################################

set(ccmath_internal_headers
        ${ccmath_internal_config_headers}
        ${ccmath_internal_generic_headers}
        ${ccmath_internal_helpers_headers}
        ${ccmath_internal_predef_headers}
        ${ccmath_internal_runtime_headers}
        ${ccmath_internal_support_headers}
        ${ccmath_internal_types_headers}

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/setup.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/version.hpp
)




##############################################################################
# Math Section
##############################################################################


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

### Power/Impl headers - TODO: Remove this an instead use the generic power headers
##########################################
set(ccmath_math_power_impl_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/power/impl/pow_impl.hpp
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
set(ccmath_math_root_headers
        # Functions without a specified category
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/gamma.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/lerp.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/math/lgamma.hpp
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
        ${ccmath_math_root_headers}
)




##############################################################################
# Root Section
##############################################################################



#######################################
## CCMath headers (root)
#######################################

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
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/numbers.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/ccmath.hpp
)



#######################################
## CCMath headers (base) - all headers
#######################################

set(ccmath_headers
        ${ccmath_math_headers}
        ${ccmath_internal_headers}
        ${ccmath_root_headers}
)
