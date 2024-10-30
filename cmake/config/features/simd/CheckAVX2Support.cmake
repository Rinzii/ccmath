include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m256i avx2_test = _mm256_add_epi32(_mm256_set1_epi32(1), _mm256_set1_epi32(2));
            return 0;
        }
    " CCMATH_SIMD_HAS_AVX2_SUPPORT)

if (CCMATH_SIMD_HAS_AVX2_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX2)
endif ()
