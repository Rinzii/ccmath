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

#include <ccmath/internal/support/fenv/fenv_support.hpp>
#include <ccmath/internal/support/fenv/rounding_mode.hpp>
#include <ccmath/internal/support/fp/directional_rounding_utils.hpp>
#include <ccmath/math/compare/isinf.hpp>
#include <ccmath/math/compare/isnan.hpp>
#include <type_traits>

namespace ccm
{
	/**
	 * @brief Rounds the floating-point argument num to an integer value (in floating-point format), using the current rounding mode. The library provides
	 * overloads of std::rint for all cv-unqualified floating-point types as the type of the parameter num.
	 * @tparam T The type of the number.
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	template <class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	constexpr T rint(T num)
	{
		if (ccm::isinf(num) || ccm::isnan(num)) { ccm::support::fenv::raise_except_if_required(FE_INVALID); }
		const auto rounding_mode{ccm::support::fenv::get_rounding_mode()};
		return ccm::support::fp::directional_round(num, rounding_mode);
	}

	/**
	 * @brief Additional overloads are provided for all integer types, which are treated as double.
	 * @tparam Integer The type of the integral value.
	 * @param num An integral value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	template <class Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr double rint(Integer num)
	{
		return static_cast<double>(num);
	}

	/**
	 * @brief Additional overloads are provided for all integer types, which are treated as double.
	 * @tparam Integer The type of the integral value.
	 * @param num An integral value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	template <class Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr long lrint(Integer num)
	{
		return static_cast<long>(num);
	}

	/**
	 * @brief Additional overloads are provided for all integer types, which are treated as double.
	 * @tparam Integer The type of the integral value.
	 * @param num An integral value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	template <class Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	constexpr long long llrint(Integer num)
	{
		return static_cast<long long>(num);
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value (in floating-point format), using the current rounding mode. The library provides
	 * overloads of std::rint for all cv-unqualified floating-point types as the type of the parameter num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr float rint(float num)
	{
		return ccm::rint<float>(num);
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value (in floating-point format), using the current rounding mode. The library provides
	 * overloads of std::rint for all cv-unqualified floating-point types as the type of the parameter num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr double rint(double num)
	{
		return ccm::rint<double>(num);
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value (in floating-point format), using the current rounding mode. The library provides
	 * overloads of std::rint for all cv-unqualified floating-point types as the type of the parameter num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long double rint(long double num)
	{
		return ccm::rint<long double>(num);
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value (in floating-point format), using the current rounding mode. The library provides
	 * overloads of std::rint for all cv-unqualified floating-point types as the type of the parameter num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr float rintf(float num)
	{
		return ccm::rint<float>(num);
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value (in floating-point format), using the current rounding mode. The library provides
	 * overloads of std::rint for all cv-unqualified floating-point types as the type of the parameter num.
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long double rintl(long double num)
	{
		return ccm::rint<long double>(num);
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long lrint(float num)
	{
		return static_cast<long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long lrint(double num)
	{
		if (ccm::isnan(num))
		{
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return std::numeric_limits<long>::min();
		}
		return static_cast<long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long lrint(long double num)
	{
		return static_cast<long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long lrintf(float num)
	{
		return static_cast<long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long lrintl(long double num)
	{
		return static_cast<long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long long llrint(float num)
	{
		return static_cast<long long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long long llrint(double num)
	{
		if (ccm::isnan(num))
		{
			ccm::support::fenv::raise_except_if_required(FE_INVALID);
			return std::numeric_limits<long long>::min();
		}
		return static_cast<long long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long long llrint(long double num)
	{
		return static_cast<long long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long long llrintf(float num)
	{
		return static_cast<long long>(ccm::rint(num));
	}

	/**
	 * @brief Rounds the floating-point argument num to an integer value, using the current rounding mode. The library provides overloads of std::lrint and
	 * std::llrint for all cv-unqualified floating-point types as the type of the parameter num.(
	 * @param num A floating-point value.
	 * @return If no errors occur, the nearest integer value to num, according to the current rounding mode, is returned.
	 */
	constexpr long long llrintl(long double num)
	{
		return static_cast<long long>(ccm::rint(num));
	}

} // namespace ccm
