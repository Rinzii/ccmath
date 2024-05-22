/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// C++20 std::type_identity support for C++17

#pragma once

namespace ccm
{
	template<class T>
	struct type_identity { using type = T; };

	template<class T>
	using type_identity_t = typename type_identity<T>::type;
} // namespace ccm

