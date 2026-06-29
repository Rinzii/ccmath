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
	// Native long-double powl exists only when long double is x87 binary80
	// (CCM_TYPES_LONG_DOUBLE_IS_FLOAT80). On other formats powl uses the double pow kernel
	// by default. That path is source compatible but not native long-double precision yet.
	// Set CCMATH_DISABLE_REDUCED_PRECISION_POWL=ON to return quiet NaN on incomplete tiers.
	constexpr bool reduced_precision_powl_fallback_enabled() noexcept
	{
#ifdef CCM_CONFIG_DISABLE_REDUCED_PRECISION_POWL
		return false;
#else
		return true;
#endif
	}

} // namespace ccm::config
