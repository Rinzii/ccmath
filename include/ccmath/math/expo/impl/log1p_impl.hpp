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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/types/fp_types.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/expo/impl/log_data.hpp"
#include "ccmath/math/expo/impl/log_double_impl.hpp"
#include "ccmath/math/expo/impl/log_float_impl.hpp"

#include <limits>

namespace ccm::internal::impl
{
	constexpr float log1p_ln2_hi_f = 0x1.62e430p-1f;
	constexpr float log1p_ln2_lo_f = 0x1.05c610p-29f;
	constexpr float log1p_lg1_f	   = 0x1.555556p-1f;
	constexpr float log1p_lg2_f	   = 0x1.99999ap-2f;
	constexpr float log1p_lg3_f	   = 0x1.24924ap-2f;
	constexpr float log1p_lg4_f	   = 0x1.f0a3d8p-3f;

	constexpr double log1p_lg1_d = 0x1.5555555555555p-1;
	constexpr double log1p_lg2_d = 0x1.9999999999999p-2;
	constexpr double log1p_lg3_d = 0x1.2492492492492p-2;
	constexpr double log1p_lg4_d = 0x1.c71c71c71c6d6p-3;
	constexpr double log1p_lg5_d = 0x1.745d1745d0a63p-3;
	constexpr double log1p_lg6_d = 0x1.3b13b13b0ff6ep-3;
	constexpr double log1p_lg7_d = 0x1.111111111126ep-3;

	constexpr auto log1p_log_data_flt = ccm::internal::log_data<double>();
	constexpr auto log1p_ln2_hi_d	  = log1p_log_data_flt.ln2hi;
	constexpr auto log1p_ln2_lo_d	  = log1p_log_data_flt.ln2lo;

	constexpr float log1p_float_impl(float x)
	{
		if (x > 0.414284F || x < -0.292893F)
		{
			const float u = 1.0F + x;
			if (u == 1.0F)
			{
				return x;
			}

			return log_float_impl(u);
		}

		std::uint32_t ix = ccm::support::float_to_uint32(x);
		int k			 = 1;
		ccm::double_t f{};
		ccm::double_t c{};
		ccm::double_t s{};
		ccm::double_t z{};
		ccm::double_t w{};
		ccm::double_t t1{};
		ccm::double_t t2{};
		ccm::double_t r{};
		ccm::double_t hfsq{};
		ccm::double_t dk{};

		if (ix < 0x3ed413d0U || (ix >> 31) != 0U)
		{
			if (ix >= 0xbf800000U)
			{
				if (x == -1.0F)
				{
					return -std::numeric_limits<float>::infinity();
				}

				return -std::numeric_limits<float>::quiet_NaN();
			}

			if ((ix << 1U) < (0x33800000U << 1U))
			{
				return x;
			}

			if (ix <= 0xbe95f619U)
			{
				k = 0;
				c = 0.0;
				f = x;
			}
		} else if (ix >= 0x7f800000U)
		{
			return x;
		}

		if (k != 0)
		{
			const float u	 = 1.0F + x;
			std::uint32_t iu = ccm::support::float_to_uint32(u);
			iu += 0x3f800000U - 0x3f3504f3U;
			k = static_cast<int>(iu >> 23) - 0x7f;
			if (k < 25)
			{
				c = k >= 2 ? static_cast<ccm::double_t>(1.0F - (u - x)) : static_cast<ccm::double_t>(x - (u - 1.0F));
				c /= u;
			} else
			{
				c = 0.0;
			}

			iu = (iu & 0x007fffffU) + 0x3f3504f3U;
			f  = ccm::support::uint32_to_float(iu) - 1.0F;
		}

		s	 = f / (2.0F + f);
		z	 = s * s;
		w	 = z * z;
		t1	 = w * (log1p_lg2_f + w * log1p_lg4_f);
		t2	 = z * (log1p_lg1_f + w * log1p_lg3_f);
		r	 = t2 + t1;
		hfsq = 0.5F * f * f;
		dk	 = static_cast<ccm::double_t>(k);

		return static_cast<float>(s * (hfsq + r) + (dk * log1p_ln2_lo_f + c) - hfsq + f + dk * log1p_ln2_hi_f);
	}

