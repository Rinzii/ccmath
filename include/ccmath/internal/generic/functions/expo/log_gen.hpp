/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/floating_point_traits.hpp"
#include "ccmath/internal/support/fp/bit_mask_traits.hpp"
#include "ccmath/internal/support/fp/directional_rounding_utils.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/support/unreachable.hpp"
#include "ccmath/internal/types/big_int.hpp"
#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/power/sqrt.hpp"

#include <limits>

namespace ccm::gen
{
	namespace internal::impl
	{

		template <typename T>
		constexpr std::enable_if_t<std::is_floating_point_v<T>, T> log_impl(T x, T y) noexcept
		{

			return 0;
		}
	} // namespace internal::impl

	template <typename T>
	constexpr std::enable_if_t<std::is_floating_point_v<T>, T> log_gen(T base, T exp) noexcept
	{
		return internal::impl::log_impl(base, exp);
	}
} // namespace ccm::gen