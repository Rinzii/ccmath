/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/types/fp_types.hpp"
#include "ccmath/math/exponential/impl/log2_data.hpp"

#include <array>
#include <cstdint>

namespace ccm::internal
{
	namespace impl
	{
		constexpr auto internalLog2DataDbl			   = ccm::internal::log2_data<double>();
		constexpr auto log2_tab_values_dbl			   = log2_data<double>::tab;
		constexpr auto log2_tab2_values_dbl			   = log2_data<double>::tab2;
		constexpr auto log2_poly_values_dbl			   = internalLog2DataDbl.poly;
		constexpr auto log2_poly1_values_dbl		   = internalLog2DataDbl.poly1;
		constexpr auto log2_inverse_ln2_high_value_dbl = internalLog2DataDbl.invln2hi;
		constexpr auto log2_inverse_ln2_low_value_dbl  = internalLog2DataDbl.invln2lo;
		constexpr auto k_log2TableN_dbl				   = (1 << k_log2TableBitsDbl);
		constexpr auto k_log2TableOff_dbl			   = 0x3fe6000000000000;

		constexpr double log2_double_impl(double x)
		{
			ccm::double_t result{};
			ccm::double_t lowPart{};

			std::uint64_t intX		= support::double_to_uint64(x);
			const std::uint32_t top = support::top16_bits_of_double(x);

			constexpr std::uint64_t low	 = support::double_to_uint64(1.0 - 0x1.5b51p-5);
			constexpr std::uint64_t high = support::double_to_uint64(1.0 + 0x1.6ab2p-5);

			// Handle special cases where x is very close to 1.0
			if (CCM_UNLIKELY(intX - low < high - low))
			{
				// Handle the case where x is exactly 1.0
				if (CCM_UNLIKELY(intX == ccm::support::double_to_uint64(1.0))) { return 0.0; }

				const ccm::double_t rem		 = x - 1.0;
				const ccm::double_t remHi	 = support::uint64_to_double(support::double_to_uint64(rem) & -1ULL << 32);
				const ccm::double_t remLo	 = rem - remHi;
				const ccm::double_t highPart = remHi * log2_inverse_ln2_high_value_dbl;
				lowPart						 = remLo * log2_inverse_ln2_high_value_dbl + rem * log2_inverse_ln2_low_value_dbl;

				const ccm::double_t remSqr	= rem * rem; // rounding error: 0x1p-62.
				const ccm::double_t remQuad = remSqr * remSqr;

				// The Worst-case error is less than 0.55 ULP
				const ccm::double_t polynomialTerm = remSqr * (log2_poly1_values_dbl[0] + rem * log2_poly1_values_dbl[1]);
				result							   = highPart + polynomialTerm;
				lowPart += highPart - result + polynomialTerm;
				lowPart += remQuad *
						   (log2_poly1_values_dbl[2] + rem * log2_poly1_values_dbl[3] + remSqr * (log2_poly1_values_dbl[4] + rem * log2_poly1_values_dbl[5]) +
							remQuad * (log2_poly1_values_dbl[6] + rem * log2_poly1_values_dbl[7] +
									   remSqr * (log2_poly1_values_dbl[8] + rem * log2_poly1_values_dbl[9])));
				result += lowPart;

				return result;
			}

			if (CCM_UNLIKELY(top - 0x0010 >= 0x7ff0 - 0x0010))
			{

				// x is subnormal, normalize it.
				intX = support::double_to_uint64(x * 0x1p52); // 0x1p52 = 4.5036e+15 = 2^52
				intX -= 52ULL << 52;
			}

			// x = 2^expo normVal; where normVal is in range [k_logTableOff_dbl, 2 * k_logTableOff_dbl) and exact.
			//  The range is split into N sub-intervals.
			// The ith sub-interval contains normVal and c is near its center.
			const std::uint64_t tmp = intX - k_log2TableOff_dbl;
			// NOLINTBEGIN
			std::int64_t i	  = (tmp >> (52 - ccm::internal::k_log2TableBitsDbl)) % k_log2TableN_dbl;
			std::int64_t expo = static_cast<std::int64_t>(tmp) >> 52; // Arithmetic shift.
			// NOLINTEND
			const std::uint64_t intNorm		   = intX - (tmp & 0xfffULL << 52);
			const ccm::double_t inverseCoeff   = log2_tab_values_dbl.at(static_cast<unsigned long>(i)).invc;
			const ccm::double_t logarithmCoeff = log2_tab_values_dbl.at(static_cast<unsigned long>(i)).logc;
			const ccm::double_t normVal		   = support::uint64_to_double(intNorm);
			const auto expoDbl				   = static_cast<double_t>(expo);

			// rounding error: 0x1p-55/N + 0x1p-65.
			const ccm::double_t rem =
				(normVal - log2_tab2_values_dbl.at(static_cast<unsigned long>(i)).chi - log2_tab2_values_dbl.at(static_cast<unsigned long>(i)).clo) *
				inverseCoeff;
			const ccm::double_t remHi		= support::uint64_to_double(support::double_to_uint64(rem) & -1ULL << 32);
			const ccm::double_t remLo		= rem - remHi;
			const ccm::double_t remHighPart = remHi * log2_inverse_ln2_high_value_dbl;
			const ccm::double_t remLowPart	= remLo * log2_inverse_ln2_high_value_dbl + rem * log2_inverse_ln2_low_value_dbl;

			// hi + lo = rem/ln2 + log2(c) + expo
			const ccm::double_t logExpoSum = expoDbl + logarithmCoeff;
			const ccm::double_t highPart   = logExpoSum + remHighPart;
			lowPart						   = logExpoSum - highPart + remHighPart + remLowPart;

			// log2(rem+1) = rem/ln2 + rem^2*poly(rem)
			// Evaluation is optimized assuming super scalar pipelined execution.
			const ccm::double_t remSqr	= rem * rem; // rounding error: 0x1p-62.
			const ccm::double_t remQuad = remSqr * remSqr;

			// Worst-case error if |result| > 0x1p-4: 0.550 ULP.
			// ~ 0.5 + 2/N/ln2 + abs-poly-error*0x1p56+0.003 ULP.
			const ccm::double_t polynomialTerm = log2_poly_values_dbl[0] + rem * log2_poly_values_dbl[1] +
												 remSqr * (log2_poly_values_dbl[2] + rem * log2_poly_values_dbl[3]) +
												 remQuad * (log2_poly_values_dbl[4] + rem * log2_poly_values_dbl[5]);
			result = lowPart + remSqr * polynomialTerm + highPart;

			return result;
		}
	} // namespace impl

	constexpr double log2_double(double num) noexcept
	{
		return impl::log2_double_impl(num);
	}
} // namespace ccm::internal
