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
	template <typename T>
	struct make_unsigned;
	template <>
	struct make_unsigned<char> : type_identity<unsigned char>
	{
	};
	template <>
	struct make_unsigned<signed char> : type_identity<unsigned char>
	{
	};
	template <>
	struct make_unsigned<short> : type_identity<unsigned short>
	{
	};
	template <>
	struct make_unsigned<int> : type_identity<unsigned int>
	{
	};
	template <>
	struct make_unsigned<long> : type_identity<unsigned long>
	{
	};
	template <>
	struct make_unsigned<long long> : type_identity<unsigned long long>
	{
	};
	template <>
	struct make_unsigned<unsigned char> : type_identity<unsigned char>
	{
	};
	template <>
	struct make_unsigned<unsigned short> : type_identity<unsigned short>
	{
	};
	template <>
	struct make_unsigned<unsigned int> : type_identity<unsigned int>
	{
	};
	template <>
	struct make_unsigned<unsigned long> : type_identity<unsigned long>
	{
	};
	template <>
	struct make_unsigned<unsigned long long> : type_identity<unsigned long long>
	{
	};
#ifdef CCM_TYPES_HAS_INT128
	template <>
	struct make_unsigned<__int128_t> : type_identity<__uint128_t>
	{
	};
	template <>
	struct make_unsigned<__uint128_t> : type_identity<__uint128_t>
	{
	};
#endif
	template <typename T>
	using make_unsigned_t = typename make_unsigned<T>::type;
} // namespace ccm::support::traits