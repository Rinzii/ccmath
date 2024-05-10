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
#include "ccmath/math/exponential/impl/log_data.hpp"

#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace ccm::internal
{
	namespace impl
	{
		constexpr auto internalLogDataFlt  = ccm::internal::log_data<float>();
		constexpr auto log_tab_values_flt  = log_data<float>::tab;
		constexpr auto log_poly_values_flt = internalLogDataFlt.poly;
		constexpr auto log_ln2_value_flt   = internalLogDataFlt.ln2;
		constexpr auto k_logTableN_flt	   = (1 << ccm::internal::k_logTableBitsFlt);
		constexpr auto k_logTableOff_flt   = 0x3f330000;

		constexpr float log_float_impl(float x)
		{
			// Declare variables for intermediate calculations
			ccm::double_t normVal{};
			ccm::double_t rem{};
			ccm::double_t remSqr{};
			ccm::double_t result{};
			ccm::double_t result0{};
			ccm::double_t inverseCoeff{};
			ccm::double_t logarithmCoeff{};

			// Declare variables for bitwise operations
			std::uint32_t intX{};
			std::uint32_t intNorm{};
			std::uint32_t tmp{};

			// Declare variables for exponent and loop iteration
			int expo{};
			int i{};

			intX = ccm::support::float_to_uint32(x);

			// Correct the sign of zero with downward rounding when x is equal to 1.0
			if (CCM_UNLIKELY(intX == 0x3f800000)) { return 0; }

			if (CCM_UNLIKELY(intX - 0x00800000 >= 0x7f800000 - 0x00800000))
			{
				// If x is subnormal then normalize it.
				intX = ccm::support::float_to_uint32(x * 0x1p23F);
				intX -= 23 << 23;
			}

			// x = 2^exp normVal; where normVal is in range [k_logTableOff_flt, 2 * k_logTableOff_flt] and exact
			tmp			   = intX - k_logTableOff_flt;
			i			   = (tmp >> (23 - ccm::internal::k_logTableBitsFlt)) % k_logTableN_flt; // NOLINT
			expo		   = static_cast<std::int32_t>(tmp) >> 23;
			intNorm		   = intX - (tmp & static_cast<std::uint32_t>(0x1ff << 23));
			inverseCoeff   = log_tab_values_flt.at(static_cast<unsigned long>(i)).invc;
			logarithmCoeff = log_tab_values_flt.at(static_cast<unsigned long>(i)).logc;
			normVal		   = static_cast<ccm::double_t>(ccm::support::uint32_to_float(intNorm));

			// log(x) = log1p(normVal / inverseCoeff - 1) + log(inverseCoeff) + expo * Ln2
			rem		= normVal * inverseCoeff - 1;
			result0 = logarithmCoeff + static_cast<ccm::double_t>(expo) * log_ln2_value_flt;

			// Polynomial approximation for log1p(rem)
			remSqr = rem * rem;
			result = log_poly_values_flt[1] * rem + log_poly_values_flt[2];
			result = log_poly_values_flt[0] * remSqr + result;
			result = result * remSqr + (result0 + rem);

			return static_cast<float>(result);
		}

	} // namespace impl

	constexpr float log_float(float num) noexcept
	{
		return impl::log_float_impl(num);
	}
} // namespace ccm::internal
