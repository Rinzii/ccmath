/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present cmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/basic/abs.hpp"
#include "ccmath/detail/compare/isinf.hpp"
#include "ccmath/detail/compare/isnan.hpp"
#include "ccmath/detail/compare/signbit.hpp"
#include "ccmath/internal/helpers/find_number.hpp"
#include "ccmath/internal/helpers/pow_integral.hpp"
#include "ccmath/numbers.hpp"

#include <limits>

namespace ccm
{
	namespace
	{
		namespace impl
		{
			// see https://en.wikipedia.org/wiki/Euler%27s_continued_fraction_formula
			template <typename T>
			constexpr T exp_cont_frac_recursive(const T x, const int depth_end, const std::size_t maxIterationDepth = 25) noexcept
			{
				int depth = static_cast<int>(maxIterationDepth) - 1;
				T result  = T{1};

				while (depth > depth_end)
				{
					result = static_cast<T>(1) + x / static_cast<T>(depth - 1) - x / depth / result;
					--depth;
				}

				return result;
			}

			template <typename T>
			constexpr T exp_cont_frac(const T x) noexcept
			{
				return static_cast<T>(1) / (static_cast<T>(1) - x / exp_cont_frac_recursive(x, 2));
			}

			template <typename T>
			constexpr T exp_split(const T x) noexcept
			{
				return static_cast<T>(ccm::helpers::pow_integral(ccm::numbers::e_v<long double>, ccm::helpers::find_num_whole_part(x))) *
					   exp_cont_frac(ccm::helpers::find_num_fract_part(x));
			}

			template <typename T>
			constexpr T exp_impl(const T x) noexcept
			{
				if (ccm::isnan(x)) { return std::numeric_limits<T>::quiet_NaN(); }

				if (ccm::isinf(x)) { return ccm::signbit(x) ? T{0} : std::numeric_limits<T>::infinity(); }

				if (ccm::abs<T>(x) < std::numeric_limits<T>::min()) { return T{1}; }

				return ccm::abs(x) < static_cast<T>(2) ? exp_cont_frac(x) : exp_split(x);
			}
		} // namespace impl
	}	  // namespace

	template <typename T>
	inline constexpr T exp(T x)
	{
		return impl::exp_impl(x);
	}

} // namespace ccm
