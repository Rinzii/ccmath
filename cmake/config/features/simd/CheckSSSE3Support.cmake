include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
        #include <tmmintrin.h>
        int main() {
            __m128i ssse3_test = _mm_abs_epi8(_mm_set1_epi8(-1));
            return 0;
        }
    " CCMATH_SIMD_HAS_SSSE3_SUPPORT)

if (CCMATH_SIMD_HAS_SSSE3_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_SSSE3)
endif ()
