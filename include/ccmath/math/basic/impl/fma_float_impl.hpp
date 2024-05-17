/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cmath>

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/compare/signbit.hpp"

#include "ccmath/internal/support/bits.hpp"

#include <limits>
#include <type_traits>

namespace ccm::internal::impl
{

	constexpr float fma_float(float x, float y, float z) noexcept
    {
		// Product will be exact
		double product = static_cast<double>(x) * static_cast<double>(y);
		auto z_d = static_cast<double>(z);
		double sum = product + z_d;

		constexpr std::int64_t exponent_bias = (1U << (8 - 1U)) - 1U;
		constexpr std::int64_t infinity = exponent_bias + 1;


		// exponent mask
		constexpr std::uint64_t exponent_mask = 0x7ff0000000000000;

		constexpr std::uint64_t sum_encode = (infinity << 23U) & exponent_mask;

		std::uint64_t sum_exponent_bits = support::bit_cast<std::uint64_t>(sum) & exponent_mask;

		bool sum_is_finite = sum_exponent_bits != sum_encode;

		constexpr std::uint64_t sign_mask = 0x8000000000000000;

		std::uint64_t sum_sign_bit = support::bit_cast<std::uint64_t>(sum) & sign_mask;

		bool sum_is_zero = sum_sign_bit == 0;



		if (!(sum_is_finite || sum_is_zero))
		{

		}





		return 0;
    }

} // namespace ccm::internal::impl