/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

namespace ccm
{
	namespace
	{
		constexpr int ExpMaxIterDepth = 25;

		namespace detail
		{
			// see https://en.wikipedia.org/wiki/Euler%27s_continued_fraction_formula
			template <typename T>
			constexpr T exp_cont_frac_recursive(const T x, const int depth_end) noexcept
			{
				int depth = ExpMaxIterDepth - 1;
				T result  = T{1};

				while (depth > depth_end)
				{
					result = T{1} + x / T{depth - 1} - x / depth / result;
					--depth;
				}
			}

		} // namespace detail

	} // namespace

	template <typename T>
	constexpr T exp(T x)
	{
		return x * x;
	}

} // namespace ccm
