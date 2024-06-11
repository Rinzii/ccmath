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
#include "ccmath/internal/support/bit_mask.hpp"
#include "ccmath/internal/types/fp_types.hpp"
#include "ccmath/math/exponential/impl/exp2_data.hpp"

#include <ccmath/internal/support/poly_eval.hpp>

#include <cstdint>
#include <limits>

namespace ccm::internal
{
	namespace impl
	{

		constexpr double handle_special_cases(ccm::double_t tmp, std::uint64_t sign_bits, std::uint64_t expo_uint64)
		{
			namespace sp = support;

			ccm::double_t scale{};
			ccm::double_t result{};

			// If expo > 0, then the exponent of scale might of overflowed by 1.
			if ((expo_uint64 & 0x80000000) == 0)
			{
				sign_bits -= 1ULL << 52;
				scale  = sp::uint64_to_double(sign_bits);
				result = 2 * (scale + scale * tmp);
				return result;
			}

			// If k < 0 then we need to take special care with the subnormal range.
			sign_bits += 1022ULL << 52;
			scale  = sp::uint64_to_double(sign_bits);
			result = scale + scale * tmp;

			// Round result to the right precision before scaling it into the subnormal
			// range to avoid double rounding that can cause 0.5+E/2 ulp error where
			// E is the worst-case ulp error outside the subnormal range.  So this
			// is only useful if the goal is better than 1 ulp worst-case error.
			if (result < 1.0)
			{
				ccm::double_t hi{};
				ccm::double_t lo{};
				lo	   = scale - result + scale * tmp;
				hi	   = 1.0 + result;
				lo	   = 1.0 - hi + result + lo;
				result = (hi + lo) - 1.0;

				// Avoid -0.0 with downward rounding.
				if (result == 0.0) { result = 0.0; }
			}
			result = 0x1p-1022 * result;
			return result;
		}

		constexpr double exp2_double_impl(double x)
		{
			constexpr auto exp2_data_double				 = ccm::internal::exp2_data<double>();
			constexpr auto shift_for_index_calculation	 = exp2_data_double.shift;
			constexpr auto lookup_table					 = exp2_data_double.tab;
			constexpr auto first_polynomial_coefficient	 = exp2_data_double.poly[0];
			constexpr auto second_polynomial_coefficient = exp2_data_double.poly[1];
			constexpr auto third_polynomial_coefficient	 = exp2_data_double.poly[2];
			constexpr auto fourth_polynomial_coefficient = exp2_data_double.poly[3];
			constexpr auto fifth_polynomial_coefficient	 = exp2_data_double.poly[4];
			constexpr auto table_size					 = (1 << exp2_data<double>::table_bits);
			constexpr auto table_bits					 = exp2_data<double>::table_bits;

			namespace sp = support;

			std::uint32_t abs_top{};
			ccm::double_t expo{};

			abs_top = sp::top12_bits_of_double(x) & 0x7FF;
			if (CCM_UNLIKELY(abs_top - sp::top12_bits_of_double(0x1p-54) >= sp::top12_bits_of_double(512.0) - sp::top12_bits_of_double(0x1p-54)))
			{
				// Avoid pointless underflow for tiny x.
				// 0 Is a common input.
				if (abs_top - sp::top12_bits_of_double(0x1p-54) >= sp::bitmask_traits<float>::neg_zero) { return x + 1.0; }

				if (abs_top >= sp::top12_bits_of_double(1024.0))
				{
					if (sp::double_to_uint64(x) == sp::double_to_uint64(-std::numeric_limits<double>::infinity())) { return 0.0; }

					if (abs_top >= sp::top12_bits_of_double(std::numeric_limits<double>::infinity())) { return x + 1.0; }

					// Handle overflow
					if ((sp::double_to_uint64(x) >> 63) == 0U) { return 0x1p769 * 0x1p769; }

					// Handle underflow
					if (sp::double_to_uint64(x) >= sp::double_to_uint64(-1075.0)) { return 0x1p-767 * 0x1p-767; }
				}

				if (2 * sp::double_to_uint64(x) > 2 * sp::double_to_uint64(928.0))
				{
					abs_top = 0; // Prep for handling the special case of large x.
				}
			}

			// exp2(x) = 2^(k/N) * 2^r, with 2^r in [2^(-1/2N),2^(1/2N)].
			// x = k/N + r, with int k and r in [-1/2N, 1/2N].
			expo						   = x + shift_for_index_calculation;
			const std::uint64_t expo_int64 = sp::double_to_uint64(expo); // This is k.
			expo -= shift_for_index_calculation;						 // This is k/N for int k
			const ccm::double_t rem = x - expo;

			// 2^(k/N) ~= scale * (1 + tail).
			const std::uint64_t index = 2 * (expo_int64 % table_size);
			const std::uint64_t top	  = expo_int64 << (52 - table_bits);
			const ccm::double_t tail  = sp::uint64_to_double(lookup_table.at(index));

			// This is only a valid scale when -1023*N < k < 1024*N.
			const std::uint64_t sign_bits = lookup_table.at(index + 1) + top;

			const ccm::double_t remSqr = rem * rem;

			const ccm::double_t tmp = tail + rem * first_polynomial_coefficient +
									  remSqr * (second_polynomial_coefficient + rem * third_polynomial_coefficient) +
									  remSqr * remSqr * (fourth_polynomial_coefficient + rem * fifth_polynomial_coefficient);
			if (CCM_UNLIKELY(abs_top == 0.0)) { return handle_special_cases(tmp, sign_bits, expo_int64); }

			const ccm::double_t scale = sp::uint64_to_double(sign_bits);

			return scale + scale * tmp;
		}
	} // namespace impl

	constexpr double exp2_double(double num)
	{
		return impl::exp2_double_impl(num);
	}

} // namespace ccm::internal