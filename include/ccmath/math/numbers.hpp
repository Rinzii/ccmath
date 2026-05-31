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

#include <type_traits>

/*
 * @brief This namespace contains a set of constants that are useful for mathematical operations.
 */
namespace ccm::numbers
{
	// Values taken from llvm-project
	// https://github.com/llvm/llvm-project/blob/main/libcxx/include/numbers

	// These are added here to still offer support for c++17.

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real e_v = static_cast<Real>(2.718281828459045235360287471352662L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real log2e_v = static_cast<Real>(1.442695040888963407359924681001892L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real log10e_v = static_cast<Real>(0.434294481903251827651128918916605L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real pi_v = static_cast<Real>(3.141592653589793238462643383279502L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real inv_pi_v = static_cast<Real>(0.318309886183790671537767526745028L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real inv_sqrtpi_v = static_cast<Real>(0.564189583547756286948079451560772L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real ln2_v = static_cast<Real>(0.693147180559945309417232121458176L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real ln10_v = static_cast<Real>(2.302585092994045684017991454684364L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real sqrt2_v = static_cast<Real>(1.414213562373095048801688724209698L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real sqrt3_v = static_cast<Real>(1.732050807568877293527446341505872L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real inv_sqrt3_v = static_cast<Real>(0.577350269189625764509148780501957L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real egamma_v = static_cast<Real>(0.577215664901532860606512090082402L);

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real phi_v = static_cast<Real>(1.618033988749894848204586834365638L);

	inline constexpr double e		   = e_v<double>;
	inline constexpr double log2e	   = log2e_v<double>;
	inline constexpr double log10e	   = log10e_v<double>;
	inline constexpr double pi		   = pi_v<double>;
	inline constexpr double inv_pi	   = inv_pi_v<double>;
	inline constexpr double inv_sqrtpi = inv_sqrtpi_v<double>;
	inline constexpr double ln2		   = ln2_v<double>;
	inline constexpr double ln10	   = ln10_v<double>;
	inline constexpr double sqrt2	   = sqrt2_v<double>;
	inline constexpr double sqrt3	   = sqrt3_v<double>;
	inline constexpr double inv_sqrt3  = inv_sqrt3_v<double>;
	inline constexpr double egamma	   = egamma_v<double>;
	inline constexpr double phi		   = phi_v<double>;
} // namespace ccm::numbers
