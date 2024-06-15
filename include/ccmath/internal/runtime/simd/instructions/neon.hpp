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
	#ifdef CCMATH_SIMD_NEON
		#include <arm_neon.h>

namespace ccm::intrin
{
	namespace abi
	{
		class neon
		{
		};

	} // namespace abi

	template <>
	struct simd_mask<float, abi::neon>
	{

		using value_type					 = bool;
		using simd_type						 = simd_mask<float, abi::neon>;
		using abi_type						 = abi::neon;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		CCM_ALWAYS_INLINE inline simd_mask(bool value) : m_value(vreinterpretq_u32_s32(vdupq_n_s32(-int(value)))) {}
		static constexpr int size() { return 4; }
		CCM_ALWAYS_INLINE inline constexpr simd_mask(uint32x4_t const & value_in) : m_value(value_in) {}
		[[nodiscard]] CCM_ALWAYS_INLINE inline constexpr uint32x4_t get() const { return m_value; }
		CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const { return simd_mask(vorrq_u32(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const { return simd_mask(vandq_u32(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd_mask operator!() const { return simd_mask(vmvnq_u32(m_value)); }

	private:
		uint32x4_t m_value;
	};

	CCM_ALWAYS_INLINE inline bool all_of(simd_mask<float, abi::neon> const & a)
	{
		return vminvq_u32(a.get()) == static_cast<std::uint32_t>(-1);
	}

	CCM_ALWAYS_INLINE inline bool any_of(simd_mask<float, abi::neon> const & a)
	{
		return vmaxvq_u32(a.get()) == static_cast<std::uint32_t>(-1);
	}

	template <>
	struct simd<float, abi::neon>
	{
		using value_type				= float;
		using abi_type					= abi::neon;
		using mask_type					= simd_mask<float, abi_type>;
		using storage_type				= simd_storage<float, abi_type>;
		CCM_ALWAYS_INLINE inline simd() = default;
		static constexpr int size() { return 4; }
		CCM_ALWAYS_INLINE inline simd(float value) : m_value(vdupq_n_f32(value)) {}
		CCM_ALWAYS_INLINE inline simd(float a, float b, float c, float d) : m_value((float32x4_t){a, b, c, d}) {}
		CCM_ALWAYS_INLINE inline simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE inline simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE inline simd(float const * ptr, Flags flags)
		{
			copy_from(ptr, flags);
		}
		CCM_ALWAYS_INLINE inline simd(float const * ptr, int stride) : simd(ptr[0], ptr[stride], ptr[2 * stride], ptr[3 * stride]) {}
		CCM_ALWAYS_INLINE inline constexpr simd(float32x4_t const & value_in) : m_value(value_in) {}
		CCM_ALWAYS_INLINE inline simd operator*(simd const & other) const { return simd(vmulq_f32(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator/(simd const & other) const { return simd(vdivq_f32(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator+(simd const & other) const { return simd(vaddq_f32(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator-(simd const & other) const { return simd(vsubq_f32(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator-() const { return simd(vnegq_f32(m_value)); }
		CCM_ALWAYS_INLINE inline void copy_from(float const * ptr, element_aligned_tag) { m_value = vld1q_f32(ptr); }
		CCM_ALWAYS_INLINE inline void copy_to(float * ptr, element_aligned_tag) const { vst1q_f32(ptr, m_value); }
		[[nodiscard]] CCM_ALWAYS_INLINE inline constexpr float32x4_t get() const { return m_value; }
		[[nodiscard]] CCM_ALWAYS_INLINE inline float convert() const { return vgetq_lane_f32(m_value, 0); }
		CCM_ALWAYS_INLINE simd_mask<float, abi::neon> operator<(simd const & other) const
		{
			return simd_mask<float, abi::neon>(vcltq_f32(m_value, other.m_value));
		}
		CCM_ALWAYS_INLINE inline simd_mask<float, abi::neon> operator==(simd const & other) const
		{
			return simd_mask<float, abi::neon>(vceqq_f32(m_value, other.m_value));
		}

	private:
		float32x4_t m_value;
	};

	CCM_ALWAYS_INLINE inline simd<float, abi::neon> choose(simd_mask<float, abi::neon> const & a, simd<float, abi::neon> const & b,
														   simd<float, abi::neon> const & c)
	{
		return simd<float, abi::neon>(vreinterpretq_f32_u32(vbslq_u32(a.get(), vreinterpretq_u32_f32(b.get()), vreinterpretq_u32_f32(c.get()))));
	}

	template <>
	struct simd_mask<double, abi::neon>
	{
		using value_type					 = bool;
		using simd_type						 = simd<double, abi::neon>;
		using abi_type						 = abi::neon;
		CCM_ALWAYS_INLINE inline simd_mask() = default;
		CCM_ALWAYS_INLINE inline simd_mask(bool value) : m_value(vreinterpretq_u64_s64(vdupq_n_s64(-std::int64_t(value)))) {}
		static constexpr int size() { return 4; }
		CCM_ALWAYS_INLINE inline constexpr simd_mask(uint64x2_t const & value_in) : m_value(value_in) {}
		[[nodiscard]] CCM_ALWAYS_INLINE inline constexpr uint64x2_t get() const { return m_value; }
		CCM_ALWAYS_INLINE inline simd_mask operator||(simd_mask const & other) const { return simd_mask(vorrq_u64(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd_mask operator&&(simd_mask const & other) const { return simd_mask(vandq_u64(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd_mask operator!() const { return simd_mask(vreinterpretq_u64_u32(vmvnq_u32(vreinterpretq_u32_u64(m_value)))); }

	private:
		uint64x2_t m_value;
	};

	CCM_ALWAYS_INLINE inline bool all_of(simd_mask<double, abi::neon> const & a)
	{
		return all_of(simd_mask<float, abi::neon>(vreinterpretq_u32_u64(a.get())));
	}

	CCM_ALWAYS_INLINE inline bool any_of(simd_mask<double, abi::neon> const & a)
	{
		return any_of(simd_mask<float, abi::neon>(vreinterpretq_u32_u64(a.get())));
	}

	template <>
	struct simd<double, abi::neon>
	{
		using value_type										= double;
		using abi_type											= abi::neon;
		using mask_type											= simd_mask<double, abi_type>;
		using storage_type										= simd_storage<double, abi_type>;
		CCM_ALWAYS_INLINE inline simd()							= default;
		CCM_ALWAYS_INLINE inline simd(simd const &)				= default;
		CCM_ALWAYS_INLINE inline simd(simd &&)					= default;
		CCM_ALWAYS_INLINE inline simd & operator=(simd const &) = default;
		CCM_ALWAYS_INLINE inline simd & operator=(simd &&)		= default;
		static constexpr int size() { return 2; }
		CCM_ALWAYS_INLINE inline simd(double value) : m_value(vdupq_n_f64(value)) {}
		CCM_ALWAYS_INLINE inline simd(double a, double b) : m_value((float64x2_t){a, b}) {}
		CCM_ALWAYS_INLINE inline simd(storage_type const & value) { copy_from(value.data(), element_aligned_tag()); }
		CCM_ALWAYS_INLINE inline simd & operator=(storage_type const & value)
		{
			copy_from(value.data(), element_aligned_tag());
			return *this;
		}
		template <class Flags>
		CCM_ALWAYS_INLINE inline simd(double const * ptr, Flags flags)
		{
			copy_from(ptr, flags);
		}
		CCM_ALWAYS_INLINE inline simd(double const * ptr, int stride) : simd(ptr[0], ptr[stride]) {}
		CCM_ALWAYS_INLINE inline constexpr simd(float64x2_t const & value_in) : m_value(value_in) {}
		CCM_ALWAYS_INLINE inline simd operator*(simd const & other) const { return simd(vmulq_f64(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator/(simd const & other) const { return simd(vdivq_f64(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator+(simd const & other) const { return simd(vaddq_f64(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator-(simd const & other) const { return simd(vsubq_f64(m_value, other.m_value)); }
		CCM_ALWAYS_INLINE inline simd operator-() const { return simd(vnegq_f64(m_value)); }
		CCM_ALWAYS_INLINE inline void copy_from(double const * ptr, element_aligned_tag) { m_value = vld1q_f64(ptr); }
		CCM_ALWAYS_INLINE inline void copy_to(double * ptr, element_aligned_tag) const { vst1q_f64(ptr, m_value); }
		[[nodiscard]] CCM_ALWAYS_INLINE inline constexpr float64x2_t get() const { return m_value; }
		[[nodiscard]] CCM_ALWAYS_INLINE inline double convert() const { return vgetq_lane_f64(m_value, 0); }
		CCM_ALWAYS_INLINE inline simd_mask<double, abi::neon> operator<(simd const & other) const
		{
			return simd_mask<double, abi::neon>(vcltq_f64(m_value, other.m_value));
		}
		CCM_ALWAYS_INLINE inline simd_mask<double, abi::neon> operator==(simd const & other) const
		{
			return simd_mask<double, abi::neon>(vceqq_f64(m_value, other.m_value));
		}

	private:
		float64x2_t m_value;
	};

	CCM_ALWAYS_INLINE inline simd<double, abi::neon> choose(simd_mask<double, abi::neon> const & a, simd<double, abi::neon> const & b,
															simd<double, abi::neon> const & c)
	{
		return simd<double, abi::neon>(vreinterpretq_f64_u64(vbslq_u64(a.get(), vreinterpretq_u64_f64(b.get()), vreinterpretq_u64_f64(c.get()))));
	}
} // namespace ccm::intrin
	#endif // CCMATH_SIMD_NEON
#endif	   // CCM_CONFIG_USE_RT_SIMD