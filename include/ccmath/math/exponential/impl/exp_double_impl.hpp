/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstdint>
#include "ccmath/internal/helpers/exp_helpers.hpp"
#include "ccmath/internal/predef/unlikely.hpp"
#include "ccmath/internal/type/fp_types.hpp"

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
            inline constexpr double handle_special_case_underflow_or_overflow(ccm::double_t tmp, std::uint64_t sbits, std::uint64_t exponent_int64) // NOLINT
            {
                ccm::double_t scale{};
                ccm::double_t result{};

                if ((exponent_int64 & 0x80000000) == 0)
                {
                    // k > 0, the exponent of scale might have overflowed by <= 460.
                    sbits -= 1009ULL << 52;
                    scale = ccm::helpers::uint64_to_double(sbits);
					result	  = 0x1p1009 * (scale + scale * tmp);
                    return result;
                }

                // k < 0, need special care in the subnormal range.
                sbits += 1022ULL << 52;
                scale = ccm::helpers::uint64_to_double(sbits);
				result	  = scale + scale * tmp;
                if (result < 1.0)
                {
                    // We need to round y into the correct precision before we can begin scaling it into
                    // the subnormal range so that we can avoid double round that could cause a 0.5+E/2 ulp error
                    // where E is the worst case ulp error outside the subnormal range.
                    ccm::double_t hi{};
                    ccm::double_t lo{};
                    lo = scale - result + scale * tmp;
                    hi = 1.0 + result;
                    lo = 1.0 - hi + result + lo;
					result  = ccm::helpers::narrow_eval(hi + lo) - 1.0;
                    // Prevent -0.0 with downward rounding.
                    if (result == 0.0) { result = 0.0; }
                }

				result = 0x1p-1022 * result;

                return result;
			}

			inline constexpr double exp_double_impl(double x)
            {
				std::uint32_t abs_top{};
				std::uint64_t expo_int64{};
				std::uint64_t idx{};
                std::uint64_t top{};
				std::uint64_t sbits{};

				ccm::double_t expo{};
				ccm::double_t z{};
				ccm::double_t rem{};
				ccm::double_t remSqr{};
				ccm::double_t scale{};
				ccm::double_t tail{};
				ccm::double_t tmp{};

				abs_top = ccm::helpers::top12_bits_of_double(x) & 0x7FF;
				if (CCM_UNLIKELY(abs_top - ccm::helpers::top12_bits_of_double(0x1p-54) >= ccm::helpers::top12_bits_of_double(512.0) - ccm::helpers::top12_bits_of_double(0x1p-54)))
				{
					if( abs_top - ccm::helpers::top12_bits_of_double(0x1p-54) >=  0x80000000)
					{
						// Avoid raising underflow for tiny x.
						return 1.0 + x;
					}
				}




				return 0;
			}
		} // namespace impl
	}
} // namespace ccm::internal
