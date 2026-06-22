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

#include "ccmath/internal/predef/compiler_suppression/msvc_compiler_suppression.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/fenv/fenv_support.hpp"
#include "ccmath/internal/support/fenv/host_fenv.hpp"
#include "ccmath/internal/support/fp/fp_bits.hpp"
#include "ccmath/internal/types/fp_types.hpp"
#include "ccmath/math/expo/impl/exp2_data.hpp"

#include <cerrno>
#include <cstdint>
#include <limits>

// TODO: Replace with more modern implementation.
CCM_DISABLE_MSVC_WARNING(4056)

namespace ccm::internal
{
	namespace impl
	{
		constexpr float exp2_float_overflow_result()
		{
			using FloatBits = ccm::support::fp::FPBits<float>;

			if (!support::is_constant_evaluated())
			{
				support::fenv::set_errno_if_required(ERANGE);
				support::fenv::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
			}

			switch (support::fenv::get_rounding_mode())
			{
			case FE_DOWNWARD:
			case FE_TOWARDZERO: return FloatBits::max_normal().get_val();
			case FE_UPWARD:
			case FE_TONEAREST: [[fallthrough]];
			default: return FloatBits::inf().get_val();
			}
		}

		constexpr float exp2_float_underflow_result()
		{
			using FloatBits = ccm::support::fp::FPBits<float>;

			if (!support::is_constant_evaluated())
			{
				support::fenv::set_errno_if_required(ERANGE);
				support::fenv::raise_except_if_required(FE_UNDERFLOW | FE_INEXACT);
			}

			return support::fenv::get_rounding_mode() == FE_UPWARD ? FloatBits::min_subnormal().get_val() : 0.0F;
		}

		constexpr float exp2_float_impl(float x)
		{
			constexpr auto exp2_data_float		   = ccm::internal::exp2_data<float>();
			constexpr auto lookup_table			   = exp2_data_float.tab;
			constexpr auto polynomial_coefficients = exp2_data_float.poly;
			constexpr auto shift_scaled			   = exp2_data_float.shift_scaled;
			constexpr auto table_bits			   = exp2_data<float>::table_bits;
			constexpr auto shifted_table_bits	   = exp2_data<float>::shifted_table_bits;

			namespace sp = support;

			std::uint64_t intermediate_value{};
			ccm::double_t corrected_input_double{};
			ccm::double_t result{};

			const auto input_as_double = static_cast<ccm::double_t>(x);
			const auto x_bits		   = support::fp::FPBits<float>(x);

			// If |x| >= 128 or x is NaN, then we need to handle the special case.
			if (const std::uint32_t abs_top = sp::top12_bits_of_float(x) & 0x7ff; CCM_UNLIKELY(abs_top >= sp::top12_bits_of_float(128.0F)))
			{
				if (x_bits.is_inf()) { return x_bits.is_neg() ? 0.0F : x; }

				// Use floating-point classification instead of raw unsigned bit comparisons so
				// large negative finite inputs do not alias the NaN/Inf branch.
				if (x_bits.is_nan()) { return x + x; }

				// Handle overflow
				if (x > 0.0F) { return exp2_float_overflow_result(); }

				// Handle underflow
				if (x <= -150.0F) { return exp2_float_underflow_result(); }

				// Handle errno underflow. We assume we want errno behavior for underflow.
				if (x < -149.0F) { return exp2_float_underflow_result(); }
			}

			// x = k/N + r with r in [-1/(2N), 1/(2N)] and int k.
			corrected_input_double			 = static_cast<double>(input_as_double + shift_scaled);
			const std::uint64_t lookup_index = sp::double_to_uint64(corrected_input_double);
			corrected_input_double -= shift_scaled; // This is k/N for int k
			const ccm::double_t rem = input_as_double - corrected_input_double;

			// exp2(x) = 2^(k/N) * 2^r ~= s * (C0*r^3 + C1*r^2 + C2*r + 1)
			intermediate_value = lookup_table.at(lookup_index % shifted_table_bits);
			intermediate_value += lookup_index << (52 - table_bits);
			const ccm::double_t scale					= sp::uint64_to_double(intermediate_value);
			const ccm::double_t polynomial_intermediate = polynomial_coefficients.at(0) * rem + polynomial_coefficients.at(1);
			const ccm::double_t remSqr					= rem * rem;
			result										= polynomial_coefficients.at(2) * rem + 1;
			result										= polynomial_intermediate * remSqr + result;
			result										= result * scale;

			return static_cast<float>(result);
		}
	} // namespace impl

	constexpr float exp2_float(float x)
	{ return impl::exp2_float_impl(x); }
} // namespace ccm::internal
CCM_RESTORE_MSVC_WARNING()
