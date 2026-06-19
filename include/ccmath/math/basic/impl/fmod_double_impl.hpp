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

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"

#include <cstdint>

namespace ccm::internal
{
	namespace impl
	{
		// Exact floating-point remainder for double using the fdlibm integer bit-reduction.
		// The quotient is reduced one binary digit at a time on the raw significand integers, so
		// the result is exact for every magnitude and is independent of the active rounding mode,
		// unlike the x - trunc(x / y) * y formula which loses bits once x / y is not representable.
		// NOLINTNEXTLINE(readability-function-cognitive-complexity)
		constexpr double fmod_double_impl(double x, double y) noexcept
		{
			std::int64_t hx = support::double_to_int64(x);
			std::int64_t hy = support::double_to_int64(y);

			// Sign of the result follows x.
			const std::int64_t sx = hx & static_cast<std::int64_t>(0x8000000000000000ULL);

			hx &= 0x7fffffffffffffffLL; // |x|
			hy &= 0x7fffffffffffffffLL; // |y|

			// Purge exceptional inputs: y is zero, x is inf or NaN, or y is NaN. The (x * y) / (x * y)
			// form yields the IEEE-mandated NaN for these cases.
			if (CCM_UNLIKELY(hy == 0 || hx >= 0x7ff0000000000000LL || hy > 0x7ff0000000000000LL)) { return (x * y) / (x * y); }

			// |x| < |y| leaves x unchanged (this also covers x == 0). |x| == |y| gives signed zero.
			if (hx < hy) { return x; }
			if (hx == hy) { return support::int64_to_double(sx); }

			// Unbiased exponent of x, handling subnormals by normalizing the leading significand bit.
			int ix = 0;
			if (hx < 0x0010000000000000LL)
			{
				ix = -1022;
				for (std::int64_t i = hx << 11; i > 0; i <<= 1) { ix -= 1; }
			}
			else
			{
				ix = static_cast<int>(hx >> 52) - 1023;
			}

			// Unbiased exponent of y.
			int iy = 0;
			if (hy < 0x0010000000000000LL)
			{
				iy = -1022;
				for (std::int64_t i = hy << 11; i > 0; i <<= 1) { iy -= 1; }
			}
			else
			{
				iy = static_cast<int>(hy >> 52) - 1023;
			}

			// Promote both significands to integers with the implicit bit made explicit (subnormals are
			// shifted up so their leading set bit sits in the same position a normal significand would).
			if (ix >= -1022) { hx = 0x0010000000000000LL | (0x000fffffffffffffLL & hx); }
			else
			{
				hx <<= (-1022 - ix);
			}
			if (iy >= -1022) { hy = 0x0010000000000000LL | (0x000fffffffffffffLL & hy); }
			else
			{
				hy <<= (-1022 - iy);
			}

			// Fixed-point remainder: shift-and-subtract for each binary digit of the quotient.
			int n = ix - iy;
			while (n--)
			{
				const std::int64_t hz = hx - hy;
				if (hz < 0) { hx = hx + hx; }
				else
				{
					if (hz == 0) { return support::int64_to_double(sx); }
					hx = hz + hz;
				}
			}
			const std::int64_t hz = hx - hy;
			if (hz >= 0) { hx = hz; }

			// An exact zero remainder keeps the sign of x.
			if (hx == 0) { return support::int64_to_double(sx); }

			// Renormalize the remainder back into a floating-point significand.
			while (hx < 0x0010000000000000LL)
			{
				hx = hx + hx;
				iy -= 1;
			}
			if (iy >= -1022) // normal result
			{
				hx = (hx - 0x0010000000000000LL) | (static_cast<std::int64_t>(iy + 1023) << 52);
			}
			else // subnormal result
			{
				hx >>= (-1022 - iy);
			}
			return support::int64_to_double(hx | sx);
		}
	} // namespace impl

	constexpr double fmod_double(double x, double y) noexcept
	{ return impl::fmod_double_impl(x, y); }
} // namespace ccm::internal
