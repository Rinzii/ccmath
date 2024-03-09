/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isnan.hpp"

#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "ccmath/detail/exponential/details/log2_data.hpp"
#include "ccmath/internal/helpers/bits.hpp"
#include "ccmath/internal/helpers/floating_point_type.hpp"
#include "ccmath/internal/predef/unlikely.hpp"

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
			constexpr ccm::internal::log2_data<double> internalLogDataDbl = ccm::internal::log2_data<double>();
			constexpr auto tab_values_dbl								  = internalLogDataDbl.tab;
			constexpr auto tab2_values_dbl								  = internalLogDataDbl.tab2;
			constexpr auto poly_values_dbl								  = internalLogDataDbl.poly;
			constexpr auto poly1_values_dbl								  = internalLogDataDbl.poly1;
			constexpr auto inverse_ln2_high_value_dbl					  = internalLogDataDbl.invln2hi;
			constexpr auto inverse_ln2_low_value_dbl					  = internalLogDataDbl.invln2lo;
			constexpr auto k_logTableN_dbl								  = (1 << ccm::internal::k_log2TableBitsDbl);
			constexpr auto k_logTableOff_dbl							  = 0x3fe6000000000000;

			inline constexpr double log2_double_impl(double x)
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

				std::uint64_t intX{};
				std::uint64_t intNorm{};
				std::uint64_t tmp{};

				std::uint32_t top{};

				int expo{};
				int i{};

				intX = ccm::helpers::double_to_uint64(x);
				top	 = ccm::helpers::top16_bits_of_double(x);

				constexpr std::uint64_t low	 = ccm::helpers::double_to_uint64(1.0 - 0x1.5b51p-5);
				constexpr std::uint64_t high = ccm::helpers::double_to_uint64(1.0 + 0x1.6ab2p-5);

				// Handle special cases where x is very close to 1.0
				if (CCM_UNLIKELY(intX - low < high - low))
				{
					// Handle the case where x is exactly 1.0
					if (CCM_UNLIKELY(intX == ccm::helpers::double_to_uint64(1.0))) { return 0.0; }

					rem = x - 1.0;

					ccm::double_t remHi{};
					ccm::double_t remLo{};
					remHi	 = ccm::helpers::uint64_to_double(ccm::helpers::double_to_uint64(rem) & -1ULL << 32);
					remLo	 = rem - remHi;
					highPart = remHi * ccm::internal::impl::inverse_ln2_high_value_dbl;
					lowPart	 = remLo * ccm::internal::impl::inverse_ln2_high_value_dbl + rem * ccm::internal::impl::inverse_ln2_low_value_dbl;

					remSqr	= rem * rem; // rounding error: 0x1p-62.
					remQuad = remSqr * remSqr;

					// Worst-case error is less than 0.55 ULP
					polynomialTerm = remSqr * (poly1_values_dbl[0] + rem * poly1_values_dbl[1]);
					result		   = highPart + polynomialTerm;
					lowPart += highPart - result + polynomialTerm;
					lowPart +=
						remQuad * (poly1_values_dbl[2] + rem * poly1_values_dbl[3] + remSqr * (poly1_values_dbl[4] + rem * poly1_values_dbl[5]) +
								   remQuad * (poly1_values_dbl[6] + rem * poly1_values_dbl[7] + remSqr * (poly1_values_dbl[8] + rem * poly1_values_dbl[9])));
					result += lowPart;

					return result;
				}

				if (CCM_UNLIKELY(top - 0x0010 >= 0x7ff0 - 0x0010))
				{

					// x is subnormal, normalize it.
					intX = ccm::helpers::double_to_uint64(x * 0x1p52); // 0x1p52 = 4.5036e+15 = 2^52
					intX -= 52ULL << 52;
				}

				// x = 2^expo normVal; where normVal is in range [k_logTableOff_dbl, 2 * k_logTableOff_dbl) and exact.
				//  The range is split into N sub-intervals.
				// The ith sub-interval contains normVal and c is near its center.
				tmp = intX - k_logTableOff_dbl;
				// NOLINTBEGIN
				i	 = (tmp >> (52 - ccm::internal::k_log2TableBitsDbl)) % k_logTableN_dbl;
				expo = static_cast<std::int64_t>(tmp) >> 52; // Arithmetic shift.
				// NOLINTEND
				intNorm	   = intX - (tmp & 0xfffULL << 52);
				inverseCoeff   = tab_values_dbl[i].invc;
				logarithmCoeff = tab_values_dbl[i].logc;
				normVal		   = ccm::helpers::uint64_to_double(intNorm);
				expoDbl		   = static_cast<double_t>(expo);

				ccm::double_t remHi{};
				ccm::double_t remLo{};

				// rounding error: 0x1p-55/N + 0x1p-65.
				rem			= (normVal - tab2_values_dbl[i].chi - tab2_values_dbl[i].clo) * inverseCoeff;
				remHi		= ccm::helpers::uint64_to_double(ccm::helpers::double_to_uint64(rem) & -1ULL << 32);
				remLo		= rem - remHi;
				remHighPart = remHi * inverse_ln2_high_value_dbl;
				remLowPart	= remLo * inverse_ln2_high_value_dbl + rem * inverse_ln2_low_value_dbl;

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
				polynomialTerm = poly_values_dbl[0] + rem * poly_values_dbl[1] + remSqr * (poly_values_dbl[2] + rem * poly_values_dbl[3]) +
								 remQuad * (poly_values_dbl[4] + rem * poly_values_dbl[5]);
				result = lowPart + remSqr * polynomialTerm + highPart;

				return result;
			}
		} // namespace impl
	}	  // namespace

	template <typename T>
	[[nodiscard]] inline constexpr T log2_double(T num) noexcept
	{
		return static_cast<T>(impl::log2_double_impl(static_cast<double>(num)));
	}
} // namespace ccm::internal
