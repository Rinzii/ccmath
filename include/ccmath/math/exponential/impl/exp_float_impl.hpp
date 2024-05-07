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

namespace ccm::internal::impl
{
	constexpr auto internal_exp_data_flt = ccm::internal::exp_data<float>();
	constexpr auto exp_inv_ln2_N_flt	 = internal_exp_data_flt.invln2_scaled;
	constexpr auto exp_shift_flt		 = internal_exp_data_flt.shift;
	constexpr auto exp_tab_flt			 = internal_exp_data_flt.tab;
	constexpr auto exp_poly_scaled_flt	 = internal_exp_data_flt.poly_scaled;
	constexpr auto k_exp_table_n_flt	 = (1 << ccm::internal::k_exp_table_bits_flt);

	constexpr float exp_float_impl(float x)
	{
		std::uint64_t expo_int64{};
		std::uint64_t tmp{};

		ccm::double_t expo{};
		ccm::double_t scaled_input{};
		ccm::double_t rem{};
		ccm::double_t remSqr{};
		ccm::double_t result{};
		ccm::double_t scale{};

		const auto x_dbl_t = static_cast<ccm::double_t>(x);
		const std::uint32_t abs_top = ccm::support::top12_bits_of_float(x) & 0x7ff;

		if (CCM_UNLIKELY(abs_top >= ccm::support::top12_bits_of_float(88.0F)))
		{
			// |x| >= 88 or x is nan.
			if (support::float_to_uint32(x) == support::float_to_uint32(-std::numeric_limits<float>::infinity())) { return 0.0F; }

			if (abs_top >= support::top12_bits_of_float(std::numeric_limits<float>::infinity())) { return x + x; }

			// Handle overflow
			if (x > 0x1.62e42ep6F) // x > log(0x1p128) ~= 88.72
			{
				return 0x1p97F * 0x1p97F;
			}

			// Handle underflow
			if (x < -0x1.9fe368p6F) // x < log(0x1p-150) ~= -103.97
			{
				return 0x1.4p-75F * 0x1.4p-75F;
			}
		}

		// x*N/Ln2 = k + r with r in [-1/2, 1/2] and int k.
		scaled_input = exp_inv_ln2_N_flt * x_dbl_t;

		/*
		 * Round and convert z to int, the result is in [-150*N, 128*N] and
		 * ideally ties-to-even rule is used, otherwise the magnitude of r
		 * can be bigger which gives larger approximation error.
		 */
		expo	   = helpers::narrow_eval(scaled_input + exp_shift_flt); // Must be double
		expo_int64 = support::double_to_uint64(expo);
		expo -= exp_shift_flt;
		rem = scaled_input - expo;

		// exp(x) = 2^(k/N) * 2^(r/N) ~= s * (C0*r^3 + C1*r^2 + C2*r + 1)
		tmp = static_cast<std::uint64_t>(exp_tab_flt.at(expo_int64 % k_exp_table_n_flt));
		tmp += (expo_int64 << (52 - k_exp_table_bits_flt));
		scale		 = support::uint64_to_double(tmp);
		scaled_input = exp_poly_scaled_flt.at(0) * rem + exp_poly_scaled_flt.at(1);
		remSqr		 = rem * rem;
		result		 = exp_poly_scaled_flt.at(2) * rem + 1.0F;
		result		 = scaled_input * remSqr + result;
		result		 = scale * result;

		return static_cast<float>(result);
	}
} // namespace ccm::internal::impl
