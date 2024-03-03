/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/exponential/details/log_float_impl.hpp"
#include "ccmath/detail/exponential/details/log_double_impl.hpp"
#include "ccmath/detail/exponential/details/log_ldouble_impl.hpp"

namespace ccm
{
	template <typename T>
	inline constexpr T log(const T num) noexcept
    {
		if (num <= static_cast<T>(0)) { return -std::numeric_limits<T>::infinity(); }

		if (num == static_cast<T>(1)) { return static_cast<T>(0); }

		if (num < static_cast<T>(0)) { return std::numeric_limits<T>::quiet_NaN(); }

		if (num == std::numeric_limits<T>::infinity()) { return std::numeric_limits<T>::infinity(); }

		if (ccm::isnan(num)) { return std::numeric_limits<T>::quiet_NaN(); }

		/*
		if constexpr (std::is_same_v<T, float>) { return ccm::detail::log_float_impl(num); }
        else if constexpr (std::is_same_v<T, double>) { return ccm::detail::log_double_impl(num); }
        else if constexpr (std::is_same_v<T, long double>) { return ccm::detail::log_ldouble_impl(num); }
        else { return std::numeric_limits<T>::quiet_NaN(); }
        */

		return ccm::internal::log_double(num);
	}

} // namespace ccm
