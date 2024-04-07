/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/types/fp_types.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <cstdint>
#include <limits>

namespace ccm::internal::impl
{
	constexpr double scalbn_double_impl(double arg, int exp) noexcept
	{
		ccm::double_t tmp = arg;

		if (exp > 1023)
		{
			tmp *= 0x1p1023;
			exp -= 1023;
			if (exp > 1023)
			{
				tmp *= 0x1p1023;
				exp -= 1023;
				if (exp > 1023) { exp = 1023; }
			}
		}
		else if (exp < -1022)
		{
			tmp *= 0x1p-1022 * 0x1p53;
			exp += 1022 - 53;
			if (exp < -1022)
			{
				tmp *= 0x1p-1022 * 0x1p53;
				exp += 1022 - 53;
				if (exp < -1022) { exp = -1022; }
			}
		}

		const std::uint64_t bits = ccm::support::bit_cast<std::uint64_t>(1023 + static_cast<std::uint64_t>(exp)) << 52;
		arg						 = tmp * ccm::support::bit_cast<double>(bits);

		return arg;
	}
} // namespace ccm::internal::impl
