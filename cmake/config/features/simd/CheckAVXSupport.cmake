include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m256d avx_test = _mm256_set1_pd(1.0);
            return 0;
        }
    " CCMATH_SIMD_HAS_AVX_SUPPORT)

if (CCMATH_SIMD_HAS_AVX_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX)
endif ()
