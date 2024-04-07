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
#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/basic/fmod.hpp"

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
			inline constexpr long double remquo_ldouble_impl(long double x, long double y, int * quo) noexcept
			{
				std::int32_t x_exponent{};
				std::int32_t y_exponent{};
				std::int32_t x_int32{};
				std::int32_t y_int32{};

				std::uint32_t x_sign{};
				std::uint32_t y_msb{};
				std::uint32_t x_lsb{};

				int quotient_sign{};
				int computed_quotient{};

				// TODO: For the time being this is mega on hold until we have access to
				//       necessary implementations of int128_t. Without them extracting the
				//       needed bits to perform the necessary operations would be extremely challenging
				//       due to the fact that long double is extremely platform dependent.

				return 0;
			}
		} // namespace impl
	} // namespace

	template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	inline constexpr T remquo_ldouble(T x, T y, int * quo) noexcept
	{
		return static_cast<T>(impl::remquo_ldouble_impl(x, y, quo));
	}
} // namespace ccm::internal
