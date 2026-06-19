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
		// Exact floating-point remainder for float using the fdlibm integer bit-reduction.
		// The quotient is reduced one binary digit at a time on the raw significand integers, so
		// the result is exact for every magnitude and is independent of the active rounding mode,
		// unlike the x - trunc(x / y) * y formula which loses bits once x / y is not representable.
		// NOLINTNEXTLINE(readability-function-cognitive-complexity)
		constexpr float fmod_float_impl(float x, float y) noexcept
		{
			std::int32_t hx = support::float_to_int32(x);
			std::int32_t hy = support::float_to_int32(y);

			// Sign of the result follows x.
			const std::int32_t sx = hx & static_cast<std::int32_t>(0x80000000U);

			hx &= 0x7fffffff; // |x|
			hy &= 0x7fffffff; // |y|

			// Purge exceptional inputs: y is zero, x is inf or NaN, or y is NaN. The (x * y) / (x * y)
			// form yields the IEEE-mandated NaN for these cases.
			if (CCM_UNLIKELY(hy == 0 || hx >= 0x7f800000 || hy > 0x7f800000)) { return (x * y) / (x * y); }

			// |x| < |y| leaves x unchanged (this also covers x == 0). |x| == |y| gives signed zero.
			if (hx < hy) { return x; }
			if (hx == hy) { return support::int32_to_float(sx); }

			// Unbiased exponent of x, handling subnormals by normalizing the leading significand bit.
			int ix = 0;
			if (hx < 0x00800000)
			{
				ix = -126;
				for (std::int32_t i = hx << 8; i > 0; i <<= 1) { ix -= 1; }
			}
			else
			{
				ix = (hx >> 23) - 127;
			}

			// Unbiased exponent of y.
			int iy = 0;
			if (hy < 0x00800000)
			{
				iy = -126;
				for (std::int32_t i = hy << 8; i > 0; i <<= 1) { iy -= 1; }
			}
			else
			{
				iy = (hy >> 23) - 127;
			}

			// Promote both significands to integers with the implicit bit made explicit (subnormals are
			// shifted up so their leading set bit sits in the same position a normal significand would).
			if (ix >= -126) { hx = 0x00800000 | (0x007fffff & hx); }
			else
			{
				hx <<= (-126 - ix);
			}
			if (iy >= -126) { hy = 0x00800000 | (0x007fffff & hy); }
			else
			{
				hy <<= (-126 - iy);
			}

			// Fixed-point remainder: shift-and-subtract for each binary digit of the quotient.
			int n = ix - iy;
			while (n--)
			{
				const std::int32_t hz = hx - hy;
				if (hz < 0) { hx = hx + hx; }
				else
				{
					if (hz == 0) { return support::int32_to_float(sx); }
					hx = hz + hz;
				}
			}
			const std::int32_t hz = hx - hy;
			if (hz >= 0) { hx = hz; }

			// An exact zero remainder keeps the sign of x.
			if (hx == 0) { return support::int32_to_float(sx); }

			// Renormalize the remainder back into a floating-point significand.
			while (hx < 0x00800000)
			{
				hx = hx + hx;
				iy -= 1;
			}
			if (iy >= -126) // normal result
			{
				hx = (hx - 0x00800000) | ((iy + 127) << 23);
			}
			else // subnormal result
			{
				hx >>= (-126 - iy);
			}
			return support::int32_to_float(hx | sx);
		}
	} // namespace impl

	constexpr float fmod_float(float x, float y) noexcept
	{ return impl::fmod_float_impl(x, y); }
} // namespace ccm::internal
