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

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{

            inline constexpr double nan_double_impl(const char* arg) noexcept
            {
				static_assert(std::numeric_limits<double>::is_iec559, "IEEE-754 representation required for this implementation");

				std::uint64_t dbl_bits {0};
				bool has_hex_been_detected {false};

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
							return std::numeric_limits<double>::quiet_NaN(); // Default NaN
						}
					}
                }

				if (has_hex_been_detected)
				{
					// Calculate tag_value by handling wrapping for numbers larger than 8 digits
                    for (std::size_t i = 0; arg[i] != '\0'; ++i) {
                        dbl_bits *= 16;
                        dbl_bits += static_cast<uint8_t>(ccm::helpers::digit_to_int(arg[i])); // Convert ASCII to numeric value
                        if (i >= 15) {
                            dbl_bits %= static_cast<uint64_t>(1e18); // Wrap around for numbers larger than 8 digits
                        }
                    }
				}
				else
                {
                    // Calculate tag_value by handling wrapping for numbers larger than 8 digits
                    for (std::size_t i = 0; arg[i] != '\0'; ++i) {
                        dbl_bits *= 10;
                        dbl_bits += static_cast<uint8_t>(arg[i] - '0'); // Convert ASCII to numeric value
                        if (i >= 15) {
                            dbl_bits %= static_cast<uint64_t>(1e18); // Wrap around for numbers larger than 8 digits
                        }
                    }
                }
				// NOLINTEND

				// Set the tag bits for NaN
				dbl_bits |= UINT64_C(0x7FF8000000000000);

				// Convert the uint64_t tag into a double NaN
				return ccm::helpers::bit_cast<double>(dbl_bits);
            }
		} // namespace impl
	} // namespace
} // namespace ccm::internal
