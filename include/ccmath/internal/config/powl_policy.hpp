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

namespace ccm::config
{
	// When enabled, incomplete powl tiers may delegate to the double pow_impl path.
	// This is reduced precision and must not be reported as native long-double support.
	// Default is OFF. Set CCMATH_ENABLE_REDUCED_PRECISION_POWL=ON for source compatibility.
	constexpr bool reduced_precision_powl_fallback_enabled() noexcept
	{
#if defined(CCM_CONFIG_ENABLE_REDUCED_PRECISION_POWL)
		return true;
#else
		return false;
#endif
	}

} // namespace ccm::config
