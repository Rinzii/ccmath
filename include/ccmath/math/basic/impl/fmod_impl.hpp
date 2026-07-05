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
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/fmanip/copysign.hpp"
#include "ccmath/math/fmanip/scalbn.hpp"

#include <cstdint>
#include <limits>
#include <type_traits>

namespace ccm::internal
{
	namespace impl
	{
		// Exact floating-point remainder for any IEEE type, read straight off FPBits. Each magnitude is
		// an integer significand scaled by a power of two, abs(v) == get_explicit_mantissa(v) *
		// 2^(get_explicit_exponent(v) - mant_bits), an identity FPBits gives for normals and subnormals
		// alike. The remainder of abs(x) by abs(y) is then
		// ((sig_x * 2^gap) mod sig_y) * 2^(exp_y - mant_bits), with gap the difference of the two binade
		// exponents. The modular product is built a chunk of bits at a time with 64-bit arithmetic, so
		// the loop runs in gap / chunk steps instead of one step per bit, and the result stays exact for
		// every magnitude and independent of the active rounding mode.
		template <typename T> constexpr T fmod_impl(T x, T y) noexcept
		{
			static_assert(std::is_floating_point_v<T>, "fmod_impl requires a floating-point type");
			// The reduction lifts the significand into a uint64_t and shifts it by chunk = 63 - mant_bits
			// bits, so it only supports types up to binary64. Wider types (80-bit or 128-bit long double)
			// would give a non-positive chunk and must reduce through the double kernel at the call site.
			static_assert(std::numeric_limits<T>::digits <= 53, "fmod_impl only supports types up to binary64");
			using FPBits_t = support::fp::FPBits<T>;

			const FPBits_t x_bits(x);
			const FPBits_t y_bits(y);

			// NaN for an infinite dividend, a zero divisor, or a NaN argument.
			if (CCM_UNLIKELY(x_bits.is_inf() || y_bits.is_zero() || x_bits.is_nan() || y_bits.is_nan()))
			{
				return FPBits_t::quiet_nan().get_val();
			}

			// Compare magnitudes through the exponent and significand bits. For finite values that integer
			// order matches the floating-point order, so this also covers fmod(x, +/-inf) = x.
			const auto x_mag = x_bits.abs().uintval();
			const auto y_mag = y_bits.abs().uintval();

			// Any abs(x) < abs(y) returns x unchanged.
			if (x_mag < y_mag)
			{
				return x;
			}
			// abs(x) == abs(y) leaves a zero that keeps the sign of x.
			if (x_mag == y_mag)
			{
				return ccm::copysign(static_cast<T>(0), x);
			}

			constexpr int mant_bits = std::numeric_limits<T>::digits - 1;
			// Largest shift that keeps a reduced significand (below 2^(mant_bits + 1)) inside 64 bits.
			constexpr int chunk = 63 - mant_bits;

			const int exp_x			  = x_bits.get_explicit_exponent();
			const int exp_y			  = y_bits.get_explicit_exponent();
			const std::uint64_t sig_y = static_cast<std::uint64_t>(y_bits.get_explicit_mantissa());
			std::uint64_t r			  = static_cast<std::uint64_t>(x_bits.get_explicit_mantissa()) % sig_y;

			for (int gap = exp_x - exp_y; gap > 0;)
			{
				const int step = gap < chunk ? gap : chunk;
				r			   = (r << step) % sig_y;
				gap -= step;
			}

			if (r == 0)
			{
				return ccm::copysign(static_cast<T>(0), x);
			}
			return ccm::copysign(ccm::scalbn(static_cast<T>(r), exp_y - mant_bits), x);
		}
	} // namespace impl

	// Thin wrapper so the public dispatch reads the same as the other basic functions. float and
	// double evaluate natively. long double is reduced through the double kernel by the caller,
	// matching the existing fmodl and remquol convention.
	template <typename T> constexpr T fmod(T x, T y) noexcept
	{
		return impl::fmod_impl(x, y);
	}
} // namespace ccm::internal
