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
#include "ccmath/math/compare/isnan.hpp"
#include "ccmath/math/exponential/impl/log2_data.hpp"

#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace ccm::internal
{
	namespace impl
	{
		constexpr ccm::internal::log2_data<double> internalLog2DataDbl = ccm::internal::log2_data<double>();
		constexpr auto log2_tab_values_dbl							   = ccm::internal::log2_data<double, 0>::tab;
		constexpr auto log2_tab2_values_dbl							   = ccm::internal::log2_data<double, 0>::tab2;
		constexpr auto log2_poly_values_dbl							   = internalLog2DataDbl.poly;
		constexpr auto log2_poly1_values_dbl						   = internalLog2DataDbl.poly1;
		constexpr auto log2_inverse_ln2_high_value_dbl				   = internalLog2DataDbl.invln2hi;
		constexpr auto log2_inverse_ln2_low_value_dbl				   = internalLog2DataDbl.invln2lo;
		constexpr auto k_log2TableN_dbl								   = (1 << ccm::internal::k_log2TableBitsDbl);
		constexpr auto k_log2TableOff_dbl							   = 0x3fe6000000000000;

		constexpr double log2_double_impl(double x)
		{
			// double_t tends to have better performance on platforms with FLT_EVAL_METHOD==2.
			ccm::double_t normVal{};
			ccm::double_t rem{};
			ccm::double_t remSqr{};
			ccm::double_t remQuad{};
			ccm::double_t result{};
			ccm::double_t inverseCoeff{};
			ccm::double_t logarithmCoeff{};
			ccm::double_t expoDbl{};
			ccm::double_t highPart{};
			ccm::double_t lowPart{};
			ccm::double_t remHighPart{};
			ccm::double_t remLowPart{};
			ccm::double_t logExpoSum{};
			ccm::double_t polynomialTerm{};

			std::uint64_t intNorm{};
			std::uint64_t tmp{};

			std::int64_t expo{};
			std::int64_t i{};

			std::uint64_t intX = ccm::support::double_to_uint64(x);
			std::uint32_t top  = ccm::support::top16_bits_of_double(x);

			constexpr std::uint64_t low	 = ccm::support::double_to_uint64(1.0 - 0x1.5b51p-5);
			constexpr std::uint64_t high = ccm::support::double_to_uint64(1.0 + 0x1.6ab2p-5);

			// Handle special cases where x is very close to 1.0
			if (CCM_UNLIKELY(intX - low < high - low))
			{
				// Handle the case where x is exactly 1.0
				if (CCM_UNLIKELY(intX == ccm::support::double_to_uint64(1.0))) { return 0.0; }

				rem = x - 1.0;

				ccm::double_t remHi{};
				ccm::double_t remLo{};
				remHi	 = ccm::support::uint64_to_double(ccm::support::double_to_uint64(rem) & -1ULL << 32);
				remLo	 = rem - remHi;
				highPart = remHi * log2_inverse_ln2_high_value_dbl;
				lowPart	 = remLo * log2_inverse_ln2_high_value_dbl + rem * log2_inverse_ln2_low_value_dbl;

				remSqr	= rem * rem; // rounding error: 0x1p-62.
				remQuad = remSqr * remSqr;

				// The Worst-case error is less than 0.55 ULP
				polynomialTerm = remSqr * (log2_poly1_values_dbl[0] + rem * log2_poly1_values_dbl[1]);
				result		   = highPart + polynomialTerm;
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
				intX = ccm::support::double_to_uint64(x * 0x1p52); // 0x1p52 = 4.5036e+15 = 2^52
				intX -= 52ULL << 52;
			}

			// x = 2^expo normVal; where normVal is in range [k_logTableOff_dbl, 2 * k_logTableOff_dbl) and exact.
			//  The range is split into N sub-intervals.
			// The ith sub-interval contains normVal and c is near its center.
			tmp = intX - k_log2TableOff_dbl;
			// NOLINTBEGIN
			i	 = (tmp >> (52 - ccm::internal::k_log2TableBitsDbl)) % k_log2TableN_dbl;
			expo = static_cast<std::int64_t>(tmp) >> 52; // Arithmetic shift.
			// NOLINTEND
			intNorm		   = intX - (tmp & 0xfffULL << 52);
			inverseCoeff   = log2_tab_values_dbl.at(static_cast<unsigned long>(i)).invc;
			logarithmCoeff = log2_tab_values_dbl.at(static_cast<unsigned long>(i)).logc;
			normVal		   = ccm::support::uint64_to_double(intNorm);
			expoDbl		   = static_cast<double_t>(expo);

			ccm::double_t remHi{};
			ccm::double_t remLo{};

			// rounding error: 0x1p-55/N + 0x1p-65.
			rem = (normVal - log2_tab2_values_dbl.at(static_cast<unsigned long>(i)).chi - log2_tab2_values_dbl.at(static_cast<unsigned long>(i)).clo) *
				  inverseCoeff;
			remHi		= ccm::support::uint64_to_double(ccm::support::double_to_uint64(rem) & -1ULL << 32);
			remLo		= rem - remHi;
			remHighPart = remHi * log2_inverse_ln2_high_value_dbl;
			remLowPart	= remLo * log2_inverse_ln2_high_value_dbl + rem * log2_inverse_ln2_low_value_dbl;

			// hi + lo = rem/ln2 + log2(c) + expo
			logExpoSum = expoDbl + logarithmCoeff;
			highPart   = logExpoSum + remHighPart;
			lowPart	   = logExpoSum - highPart + remHighPart + remLowPart;

			// log2(rem+1) = rem/ln2 + rem^2*poly(rem)
			// Evaluation is optimized assuming super scalar pipelined execution.
			remSqr	= rem * rem; // rounding error: 0x1p-62.
			remQuad = remSqr * remSqr;

			// Worst-case error if |result| > 0x1p-4: 0.550 ULP.
			// ~ 0.5 + 2/N/ln2 + abs-poly-error*0x1p56+0.003 ULP.
			polynomialTerm = log2_poly_values_dbl[0] + rem * log2_poly_values_dbl[1] + remSqr * (log2_poly_values_dbl[2] + rem * log2_poly_values_dbl[3]) +
							 remQuad * (log2_poly_values_dbl[4] + rem * log2_poly_values_dbl[5]);
			result = lowPart + remSqr * polynomialTerm + highPart;

			return result;
		}
	} // namespace impl

	template <typename T>
	[[nodiscard]] constexpr T log2_double(T num) noexcept
	{
		return static_cast<T>(impl::log2_double_impl(static_cast<double>(num)));
	}
} // namespace ccm::internal
