/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include "ccmath/internal/support/helpers/fpclassify_helper.hpp"
#include "ccmath/math/basic/abs.hpp"
#include "ccmath/math/compare/isinf.hpp"
#include "ccmath/math/compare/isnan.hpp"

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
	constexpr int fpclassify(T num)
	{
		if (ccm::isnan(num)) { return ccm::support::helpers::floating_point_defines::eFP_NAN; }
		if (ccm::isinf(num)) { return ccm::support::helpers::floating_point_defines::eFP_INFINITE; }
		if (num == static_cast<T>(0)) { return ccm::support::helpers::floating_point_defines::eFP_ZERO; }
		if (ccm::abs(num) < std::numeric_limits<T>::min() && ccm::abs(num) > 0)
		{
			return ccm::support::helpers::floating_point_defines::eFP_SUBNORMAL;
		}
		return ccm::support::helpers::floating_point_defines::eFP_NORMAL;
	}
} // namespace ccm
