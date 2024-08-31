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
		constexpr float log_impl(float num) noexcept
		{
			//constexpr double ln2 = 0x1.62e42fefa39efp-1;
			//using FPBits_t = typename support::fp::FPBits<float>;

			//FPBits_t num_bits(num);

			//std::uint32_t num_bits_storage = num_bits.uintval();

			//int m = -FPBits_t::exponent_bias;



			return 0;
		}

		constexpr double log_impl(double num) noexcept
		{
			return 0;
		}

		constexpr long double log_impl(long double num) noexcept
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