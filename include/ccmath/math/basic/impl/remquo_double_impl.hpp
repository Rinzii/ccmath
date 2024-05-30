/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/compiler_suppression/clang_compiler_suppression.hpp"
#include "ccmath/internal/predef/compiler_suppression/gcc_compiler_suppression.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/basic/fmod.hpp"

#include <cstdint>

namespace ccm::internal
{
	namespace impl
	{
		// NOLINTNEXTLINE(readability-function-cognitive-complexity)
		constexpr double remquo_double_impl(double x, double y, int * quo) noexcept
		{
			std::int64_t x_i64{};
			std::int64_t y_i64{};
			int computed_quotient{};

			x_i64 = support::double_to_int64(x);
			y_i64 = support::double_to_int64(y);

			// Determine the signs of x and the quotient.
			const std::uint64_t x_sign		  = static_cast<std::uint64_t>(x_i64) & 0x8000000000000000ULL;
			const std::uint64_t quotient_sign = x_sign ^ (static_cast<std::uint64_t>(y_i64) & 0x8000000000000000ULL);

			// Clear the sign bits from the int64_t representations of x and y.
			x_i64 &= 0x7fffffffffffffffULL;
			y_i64 &= 0x7fffffffffffffffULL;

			// If y is zero.
			if (CCM_UNLIKELY(y_i64 == 0)) { return (x * y) / (x * y); }

			// GCC and Clang do not like comparing signed and unsigned integers.
			// The outcome of these comparisons is well-defined, so we can safely disable these warnings.
			// Must disable clang-form here as clang-format likes to add spaces between the hyphens for flags
			// clang-format off
			CCM_DISABLE_GCC_WARNING(-Wsign-compare)
			CCM_DISABLE_CLANG_WARNING(-Wsign-compare)
			// clang-format on

			// If x is not finite or y is NaN.
			if (CCM_UNLIKELY(x_i64 >= 0x7ff0000000000000ULL || y_i64 > 0x7ff0000000000000ULL)) // NOLINT(readability-simplify-boolean-expr)
			{
				return (x * y) / (x * y);
			} // NOLINT(readability-simplify-boolean-expr)

			// b (or bit 54) represents the highest bit we can compare against for both signed and unsigned integers without causing an overflow.
			// Here we are checking that if y_i64 is within the range of signed 64-bit integers that can be represented without setting the MSB (i.e.,
			// positive or zero).
			if (y_i64 <= 0x7fbfffffffffffffULL)
			{
				x = ccm::fmod(x, 8 * y); // now x < (8 * y)
			}

			if (CCM_UNLIKELY(x_i64 == y_i64))
			{
				*quo = quotient_sign != 0U ? -1 : 1;
				return 0.0 * x;
			}

			x				  = ccm::fabs(x);
			y				  = support::int64_to_double(y_i64);
			computed_quotient = 0;

			if (y_i64 <= 0x7fcfffffffffffffULL && x >= 4 * y)
			{
				x -= 4 * y;
				computed_quotient += 4;
			}

			if (y_i64 <= 0x7fdfffffffffffffULL && x >= 2 * y)
			{
				x -= 2 * y;
				computed_quotient += 2;
			}

			if (y_i64 < 0x0020000000000000ULL)
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
				const double y_half = 0.5 * y;
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

			CCM_RESTORE_GCC_WARNING()
			CCM_RESTORE_CLANG_WARNING()

			*quo = quotient_sign != 0U ? -computed_quotient : computed_quotient;

			// Make sure that the correct sign of zero results in round down mode.
			if (x == 0.0) { x = 0.0; }
			if (x_sign != 0U) { x = -x; }

			return x;
		}
	} // namespace impl

	constexpr double remquo_double(double x, double y, int * quo) noexcept
	{
		return impl::remquo_double_impl(x, y, quo);
	}
} // namespace ccm::internal
