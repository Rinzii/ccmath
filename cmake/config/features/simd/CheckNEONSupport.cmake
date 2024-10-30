include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
        #include <arm_neon.h>
        int main() {
            int32x4_t neon_test = vdupq_n_s32(1);
            return 0;
        }
    " CCMATH_SIMD_HAS_NEON_SUPPORT)

if (CCMATH_SIMD_HAS_NEON_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_NEON)
endif ()
