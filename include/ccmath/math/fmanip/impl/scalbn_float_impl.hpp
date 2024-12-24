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

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/types/fp_types.hpp"
#include "ccmath/math/basic/max.hpp"
#include "ccmath/math/basic/min.hpp"

#include <cstdint>

#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
CCM_DISABLE_MSVC_WARNING(4756) // 4756: overflow in constant arithmetic

namespace ccm::internal
{
	namespace impl
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
					tmp *= 0x1p127F; // This is what we are disabling the warning for.
					exp -= 127;
					exp = ccm::min(exp, 127);
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
					exp = ccm::max(exp, -126);
				}
			}

			const std::uint32_t bits = ccm::support::bit_cast<std::uint32_t>(127 + exp) << 23;
			arg						 = tmp * ccm::support::bit_cast<float>(bits);

			return arg;
		}
	} // namespace impl

	constexpr float scalbn_float(float arg, int exp) noexcept
	{
		return impl::scalbn_float_impl(arg, exp);
	}

	constexpr float scalbn_float(float arg, long exp) noexcept
	{
		return impl::scalbn_float_impl(arg, static_cast<int>(exp));
	}
} // namespace ccm::internal

CCM_RESTORE_MSVC_WARNING()
