/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstdint>
#include <limits>
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
			inline constexpr double fma_ldouble_impl(double x, double y, double z) noexcept
			{

				// #pragma STDC FENV_ACCESS ON

				return 0;
			}
		} // namespace impl
	} // namespace
} // namespace ccm::internal
