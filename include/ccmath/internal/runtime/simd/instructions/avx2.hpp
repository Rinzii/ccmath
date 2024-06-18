/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

#include "ccmath/internal/config/arch/check_simd_support.hpp"
#include "ccmath/internal/runtime/simd/common.hpp"

#ifdef CCMATH_HAS_SIMD
   #ifdef CCMATH_HAS_SIMD_AVX2
	   #include <immintrin.h>

namespace ccm::intrin
{
   namespace abi
   {

	   struct avx2
	   {
	   };

   } // namespace abi

   template <>
   struct simd_mask<float, abi::avx2>
   {

	   using value_type					 = bool;
	   using simd_type						 = simd<float, abi::avx2>;
	   using abi_type						 = abi::avx2;
	   CCM_ALWAYS_INLINE inline simd_mask() = default;
	   CCM_ALWAYS_INLINE inline simd_mask(bool value) : m_value(_mm256_castsi256_ps(_mm256_set1_epi32(-static_cast<int>(value)))) { }
	   static constexpr int size() { return 8; }
	   CCM_ALWAYS_INLINE inline constexpr simd_mask(__m256 const & value_in) : m_value(value_in) {}
	   [[nodiscard]] constexpr __m256 get() const { return m_value; }
	   CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const { return {_mm256_or_ps(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const { return {_mm256_and_ps(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd_mask operator!() const { return {_mm256_andnot_ps(m_value, simd_mask(true).get())}; }

   private:
	   __m256 m_value;
   };

   CCM_ALWAYS_INLINE inline bool all_of(simd_mask<float, abi::avx2> const & a)
   {
	   return _mm256_testc_ps(a.get(), simd_mask<float, abi::avx2>(true).get()) != 0;
   }

   CCM_ALWAYS_INLINE inline bool any_of(simd_mask<float, abi::avx2> const & a)
   {
	   return _mm256_testc_ps(simd_mask<float, abi::avx2>(false).get(), a.get()) == 0;
   }

   template <>
   struct simd<float, abi::avx2>
   {

	   using value_type				= float;
	   using abi_type					= abi::avx2;
	   using mask_type					= simd_mask<float, abi_type>;
	   using storage_type				= simd_storage<float, abi_type>;
	   CCM_ALWAYS_INLINE inline simd() = default;
	   static constexpr int size() { return 8; }
	   CCM_ALWAYS_INLINE inline simd(float value) : m_value(_mm256_set1_ps(value)) {}
	   CCM_ALWAYS_INLINE inline simd(float a, float b, float c, float d, float e, float f, float g, float h) : m_value(_mm256_setr_ps(a, b, c, d, e, f, g, h))
	   {
	   }
	   CCM_ALWAYS_INLINE inline simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
	   CCM_ALWAYS_INLINE inline simd & operator=(storage_type const & value)
	   {
		   copy_from(value.data(), element_aligned_tag());
		   return *this;
	   }
	   template <class Flags>
	   CCM_ALWAYS_INLINE inline simd(float const * ptr, Flags /*flags*/) : m_value(_mm256_loadu_ps(ptr))
	   {
	   }
	   CCM_ALWAYS_INLINE inline simd(float const * ptr, int stride)
		   : simd(ptr[0], ptr[stride], ptr[2 * stride], ptr[3 * stride], ptr[4 * stride], ptr[5 * stride], ptr[6 * stride], ptr[7 * stride])
	   {
	   }
	   CCM_ALWAYS_INLINE inline constexpr simd(__m256 const & value_in) : m_value(value_in) {}
	   CCM_ALWAYS_INLINE inline simd operator*(simd const & other) const { return {_mm256_mul_ps(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd operator/(simd const & other) const { return {_mm256_div_ps(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd operator+(simd const & other) const { return {_mm256_add_ps(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd operator-(simd const & other) const { return {_mm256_sub_ps(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator-() const { return {_mm256_sub_ps(_mm256_set1_ps(0.0F), m_value)}; }
	   CCM_ALWAYS_INLINE inline void copy_from(float const * ptr, element_aligned_tag /*unused*/) { m_value = _mm256_loadu_ps(ptr); }
	   CCM_ALWAYS_INLINE inline void copy_to(float * ptr, element_aligned_tag /*unused*/) const { _mm256_storeu_ps(ptr, m_value); }
	   [[nodiscard]] constexpr __m256 get() const { return m_value; }
	   [[nodiscard]] CCM_ALWAYS_INLINE inline float convert() const { return _mm256_cvtss_f32(m_value); }
	   CCM_ALWAYS_INLINE inline simd_mask<float, abi::avx2> operator<(simd const & other) const
	   {
		   return {_mm256_cmp_ps(m_value, other.m_value, _CMP_LT_OS)};
	   }
	   CCM_ALWAYS_INLINE inline simd_mask<float, abi::avx2> operator==(simd const & other) const
	   {
		   return {_mm256_cmp_ps(m_value, other.m_value, _CMP_EQ_OS)};
	   }

   private:
	   __m256 m_value;
   };

   CCM_ALWAYS_INLINE inline simd<float, abi::avx2> choose(simd_mask<float, abi::avx2> const & a, simd<float, abi::avx2> const & b,
														 simd<float, abi::avx2> const & c)
   {
	   return {_mm256_blendv_ps(c.get(), b.get(), a.get())};
   }

   template <>
   struct simd_mask<double, abi::avx2>
   {
	   using value_type					 = bool;
	   using simd_type						 = simd<double, abi::avx2>;
	   using abi_type						 = abi::avx2;
	   CCM_ALWAYS_INLINE inline simd_mask() = default;
	   CCM_ALWAYS_INLINE inline simd_mask(bool value) : m_value(_mm256_castsi256_pd(_mm256_set1_epi64x(-static_cast<std::int64_t>(value)))) { }
	   CCM_ALWAYS_INLINE inline static constexpr int size() { return 4; }
	   CCM_ALWAYS_INLINE inline constexpr simd_mask(__m256d const & value_in) : m_value(value_in) {}
	   [[nodiscard]] constexpr __m256d get() const { return m_value; }
	   CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const { return {_mm256_or_pd(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const { return {_mm256_and_pd(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd_mask operator!() const { return {_mm256_andnot_pd(m_value, simd_mask(true).get())}; }

   private:
	   __m256d m_value;
   };

   CCM_ALWAYS_INLINE inline bool all_of(simd_mask<double, abi::avx2> const & a)
   {
	   return _mm256_testc_pd(a.get(), simd_mask<double, abi::avx2>(true).get()) != 0;
   }

   CCM_ALWAYS_INLINE inline bool any_of(simd_mask<double, abi::avx2> const & a)
   {
	   return _mm256_testc_pd(simd_mask<double, abi::avx2>(false).get(), a.get()) == 0;
   }

   template <>
   struct simd<double, abi::avx2>
   {

   public:
	   using value_type										= double;
	   using abi_type											= abi::avx2;
	   using mask_type											= simd_mask<double, abi_type>;
	   using storage_type										= simd_storage<double, abi_type>;
	   CCM_ALWAYS_INLINE inline simd()							= default;
	   CCM_ALWAYS_INLINE inline simd(simd const &)				= default;
	   CCM_ALWAYS_INLINE inline simd(simd &&)					= default;
	   CCM_ALWAYS_INLINE inline simd & operator=(simd const &) = default;
	   CCM_ALWAYS_INLINE inline simd & operator=(simd &&)		= default;
	   CCM_ALWAYS_INLINE inline static constexpr int size() { return 4; }
	   CCM_ALWAYS_INLINE inline simd(double value) : m_value(_mm256_set1_pd(value)) {}
	   CCM_ALWAYS_INLINE inline simd(double a, double b, double c, double d) : m_value(_mm256_setr_pd(a, b, c, d)) {}
	   CCM_ALWAYS_INLINE inline simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
	   CCM_ALWAYS_INLINE inline simd & operator=(storage_type const & value)
	   {
		   copy_from(value.data(), element_aligned_tag());
		   return *this;
	   }
	   template <class Flags>
	   CCM_ALWAYS_INLINE inline simd(double const * ptr, Flags flags) : m_value(_mm256_loadu_pd(ptr))
	   {
	   }
	   CCM_ALWAYS_INLINE inline simd(double const * ptr, int stride) : simd(ptr[0], ptr[stride], ptr[2 * stride], ptr[3 * stride]) {}
	   CCM_ALWAYS_INLINE inline constexpr simd(__m256d const & value_in) : m_value(value_in) {}
	   CCM_ALWAYS_INLINE inline simd operator*(simd const & other) const { return {_mm256_mul_pd(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd operator/(simd const & other) const { return {_mm256_div_pd(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd operator+(simd const & other) const { return {_mm256_add_pd(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE inline simd operator-(simd const & other) const { return {_mm256_sub_pd(m_value, other.m_value)}; }
	   CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator-() const { return {_mm256_sub_pd(_mm256_set1_pd(0.0), m_value)}; }
	   CCM_ALWAYS_INLINE inline void copy_from(double const * ptr, element_aligned_tag /*unused*/) { m_value = _mm256_loadu_pd(ptr); }
	   CCM_ALWAYS_INLINE inline void copy_to(double * ptr, element_aligned_tag /*unused*/) const { _mm256_storeu_pd(ptr, m_value); }
	   [[nodiscard]] CCM_ALWAYS_INLINE inline constexpr __m256d get() const { return m_value; }
	   [[nodiscard]] CCM_ALWAYS_INLINE inline double convert() const { return _mm256_cvtsd_f64(m_value); }
	   CCM_ALWAYS_INLINE inline simd_mask<double, abi::avx2> operator<(simd const & other) const
	   {
		   return {_mm256_cmp_pd(m_value, other.m_value, _CMP_LT_OS)};
	   }
	   CCM_ALWAYS_INLINE inline simd_mask<double, abi::avx2> operator==(simd const & other) const
	   {
		   return {_mm256_cmp_pd(m_value, other.m_value, _CMP_EQ_OS)};
	   }

   private:
	   __m256d m_value;
   };

   CCM_ALWAYS_INLINE inline simd<double, abi::avx2> choose(simd_mask<double, abi::avx2> const & a, simd<double, abi::avx2> const & b,
														  simd<double, abi::avx2> const & c)
   {
	   return {_mm256_blendv_pd(c.get(), b.get(), a.get())};
   }
} // namespace ccm::intrin

   #endif // CCMATH_HAS_SIMD_AVX
#endif	   // CCM_CONFIG_USE_RT_SIMD