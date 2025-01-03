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

#include <cstdint>
#include <limits>

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/math/basic/fabs.hpp"
#include "ccmath/math/basic/fmod.hpp"

namespace ccm::internal
{
	namespace impl
	{
		constexpr long double remquo_ldouble_impl(long double x, long double y, int * quo) noexcept
		{
		/*
			std::int32_t x_exponent{};
			std::int32_t y_exponent{};
			std::int32_t x_int32{};
			std::int32_t y_int32{};

			std::uint32_t x_sign{};
			std::uint32_t y_msb{};
			std::uint32_t x_lsb{};

			int quotient_sign{};
			int computed_quotient{};
		 */
			// TODO: For the time being this is mega on hold until we have access to
			//       necessary implementations of int128_t. Without them extracting the
			//       needed bits to perform the necessary operations would be extremely challenging
			//       due to the fact that long double is extremely platform dependent.

			return 0;
		}
	} // namespace impl

	constexpr long double remquo_ldouble(long double x, long double y, int * quo) noexcept
	{
		return impl::remquo_ldouble_impl(x, y, quo);
	}
} // namespace ccm::internal
