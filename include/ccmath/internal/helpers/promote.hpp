/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <type_traits>

namespace ccm::helpers
{
	template <typename T, bool = std::is_integral_v<T>>
    struct promote
    {
        using type = double;
    };

	template<typename T>
	struct promote<T, false>
	{ };

	template<>
	struct promote<long double, false>
    {
        using type = long double;
    };

	template<>
	struct promote<double, false>
    {
        using type = double;
    };

	template<>
	struct promote<float, false>
    {
        using type = float;
    };

	template<typename... T>
	using promote_t = decltype((typename promote<T>::type(0) + ...)); // Assume we have fold expression

	// Deducing the promoted type is done by promoted_t<T...>,
	// then promote is used to provide the nested type member.
	template <typename T, typename U>
	using promote_2 = promote<promote_t<T, U>>;

	template <typename T, typename U, typename V>
	using promote_3 = promote<promote_t<T, U, V>>;

	template <typename T, typename U, typename V, typename W>
	using promote_4 = promote<promote_t<T, U, V, W>>;

	template <typename T, typename U>
	using promote_2_t = typename promote_2<T, U>::type;

	template <typename T, typename U, typename V>
	using promote_3_t = typename promote_3<T, U, V>::type;

	template <typename T, typename U, typename V, typename W>
	using promote_4_t = typename promote_4<T, U, V, W>::type;



}
