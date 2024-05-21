/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/compare/signbit.hpp"

#include <limits>
#include <type_traits>

namespace ccm::internal::impl
{

	constexpr long double fma_ldouble(long double x, long double y, long double z) noexcept
    {

		if (CCM_UNLIKELY(x == 0 || y == 0 || z == 0)) { return x * y + z; }

		// If x is zero and y is infinity, or if y is zero and x is infinity and...
		if ((x == 0 && ccm::isinf(y)) || (y == 0 && ccm::isinf(x)))
		{
			// ...z is NaN, return +NaN...
			if (ccm::isnan(z))
			{
				return std::numeric_limits<long double>::quiet_NaN();
			}

			// ...else return -NaN if Z is not NaN.
			return -std::numeric_limits<long double>::quiet_NaN();
		}

		// If x is zero and y is infinity, or if y is zero and x is infinity and Z is NaN, then the result is -NaN.
		if (ccm::isinf(x * y) && ccm::isinf(z) && ccm::signbit(x * y) != ccm::signbit(z))
		{
			return -std::numeric_limits<long double>::quiet_NaN();
		}

		// If x or y are NaN, NaN is returned.
		if (ccm::isnan(x) || ccm::isnan(y)) { return std::numeric_limits<long double>::quiet_NaN(); }

		// If z is NaN, and x * y is not 0 * Inf or Inf * 0, then +NaN is returned
		// On clang this will not return a consistent sign bit due to how NaN is returned based on specified compiler flag.
		if (ccm::isnan(z) && (x * y != 0 * std::numeric_limits<long double>::infinity() || x * y != std::numeric_limits<long double>::infinity() * 0))
		{
			return -std::numeric_limits<long double>::quiet_NaN();
		}

		// Hope the compiler optimizes this.
		return (x * y) + z;
    }

} // namespace ccm::internal::impl