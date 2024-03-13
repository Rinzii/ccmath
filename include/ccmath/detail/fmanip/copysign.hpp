/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/helpers/bits.hpp"
#include "ccmath/detail/compare/signbit.hpp"
#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/basic/abs.hpp"

namespace ccm
{
	namespace
    {
        namespace impl
        {
            template <typename T>
            inline constexpr T copysign_impl(T x, T y)
            {
				if (ccm::isnan(x) || ccm::isnan(y))
                {
                    if (ccm::signbit(y)) { return -std::numeric_limits<T>::quiet_NaN(); }
					else { return std::numeric_limits<T>::quiet_NaN(); }
                }

				int sign_bit = ccm::signbit(y) ? -1 : 1;
				return ccm::abs(x) * sign_bit;
            }
        } // namespace impl
    }	  // namespace

    template <typename T>
    inline constexpr T copysign(T x, T y)
    {
        return impl::copysign_impl(x, y);
    }

} // namespace ccm
