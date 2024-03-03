/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <limits>
#include <type_traits>

#include "ccmath/internal/helpers/is_odd.hpp"

#include <unordered_map>

namespace ccm
{
	namespace
	{
		namespace detail
		{
			// check that exponent does not have a fractional part
			template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
            [[nodiscard]]
			inline constexpr bool is_integral(T exponent) noexcept
            {
                return exponent == static_cast<T>(static_cast<int>(exponent));
            }

			//lookup table for 2^i
			template <typename T>
			[[nodiscard]]
			inline constexpr T two_to_pow_lookup_table(const T exponent) noexcept
            {
                constexpr T lookup[] = {
                    static_cast<T>(1), static_cast<T>(2), static_cast<T>(4), static_cast<T>(8), static_cast<T>(16), static_cast<T>(32), static_cast<T>(64), static_cast<T>(128),
                    static_cast<T>(256), static_cast<T>(512), static_cast<T>(1024), static_cast<T>(2048), static_cast<T>(4096), static_cast<T>(8192), static_cast<T>(16384), static_cast<T>(32768),
                    static_cast<T>(65536), static_cast<T>(131072), static_cast<T>(262144), static_cast<T>(524288), static_cast<T>(1048576), static_cast<T>(2097152), static_cast<T>(4194304), static_cast<T>(8388608),
                    static_cast<T>(16777216), static_cast<T>(33554432), static_cast<T>(67108864), static_cast<T>(134217728), static_cast<T>(268435456), static_cast<T>(536870912), static_cast<T>(1073741824), static_cast<T>(2147483648)
                };

                return lookup[exponent];
            }



		} // namespace detail
	}	  // namespace

} // namespace ccm
