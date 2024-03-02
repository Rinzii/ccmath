/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

#include "ccmath/detail/compare/isinf.hpp"
#include "ccmath/detail/compare/isnan.hpp"

namespace ccm
{
	/// @cond MATH_DETAIL
	namespace
	{
		namespace impl
		{
			template <typename T>
			inline constexpr T fma_impl_internal(T x, T y, T z) noexcept
			{
				// If the compiler has a builtin, use it
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
				// TODO: Add a wrapper for if constexpr
				if constexpr (std::is_same_v<T, float>) { return __builtin_fmaf(x, y, z); }
				else if constexpr (std::is_same_v<T, double>) { return __builtin_fma(x, y, z); }
				else if constexpr (std::is_same_v<T, long double>) { return __builtin_fmal(x, y, z); }
#endif
				// If the compiler doesn't have a builtin, use the following and hope that the compiler is smart enough to optimize it
				return (x * y) + z;
			}

			// Special case for floating point types
			template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
			inline constexpr Real fma_impl_switch(Real x, Real y, Real z) noexcept
			{
				// Handle infinity
				if ((x == Real{0} && ccm::isinf(y)) || (y == Real{0} && ccm::isinf(x))) { return std::numeric_limits<Real>::quiet_NaN(); }
				if (x * y == std::numeric_limits<Real>::infinity() && z == -std::numeric_limits<Real>::infinity())
				{
					return std::numeric_limits<Real>::infinity();
				}

				// Handle NaN
				if (ccm::isnan(x) || ccm::isnan(y)) { return std::numeric_limits<Real>::quiet_NaN(); }
				if (ccm::isnan(z) && (x * y != 0 * std::numeric_limits<Real>::infinity() || x * y != std::numeric_limits<Real>::infinity() * 0))
				{
					return std::numeric_limits<Real>::quiet_NaN();
				}

				return fma_impl_internal(x, y, z);
			}

			template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
			inline constexpr Integer fma_impl_switch(Integer x, Integer y, Integer z) noexcept
			{
				return fma_impl_internal(x, y, z);
			}
		} // namespace impl
	}	  // namespace
	/// @endcond

	template <typename T>
	/**
	 * @brief Fused multiply-add operation.
	 * @tparam T A numeric type.
	 * @param x A floating-point or integer value.
	 * @param y A floating-point or integer value.
	 * @param z A floating-point or integer value.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	inline constexpr T fma(T x, T y, T z) noexcept
	{
		// the switch decides which implementation to use based on the type
		return impl::fma_impl_switch(x, y, z);
	}

	/**
	 * @brief Fused multiply-add operation.
	 * @tparam T A floating-point or integer type converted to a common type.
	 * @tparam U A floating-point or integer type converted to a common type.
	 * @tparam V A floating-point or integer type converted to a common type.
	 * @param x A floating-point or integer value converted to a common type.
	 * @param y A floating-point or integer value converted to a common type.
	 * @param z A floating-point or integer value converted to a common type.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	template <typename T, typename U, typename V>
	inline constexpr auto fma(T x, U y, V z) noexcept
	{
		// If our type is an integer epsilon will be set to 0 by default.
		// Instead, set epsilon to 1 so that our type is always at least the widest floating point type.
		constexpr auto TCommon = std::numeric_limits<T>::epsilon() > 0 ? std::numeric_limits<T>::epsilon() : 1;
		constexpr auto UCommon = std::numeric_limits<U>::epsilon() > 0 ? std::numeric_limits<U>::epsilon() : 1;
		constexpr auto VCommon = std::numeric_limits<V>::epsilon() > 0 ? std::numeric_limits<V>::epsilon() : 1;

		using epsilon_type = std::common_type_t<decltype(TCommon), decltype(UCommon), decltype(VCommon)>;

		using shared_type = std::conditional_t<
			TCommon <= std::numeric_limits<epsilon_type>::epsilon() && TCommon <= UCommon, T,
			std::conditional_t<UCommon <= std::numeric_limits<epsilon_type>::epsilon() && UCommon <= TCommon, U,
							   std::conditional_t<VCommon <= std::numeric_limits<epsilon_type>::epsilon() && VCommon <= UCommon, V, epsilon_type>>>;

		return ccm::fma(static_cast<shared_type>(x), static_cast<shared_type>(y), static_cast<shared_type>(z));
	}

	/**
	 * @brief Fused multiply-add operation.
	 * @tparam T A floating-point or integer type converted to a common type.
	 * @tparam U A floating-point or integer type converted to a common type.
	 * @tparam V A floating-point or integer type converted to a common type.
	 * @param x A floating-point or integer value converted to a common type.
	 * @param y A floating-point or integer value converted to a common type.
	 * @param z A floating-point or integer value converted to a common type.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	template <typename T, typename U, typename V, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<U> && std::is_integral_v<V>, bool> = true>
	inline constexpr auto fma(T x, U y, V z) noexcept // Special case for if all types are integers.
	{
		using shared_type = std::common_type_t<T, U, V>;
		return ccm::fma(static_cast<shared_type>(x), static_cast<shared_type>(y), static_cast<shared_type>(z));
	}

	/**
	 * @brief Fused multiply-add operation.
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @param z A floating-point value.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	inline constexpr float fmaf(float x, float y, float z) noexcept
	{
		return ccm::fma(x, y, z);
	}

	/**
	 * @brief Fused multiply-add operation.
	 * @param x A floating-point value.
	 * @param y A floating-point value.
	 * @param z A floating-point value.
	 * @return If successful, returns the value of x * y + z as if calculated to infinite precision and rounded once to fit the result type (or, alternatively,
	 * calculated as a single ternary floating-point operation).
	 */
	inline constexpr long double fmal(long double x, long double y, long double z) noexcept
	{
		return ccm::fma(x, y, z);
	}
} // namespace ccm

/// @ingroup basic
