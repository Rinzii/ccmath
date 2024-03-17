/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/abs.hpp"
#include "ccmath/numbers.hpp"

#include <limits>

namespace ccm
{
	namespace
	{
		namespace impl
		{

		} // namespace impl
	}	  // namespace

	template <typename T>
	inline constexpr T exp(T x)
	{
		return x;
	}

} // namespace ccm
