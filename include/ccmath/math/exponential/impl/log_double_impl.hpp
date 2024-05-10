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
#include "ccmath/math/exponential/impl/log_data.hpp"

#include <array>
#include <cstdint>

namespace ccm::internal
{
	namespace impl
	{
		constexpr auto internalLogDataDbl	= ccm::internal::log_data<double>();
		constexpr auto log_tab_values_dbl	= log_data<double>::tab;
		constexpr auto log_tab2_values_dbl	= log_data<double>::tab2;
		constexpr auto log_poly_values_dbl	= internalLogDataDbl.poly;
		constexpr auto log_poly1_values_dbl = internalLogDataDbl.poly1;
		constexpr auto log_ln2hi_value_dbl	= internalLogDataDbl.ln2hi;
		constexpr auto log_ln2lo_value_dbl	= internalLogDataDbl.ln2lo;
		constexpr auto k_logTableN_dbl		= (1 << ccm::internal::k_logTableBitsDbl);
		constexpr auto k_logTableOff_dbl	= 0x3fe6000000000000;

		constexpr double log_double_impl(double x)
		{
			// Declare variables for intermediate calculations
			ccm::double_t workspace{};
			ccm::double_t result{};
			ccm::double_t lowPart{};

			// Convert input double to uint64_t and extract top 16 bits
			std::uint64_t intX		= ccm::support::double_to_uint64(x);
			const std::uint32_t top = ccm::support::top16_bits_of_double(x);

			// Constants for comparison
			constexpr std::uint64_t low	 = ccm::support::double_to_uint64(1.0 - 0x1p-4);
			constexpr std::uint64_t high = ccm::support::double_to_uint64(1.0 + 0x1p-4);

			// Handle special cases where input is close to 1.0
			if (CCM_UNLIKELY(intX - low < high - low))
			{
				// Handle the case where x is exactly 1.0
				if (CCM_UNLIKELY(intX == ccm::support::double_to_uint64(1.0))) { return 0; }

				// Compute the logarithm using polynomial approximation
				const ccm::double_t rem		 = x - 1.0;
				const ccm::double_t remSqr	 = rem * rem;
				const ccm::double_t remCubed = rem * remSqr;
				result						 = remCubed * (log_poly1_values_dbl[1] + rem * log_poly1_values_dbl[2] + remSqr * log_poly1_values_dbl[3] +
									   remCubed * (log_poly1_values_dbl[4] + rem * log_poly1_values_dbl[5] + remSqr * log_poly1_values_dbl[6] +
												   remCubed * (log_poly1_values_dbl[7] + rem * log_poly1_values_dbl[8] + remSqr * log_poly1_values_dbl[9] +
															   remCubed * log_poly1_values_dbl[10])));

				// Additional error correction
				// Worst-case error is around 0.507 ULP.
				workspace					 = rem * 0x1p27;
				const ccm::double_t rhi		 = rem + workspace - workspace;
				const ccm::double_t rlo		 = rem - rhi;
				workspace					 = rhi * rhi * log_poly1_values_dbl[0]; // poly1_values[0] == -0.5.
				const ccm::double_t highPart = rem + workspace;
				lowPart						 = rem - highPart + workspace;
				lowPart += log_poly1_values_dbl[0] * rlo * (rhi + rem);
				result += lowPart;
				result += highPart;
				return result;
			}

			// Handle special cases for very small or very large inputs
			if (CCM_UNLIKELY(top - 0x0010 >= 0x7ff0 - 0x0010))
			{
				// x is subnormal, normalize it.
				intX = support::double_to_uint64(x * 0x1p52);
				intX -= 52ULL << 52;
			}

			/*
			 * x = 2^expo normVal; where normVal is in range [0x3fe6000000000000, 2 * 0x3fe6000000000000) and exact.
			 * The range is split into k_logTableN sub-intervals.
			 * The i-th sub-interval contains normVal and c is near its center.
			 */

			// Calculate logarithm for normalized inputs
			const std::uint64_t tmp = intX - k_logTableOff_dbl;
			// NOLINTBEGIN
			const std::int64_t i	= (tmp >> (52 - ccm::internal::k_logTableBitsDbl)) % k_logTableN_dbl;
			const std::int64_t expo = static_cast<std::int64_t>(tmp) >> 52;
			// NOLINTEND
			const std::uint64_t intNorm		   = intX - (tmp & 0xfffULL << 52); // Arithmetic shift
			const ccm::double_t inverseCoeff   = log_tab_values_dbl.at(static_cast<unsigned long>(i)).invc;
			const ccm::double_t logarithmCoeff = log_tab_values_dbl.at(static_cast<unsigned long>(i)).logc;
			const ccm::double_t normVal		   = support::uint64_to_double(intNorm);

			// Calculate intermediate value for logarithm computation
			// log(x) = log1p(normVal/c-1) + log(c) + expo*Ln2.
			// r ~= z/c - 1, |r| < 1/(2*N)
			const ccm::double_t rem =
				(normVal - log_tab2_values_dbl.at(static_cast<unsigned long>(i)).chi - log_tab2_values_dbl.at(static_cast<unsigned long>(i)).clo) *
				inverseCoeff;
			const auto scaleFactor = static_cast<ccm::double_t>(expo);

			// Calculate high and low parts of logarithm
			// hi + lo = r + log(c) + expo*Ln2.
			workspace					 = scaleFactor * log_ln2hi_value_dbl + logarithmCoeff;
			const ccm::double_t highPart = workspace + rem;
			lowPart						 = workspace - highPart + rem + scaleFactor * log_ln2lo_value_dbl;

			// Final computation of logarithm
			// log(x) = lo + (log1p(rem) - rem) + hi.
			const ccm::double_t remSqr = rem * rem; // rounding error: 0x1p-54/k_logTableN^2.
			// Worst case error if |result| > 0x1p-4: 0.520 ULP
			// 0.5 + 2.06/k_logTableN + abs-poly-error*2^56+0.001 ULP
			result = lowPart + remSqr * log_poly_values_dbl[0] +
					 rem * remSqr * (log_poly_values_dbl[1] + rem * log_poly_values_dbl[2] + remSqr * (log_poly_values_dbl[3] + rem * log_poly_values_dbl[4])) +
					 highPart;
			return result;
		}
	} // namespace impl

	constexpr double log_double(double num) noexcept
	{
		return impl::log_double_impl(num);
	}
} // namespace ccm::internal
