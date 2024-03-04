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

#include "ccmath/internal/helpers/bits.hpp"
#include "ccmath/internal/helpers/floating_point_type.hpp"
#include "ccmath/internal/predef/unlikely.hpp"

namespace ccm::internal
{
	namespace
	{
		namespace impl
		{
			/*
			constexpr ccm::internal::log_data<float> internalLogDataFlt = ccm::internal::log_data<float>();
			constexpr auto tab_values_flt								= internalLogDataFlt.tab;
			constexpr auto poly_values_flt								= internalLogDataFlt.poly;
			constexpr auto ln2_value_flt								= internalLogDataFlt.ln2;
			constexpr auto k_logTableN_flt								= (1 << ccm::internal::k_logTableBitsFlt);
			constexpr auto k_logTableOff_flt							= 0x3f330000;
			*/
			inline constexpr double log2_double_impl(double x)
			{
				ccm::double_t z{};
				ccm::double_t r{};
				ccm::double_t r2{};
				ccm::double_t r4{};
				ccm::double_t y{};
				ccm::double_t invc{};
				ccm::double_t logc{};
				ccm::double_t kd{};
				ccm::double_t hi{};
				ccm::double_t lo{};
				ccm::double_t t1{};
				ccm::double_t t2{};
				ccm::double_t t3{};
				ccm::double_t p{};

				std::uint64_t ix{};
				std::uint64_t iz{};
				std::uint64_t tmp{};

				std::uint32_t top{};

				int k{};
				int i{};

				ix = ccm::helpers::double_to_uint64(x);
				top = ccm::helpers::top16_bits_of_double(x);

				constexpr std::uint64_t low = ccm::helpers::double_to_uint64(1.0 - 0x1.5b51p-5);
				constexpr std::uint64_t high = ccm::helpers::double_to_uint64(1.0 + 0x1.6ab2p-5);

				// Handle special cases where x is very close to 1.0
				if (CCM_UNLIKELY(ix - low < high - low))
                {
					// Handle the case where x is exactly 1.0
					if (CCM_UNLIKELY(ix == ccm::helpers::double_to_uint64(1.0)))
                    {
                        return 0.0;
                    }

					r = x - 1.0;

					ccm::double_t rhi{};
					ccm::double_t rlo{};

					rhi = ccm::helpers::uint64_to_double(ccm::helpers::double_to_uint64(r) & -1ULL << 32);
					rlo = r - rhi;
					hi = rhi * ccm::internal::ln2_hi;
                }

			}
		}
    }
}
