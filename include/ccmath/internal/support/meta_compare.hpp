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

namespace ccm::support
{
	/// @cond HIDDEN
	namespace detail
	{
		// A variadic alias template that resolves to its first argument.
		template <typename T, typename...>
		using first_t = T;

		// These are deliberately not defined.
		template <typename... Bn>
		auto or_fn(int) -> first_t<std::false_type, std::enable_if_t<!static_cast<bool>(Bn::value)>...>;

		template <typename... Bn>
		auto or_fn(...) -> std::true_type;

		template <typename... Bn>
		auto and_fn(int) -> first_t<std::true_type, std::enable_if_t<static_cast<bool>(Bn::value)>...>;

		template <typename... Bn>
		auto and_fn(...) -> std::false_type;
	} // namespace detail

	template <typename... Bn>
	struct or_ : decltype(detail::or_fn<Bn...>(0))
	{
	};

	template <typename... Bn>
	struct and_ : decltype(detail::and_fn<Bn...>(0))
	{
	};

	template <typename Pp>
	struct not_ : std::bool_constant<!static_cast<bool>(Pp::value)>
	{
	};
	/// @endcond

	template <typename... Bn>
	inline constexpr bool or_v = or_<Bn...>::value;
	template <typename... Bn>
	inline constexpr bool and_v = and_<Bn...>::value;
	template <typename Pp>
	inline constexpr bool not_v = not_<Pp>::value;
} // namespace ccm::support
