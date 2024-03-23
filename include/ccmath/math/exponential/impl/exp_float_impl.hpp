/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/helpers/exp_helpers.hpp"
#include "ccmath/internal/types/fp_types.hpp"
#include <cstdint>
#include <type_traits>

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
			inline constexpr float exp_float_impl(float x)
			{
				return 0;
			}

		} // namespace impl
	}	  // namespace
} // namespace ccm::internal
