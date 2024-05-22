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
	template <bool B, typename T = void> struct enable_if;
	template <typename T> struct enable_if<true, T> : type_identity<T> {};
	template <bool B, typename T = void>
	using enable_if_t = typename enable_if<B, T>::type;

} // namespace ccm::support::traits