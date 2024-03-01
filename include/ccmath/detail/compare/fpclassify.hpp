/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/basic/abs.hpp"
#include "ccmath/detail/compare/isinf.hpp"
#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/internal/helpers/fpclassify_helper.hpp"

namespace ccm
{
	/**
	 * @brief Classify the floating point number
	 * @attention This function is heavily implementation defined and may not work as expected on unknown compilers
	 * @tparam T The type of the floating point number
	 * @param num The number to classify
	 * @return The classification of the number as an integer
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	inline constexpr int fpclassify(T num)
	{
		if (ccm::isnan(num)) { return static_cast<int>(ccm::helpers::floating_point_defines::eFP_NAN); }
		else if (ccm::isinf(num)) { return static_cast<int>(ccm::helpers::floating_point_defines::eFP_INFINITE); }
		else if (num == static_cast<T>(0)) { return static_cast<int>(ccm::helpers::floating_point_defines::eFP_ZERO); }
		else if (ccm::abs(num) < std::numeric_limits<T>::min() && ccm::abs(num) > 0)
		{
			return static_cast<int>(ccm::helpers::floating_point_defines::eFP_SUBNORMAL);
		}
		else { return static_cast<int>(ccm::helpers::floating_point_defines::eFP_NORMAL); }
	}
} // namespace ccm
