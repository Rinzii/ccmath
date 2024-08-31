/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/arch/check_simd_support.hpp"
#include "ccmath/internal/math/runtime/simd/common.hpp"

#ifdef CCMATH_HAS_SIMD
	#ifdef CCMATH_HAS_SIMD_SSSE3
		#include <tmmintrin.h> // SSSE3
	#endif

	#if defined(__FMA__) || defined(CCMATH_HAS_SIMD_AVX2)
		#include <immintrin.h>
	#endif

	#ifdef CCMATH_HAS_SIMD_SSSE3

namespace ccm::intrin
{

	namespace abi
	{
		struct ssse3
		{
		};

	} // namespace abi

	template <>
	struct simd_mask<float, abi::ssse3>
	{
		using value_type					 = bool;
		using simd_type						 = simd<float, abi::ssse3>;
		using abi_type						 = abi::ssse3;
		CCM_ALWAYS_INLINE simd_mask() = default;
		CCM_ALWAYS_INLINE explicit simd_mask(bool value) : m_value(_mm_castsi128_ps(_mm_set1_epi32(-static_cast<int>(value)))) {}
		static constexpr int size() { return 4; }
		constexpr explicit simd_mask(__m128 const & value_in) : m_value(value_in) {}
		[[nodiscard]] CCM_ALWAYS_INLINE constexpr __m128 get() const { return m_value; }
		CCM_ALWAYS_INLINE simd_mask operator||(simd_mask const & other) const { return simd_mask(_mm_or_ps(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE simd_mask operator&&(simd_mask const & other) const { return simd_mask(_mm_and_ps(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE simd_mask operator!() const { return simd_mask(_mm_andnot_ps(m_value, simd_mask(true).get())); }

	private:
		__m128 m_value;
	};

	CCM_ALWAYS_INLINE bool all_of(simd_mask<float, abi::ssse3> const & a)
	{
		return _mm_movemask_ps(a.get()) == 0xF;
	}

	CCM_ALWAYS_INLINE bool any_of(simd_mask<float, abi::ssse3> const & a)
	{
		return _mm_movemask_ps(a.get()) != 0x0;
	}

	template <>
	struct simd<float, abi::ssse3>
	{
		using value_type				= float;
		using abi_type					= abi::ssse3;
		using mask_type					= simd_mask<float, abi_type>;
		using storage_type				= simd_storage<float, abi_type>;
		CCM_ALWAYS_INLINE simd() = default;
		static constexpr int size() { return 4; }
		CCM_ALWAYS_INLINE simd(float value) : m_value(_mm_set1_ps(value)) {}
		CCM_ALWAYS_INLINE simd(float a, float b, float c, float d) : m_value(_mm_setr_ps(a, b, c, d)) {}
		CCM_ALWAYS_INLINE simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE simd(float const * ptr, Flags /*flags*/) : m_value(_mm_loadu_ps(ptr))
		{
		}
		CCM_ALWAYS_INLINE simd(float const * ptr, int stride) : simd(ptr[0], ptr[stride], ptr[2 * stride], ptr[3 * stride]) {}
		CCM_ALWAYS_INLINE constexpr simd(__m128 const & value_in) : m_value(value_in) {}
		CCM_ALWAYS_INLINE simd operator*(simd const & other) const { return {_mm_mul_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator/(simd const & other) const { return {_mm_div_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator+(simd const & other) const { return {_mm_add_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator-(simd const & other) const { return {_mm_sub_ps(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator-() const { return {_mm_sub_ps(_mm_set1_ps(0.0F), m_value)}; }
		CCM_ALWAYS_INLINE void copy_from(float const * ptr, element_aligned_tag /*unused*/) { m_value = _mm_loadu_ps(ptr); }
		CCM_ALWAYS_INLINE void copy_to(float * ptr, element_aligned_tag /*unused*/) const { _mm_storeu_ps(ptr, m_value); }
		[[nodiscard]] CCM_ALWAYS_INLINE constexpr __m128 get() const { return m_value; }
		[[nodiscard]] CCM_ALWAYS_INLINE float convert() const { return _mm_cvtss_f32(m_value); }
		CCM_ALWAYS_INLINE simd_mask<float, abi::ssse3> operator<(simd const & other) const
		{
			return simd_mask<float, abi::ssse3>(_mm_cmplt_ps(m_value, other.m_value));
		}
		CCM_ALWAYS_INLINE simd_mask<float, abi::ssse3> operator==(simd const & other) const
		{
			return simd_mask<float, abi::ssse3>(_mm_cmpeq_ps(m_value, other.m_value));
		}

	private:
		__m128 m_value;
	};

	CCM_ALWAYS_INLINE simd<float, abi::ssse3> choose(simd_mask<float, abi::ssse3> const & a, simd<float, abi::ssse3> const & b,
															simd<float, abi::ssse3> const & c)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<float, abi::ssse3>(_mm_add_ps(_mm_and_ps(a.get(), b.get()), _mm_andnot_ps(a.get(), c.get())));
	}

	template <>
	struct simd_mask<double, abi::ssse3>
	{
		using value_type					 = bool;
		using simd_type						 = simd<double, abi::ssse3>;
		using abi_type						 = abi::ssse3;
		CCM_ALWAYS_INLINE simd_mask() = default;
		CCM_ALWAYS_INLINE explicit simd_mask(bool value) : m_value(_mm_castsi128_pd(_mm_set1_epi64x(-static_cast<std::int64_t>(value)))) {}
		static constexpr int size() { return 4; }
		constexpr explicit simd_mask(__m128d const & value_in) : m_value(value_in) {}
		[[nodiscard]] constexpr __m128d get() const { return m_value; }
		CCM_ALWAYS_INLINE simd_mask operator||(simd_mask const & other) const { return simd_mask(_mm_or_pd(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE simd_mask operator&&(simd_mask const & other) const { return simd_mask(_mm_and_pd(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE simd_mask operator!() const { return simd_mask(_mm_andnot_pd(m_value, simd_mask(true).get())); }

	private:
		__m128d m_value;
	};

	CCM_ALWAYS_INLINE bool all_of(simd_mask<double, abi::ssse3> const & a)
	{
		return _mm_movemask_pd(a.get()) == 0x3;
	}

	CCM_ALWAYS_INLINE bool any_of(simd_mask<double, abi::ssse3> const & a)
	{
		return _mm_movemask_pd(a.get()) != 0x0;
	}

	template <>
	struct simd<double, abi::ssse3>
	{
		using value_type										= double;
		using abi_type											= abi::ssse3;
		using mask_type											= simd_mask<double, abi_type>;
		using storage_type										= simd_storage<double, abi_type>;
		CCM_ALWAYS_INLINE simd()							= default;
		CCM_ALWAYS_INLINE simd(simd const &)				= default;
		CCM_ALWAYS_INLINE simd(simd &&)					= default;
		CCM_ALWAYS_INLINE simd & operator=(simd const &) = default;
		CCM_ALWAYS_INLINE simd & operator=(simd &&)		= default;
		static constexpr int size() { return 2; }
		CCM_ALWAYS_INLINE simd(double value) : m_value(_mm_set1_pd(value)) {}
		CCM_ALWAYS_INLINE simd(double a, double b) : m_value(_mm_setr_pd(a, b)) {}
		CCM_ALWAYS_INLINE simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE simd(double const * ptr, Flags /*flags*/) : m_value(_mm_loadu_pd(ptr))
		{
		}
		CCM_ALWAYS_INLINE simd(double const * ptr, int stride) : simd(ptr[0], ptr[stride]) {}
		CCM_ALWAYS_INLINE constexpr simd(__m128d const & value_in) : m_value(value_in) {}
		CCM_ALWAYS_INLINE simd operator*(simd const & other) const { return {_mm_mul_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator/(simd const & other) const { return {_mm_div_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator+(simd const & other) const { return {_mm_add_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator-(simd const & other) const { return {_mm_sub_pd(m_value, other.m_value)}; }
		CCM_ALWAYS_INLINE simd operator-() const { return {_mm_sub_pd(_mm_set1_pd(0.0), m_value)}; }
		CCM_ALWAYS_INLINE void copy_from(double const * ptr, element_aligned_tag /*unused*/) { m_value = _mm_loadu_pd(ptr); }
		CCM_ALWAYS_INLINE void copy_to(double * ptr, element_aligned_tag /*unused*/) const { _mm_storeu_pd(ptr, m_value); }
		[[nodiscard]] constexpr __m128d get() const { return m_value; }
		[[nodiscard]] CCM_ALWAYS_INLINE double convert() const { return _mm_cvtsd_f64(m_value); }

		CCM_ALWAYS_INLINE simd_mask<double, abi::ssse3> operator<(simd const & other) const
		{
			return simd_mask<double, abi::ssse3>(_mm_cmplt_pd(m_value, other.m_value));
		}
		CCM_ALWAYS_INLINE simd_mask<double, abi::ssse3> operator==(simd const & other) const
		{
			return simd_mask<double, abi::ssse3>(_mm_cmpeq_pd(m_value, other.m_value));
		}

	private:
		__m128d m_value;
	};

	CCM_ALWAYS_INLINE simd<double, abi::ssse3> choose(simd_mask<double, abi::ssse3> const & a, simd<double, abi::ssse3> const & b,
															 simd<double, abi::ssse3> const & c)
	{
		// NOLINTNEXTLINE(modernize-return-braced-init-list)
		return simd<double, abi::ssse3>(_mm_add_pd(_mm_and_pd(a.get(), b.get()), _mm_andnot_pd(a.get(), c.get())));
	}
} // namespace ccm::intrin

	#endif // CCMATH_HAS_SIMD_SSSE3
#endif	   // CCMATH_HAS_SIMD