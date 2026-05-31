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

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/expo/impl/exp2_double_impl.hpp"
#include "ccmath/math/expo/impl/log2_double_impl.hpp"
#include "ccmath/math/nearest/floor.hpp"

#include <limits>
#include <type_traits>

namespace ccm::gen::impl
{
	namespace internal::impl
	{
		constexpr bool is_odd_integer(double val) noexcept
		{
			using FPBits_t						 = support::fp::FPBits<double>;
			const std::uint64_t x_u				 = ccm::support::bit_cast<std::uint64_t>(val);
			const auto x_e						 = static_cast<std::int32_t>((x_u & FPBits_t::exponent_mask) >> FPBits_t::fraction_length);
			const std::int32_t lsb				 = support::countr_zero(x_u | FPBits_t::exponent_mask);
			constexpr std::int32_t unit_exponent = FPBits_t::exponent_bias + static_cast<std::int32_t>(FPBits_t::fraction_length);
			return (x_e + lsb == unit_exponent);
		}

		constexpr double pow_impl(double base, double exp) noexcept
		{
			if (exp == 0.0) { return 1.0; }
			if (base == 1.0) { return 1.0; }

			if (ccm::isnan(base) || ccm::isnan(exp)) { return std::numeric_limits<double>::quiet_NaN(); }

			if (base == 0.0)
			{
				if (exp < 0.0) { return std::numeric_limits<double>::infinity(); }
				return 0.0;
			}

			if (base < 0.0 && exp != ccm::floor(exp)) { return std::numeric_limits<double>::quiet_NaN(); }

			if (base == std::numeric_limits<double>::infinity()) { return exp < 0.0 ? 0.0 : std::numeric_limits<double>::infinity(); }

			if (exp == std::numeric_limits<double>::infinity())
			{
				using FPBits		= support::fp::FPBits<double>;
				const auto base_abs = FPBits(FPBits(base).abs()).uintval();
				if (base_abs == FPBits(1.0).uintval()) { return 1.0; }
				return (base_abs < FPBits(1.0).uintval()) == (exp < 0.0) ? 0.0 : std::numeric_limits<double>::infinity();
			}

			if (exp == -std::numeric_limits<double>::infinity())
			{
				using FPBits		= support::fp::FPBits<double>;
				const auto base_abs = FPBits(FPBits(base).abs()).uintval();
				if (base_abs == FPBits(1.0).uintval()) { return 1.0; }
				return (base_abs < FPBits(1.0).uintval()) ? std::numeric_limits<double>::infinity() : 0.0;
			}

			double result_sign = 1.0;
			if (base < 0.0)
			{
				using FPBits = support::fp::FPBits<double>;
				if (is_odd_integer(exp)) { result_sign = -1.0; }
				base = FPBits(FPBits(base).abs()).get_val();
			}

			return result_sign * ccm::internal::exp2_double(exp * ccm::internal::log2_double(base));
		}
	} // namespace internal::impl

	constexpr double pow_impl(double base, double exp) noexcept
	{
		return internal::impl::pow_impl(base, exp);
	}

} // namespace ccm::gen::impl
