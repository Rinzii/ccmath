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
		constexpr auto internalLog2DataFlt	= ccm::internal::log2_data<float>();
		constexpr auto log2_tab_values_flt	= ccm::internal::log2_data<float>::tab;
		constexpr auto log2_poly_values_flt = internalLog2DataFlt.poly;
		constexpr auto k_log2TableN_flt		= (1 << ccm::internal::k_log2TableBitsFlt);
		constexpr auto k_log2TableOff_flt	= 0x3f330000;

		constexpr float log2_float_impl(float x)
		{
			std::uint32_t intX = support::float_to_uint32(x);

			// If x == 1 then fix the result to 0 with downward rounding
			if (CCM_UNLIKELY(intX == 0x3f800000)) { return 0; }

			if (CCM_UNLIKELY(intX - 0x00800000 >= 0x7f800000 - 0x00800000))
			{
				// TODO: Maybe handle division by zero here?

				// If x is NaN, return NaN
				if (((intX & 0x80000000) != 0U) || intX * 2 >= 0xff000000)
				{
					const float invalidResult = (x - x) / (x - x);
					return invalidResult;
				}

				// If x is subnormal, normalize it
				intX = support::float_to_uint32(x * 0x1p23F);
				intX -= 23 << 23;
			}

			// x = 2^expo * normVal; where normVal is in range [k_logTableOff_flt, 2 * k_logTableOff_flt] and exact.
			// We split the rang into N sub-intervals.
			// The i-th sub-interval contains normVal and c is near its center.
			const std::uint32_t tmp		= intX - k_log2TableOff_flt;
			const int i					= (tmp >> (23 - ccm::internal::k_log2TableBitsFlt)) % k_log2TableN_flt; // NOLINT
			const std::uint32_t top		= tmp & 0xff800000;
			const std::uint32_t intNorm = intX - top;
			// NOLINTNEXTLINE
			const int expo					   = tmp >> 23; // Arithmetic shift.
			const ccm::double_t inverseCoeff   = log2_tab_values_flt.at(static_cast<unsigned long>(i)).invc;
			const ccm::double_t logarithmCoeff = log2_tab_values_flt.at(static_cast<unsigned long>(i)).logc;
			const auto normVal				   = static_cast<ccm::double_t>(support::uint32_to_float(intNorm));

			// log2(x) = log1p(normVal/c-1)/ln2 + log2(c) + expo
			const ccm::double_t rem		= normVal * inverseCoeff - 1.0;
			const ccm::double_t result0 = logarithmCoeff + static_cast<ccm::double_t>(expo);

			// Pipelined polynomial evaluation to approximate log1p(r)/ln2.
			const ccm::double_t remSqr		   = rem * rem;
			ccm::double_t result			   = log2_poly_values_flt[1] * rem + log2_poly_values_flt[2];
			result							   = log2_poly_values_flt[0] * remSqr + result;
			const ccm::double_t polynomialTerm = log2_poly_values_flt[3] * rem + result0;
			result							   = result * remSqr + polynomialTerm;

			return static_cast<float>(result);
		}
	} // namespace impl

	constexpr float log2_float(float num) noexcept
	{
		return impl::log2_float_impl(num);
	}
} // namespace ccm::internal
