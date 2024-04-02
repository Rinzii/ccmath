/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

#include <absl/numeric/int128.h>

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
	#include "ccmath/math/fmanip/impl/scalbn_double_impl.hpp"
#endif

#include <array>
#include <cstdint>
#include <limits>

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
			inline constexpr long double scalbn_ldouble_impl(long double arg, int exp) noexcept
			{
				return ccm::internal::impl::scalbn_double_impl(arg, exp);
			}
		}
#elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113) && LDBL_MAX_EXP == 16384
			inline constexpr long double scalbn_ldouble_impl(long double arg, int exp) noexcept
			{
				// TODO: Handle special cases.
				// TODO: Return num * FLT_RADIX^exp once pow has been implemented.
				return 0;
			}
#endif
		} // namespace impl
	} // namespace
} // namespace ccm::internal
