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
		std::int64_t i0{};
		std::int32_t j0{};

		i0 = ccm::support::double_to_int64(x);
		// NOLINTNEXTLINE
		j0 = ((i0 >> 52) & 0x7ff) - 0x3ff; // Get exponent of x
		if (j0 < 52)
		{
			if (j0 < 0)
			{
				// *iptr is equal to ±0
				*iptr = ccm::support::int64_to_double(static_cast<std::int64_t>(static_cast<unsigned long>(i0) & UINT64_C(0x8000000000000000)));
				return x;
			}

			std::uint64_t i = UINT64_C(0x000fffffffffffff) >> j0;
			// x is an integral value
			if ((static_cast<std::uint64_t>(i0) & i) == 0)
			{
				*iptr = x;
				// return ±0
				x = ccm::support::int64_to_double(static_cast<std::int64_t>(static_cast<unsigned long>(i0) & UINT64_C(0x8000000000000000)));
				return x;
			}

			*iptr = ccm::support::int64_to_double(static_cast<std::int64_t>(static_cast<std::uint64_t>(i0) & (~i)));
			return x - *iptr;
		}

		// No fractional part
		*iptr = x * 1.0;

		// Handle the NaN's separately
		if (j0 == 0x400 && ((static_cast<unsigned long>(i0) & UINT64_C(0xfffffffffffff)) != 0U))
		{
			return x * 1.0;
		}

		// Return ±0
		x = ccm::support::int64_to_double(static_cast<std::int64_t>(static_cast<unsigned long>(i0) & UINT64_C(0x8000000000000000)));

		return x;
	}

} // namespace ccm::internal::impl