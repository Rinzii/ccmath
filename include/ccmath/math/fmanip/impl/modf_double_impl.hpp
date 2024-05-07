/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/bits.hpp"

namespace ccm::internal::impl
{
	constexpr double modf_double_impl(double x, double* iptr) noexcept
	{
		const std::int64_t integerValue = support::double_to_int64(x);
		// NOLINTNEXTLINE
		const std::int32_t exponent = ((integerValue >> 52) & 0x7ff) - 0x3ff; // Get exponent of x
		if (exponent < 52)
		{
			if (exponent < 0)
			{
				// *iptr is equal to ±0
				*iptr = support::int64_to_double(static_cast<std::int64_t>(static_cast<unsigned long>(integerValue) & UINT64_C(0x8000000000000000)));
				return x;
			}

			const std::uint64_t i = UINT64_C(0x000fffffffffffff) >> exponent;
			// x is an integral value
			if ((static_cast<std::uint64_t>(integerValue) & i) == 0)
			{
				*iptr = x;
				// return ±0
				x = support::int64_to_double(static_cast<std::int64_t>(static_cast<unsigned long>(integerValue) & UINT64_C(0x8000000000000000)));
				return x;
			}

			*iptr = support::int64_to_double(static_cast<std::int64_t>(static_cast<std::uint64_t>(integerValue) & (~i)));
			return x - *iptr;
		}

		// No fractional part
		*iptr = x * 1.0;

		// Handle the NaN's separately
		if (exponent == 0x400 && ((static_cast<unsigned long>(integerValue) & UINT64_C(0xfffffffffffff)) != 0U))
		{
			return x * 1.0;
		}

		// Return ±0
		x = support::int64_to_double(static_cast<std::int64_t>(static_cast<unsigned long>(integerValue) & UINT64_C(0x8000000000000000)));

		return x;
	}

} // namespace ccm::internal::impl