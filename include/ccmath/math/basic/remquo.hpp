/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/math/basic/impl/remquo_double_impl.hpp"
#include "ccmath/math/basic/impl/remquo_float_impl.hpp"

namespace ccm
{
	/**
	 * @brief Signed remainder as well as the three last bits of the division operation
	 * @tparam T The type of the arguments
	 * @param x Floating-point or integer value
	 * @param y Floating-point or integer value
	 * @param quo Pointer to int to store the sign and some bits of x / y
	 * @return If successful, returns the floating-point remainder of the division x / y as defined in ccm::remainder, and stores, in *quo, the sign and at
	 * least three of the least significant bits of x / y
	 *
	 * @attention If you want the quotient pointer to work within a constant context you must perform something like as follows: (The below code will work with
	 * constexpr and static_assert)
	 *
	 * @code
	 * constexpr double get_remainder(double x, double y)
	 * {
	 *      int quotient {0};
	 *      double remainder = ccm::remquo(x, y, &quotient);
	 *      return remainder;
	 *  }
	 *
	 *  constexpr int get_quotient(double x, double y)
	 *  {
	 *      int quotient {0};
	 *      ccm::remquo(x, y, &quotient);
	 *      return quotient;
	 *  }
	 *  @endcode
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T remquo(T x, T y, int * quo)
	{
		if constexpr (std::is_same_v<T, float>) { return ccm::internal::remquo_float(x, y, quo); }
		else { return ccm::internal::remquo_double(x, y, quo); }
	}

	/**
	 * @brief Signed remainder as well as the three last bits of the division operation
	 * @tparam Arithmetic1 The type of the first argument
	 * @tparam Arithmetic2 The type of the second argument
	 * @param x Floating-point or integer values
	 * @param y Floating-point or integer values
	 * @param quo Pointer to int to store the sign and some bits of x / y
	 * @return If successful, returns the floating-point remainder of the division x / y as defined in ccm::remainder, and stores, in *quo, the sign and at
	 * least three of the least significant bits of x / y
	 *
	 * @attention If you want the quotient pointer to work within a constant context you must perform something like as follows: (The below code will work with
	 * constexpr and static_assert)
	 *
	 * @code
	 * constexpr double get_remainder(double x, double y)
	 * {
	 *      int quotient {0};
	 *      double remainder = ccm::remquo(x, y, &quotient);
	 *      return remainder;
	 *  }
	 *
	 *  constexpr int get_quotient(double x, double y)
	 *  {
	 *      int quotient {0};
	 *      ccm::remquo(x, y, &quotient);
	 *      return quotient;
	 *  }
	 *  @endcode
	 */
	template <class Arithmetic1, class Arithmetic2, std::enable_if_t<std::is_arithmetic_v<Arithmetic1> && std::is_arithmetic_v<Arithmetic2>, bool> = true>
	constexpr std::common_type_t<Arithmetic1, Arithmetic2> remquo(Arithmetic1 x, Arithmetic2 y, int * quo)
	{
		using shared_type = std::common_type_t<Arithmetic1, Arithmetic2>;
		return ccm::remquo<shared_type>(static_cast<shared_type>(x), static_cast<shared_type>(y), quo);
	}

	/**
	 * @brief Signed remainder as well as the three last bits of the division operation
	 * @param x Floating-point value
	 * @param y Floating-point value
	 * @param quo Pointer to int to store the sign and some bits of x / y
	 * @return If successful, returns the floating-point remainder of the division x / y as defined in ccm::remainder, and stores, in *quo, the sign and at
	 * least three of the least significant bits of x / y
	 *
	 * @attention If you want the quotient pointer to work within a constant context you must perform something like as follows: (The below code will work with
	 * constexpr and static_assert)
	 *
	 * @code
	 * constexpr double get_remainder(double x, double y)
	 * {
	 *      int quotient {0};
	 *      double remainder = ccm::remquo(x, y, &quotient);
	 *      return remainder;
	 *  }
	 *
	 *  constexpr int get_quotient(double x, double y)
	 *  {
	 *      int quotient {0};
	 *      ccm::remquo(x, y, &quotient);
	 *      return quotient;
	 *  }
	 *  @endcode
	 */
	constexpr float remquof(float x, float y, int * quo)
	{
		return ccm::remquo<float>(x, y, quo);
	}

	/**
	 * @brief Signed remainder as well as the three last bits of the division operation
	 * @param x Floating-point value
	 * @param y Floating-point value
	 * @param quo Pointer to int to store the sign and some bits of x / y
	 * @return If successful, returns the floating-point remainder of the division x / y as defined in ccm::remainder, and stores, in *quo, the sign and at
	 * least three of the least significant bits of x / y
	 *
	 * @attention If you want the quotient pointer to work within a constant context you must perform something like as follows: (The below code will work with
	 * constexpr and static_assert)
	 *
	 * @code
	 * constexpr double get_remainder(double x, double y)
	 * {
	 *      int quotient {0};
	 *      double remainder = ccm::remquo(x, y, &quotient);
	 *      return remainder;
	 *  }
	 *
	 *  constexpr int get_quotient(double x, double y)
	 *  {
	 *      int quotient {0};
	 *      ccm::remquo(x, y, &quotient);
	 *      return quotient;
	 *  }
	 *  @endcode
	 */
	constexpr long double remquol(long double x, long double y, int * quo)
	{
		// Currently, due to technical issues, the long double version of remquo is not implemented.
		// For now, we will cast the long double variables to a double and call the double version of remquo.
		return static_cast<long double>(ccm::remquo<double>(static_cast<double>(x), static_cast<double>(y), quo));
	}
} // namespace ccm

/// @ingroup basic
