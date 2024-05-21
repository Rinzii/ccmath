/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/type_traits/bool_constant.hpp"
#include "ccmath/internal/support/type_traits/is_arithmetic.hpp"

namespace ccm::support::traits
{

	template <class T, bool = is_integral_v<T>>
	struct Internal_Sign_base {
		using U = remove_cv_t<T>;

		static constexpr bool Signed   = static_cast<U>(-1) < static_cast<U>(0);
		static constexpr bool Unsigned = !Signed;
	};

	template <class T>
	struct Internal_Sign_base<T, false>
	{
		static constexpr bool Signed	= is_floating_point_v<T>;
		static constexpr bool Unsigned = false;
	};

	template <typename T>
	struct is_unsigned : bool_constant<Internal_Sign_base<T>::Unsigned>
	{
	};

	template <typename T>
	constexpr bool is_unsigned_v = Internal_Sign_base<T>::Unsigned;

} // namespace ccm::support::traits