/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/internal/config/arch/check_simd_support.hpp"
#include "ccmath/internal/math/runtime/simd/common.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_AVX
		#include <immintrin.h>

namespace ccm::intrin
{
	namespace abi
	{

		struct avx
		{
		};

	} // namespace abi

	template <>
	struct simd_mask<float, abi::avx>
	{

		using value_type					 = bool;
		using simd_type						 = simd<float, abi::avx>;
		using abi_type						 = abi::avx;
		CCM_ALWAYS_INLINE simd_mask() = default;
		CCM_ALWAYS_INLINE simd_mask(bool value) : m_value(_mm256_castsi256_ps(_mm256_set1_epi32(-static_cast<int>(value)))) { } // NOLINT
		static constexpr int size() { return 8; }
		CCM_ALWAYS_INLINE constexpr simd_mask(__m256 const & value_in) : m_value(value_in) {} // NOLINT
		[[nodiscard]] constexpr __m256 get() const { return m_value; }
		CCM_ALWAYS_INLINE simd_mask operator||(simd_mask const & other) const { return {_mm256_or_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd_mask operator&&(simd_mask const & other) const { return {_mm256_and_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd_mask operator!() const { return {_mm256_andnot_ps(m_value, simd_mask(true).get())}; }

	private:
		__m256 m_value;
	};

	CCM_ALWAYS_INLINE bool all_of(simd_mask<float, abi::avx> const & a)
	{
		return _mm256_testc_ps(a.get(), simd_mask<float, abi::avx>(true).get()) != 0;
	}

	CCM_ALWAYS_INLINE bool any_of(simd_mask<float, abi::avx> const & a)
	{
		return _mm256_testc_ps(simd_mask<float, abi::avx>(false).get(), a.get()) == 0;
	}

	template <>
	struct simd<float, abi::avx>
	{

		using value_type				= float;
		using abi_type					= abi::avx;
		using mask_type					= simd_mask<float, abi_type>;
		using storage_type				= simd_storage<float, abi_type>;
		CCM_ALWAYS_INLINE simd() = default;
		static constexpr int size() { return 8; }
		CCM_ALWAYS_INLINE simd(float value) : m_value(_mm256_set1_ps(value)) {} // NOLINT
		CCM_ALWAYS_INLINE simd(float a, float b, float c, float d, float e, float f, float g, float h) : m_value(_mm256_setr_ps(a, b, c, d, e, f, g, h))
		{
		}
		CCM_ALWAYS_INLINE simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); } // NOLINT
		CCM_ALWAYS_INLINE simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE simd(float const * ptr, Flags /*flags*/) : m_value(_mm256_loadu_ps(ptr))
		{
		}
		CCM_ALWAYS_INLINE simd(float const * ptr, int stride)
			: simd(ptr[0], ptr[stride], ptr[2 * stride], ptr[3 * stride], ptr[4 * stride], ptr[5 * stride], ptr[6 * stride], ptr[7 * stride]) // NOLINT
		{
		}
		CCM_ALWAYS_INLINE constexpr simd(__m256 const & value_in) : m_value(value_in) {} // NOLINT
		CCM_ALWAYS_INLINE simd operator*(simd const & other) const { return {_mm256_mul_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator/(simd const & other) const { return {_mm256_div_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator+(simd const & other) const { return {_mm256_add_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator-(simd const & other) const { return {_mm256_sub_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE simd operator-() const { return {_mm256_sub_ps(_mm256_set1_ps(0.0F), m_value)}; }
		CCM_ALWAYS_INLINE void copy_from(float const * ptr, element_aligned_tag /*unused*/) { m_value = _mm256_loadu_ps(ptr); }
		CCM_ALWAYS_INLINE void copy_to(float * ptr, element_aligned_tag /*unused*/) const { _mm256_storeu_ps(ptr, m_value); }
		[[nodiscard]] constexpr __m256 get() const { return m_value; }
		[[nodiscard]] CCM_ALWAYS_INLINE float convert() const { return _mm256_cvtss_f32(m_value); }
		CCM_ALWAYS_INLINE simd_mask<float, abi::avx> operator<(simd const & other) const
		{
			return {_mm256_cmp_ps(m_value, other.m_value, _CMP_LT_OS)};
		}
		CCM_ALWAYS_INLINE simd_mask<float, abi::avx> operator==(simd const & other) const
		{
			return {_mm256_cmp_ps(m_value, other.m_value, _CMP_EQ_OS)};
		}

	private:
		__m256 m_value;
	};

	CCM_ALWAYS_INLINE simd<float, abi::avx> choose(simd_mask<float, abi::avx> const & a, simd<float, abi::avx> const & b,
														  simd<float, abi::avx> const & c)
	{
		return {_mm256_blendv_ps(c.get(), b.get(), a.get())};
	}

	template <>
	struct simd_mask<double, abi::avx>
	{
		using value_type					 = bool;
		using simd_type						 = simd<double, abi::avx>;
		using abi_type						 = abi::avx;
		CCM_ALWAYS_INLINE simd_mask() = default;
		CCM_ALWAYS_INLINE simd_mask(bool value) : m_value(_mm256_castsi256_pd(_mm256_set1_epi64x(-static_cast<std::int64_t>(value)))) { } // NOLINT
		CCM_ALWAYS_INLINE static constexpr int size() { return 4; }
		CCM_ALWAYS_INLINE constexpr simd_mask(__m256d const & value_in) : m_value(value_in) {} // NOLINT
		[[nodiscard]] constexpr __m256d get() const { return m_value; }
		CCM_ALWAYS_INLINE simd_mask operator||(simd_mask const & other) const { return {_mm256_or_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd_mask operator&&(simd_mask const & other) const { return {_mm256_and_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd_mask operator!() const { return {_mm256_andnot_pd(m_value, simd_mask(true).get())}; }

	private:
		__m256d m_value;
	};

	CCM_ALWAYS_INLINE bool all_of(simd_mask<double, abi::avx> const & a)
	{
		return _mm256_testc_pd(a.get(), simd_mask<double, abi::avx>(true).get()) != 0;
	}

	CCM_ALWAYS_INLINE bool any_of(simd_mask<double, abi::avx> const & a)
	{
		return _mm256_testc_pd(simd_mask<double, abi::avx>(false).get(), a.get()) == 0;
	}

	template <>
	struct simd<double, abi::avx> // NOLINT
	{

	public:
		using value_type										= double;
		using abi_type											= abi::avx;
		using mask_type											= simd_mask<double, abi_type>;
		using storage_type										= simd_storage<double, abi_type>;
		CCM_ALWAYS_INLINE simd()							= default;
		CCM_ALWAYS_INLINE simd(simd const &)				= default;
		CCM_ALWAYS_INLINE simd(simd &&)					= default;
		CCM_ALWAYS_INLINE simd & operator=(simd const &) = default;
		CCM_ALWAYS_INLINE simd & operator=(simd &&)		= default;
		CCM_ALWAYS_INLINE static constexpr int size() { return 4; }
		CCM_ALWAYS_INLINE simd(double value) : m_value(_mm256_set1_pd(value)) {} // NOLINT
		CCM_ALWAYS_INLINE simd(double a, double b, double c, double d) : m_value(_mm256_setr_pd(a, b, c, d)) {}
		CCM_ALWAYS_INLINE simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); } // NOLINT
		CCM_ALWAYS_INLINE simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE simd(double const * ptr, Flags flags) : m_value(_mm256_loadu_pd(ptr))
		{
		}
		CCM_ALWAYS_INLINE simd(double const * ptr, int stride) : simd(ptr[0], ptr[stride], ptr[2 * stride], ptr[3 * stride]) {} // NOLINT
		CCM_ALWAYS_INLINE constexpr simd(__m256d const & value_in) : m_value(value_in) {} // NOLINT
		CCM_ALWAYS_INLINE simd operator*(simd const & other) const { return {_mm256_mul_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator/(simd const & other) const { return {_mm256_div_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator+(simd const & other) const { return {_mm256_add_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator-(simd const & other) const { return {_mm256_sub_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE simd operator-() const { return {_mm256_sub_pd(_mm256_set1_pd(0.0), m_value)}; }
		CCM_ALWAYS_INLINE void copy_from(double const * ptr, element_aligned_tag /*unused*/) { m_value = _mm256_loadu_pd(ptr); }
		CCM_ALWAYS_INLINE void copy_to(double * ptr, element_aligned_tag /*unused*/) const { _mm256_storeu_pd(ptr, m_value); }
		[[nodiscard]] CCM_ALWAYS_INLINE constexpr __m256d get() const { return m_value; }
		[[nodiscard]] CCM_ALWAYS_INLINE double convert() const { return _mm256_cvtsd_f64(m_value); }
		CCM_ALWAYS_INLINE simd_mask<double, abi::avx> operator<(simd const & other) const
		{
			return {_mm256_cmp_pd(m_value, other.m_value, _CMP_LT_OS)};
		}
		CCM_ALWAYS_INLINE simd_mask<double, abi::avx> operator==(simd const & other) const
		{
			return {_mm256_cmp_pd(m_value, other.m_value, _CMP_EQ_OS)};
		}

	private:
		__m256d m_value;
	};

	CCM_ALWAYS_INLINE simd<double, abi::avx> choose(simd_mask<double, abi::avx> const & a, simd<double, abi::avx> const & b,
														   simd<double, abi::avx> const & c)
	{
		return {_mm256_blendv_pd(c.get(), b.get(), a.get())};
	}
} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_AVX
#endif	   // CCMATH_HAS_SIMD
