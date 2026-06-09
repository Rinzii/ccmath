/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <ccmath/ccmath.hpp>
#include <ccmath/internal/support/bits.hpp>

#include <cstdint>

namespace
{
	constexpr float kUpwardSensitive = ccm::fmaf(0x1.fffep-1F, 0x1.fffep-1F, -0x1.ep-2F);

	static_assert(ccm::support::bit_cast<std::uint32_t>(kUpwardSensitive) == ccm::support::bit_cast<std::uint32_t>(0x1.0ffc02p-1F),
				  "CCM_CONSTEXPR_ROUNDING_MODE=FE_UPWARD must affect constexpr ccm::fmaf");
} // namespace

int main()
{ return 0; }