	constexpr double log1p_double_impl(double x)
	{
		if (x > 0.414284 || x < -0.292893)
		{
			const double u = 1.0 + x;
			if (u == 1.0)
			{
				return x;
			}

			return log_double_impl(u);
		}

		std::uint64_t ix = ccm::support::double_to_uint64(x);
		std::uint32_t hx = static_cast<std::uint32_t>(ix >> 32);
		int k			 = 1;
		ccm::double_t f{};
		ccm::double_t c{};
		ccm::double_t s{};
		ccm::double_t z{};
		ccm::double_t w{};
		ccm::double_t t1{};
		ccm::double_t t2{};
		ccm::double_t r{};
		ccm::double_t hfsq{};
		ccm::double_t dk{};

		if (hx < 0x3fda827aU || (hx >> 31) != 0U)
		{
			if (hx >= 0xbff00000U)
			{
				if (x == -1.0)
				{
					return -std::numeric_limits<double>::infinity();
				}

				return -std::numeric_limits<double>::quiet_NaN();
			}

			if ((hx << 1U) < (0x3ca00000U << 1U))
			{
				return x;
			}

			if (hx <= 0xbfd2bec4U)
			{
				k = 0;
				c = 0.0;
				f = x;
			}
		} else if (hx >= 0x7ff00000U)
		{
			return x;
		}

		if (k != 0)
		{
			const double u	 = 1.0 + x;
			std::uint64_t iu = ccm::support::double_to_uint64(u);
			std::uint32_t hu = static_cast<std::uint32_t>(iu >> 32);
			hu += 0x3ff00000U - 0x3fe6a09eU;
			k = static_cast<int>(hu >> 20) - 0x3ff;
			if (k < 54)
			{
				c = k >= 2 ? 1.0 - (u - x) : x - (u - 1.0);
				c /= u;
			} else
			{
				c = 0.0;
			}

			hu = (hu & 0x000fffffU) + 0x3fe6a09eU;
			iu = (static_cast<std::uint64_t>(hu) << 32) | (iu & 0xffffffffULL);
			f  = ccm::support::uint64_to_double(iu) - 1.0;
		}

		s	 = f / (2.0 + f);
		z	 = s * s;
		w	 = z * z;
		t1	 = w * (log1p_lg2_d + w * (log1p_lg4_d + w * log1p_lg6_d));
		t2	 = z * (log1p_lg1_d + w * (log1p_lg3_d + w * (log1p_lg5_d + w * log1p_lg7_d)));
		r	 = t2 + t1;
		hfsq = 0.5 * f * f;
		dk	 = static_cast<ccm::double_t>(k);

		return s * (hfsq + r) + (dk * log1p_ln2_lo_d + c) - hfsq + f + dk * log1p_ln2_hi_d;
	}
} // namespace ccm::internal::impl

namespace ccm::internal
{
	template <typename T> constexpr T log1p_zero_result(T num) noexcept
	{
		return num;
	}

	constexpr float log1p_float(float num) noexcept
	{
		if (num == 0.0F)
		{
			return log1p_zero_result(num);
		}

		if (CCM_UNLIKELY(ccm::isnan(num)))
		{
			return std::numeric_limits<float>::quiet_NaN();
		}

		if (num == std::numeric_limits<float>::infinity())
		{
			return std::numeric_limits<float>::infinity();
		}
		if (num == -1.0F)
		{
			ccm::support::fenv::set_errno_if_required(ERANGE);
			ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
			return -std::numeric_limits<float>::infinity();
		}
		if (num < -1.0F)
		{
			ccm::support::fenv::set_errno_if_required(EDOM);
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return -std::numeric_limits<float>::quiet_NaN();
		}

		return impl::log1p_float_impl(num);
	}

	constexpr double log1p_double(double num) noexcept
	{
		if (num == 0.0)
		{
			return log1p_zero_result(num);
		}

		if (CCM_UNLIKELY(ccm::isnan(num)))
		{
			return std::numeric_limits<double>::quiet_NaN();
		}

		if (num == std::numeric_limits<double>::infinity())
		{
			return std::numeric_limits<double>::infinity();
		}
		if (num == -1.0)
		{
			ccm::support::fenv::set_errno_if_required(ERANGE);
			ccm::support::fenv::raise_except_if_required(FE_DIVBYZERO);
			return -std::numeric_limits<double>::infinity();
		}
		if (num < -1.0)
		{
			ccm::support::fenv::set_errno_if_required(EDOM);
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return -std::numeric_limits<double>::quiet_NaN();
		}

		return impl::log1p_double_impl(num);
	}
} // namespace ccm::internal
