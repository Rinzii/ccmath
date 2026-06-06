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

#include "ccmath/internal/math/generic/builtins/power/pow.hpp"
#include "ccmath/internal/math/generic/func/power/pow_gen.hpp"
#include "ccmath/internal/math/runtime/func/power/pow_rt.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"

#include <type_traits>

namespace ccm::detail
{
	template <typename T>
	using cmath_pow_argument_t = std::conditional_t<std::is_integral_v<std::remove_cv_t<T>>, double, std::remove_cv_t<T>>;

	template <typename T, typename U>
	struct cmath_pow_result
	{
		using left_type	 = cmath_pow_argument_t<T>;
		using right_type = cmath_pow_argument_t<U>;

		using type = std::conditional_t<std::is_same_v<left_type, long double> || std::is_same_v<right_type, long double>,
										long double,
										std::conditional_t<std::is_same_v<left_type, double> || std::is_same_v<right_type, double>, double, float>>;
	};

	template <typename T, typename U>
	using cmath_pow_result_t = typename cmath_pow_result<T, U>::type;
} // namespace ccm::detail

namespace ccm
{
	/**
	 * @brief Raises a floating-point base to a floating-point exponent.
	 * @tparam T Floating-point type.
	 * @param base Base value.
	 * @param exp Exponent value.
	 * @return base raised to exp.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/pow
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T pow(T base, T exp)
	{
		if constexpr (ccm::builtin::has_constexpr_pow<T>) { return ccm::builtin::pow(base, exp); }
		else
		{
			// TODO: Add in usage of builtins that meet ccmath standards.

			if (support::is_constant_evaluated()) { return gen::pow_gen(base, exp); }
			return rt::pow_rt(base, exp);
		}
	}

	/**
	 * @brief Raises arithmetic inputs to a power after applying the [c.math]/3 promotion rule.
	 * @tparam Arithmetic1 Arithmetic type of the base.
	 * @tparam Arithmetic2 Arithmetic type of the exponent.
	 * @param base Base value.
	 * @param exp Exponent value.
	 * @return base raised to exp in the promoted floating-point type.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/pow
	 */
	template <typename Arithmetic1,
			  typename Arithmetic2,
			  std::enable_if_t<std::is_arithmetic_v<Arithmetic1> && std::is_arithmetic_v<Arithmetic2> &&
								   !(std::is_floating_point_v<Arithmetic1> && std::is_same_v<std::remove_cv_t<Arithmetic1>, std::remove_cv_t<Arithmetic2>>),
							   bool> = true>
	constexpr auto pow(Arithmetic1 base, Arithmetic2 exp)
	{
		using result_type = detail::cmath_pow_result_t<Arithmetic1, Arithmetic2>;
		return ccm::pow<result_type>(static_cast<result_type>(base), static_cast<result_type>(exp));
	}

	/**
	 * @brief Raises float inputs to a power.
	 * @param base Base value.
	 * @param exp Exponent value.
	 * @return base raised to exp as float.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/pow
	 */
	constexpr float powf(float base, float exp)
	{ return ccm::pow<float>(base, exp); }

	/**
	 * @brief Raises long double inputs to a power.
	 * @param base Base value.
	 * @param exp Exponent value.
	 * @return base raised to exp as long double.
	 * @see https://en.cppreference.com/w/cpp/numeric/math/pow
	 */
	constexpr long double powl(long double base, long double exp)
	{ return ccm::pow<long double>(base, exp); }
} // namespace ccm
