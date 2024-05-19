/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/support/type_traits/type_identity.hpp"

namespace ccm::support::traits
{
	template <typename Ta>
	struct make_signed;
	template <>
	struct make_signed<char> : type_identity<char>
	{
	};
	template <>
	struct make_signed<signed char> : type_identity<char>
	{
	};
	template <>
	struct make_signed<short> : type_identity<short>
	{
	};
	template <>
	struct make_signed<int> : type_identity<int>
	{
	};
	template <>
	struct make_signed<long> : type_identity<long>
	{
	};
	template <>
	struct make_signed<long long> : type_identity<long long>
	{
	};
	template <>
	struct make_signed<unsigned char> : type_identity<char>
	{
	};
	template <>
	struct make_signed<unsigned short> : type_identity<short>
	{
	};
	template <>
	struct make_signed<unsigned int> : type_identity<int>
	{
	};
	template <>
	struct make_signed<unsigned long> : type_identity<long>
	{
	};
	template <>
	struct make_signed<unsigned long long> : type_identity<long long>
	{
	};
#ifdef CCM_TYPES_HAS_INT128
	template <>
	struct make_signed<__int128_t> : type_identity<__int128_t>
	{
	};
	template <>
	struct make_signed<__uint128_t> : type_identity<__int128_t>
	{
	};
#endif
	template <typename T>
	using make_signed_t = typename make_signed<T>::type;

} // namespace ccm::support::traits