/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/type_traits/type_identity.hpp"


namespace ccm::support::traits
{
	template <typename T>
	struct remove_cv : type_identity<T> {};

	template <typename T>
	struct remove_cv<const T> : type_identity<T> {};

	template <typename T>
	struct remove_cv<volatile T> : type_identity<T> {};

	template <typename T>
	struct remove_cv<const volatile T> : type_identity<T> {};

	template <typename T>
	using remove_cv_t = typename remove_cv<T>::type;
} // namespace ccm::support::traits