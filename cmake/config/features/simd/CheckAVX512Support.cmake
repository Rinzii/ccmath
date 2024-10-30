include(CheckCXXSourceCompiles)

# Check for AVX-512-F (Foundation)
check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m512d avx512f_test = _mm512_set1_pd(1.0);
            return 0;
        }
    " CCMATH_SIMD_HAS_AVX512F_SUPPORT)

if (CCMATH_SIMD_HAS_AVX512F_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX512F)
endif ()

# Check for AVX-512-DQ (Double and Quadword)
check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m512i avx512dq_test = _mm512_mul_epu32(_mm512_set1_epi64(1), _mm512_set1_epi64(2));
            return 0;
        }
    " CCMATH_SIMD_HAS_AVX512DQ_SUPPORT)

if (CCMATH_SIMD_HAS_AVX512DQ_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX512DQ)
endif ()

# Check for AVX-512-IFMA (Integer Fused Multiply-Add)
check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m512i avx512ifma_test = _mm512_madd52hi_epu64(_mm512_set1_epi64(1), _mm512_set1_epi64(2), _mm512_set1_epi64(3));
            return 0;
        }
    " CCMATH_SIMD_HAS_AVX512IFMA_SUPPORT)

if (CCMATH_SIMD_HAS_AVX512IFMA_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX512IFMA)
endif ()

# Check for AVX-512-CD (Conflict Detection)
check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m512i avx512cd_test = _mm512_conflict_epi64(_mm512_set1_epi64(1));
            return 0;
        }
    " CCMATH_SIMD_HAS_AVX512CD_SUPPORT)

if (CCMATH_SIMD_HAS_AVX512CD_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX512CD)
endif ()

# Check for AVX-512-BW (Byte and Word)
check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m512i avx512bw_test = _mm512_cvtepi16_epi8(_mm512_set1_epi16(1));
            return 0;
        }
    " CCMATH_SIMD_HAS_AVX512BW_SUPPORT)

if (CCMATH_SIMD_HAS_AVX512BW_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX512BW)
endif ()

# Check for AVX-512-VL (Vector Length Extensions)
check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m256i avx512vl_test = _mm256_abs_epi16(_mm256_set1_epi16(-1));
            return 0;
        }
    " CCMATH_SIMD_HAS_AVX512VL_SUPPORT)

if (CCMATH_SIMD_HAS_AVX512VL_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX512VL)
endif ()


# Check if we have full AVX-512 support
if (CCMATH_SIMD_HAS_AVX512F_SUPPORT AND
        CCMATH_SIMD_HAS_AVX512DQ_SUPPORT AND
        CCMATH_SIMD_HAS_AVX512IFMA_SUPPORT AND
        CCMATH_SIMD_HAS_AVX512CD_SUPPORT AND
        CCMATH_SIMD_HAS_AVX512BW_SUPPORT AND
        CCMATH_SIMD_HAS_AVX512VL_SUPPORT)
  add_compile_definitions(CCM_CONFIG_RT_SIMD_HAS_AVX512)
endif ()
