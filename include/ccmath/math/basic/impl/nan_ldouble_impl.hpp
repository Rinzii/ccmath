/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/helpers/digit_to_int.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/types/int128_types.hpp"

#include <cstdint>
#include <limits>


namespace ccm::internal::impl
{
	constexpr long double nan_ldouble_impl(const char * arg) noexcept;

#ifndef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
	constexpr long double nan_ldouble_impl(const char * arg) noexcept
	{

		if constexpr (!std::numeric_limits<long double>::is_iec559)
		{
			return 0.0;
		}

#if defined(_MSC_VER) && !defined(__clang__)
		// Currently, MSVC always returns a Quiet NaN no matter if a payload is
		// provided or not. This is different from GCC and Clang which do allow payloads to be set.
		// So if we detect we are using MSVC without Clang-CL then
		// we can just return NaN and not bother doing any extra work.
		// To properly mimic the behavior of MSVC.
		return std::numeric_limits<long double>::quiet_NaN(); // Default NaN
#endif

		if (arg == nullptr)
		{
			return std::numeric_limits<long double>::quiet_NaN(); // Default NaN
		}

		// NOLINTBEGIN
		if (arg[0] == '\0') //
		{
			return std::numeric_limits<long double>::quiet_NaN(); // Default NaN
		}

		bool has_hex_been_detected{false};

		// Check for a hex prefix and if its detected, skip the prefix and set the flag.
		if (arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X'))
		{
			arg += 2;
			has_hex_been_detected = true;
		}

		if (!has_hex_been_detected)
		{
			// Check that all of are the characters are numbers. If we detect a non-number, return the default NaN.
			for (std::size_t i = 0; arg[i] != '\0'; ++i) // NOLINT
			{
				if (arg[i] < '0' || arg[i] > '9')
				{
					return std::numeric_limits<long double>::quiet_NaN(); // Default NaN
				}
			}
		}

		ccm::types::uint128_t dbl_bits{0};

		if (has_hex_been_detected)
		{
			// Calculate tag_value by handling wrapping for numbers larger than 8 digits
			for (std::size_t i = 0; arg[i] != '\0'; ++i)
			{
				dbl_bits *= 16;
				dbl_bits += static_cast<std::uint8_t>(ccm::helpers::digit_to_int(arg[i])); // Convert ASCII to numeric value
				if (i >= 15)
				{
					dbl_bits %= static_cast<ccm::types::uint128_t>(1e18); // Wrap around for numbers larger than 8 digits
				}
			}
		}
		else
		{
			// Calculate tag_value by handling wrapping for numbers larger than 8 digits
			for (std::size_t i = 0; arg[i] != '\0'; ++i)
			{
				dbl_bits *= 10;
				dbl_bits += static_cast<std::uint8_t>(arg[i] - '0'); // Convert ASCII to numeric value
				if (i >= 15)
				{
					dbl_bits %= static_cast<ccm::types::uint128_t>(1e18); // Wrap around for numbers larger than 8 digits
				}
			}
		}
		// NOLINTEND

		// Set the tag bits for NaN
		dbl_bits |= ccm::support::bit_cast<ccm::types::uint128_t>(std::numeric_limits<long double>::quiet_NaN());

		// Convert the uint128_t tag into a double NaN
		return ccm::support::bit_cast<long double>(dbl_bits);
	}
#endif
} // namespace ccm::internal::impl