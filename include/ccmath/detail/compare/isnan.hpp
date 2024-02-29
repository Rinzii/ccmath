/*
* Copyright (c) 2024-Present Ian Pike
* Copyright (c) 2024-Present ccmath contributors
*
* This library is provided under the MIT License.
* See LICENSE for more information.
*/

#pragma once

#include <type_traits>

namespace ccm
{
	/**
	 * @brief
	 * @tparam T
	 * @param x
	 * @return
	 */
    template <typename T>
    inline constexpr bool isnan(T x) noexcept
    {
		#if defined(__GNUC__)
			return __builtin_isnan(x); // GCC and Clang implement this as constexpr
		#else
			return x != x; // NOLINT
		#endif
    }

	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, int> = 0>
	inline constexpr bool isnan(Integer /* unused */)
	{
			// Is nan for integers is always false. As only floating point numbers can be nan.
            return false;
    }

}
