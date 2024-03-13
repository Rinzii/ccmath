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

#include "ccmath/internal/helpers/bits.hpp"
#include "ccmath/internal/predef/unlikely.hpp"

#include "ccmath/detail/compare/isfinite.hpp"
#include "ccmath/detail/compare/isnan.hpp"

#include "ccmath/detail/basic/abs.hpp"
#include "ccmath/detail/basic/fmod.hpp"

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
			inline constexpr float remquo_float_impl(float x, float y, int * quo) noexcept
			{
				// Assume all edge case checking is done before calling this function.
				std::int32_t x_i32{};
				std::int32_t y_i32{};
				std::uint32_t x_sign{};
				int quotient_sign{};
				int computed_quotient{};

				x_i32 = ccm::helpers::float_to_int32(x);
				y_i32 = ccm::helpers::float_to_int32(y);

				// Determine the signs of x and the quotient.
				x_sign		  = static_cast<std::uint32_t>(x_i32) & 0x80000000;
				quotient_sign = x_sign ^ (static_cast<std::uint32_t>(y_i32) & 0x80000000);

				// Clear the sign bits from the int32_t representations of x and y.
				y_i32 &= 0x7fffffff;
				x_i32 &= 0x7fffffff;

				if (y_i32 <= 0x7dffffff)
				{
					x = ccm::fmod(x, 8 * y); // now x < (8 * y)
				}

				if ((x_i32 - y_i32) == 0)
				{
					*quo = quotient_sign ? -1 : 1;
					return 0.0f * x;
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
					float y_half = 0.5 * y;
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
				*quo = quotient_sign ? -computed_quotient : computed_quotient;

				// Make sure that the correct sign of zero results in round down mode.
				if (x == 0.0f) { x = 0.0f; }
				if (x_sign) { x = -x; }

				return x;
			}
		} // namespace impl
	}	  // namespace

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	inline constexpr T remquo_float(T x, T y, int * quo) noexcept
	{
		return static_cast<T>(impl::remquo_float_impl(x, y, quo));
	}
} // namespace ccm::internal
