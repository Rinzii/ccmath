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

#include "ccmath/math/trig/impl/sincos_float_data.hpp"

#include "ccmath/internal/types/double_double.hpp"

#include <array>
#include <cstddef>

namespace ccm::internal::sincos_double_data
{
	using DoubleDouble = ccm::types::DoubleDouble;

	template <typename T, std::size_t N>
	constexpr std::array<T, N> promote_array(const std::array<float, N> & src)
	{
		std::array<T, N> dst{};
		for (std::size_t i = 0; i < N; ++i) { dst[i] = static_cast<T>(src[i]); }
		return dst;
	}

	constexpr std::array<double, 16> SIN_K_PI_OVER_8 = promote_array<double>(sincos_float_data::SIN_K_PI_OVER_8);

	constexpr std::array<double, 3> MPI = promote_array<double>(sincos_float_data::MPI);

	constexpr double ONE_OVER_PI = static_cast<double>(sincos_float_data::ONE_OVER_PI);

	constexpr DoubleDouble PI_OVER_8 = { static_cast<double>(sincos_float_data::PI_OVER_8.hi),
										 static_cast<double>(sincos_float_data::PI_OVER_8.lo) };

	constexpr std::array<std::array<double, 4>, 14> EIGHT_OVER_PI = []() {
		std::array<std::array<double, 4>, 14> dst{};
		for (std::size_t i = 0; i < 14; ++i)
		{
			for (std::size_t j = 0; j < 4; ++j) { dst[i][j] = static_cast<double>(sincos_float_data::EIGHT_OVER_PI[i][j]); }
		}
		return dst;
	}();

} // namespace ccm::internal::sincos_double_data
