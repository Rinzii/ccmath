/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstdint>
#include <limits>

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/basic/fmod.hpp"

namespace ccm::internal
{
	namespace impl
	{
		// NOLINTNEXTLINE(readability-function-cognitive-complexity)
		constexpr float remquo_float_impl(float x, float y, int * quo) noexcept
		{
			std::int32_t x_i32{};
			std::int32_t y_i32{};
			int computed_quotient{};

			x_i32 = support::float_to_int32(x);
			y_i32 = support::float_to_int32(y);

			// Determine the signs of x and the quotient.
			const std::uint32_t x_sign = static_cast<std::uint32_t>(x_i32) & 0x80000000;
			const int quotient_sign	   = static_cast<int>(x_sign ^ (static_cast<std::uint32_t>(y_i32) & 0x80000000));

			// Clear the sign bits from the int32_t representations of x and y.
			x_i32 &= 0x7fffffff;
			y_i32 &= 0x7fffffff;

			// If y is zero.
			if (CCM_UNLIKELY(y_i32 == 0)) { return (x * y) / (x * y); }

			// If x is not finite or y is NaN.
			if (CCM_UNLIKELY(x_i32 >= 0x7f800000 || y_i32 > 0x7f800000)) { return (x * y) / (x * y); } // NOLINT(readability-simplify-boolean-expr)

			if (y_i32 <= 0x7dffffff)
			{
				x = ccm::fmod(x, 8 * y); // now x < (8 * y)
			}

			if ((x_i32 - y_i32) == 0)
			{
				*quo = quotient_sign != 0 ? -1 : 1;
				return 0.0F * x;
			}

			x				  = ccm::fabsf(x);
			y				  = ccm::fabsf(y);
			computed_quotient = 0;

			if (y_i32 <= 0x7e7fffff && x >= 4 * y)
			{
				x -= 4 * y;
				computed_quotient += 4;
			}
			if (y_i32 <= 0x7effffff && x >= 2 * y)
			{
				x -= 2 * y;
				computed_quotient += 2;
			}
			if (y_i32 < 0x01000000)
			{
				if (x + x > y)
				{
					x -= y;
					++computed_quotient;
					if (x + x >= y)
					{
						x -= y;
						++computed_quotient;
					}
				}
			}
			else
			{
				const float y_half = 0.5F * y;
				if (x > y_half)
				{
					x -= y;
					++computed_quotient;
					if (x >= y_half)
					{
						x -= y;
						++computed_quotient;
					}
				}
			}
			*quo = quotient_sign != 0 ? -computed_quotient : computed_quotient;

			// Make sure that the correct sign of zero results in round down mode.
			if (x == 0.0F) { x = 0.0F; }
			if (x_sign != 0U) { x = -x; }

			return x;
		}
	} // namespace impl

	constexpr float remquo_float(float x, float y, int * quo) noexcept
	{
		return impl::remquo_float_impl(x, y, quo);
	}
} // namespace ccm::internal
