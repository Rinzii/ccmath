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

#include <cstdint>
#include <limits>

namespace ccm::internal::impl
{
	constexpr float nan_float_impl(const char * arg) noexcept
	{
		if constexpr (!std::numeric_limits<float>::is_iec559)
		{
			return 0.0;
		}

#if defined(_MSC_VER) && !defined(__clang__)
		// Currently, MSVC always returns a Quiet NaN no matter if a payload is
		// provided or not. This is different from GCC and Clang which do allow payloads to be set.
		// So if we detect we are using MSVC without Clang-CL then
		// we can just return NaN and not bother doing any extra work.
		// To properly mimic the behavior of MSVC.
		return std::numeric_limits<double>::quiet_NaN(); // Default NaN
#endif

		std::uint32_t flt_bits{0};
		bool has_hex_been_detected{false};

		// NOLINTBEGIN

		// Check for a hex prefix and if its detected, skip the prefix and set the flag.
		if (arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X'))
		{
			arg += 2;
			has_hex_been_detected = true;
		}

		if (!has_hex_been_detected)
		{
			// Check that all of are characters are numbers. If we detect a non-number, return the default NaN.
			for (std::size_t i = 0; arg[i] != '\0'; ++i) // NOLINT
			{
				if (arg[i] < '0' || arg[i] > '9')
				{
					return std::numeric_limits<float>::quiet_NaN(); // Default NaN
				}
			}
		}

		if (has_hex_been_detected)
		{
			// Calculate tag_value by handling wrapping for numbers larger than 8 digits
			for (std::size_t i = 0; arg[i] != '\0'; ++i)
			{
				flt_bits *= 16;
				flt_bits += static_cast<uint8_t>(ccm::helpers::digit_to_int(arg[i])); // Convert ASCII to numeric value
				if (i >= 7)
				{
					flt_bits %= static_cast<uint32_t>(1e8); // Wrap around for numbers larger than 8 digits
				}
			}
		}
		else
		{
			// Calculate tag_value by handling wrapping for numbers larger than 8 digits
			for (std::size_t i = 0; arg[i] != '\0'; ++i)
			{
				flt_bits *= 10;
				flt_bits += static_cast<uint8_t>(ccm::helpers::digit_to_int(arg[i])); // Convert ASCII to numeric value
				if (i >= 7)
				{
					flt_bits %= static_cast<uint32_t>(1e8); // Wrap around for numbers larger than 8 digits
				}
			}
		}

		// NOLINTEND

		// Set the tag bits for NaN
		// flt_bits |= UINT32_C(0x7F800000);
		flt_bits |= ccm::support::bit_cast<std::uint32_t>(std::numeric_limits<float>::quiet_NaN());

		// Convert the bits to a float
		return ccm::support::bit_cast<float>(flt_bits);
	}
} // namespace ccm::internal::impl
