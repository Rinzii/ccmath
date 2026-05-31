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

#include "ccmath/internal/math/generic/builtins/builtin_helpers.hpp"
#include "ccmath/internal/support/always_false.hpp"

#include <type_traits>

namespace ccm::builtin
{
	template <typename T>
	inline constexpr bool has_runtime_lgamma =
#if defined(__GNUC__) || defined(__clang__)
		is_valid_builtin_type<T>;
#else
		false;
#endif

	template <typename T>
	auto runtime_lgamma(T x) -> std::enable_if_t<has_runtime_lgamma<T>, T>
	{
		if constexpr (std::is_same_v<T, float>) { return __builtin_lgammaf(x); }
		else if constexpr (std::is_same_v<T, double>) { return __builtin_lgamma(x); }
		else if constexpr (std::is_same_v<T, long double>) { return __builtin_lgammal(x); }
		else
		{
			static_assert(ccm::support::always_false<T>, "Unsupported type for __builtin_lgamma");
			return T{};
		}
	}
} // namespace ccm::builtin
