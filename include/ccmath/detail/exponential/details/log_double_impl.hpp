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

#include "ccmath/detail/exponential/details/log_data.hpp"
#include "ccmath/internal/helpers/bits.hpp"
#include "ccmath/internal/helpers/floating_point_type.hpp"
#include "ccmath/internal/predef/unlikely.hpp"

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
			constexpr ccm::internal::log_data<double> internalLogDataDbl = ccm::internal::log_data<double>();
			constexpr auto tab_values_dbl								 = internalLogDataDbl.tab;
			constexpr auto tab2_values_dbl								 = internalLogDataDbl.tab2;
			constexpr auto poly_values_dbl								 = internalLogDataDbl.poly;
			constexpr auto poly1_values_dbl								 = internalLogDataDbl.poly1;
			constexpr auto ln2hi_value_dbl								 = internalLogDataDbl.ln2hi;
			constexpr auto ln2lo_value_dbl								 = internalLogDataDbl.ln2lo;
			constexpr auto k_logTableN_dbl								 = (1 << ccm::internal::k_logTableBitsDbl);
			constexpr auto k_logTableOff_dbl							 = 0x3fe6000000000000;

			inline constexpr double log_double_impl(double x)
			{
				// Declare variables for intermediate calculations
				ccm::double_t workspace{};
				ccm::double_t normVal{};
				ccm::double_t rem{};
				ccm::double_t remSqr{};
				ccm::double_t remCubed{};
				ccm::double_t result{};
				ccm::double_t inverseCoeff{};
				ccm::double_t logarithmCoeff{};
				ccm::double_t scaleFactor{};
				ccm::double_t highPart{};
				ccm::double_t lowPart{};

				// Declare variables for bitwise operations
				std::uint64_t intX{};
				std::uint64_t intNorm{};
				std::uint64_t tmp{};
				std::uint32_t top{};

				// Declare variables for exponent and loop iteration
				int expo{};
				int i{};

				// Convert input double to uint64_t and extract top 16 bits
				intX = ccm::helpers::double_to_uint64(x);
				top	 = ccm::helpers::top16_bits_of_double(x);

				// Constants for comparison
				constexpr std::uint64_t low	 = ccm::helpers::double_to_uint64(1.0 - 0x1p-4);
				constexpr std::uint64_t high = ccm::helpers::double_to_uint64(1.0 + 0x1p-4);

				// Handle special cases where input is close to 1.0
				if (CCM_UNLIKELY(intX - low < high - low))
				{
					// Handle the case where x is exactly 1.0
					if (CCM_UNLIKELY(intX == ccm::helpers::double_to_uint64(1.0))) { return 0; }

					// Compute the logarithm using polynomial approximation
					rem		 = x - 1.0;
					remSqr	 = rem * rem;
					remCubed = rem * remSqr;
					result	 = remCubed * (poly1_values_dbl[1] + rem * poly1_values_dbl[2] + remSqr * poly1_values_dbl[3] +
										   remCubed * (poly1_values_dbl[4] + rem * poly1_values_dbl[5] + remSqr * poly1_values_dbl[6] +
													   remCubed * (poly1_values_dbl[7] + rem * poly1_values_dbl[8] + remSqr * poly1_values_dbl[9] +
																   remCubed * poly1_values_dbl[10])));

					// Additional error correction
					// Worst-case error is around 0.507 ULP.
					workspace		  = rem * 0x1p27;
					ccm::double_t rhi = rem + workspace - workspace;
					ccm::double_t rlo = rem - rhi;
					workspace		  = rhi * rhi * poly1_values_dbl[0]; // poly1_values[0] == -0.5.
					highPart		  = rem + workspace;
					lowPart			  = rem - highPart + workspace;
					lowPart += poly1_values_dbl[0] * rlo * (rhi + rem);
					result += lowPart;
					result += highPart;
					return static_cast<double>(result);
				}

				// Handle special cases for very small or very large inputs
				if (CCM_UNLIKELY(top - 0x0010 >= 0x7ff0 - 0x0010))
				{
					// x is subnormal, normalize it.
					intX = ccm::helpers::double_to_uint64(x * 0x1p52);
					intX -= 52ULL << 52;
				}

				/*
				 * x = 2^expo normVal; where normVal is in range [0x3fe6000000000000, 2 * 0x3fe6000000000000) and exact.
				 * The range is split into k_logTableN sub-intervals.
				 * The i-th sub-interval contains normVal and c is near its center.
				 */

				// Calculate logarithm for normalized inputs
				tmp = intX - k_logTableOff_dbl;
				// NOLINTBEGIN
				i	 = (tmp >> (52 - ccm::internal::k_logTableBitsDbl)) % k_logTableN_dbl;
				expo = static_cast<std::int64_t>(tmp) >> 52;
				// NOLINTEND
				intNorm		   = intX - (tmp & 0xfffULL << 52); // Arithmetic shift
				inverseCoeff   = tab_values_dbl[i].invc;
				logarithmCoeff = tab_values_dbl[i].logc;
				normVal		   = ccm::helpers::uint64_to_double(intNorm);

				// Calculate intermediate value for logarithm computation
				// log(x) = log1p(normVal/c-1) + log(c) + expo*Ln2.
				// r ~= z/c - 1, |r| < 1/(2*N)
				rem			= (normVal - tab2_values_dbl[i].chi - tab2_values_dbl[i].clo) * inverseCoeff;
				scaleFactor = static_cast<ccm::double_t>(expo);

				// Calculate high and low parts of logarithm
				// hi + lo = r + log(c) + expo*Ln2.
				workspace = scaleFactor * ln2hi_value_dbl + logarithmCoeff;
				highPart  = workspace + rem;
				lowPart	  = workspace - highPart + rem + scaleFactor * ln2lo_value_dbl;

				// Final computation of logarithm
				// log(x) = lo + (log1p(rem) - rem) + hi.
				remSqr = rem * rem; // rounding error: 0x1p-54/k_logTableN^2.
				// Worst case error if |result| > 0x1p-4: 0.520 ULP
				// 0.5 + 2.06/k_logTableN + abs-poly-error*2^56+0.001 ULP
				result = lowPart + remSqr * poly_values_dbl[0] +
						 rem * remSqr * (poly_values_dbl[1] + rem * poly_values_dbl[2] + remSqr * (poly_values_dbl[3] + rem * poly_values_dbl[4])) + highPart;
				return static_cast<double>(result);
			}
		} // namespace impl
	}	  // namespace

	template <typename T>
	[[nodiscard]] inline constexpr T log_double(T num) noexcept
	{
		return static_cast<T>(impl::log_double_impl(static_cast<double>(num)));
	}
} // namespace ccm::internal
