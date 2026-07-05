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

#include "ccmath/internal/math/runtime/pp/declaration.hpp"

#include <cstddef>
#include <type_traits>

namespace ccm::pp
{
	// [simd.traits]
	template <typename T> struct is_abi_tag : std::false_type
	{};
	template <> struct is_abi_tag<ScalarAbi> : std::true_type
	{};
	template <detail::SimdSizeType N> struct is_abi_tag<VecAbi<N>> : std::true_type
	{};
	template <typename T> inline constexpr bool is_abi_tag_v = is_abi_tag<T>::value;

	template <typename T> struct is_simd : std::false_type
	{};
	template <typename T, typename Abi> struct is_simd<basic_simd<T, Abi>> : std::true_type
	{};
	template <typename T> inline constexpr bool is_simd_v = is_simd<T>::value;

	template <typename T> struct is_simd_mask : std::false_type
	{};
	template <std::size_t Bytes, typename Abi> struct is_simd_mask<basic_simd_mask<Bytes, Abi>> : std::true_type
	{};
	template <typename T> inline constexpr bool is_simd_mask_v = is_simd_mask<T>::value;

	template <typename T, typename Abi = detail::NativeAbi<T>> struct simd_size : std::integral_constant<detail::SimdSizeType, Abi::size>
	{};
	template <typename T, typename Abi = detail::NativeAbi<T>> inline constexpr detail::SimdSizeType simd_size_v = simd_size<T, Abi>::value;

	// resize_simd_t<N, V>: same element type as V, but N lanes.
	template <detail::SimdSizeType N, typename V> struct resize_simd;
	template <detail::SimdSizeType N, typename T, typename Abi> struct resize_simd<N, basic_simd<T, Abi>>
	{
		using type = basic_simd<T, detail::deduce_t<T, N>>;
	};
	template <detail::SimdSizeType N, std::size_t B, typename Abi> struct resize_simd<N, basic_simd_mask<B, Abi>>
	{
		using type = basic_simd_mask<B, detail::deduce_t<detail::mask_integer_from<B>, N>>;
	};
	template <detail::SimdSizeType N, typename V> using resize_simd_t = typename resize_simd<N, V>::type;

	// rebind_simd_t<U, V>: same lane count as V, but element type U. The ABI tag
	// is element-agnostic, so the tag is preserved.
	template <typename U, typename V> struct rebind_simd;
	template <typename U, typename T, typename Abi> struct rebind_simd<U, basic_simd<T, Abi>>
	{
		using type = basic_simd<U, Abi>;
	};
	template <typename U, std::size_t B, typename Abi> struct rebind_simd<U, basic_simd_mask<B, Abi>>
	{
		using type = basic_simd_mask<sizeof(U), Abi>;
	};
	template <typename U, typename V> using rebind_simd_t = typename rebind_simd<U, V>::type;

	namespace detail
	{
		template <typename T> using is_vectorizable_t = is_vectorizable<T>;
	} // namespace detail
} // namespace ccm::pp
