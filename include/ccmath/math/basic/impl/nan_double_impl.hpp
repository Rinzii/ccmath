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

#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/helpers/digit_to_int.hpp"

#include <cstdint>
#include <limits>

CCM_DISABLE_MSVC_WARNING(4702) // 4702: unreachable code

namespace ccm::internal::impl
{

	constexpr double nan_double_impl(const char * arg) noexcept
	{
		if constexpr (!std::numeric_limits<double>::is_iec559)
		{
			return 0.0;
		}

#if (defined(_MSC_VER))
		// MSVC and clang-cl both return a quiet NaN without honoring a payload.
		// Skip payload parsing on all _MSC_VER toolchains.
		return std::numeric_limits<double>::quiet_NaN();
#endif

		std::uint64_t dbl_bits{ 0 };
		bool has_hex_been_detected{ false };

		if (arg == nullptr)
		{
			return std::numeric_limits<double>::quiet_NaN(); // Default NaN
		}

		// NOLINTBEGIN
		if (arg[0] == '\0') //
		{
			return std::numeric_limits<double>::quiet_NaN(); // Default NaN
		}

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
					return std::numeric_limits<double>::quiet_NaN(); // Default NaN
				}
			}
		}

		if (has_hex_been_detected)
		{
			for (std::size_t i = 0; arg[i] != '\0'; ++i)
			{
				dbl_bits *= 16;
				dbl_bits += static_cast<std::uint8_t>(ccm::support::helpers::digit_to_int(arg[i]));
				if (i >= 15)
				{
					dbl_bits %= static_cast<std::uint64_t>(1e18);
				}
			}
		} else
		{
			for (std::size_t i = 0; arg[i] != '\0'; ++i)
			{
				dbl_bits *= 10;
				dbl_bits += static_cast<std::uint8_t>(arg[i] - '0');
				if (i >= 15)
				{
					dbl_bits %= static_cast<std::uint64_t>(1e18);
				}
			}
		}
		// NOLINTEND

		dbl_bits |= ccm::support::bit_cast<std::uint64_t>(std::numeric_limits<double>::quiet_NaN());

		return ccm::support::bit_cast<double>(dbl_bits);
	}
} // namespace ccm::internal::impl

CCM_RESTORE_MSVC_WARNING()
