include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
        #include <pmmintrin.h>
        int main() {
            __m128d sse3_test = _mm_addsub_pd(_mm_set1_pd(1.0), _mm_set1_pd(2.0));
            return 0;
        }
    " CCMATH_SIMD_HAS_SSE3_SUPPORT)

if (CCMATH_SIMD_HAS_SSE3_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_SSE3)
endif ()
