include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m128d sse2_test = _mm_set1_pd(1.0);
            return 0;
        }
    " CCMATH_SIMD_HAS_SSE2_SUPPORT)

if (CCMATH_SIMD_HAS_SSE2_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_SSE2)
endif ()
