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
#include "ccmath/math/exponential/exp2.hpp"
#include "ccmath/math/exponential/exp10.hpp"

namespace ccm::internal
{
	namespace impl
	{

		constexpr float pow_float_impl(float base, float exp) noexcept
		{
			if (base == 2.0f)
			{
				return ccm::exp2<float>(exp);
			}

			// TODO: Uncomment this code to use ccm::exp10 once it has been implemented.
			/*
			if (base == 10.0f)
			{
				return ccm::exp10<float>(exp);
			}
			*/




			return 0;
		}

	} // namespace impl

	constexpr float pow_float(float base, float exp) noexcept
	{
		return impl::pow_float_impl(base, exp);
	}
} // namespace ccm::internal