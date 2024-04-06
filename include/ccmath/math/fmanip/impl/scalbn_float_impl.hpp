/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <cstdint>
#include <limits>

namespace ccm::internal::impl
{
	constexpr float scalbn_float_impl(float arg, int exp) noexcept
	{
		ccm::float_t tmp = arg;

		if (exp > 127)
		{
			tmp *= 0x1p127F;
			exp -= 127;
			if (exp > 127)
			{
				tmp *= 0x1p127F;
				exp -= 127;
				if (exp > 127) { exp = 127; }
			}
		}
		else if (exp < -126)
		{
			tmp *= 0x1p-126F * 0x1p24F;
			exp += 126 - 24;
			if (exp < -126)
			{
				tmp *= 0x1p-126F * 0x1p24F;
				exp += 126 - 24;
				if (exp < -126) { exp = -126; }
			}
		}

		const std::uint32_t bits = ccm::support::bit_cast<std::uint32_t>(127 + exp) << 23;
		arg						 = tmp * ccm::support::bit_cast<float>(bits);

		return arg;
	}

} // namespace ccm::internal::impl
