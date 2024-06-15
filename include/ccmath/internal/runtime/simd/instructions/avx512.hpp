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

#ifdef CCMATH_SIMD
	#ifdef CCMATH_SIMD_AVX512F

		#include <immintrin.h>

namespace ccm::intrin
{
	namespace abi
	{
		struct avx512
		{
		};
	} // namespace abi

	template <>
	struct simd_mask<float, abi::avx512>
	{
		using value_type					 = bool;
		using simd_type						 = simd<float, abi::avx512>;
		using abi_type						 = abi::avx512;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		CCM_ALWAYS_INLINE inline simd_mask(bool value) : m_value(-std::int16_t(value)) {}
		CCM_ALWAYS_INLINE inline static constexpr int size() { return 16; }
		CCM_ALWAYS_INLINE inline constexpr simd_mask(__mmask16 const & value_in) : m_value(value_in) {}
		[[nodiscard]] CCM_ALWAYS_INLINE inline constexpr __mmask16 get() const { return m_value; }
		CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const { return simd_mask(_kor_mask16(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const { return simd_mask(_kand_mask16(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd_mask operator!() const { return simd_mask(_knot_mask16(m_value)); }

	private:
		__mmask16 m_value;
	};

	CCM_ALWAYS_INLINE inline bool all_of(simd_mask<float, abi::avx512> const & a)
	{
		static const __mmask16 false_value(-static_cast<std::int16_t>(false));
		return _kortestc_mask16_u8(a.get(), false_value) != 0U;
	}

	CCM_ALWAYS_INLINE inline bool any_of(simd_mask<float, abi::avx512> const & a)
	{
		static const __mmask16 false_value(-static_cast<std::int16_t>(false));
		return _kortestc_mask16_u8(~a.get(), false_value) == 0U;
	}

	template <>
	struct simd<float, abi::avx512>
	{
		CCM_ALWAYS_INLINE simd() = default;
		using value_type		 = float;
		using abi_type			 = abi::avx512;
		using mask_type			 = simd_mask<float, abi_type>;
		using storage_type		 = simd_storage<float, abi_type>;
		static constexpr int size() { return 16; }
		CCM_ALWAYS_INLINE inline simd(float value) : m_value(_mm512_set1_ps(value)) {}
		CCM_ALWAYS_INLINE inline simd(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m,
									  float n, float o, float p)
			: m_value(_mm512_setr_ps(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p))
		{
		}
		CCM_ALWAYS_INLINE inline simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE inline simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE inline simd(float const * ptr, Flags /*flags*/) : m_value(_mm512_loadu_ps(ptr))
		{
		}
		CCM_ALWAYS_INLINE inline simd(float const * ptr, int stride)
			: simd(ptr[0], ptr[stride], ptr[2 * stride], ptr[3 * stride], ptr[4 * stride], ptr[5 * stride], ptr[6 * stride], ptr[7 * stride], ptr[8 * stride],
				   ptr[9 * stride], ptr[10 * stride], ptr[11 * stride], ptr[12 * stride], ptr[13 * stride], ptr[14 * stride], ptr[15 * stride])
		{
		}
		CCM_ALWAYS_INLINE inline constexpr simd(__m512 const & value_in) : m_value(value_in) {}
		CCM_ALWAYS_INLINE inline simd operator*(simd const & other) const { return simd(_mm512_mul_ps(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator/(simd const & other) const { return simd(_mm512_div_ps(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator+(simd const & other) const { return simd(_mm512_add_ps(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator-(simd const & other) const { return simd(_mm512_sub_ps(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator-() const { return simd(_mm512_sub_ps(_mm512_set1_ps(0.0), m_value)); }
		CCM_ALWAYS_INLINE inline void copy_from(float const * ptr, element_aligned_tag /*unused*/) { m_value = _mm512_loadu_ps(ptr); }
		CCM_ALWAYS_INLINE inline void copy_to(float * ptr, element_aligned_tag /*unused*/) const { _mm512_storeu_ps(ptr, m_value); }
		[[nodiscard]] CCM_ALWAYS_INLINE inline constexpr __m512 get() const { return m_value; }
		[[nodiscard]] CCM_ALWAYS_INLINE inline float convert() const { return _mm512_cvtss_f32(m_value); }
		CCM_ALWAYS_INLINE inline simd_mask<float, abi::avx512> operator<(simd const & other) const
		{
			return simd_mask<float, abi::avx512>(_mm512_cmplt_ps_mask(m_value, other.m_value));
		}
		CCM_ALWAYS_INLINE inline simd_mask<float, abi::avx512> operator==(simd const & other) const
		{
			return simd_mask<float, abi::avx512>(_mm512_cmpeq_ps_mask(m_value, other.m_value));
		}

	private:
		__m512 m_value;
	};

	CCM_ALWAYS_INLINE inline simd<float, abi::avx512> choose(simd_mask<float, abi::avx512> const & a, simd<float, abi::avx512> const & b,
															 simd<float, abi::avx512> const & c)
	{
		return simd<float, abi::avx512>(_mm512_mask_blend_ps(a.get(), c.get(), b.get()));
	}

	template <>
	struct simd_mask<double, abi::avx512>
	{
		using value_type					 = bool;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		CCM_ALWAYS_INLINE inline simd_mask(bool value) : m_value(-std::int16_t(value)) {}
		static constexpr int size() { return 8; }
		CCM_ALWAYS_INLINE inline constexpr simd_mask(__mmask8 const & value_in) : m_value(value_in) {}
		[[nodiscard]] CCM_ALWAYS_INLINE inline constexpr __mmask8 get() const { return m_value; }
		CCM_ALWAYS_INLINE simd_mask operator||(simd_mask const & other) const { return simd_mask(static_cast<__mmask8>(_mm512_kor(m_value, other.m_value))); }
		CCM_ALWAYS_INLINE simd_mask operator&&(simd_mask const & other) const { return simd_mask(static_cast<__mmask8>(_mm512_kand(m_value, other.m_value))); }
		CCM_ALWAYS_INLINE simd_mask operator!() const
		{
			static const __mmask8 true_value(simd_mask<double, abi::avx512>(true).get());
			return simd_mask(static_cast<__mmask8>(_mm512_kxor(true_value, m_value)));
		}

	private:
		__mmask8 m_value;
	};

	CCM_ALWAYS_INLINE inline bool all_of(simd_mask<double, abi::avx512> const & a)
	{
		static const __mmask16 false_value(-static_cast<std::int16_t>(false));
		const __mmask16 a_value(0xFF00 | a.get());
		return _kortestc_mask16_u8(a_value, false_value) != 0U;
	}

	CCM_ALWAYS_INLINE inline bool any_of(simd_mask<double, abi::avx512> const & a)
	{
		static const __mmask16 false_value(-static_cast<std::int16_t>(false));
		const __mmask16 a_value(0x0000 | a.get());
		return _kortestc_mask16_u8(~a_value, false_value) == 0U;
	}

	template <>
	struct simd<double, abi::avx512>
	{
		using value_type										= double;
		using abi_type											= abi::avx512;
		using mask_type											= simd_mask<double, abi_type>;
		using storage_type										= simd_storage<double, abi_type>;
		CCM_ALWAYS_INLINE inline simd()							= default;
		CCM_ALWAYS_INLINE inline simd(simd const &)				= default;
		CCM_ALWAYS_INLINE inline simd(simd &&)					= default;
		CCM_ALWAYS_INLINE inline simd & operator=(simd const &) = default;
		CCM_ALWAYS_INLINE inline simd & operator=(simd &&)		= default;
		static constexpr int size() { return 8; }
		CCM_ALWAYS_INLINE inline simd(double value) : m_value(_mm512_set1_pd(value)) {}
		CCM_ALWAYS_INLINE inline simd(double a, double b, double c, double d, double e, double f, double g, double h)
			: m_value(_mm512_setr_pd(a, b, c, d, e, f, g, h))
		{
		}
		CCM_ALWAYS_INLINE inline simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE inline simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE inline simd(double const * ptr, Flags /*flags*/) : m_value(_mm512_loadu_pd(ptr))
		{
		}
		CCM_ALWAYS_INLINE inline simd(double const * ptr, int stride)
			: simd(ptr[0], ptr[stride], ptr[2 * stride], ptr[3 * stride], ptr[4 * stride], ptr[5 * stride], ptr[6 * stride], ptr[7 * stride])
		{
		}
		CCM_ALWAYS_INLINE inline constexpr simd(__m512d const & value_in) : m_value(value_in) {}
		CCM_ALWAYS_INLINE inline simd operator*(simd const & other) const { return simd(_mm512_mul_pd(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator/(simd const & other) const { return simd(_mm512_div_pd(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator+(simd const & other) const { return simd(_mm512_add_pd(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator-(simd const & other) const { return simd(_mm512_sub_pd(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE CCM_GPU_HOST_DEVICE inline simd operator-() const { return simd(_mm512_sub_pd(_mm512_set1_pd(0.0), m_value)); }
		CCM_ALWAYS_INLINE inline void copy_from(double const * ptr, element_aligned_tag) { m_value = _mm512_loadu_pd(ptr); }
		CCM_ALWAYS_INLINE inline void copy_to(double * ptr, element_aligned_tag) const { _mm512_storeu_pd(ptr, m_value); }
		[[nodiscard]] CCM_ALWAYS_INLINE inline constexpr __m512d get() const { return m_value; }
		[[nodiscard]] CCM_ALWAYS_INLINE inline double convert() const { return _mm512_cvtsd_f64(m_value); }
		CCM_ALWAYS_INLINE inline simd_mask<double, abi::avx512> operator<(simd const & other) const
		{
			return simd_mask<double, abi::avx512>(_mm512_cmplt_pd_mask(m_value, other.m_value));
		}
		CCM_ALWAYS_INLINE inline simd_mask<double, abi::avx512> operator==(simd const & other) const
		{
			return simd_mask<double, abi::avx512>(_mm512_cmpeq_pd_mask(m_value, other.m_value));
		}

	private:
		__m512d m_value;
	};

	CCM_ALWAYS_INLINE inline simd<double, abi::avx512> choose(simd_mask<double, abi::avx512> const & a, simd<double, abi::avx512> const & b,
															  simd<double, abi::avx512> const & c)
	{
		return simd<double, abi::avx512>(_mm512_mask_blend_pd(a.get(), c.get(), b.get()));
	}

} // namespace ccm::intrin

	#endif // CCMATH_SIMD_AVX512F
#endif	   // CCM_CONFIG_USE_RT_SIMD