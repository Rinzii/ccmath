include(CheckCXXSourceCompiles)

# Check for SSE4.1 support
check_cxx_source_compiles("
        #include <smmintrin.h>
        int main() {
            __m128i sse4_1_test = _mm_min_epi32(_mm_set1_epi32(1), _mm_set1_epi32(2));
            return 0;
        }
    " CCMATH_SIMD_HAS_SSE4_1_SUPPORT)

# Check for SSE4.2 support
check_cxx_source_compiles("
        #include <nmmintrin.h>
        int main() {
            __m128i sse4_2_test = _mm_cmpestra(_mm_set1_epi32(1), 1, _mm_set1_epi32(2), 1, 0);
            return 0;
        }
    " CCMATH_SIMD_HAS_SSE4_2_SUPPORT)

if (CCMATH_SIMD_HAS_SSE4_1_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_SSE4_1)
endif ()

if (CCMATH_SIMD_HAS_SSE4_2_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_SSE4_2)
endif ()

if (CCMATH_SIMD_HAS_SSE4_1_SUPPORT AND CCMATH_SIMD_HAS_SSE4_2_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_SSE4)
endif ()
