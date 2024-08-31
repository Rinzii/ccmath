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
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/runtime_detection.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/config/type_support.hpp
)


#### Math/Generic Modules ####

### Math/Generic/Func/Basic headers
##########################################
set(ccmath_internal_math_generic_func_basic_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/basic/abs_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/basic/fdim_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/basic/fma_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/basic/fmod_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/basic/max_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/basic/min_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/basic/remainder_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/basic/remquo_gen.hpp
)


### Math/Generic/Func/Expo headers
##########################################
set(ccmath_internal_math_generic_func_expo_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/expo/expm1_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/expo/log1p_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/expo/log10_gen.hpp
)


### Math/Generic/Func/Fmanip headers
##########################################
set(ccmath_internal_math_generic_func_fmanip_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/fmanip/copysign_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/fmanip/frexp_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/fmanip/ilogb_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/fmanip/ldexp_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/fmanip/logb_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/fmanip/modf_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/fmanip/nextafter_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/fmanip/scalbn_gen.hpp
)


### Math/Generic/Func/Hyperbolic headers
##########################################
set(ccmath_internal_math_generic_func_hyperbolic_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/hyperbolic/acosh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/hyperbolic/asinh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/hyperbolic/atanh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/hyperbolic/cosh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/hyperbolic/sinh_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/hyperbolic/tanh_gen.hpp
)


### Math/Generic/Func/Misc headers
##########################################
set(ccmath_internal_math_generic_func_misc_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/misc/lerp_gen.hpp
)


### Math/Generic/Func/Nearest headers
##########################################
set(ccmath_internal_math_generic_func_nearest_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/ceil_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/floor_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/nearbyint_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/rint_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/round_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/trunc_gen.hpp
)


### Math/Generic/Func/Power headers
##########################################
set(ccmath_internal_math_generic_func_power_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/cbrt_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/hypot_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/pow_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/nearest/sqrt_gen.hpp
)


### Math/Generic/Func/Special headers
##########################################
set(ccmath_internal_math_generic_func_special_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/assoc_laguerre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/assoc_legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/beta.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/comp_ellint_1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/comp_ellint_2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/comp_ellint_3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/cyl_bessel_i.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/cyl_bessel_j.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/cyl_bessel_k.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/cyl_neumann.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/ellint_1.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/ellint_2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/ellint_3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/expint.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/hermite.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/laguerre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/riemann_zeta.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/sph_bessel.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/sph_legendre.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/special/sph_neumann.hpp
)


### Math/Generic/Func/Trig headers
##########################################
set(ccmath_internal_math_generic_func_trig_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/trig/acos_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/trig/asin_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/trig/atan2_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/trig/cos_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/trig/sin_gen.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/generic/func/trig/tan_gen.hpp
)


### Math/Generic/Func headers
##########################################
set(ccmath_internal_math_generic_func_headers
        ${ccmath_internal_math_generic_func_basic_headers}
        ${ccmath_internal_math_generic_func_expo_headers}
        ${ccmath_internal_math_generic_func_fmanip_headers}
        ${ccmath_internal_math_generic_func_hyperbolic_headers}
        ${ccmath_internal_math_generic_func_misc_headers}
        ${ccmath_internal_math_generic_func_nearest_headers}
        ${ccmath_internal_math_generic_func_power_headers}
        ${ccmath_internal_math_generic_func_special_headers}
        ${ccmath_internal_math_generic_func_trig_headers}
)


### Math/Generic headers
##########################################
set(ccmath_internal_math_generic_headers
        ${ccmath_internal_math_generic_func_headers}
)



### Math/Runtime/func headers
##########################################
set(ccmath_internal_math_runtime_func_headers
        ${ccmath_internal_math_runtime_func_power_headers}
)


### Math/Runtime headers
##########################################
set(ccmath_internal_math_runtime_headers
        ${ccmath_internal_math_runtime_func_headers}
)


### math/runtime/Simd/Func/Impl/Avx headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_avx_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/avx/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Avx2 headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_avx2_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/avx2/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Avx512 headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_avx512_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/avx512/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Neon headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_neon_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/neon/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Scalar headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_scalar_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/scalar/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Sse2 headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_sse2_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/sse2/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Sse3 headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_sse3_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/sse3/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Sse4 headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_sse4_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/sse4/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Ssse3 headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_ssse3_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/ssse3/sqrt.hpp
)


### math/runtime/Simd/Func/Impl/Vector_Size headers
##########################################
set(ccmath_internal_math_runtime_simd_func_impl_vector_size_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/impl/vector_size/sqrt.hpp
)


### math/runtime/Simd/Func headers
##########################################
set(ccmath_internal_math_runtime_simd_func_headers
        ${ccmath_internal_math_runtime_simd_func_impl_avx_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_avx2_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_avx512_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_neon_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_scalar_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_sse2_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_sse3_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_ssse3_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_sse4_headers}
        ${ccmath_internal_math_runtime_simd_func_impl_vector_size_headers}

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/func/sqrt.hpp
)


### math/runtime/Simd/Instructions headers
##########################################
set(ccmath_internal_math_runtime_simd_instructions_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/scalar.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/sse2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/sse3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/ssse3.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/sse4.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/avx.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/avx2.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/avx512.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/instructions/neon.hpp
)


### math/runtime/Simd headers
##########################################
set(ccmath_internal_math_runtime_simd_headers
        ${ccmath_internal_math_runtime_simd_func_headers}
        ${ccmath_internal_math_runtime_simd_instructions_headers}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/common.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/pack.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/simd_vectorize.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/vector_size.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/math/runtime/simd/simd.hpp
)


set(ccmath_internal_math_runtime_headers
        ${ccmath_internal_math_runtime_func_headers}
        ${ccmath_internal_math_runtime_simd_headers}
)


##########################################
## Math headers
##########################################

set(ccmath_internal_math_headers
        ${ccmath_internal_math_generic_headers}
        ${ccmath_internal_math_runtime_headers}
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

## Support/Helpers headers
##########################################

set(ccmath_internal_support_helpers_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/helpers/digit_to_int.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/helpers/fpclassify_helper.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/helpers/exp_helpers.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/helpers/exp10.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/support/helpers/internal_ldexp.hpp
)



#######################################
## Support headers
#######################################

set(ccmath_internal_support_headers
        ${ccmath_internal_support_fenv_headers}
        ${ccmath_internal_support_fp_headers}
        ${ccmath_internal_support_helpers_headers}
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
        ${ccmath_internal_predef_headers}
        ${ccmath_internal_math_runtime_headers}
        ${ccmath_internal_support_headers}
        ${ccmath_internal_types_headers}

        ${CMAKE_CURRENT_SOURCE_DIR}/include/ccmath/internal/setup.hpp
)