/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
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
	inline constexpr Real e_v = Real{2.718281828459045235360287471352662};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real log2e_v = Real{1.442695040888963407359924681001892};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real log10e_v = Real{0.434294481903251827651128918916605};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real pi_v = Real{3.141592653589793238462643383279502};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real inv_pi_v = Real{0.318309886183790671537767526745028};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real inv_sqrtpi_v = Real{0.564189583547756286948079451560772};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real ln2_v = Real{0.693147180559945309417232121458176};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real ln10_v = Real{2.302585092994045684017991454684364};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real sqrt2_v = Real{1.414213562373095048801688724209698};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real sqrt3_v = Real{1.732050807568877293527446341505872};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real inv_sqrt3_v = Real{0.577350269189625764509148780501957};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real egamma_v = Real{0.577215664901532860606512090082402};

	template <typename Real, std::enable_if_t<std::is_floating_point_v<Real>, bool> = true>
	inline constexpr Real phi_v = Real{1.618033988749894848204586834365638};

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
