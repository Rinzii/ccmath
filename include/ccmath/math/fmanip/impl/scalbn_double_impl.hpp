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
#include "ccmath/math/basic/max.hpp"
#include "ccmath/math/basic/min.hpp"

#include <cstdint>

namespace ccm::internal
{
	namespace impl
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
					exp = ccm::min(exp, 1023);
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
					exp = ccm::max(exp, -1022);
				}
			}

			const std::uint64_t bits = ccm::support::bit_cast<std::uint64_t>(1023 + static_cast<std::uint64_t>(exp)) << 52;
			arg						 = tmp * ccm::support::bit_cast<double>(bits);

			return arg;
		}
	} // namespace impl

	constexpr double scalbn_double(double arg, int exp) noexcept
	{
		return impl::scalbn_double_impl(arg, exp);
	}

	constexpr double scalbn_double(double arg, long exp) noexcept
	{
		return impl::scalbn_double_impl(arg, static_cast<int>(exp));
	}
} // namespace ccm::internal
