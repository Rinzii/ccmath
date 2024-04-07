/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/helpers/exp_helpers.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/types/fp_types.hpp"
#include "ccmath/math/exponential/impl/exp_data.hpp"
#include <cstdint>
#include <limits>

namespace ccm::internal::impl
{
	constexpr ccm::internal::exp_data<double> internal_exp_data_dbl = ccm::internal::exp_data<double>();
	constexpr auto exp_invLn2N_dbl									= internal_exp_data_dbl.invln2N;
	constexpr auto exp_negLn2HiN_dbl								= internal_exp_data_dbl.negln2hiN;
	constexpr auto exp_negLn2LoN_dbl								= internal_exp_data_dbl.negln2loN;
	constexpr auto exp_shift_dbl									= internal_exp_data_dbl.shift;
	constexpr auto exp_tab_dbl										= internal_exp_data_dbl.tab;
	constexpr auto exp_poly_coeff_one_dbl							= internal_exp_data_dbl.poly[5 - k_exp_poly_order_dbl];
	constexpr auto exp_poly_coeff_two_dbl							= internal_exp_data_dbl.poly[6 - k_exp_poly_order_dbl];
	constexpr auto exp_poly_coeff_three_dbl							= internal_exp_data_dbl.poly[7 - k_exp_poly_order_dbl];
	constexpr auto exp_poly_coeff_four_dbl							= internal_exp_data_dbl.poly[8 - k_exp_poly_order_dbl];
	constexpr auto k_exp_table_n_dbl								= (1 << ccm::internal::k_exp_table_bits_dbl);

	constexpr double handle_special_case(ccm::double_t tmp, std::uint64_t sign_bits, std::uint64_t exponent_int64) // NOLINT
	{
		ccm::double_t scale{};
		ccm::double_t result{};

		if ((exponent_int64 & 0x80000000) == 0)
		{
			// If k > 0 then the exponent of scale might have overflowed by <= 460.
			sign_bits -= 1009ULL << 52;
			scale  = ccm::support::uint64_to_double(sign_bits);
			result = 0x1p1009 * (scale + scale * tmp);
			return result;
		}

		// If k < 0 then we need special care in the subnormal range.
		sign_bits += 1022ULL << 52;
		scale  = ccm::support::uint64_to_double(sign_bits);
		result = scale + scale * tmp;
		if (result < 1.0)
		{
			// We need to round y into the correct precision before we can begin scaling it into
			// the subnormal range so that we can avoid double round that could cause a 0.5+E/2 ulp error
			// where E is the worst case ulp error outside the subnormal range.
			ccm::double_t hi{};
			ccm::double_t lo{};
			lo	   = scale - result + scale * tmp;
			hi	   = 1.0 + result;
			lo	   = 1.0 - hi + result + lo;
			result = ccm::helpers::narrow_eval(hi + lo) - 1.0;
			// Prevent -0.0 with downward rounding.
			if (result == 0.0) { result = 0.0; }
		}


		result = 0x1p-1022 * result;

		return result;
	}

	constexpr double exp_double_impl(double x)
	{
		std::uint32_t abs_top{};
		std::uint64_t expo_int64{};
		std::uint64_t index{};
		std::uint64_t top{};
		std::uint64_t sign_bits{};

		ccm::double_t expo{};
		ccm::double_t scaled_input{};
		ccm::double_t rem{};
		ccm::double_t remSqr{};
		ccm::double_t scale{};
		ccm::double_t tail{};
		ccm::double_t tmp{};

		abs_top = ccm::support::top12_bits_of_double(x) & 0x7ff;
		if (CCM_UNLIKELY(abs_top - ccm::support::top12_bits_of_double(0x1p-54) >=
						 ccm::support::top12_bits_of_double(512.0) - ccm::support::top12_bits_of_double(0x1p-54)))
		{
			// Avoid raising underflow for tiny x. 0 is a common input.
			if (abs_top - ccm::support::top12_bits_of_double(0x1p-54) >= 0x80000000) { return 1.0 + x; }

			if (abs_top >= ccm::support::top12_bits_of_double(1024.0))
			{
				if (ccm::support::double_to_uint64(x) == ccm::support::double_to_uint64(-std::numeric_limits<double>::infinity())) { return 0.0; }

				if (abs_top >= ccm::support::top12_bits_of_double(std::numeric_limits<double>::infinity())) { return 1.0 + x; }

				// Handle underflow
				if ((ccm::support::double_to_uint64(x) >> 63) != 0U) { return 0x1p-767 * 0x1p-767; }

				// Handle Overflow
				return 0x1p769 * 0x1p769;
			}
			// A large x is handled below.
			abs_top = 0;
		}

		// exp(x) = 2^(expo/N) * exp(r), with exp(rem) in [2^(-1/2N),2^(1/2N)].
		// x = ln2/N*expo + rem, with int expo and rem in [-ln2/2N, ln2/2N].
		scaled_input = exp_invLn2N_dbl * x;

		// scaled_input - expo is in [-1, 1] in non-nearest rounding modes.
		expo	   = ccm::helpers::narrow_eval(scaled_input + exp_shift_dbl);
		expo_int64 = ccm::support::double_to_uint64(expo);
		expo -= exp_shift_dbl;

		rem = x + expo * exp_negLn2HiN_dbl + expo * exp_negLn2LoN_dbl;

		// 2^(expo/N) ~= scale * (1 + tail).
		index = 2 * (expo_int64 % k_exp_table_n_dbl);
		top	  = expo_int64 << (52 - k_exp_table_bits_dbl);
		tail  = ccm::support::uint64_to_double(exp_tab_dbl.at(index));

		// This is only a valid scale when -1023*N < expo < 1024*N.
		sign_bits = exp_tab_dbl.at(index + 1) + top;

		// exp(x) = 2^(expo/N) * exp(rem) ~= scale + scale * (tail + exp(rem) - 1).
		// This evaluation is optimized assuming super scalar pipelined execution.
		remSqr = rem * rem;

		// Worst case error is less than (0.5+1.11/N+(abs poly error * 2^53))+0.25/N ulp.
		tmp = tail + rem + remSqr * (exp_poly_coeff_one_dbl + rem * exp_poly_coeff_two_dbl) +
			  remSqr * remSqr * (exp_poly_coeff_three_dbl + rem * exp_poly_coeff_four_dbl);
		if (CCM_UNLIKELY(abs_top == 0.0)) { return handle_special_case(tmp, sign_bits, expo_int64); }

		scale = ccm::support::uint64_to_double(sign_bits);

		// Note: tmp == 0 or |tmp| > 2^-65 and scale > 2^-739, so there is no spurious underflow here.
		return scale + scale * tmp;
	}
} // namespace ccm::internal::impl
