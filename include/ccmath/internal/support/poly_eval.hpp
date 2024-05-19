/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/multiply_add.hpp"

#include <type_traits>

namespace ccm::support
{

	// Helpers to evaluate polynomials using Horner's Scheme.
	//
	// With polyeval(x, a_0, a_1, ..., a_n) = a_n * x^n + ... + a_1 * x + a_0, we
	// evaluated it as:  a_0 + x * (a_1 + x * ( ... (a_(n-1) + x * a_n) ... ) ) ).
	// We will use FMA instructions if available.
	// Example: to evaluate x^3 + 2*x^2 + 3*x + 4, call
	//   polyeval( x, 4.0, 3.0, 2.0, 1.0 )
	// Code borrowed from LLVM

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) > sizeof(void *)), T>
	polyeval(const T & /*unused*/, const T &a0)
	{
		return a0;
	}

	template <typename T>
	constexpr std::enable_if_t<(sizeof(T) <= sizeof(void *)), T>
	polyeval(T /*unused*/, T a0)
	{
		return a0;
	}

	template <typename T, typename... Ts>
	constexpr std::enable_if_t<(sizeof(T) > sizeof(void *)), T>
	polyeval(const T &x, const T &a0, const Ts &...a)
	{
		return multiply_add(x, polyeval(x, a...), a0);
	}

	template <typename T, typename... Ts>
	constexpr std::enable_if_t<(sizeof(T) <= sizeof(void *)), T>
	polyeval(T x, T a0, Ts... a)
	{
		return multiply_add(x, polyeval(x, a...), a0);
	}

	struct fp_helpers
	{

	};









} // namespace ccm::support