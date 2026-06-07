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

namespace ccm::ext
{
	/**
	 * @brief Remap a value from one range to another.
	 *
	 * Maps x from the source range [src_start, src_end] to the destination
	 * range [dst_start, dst_end].
	 *
	 * This is a non-clamping linear remap. Values outside the source range
	 * are mapped outside the destination range.
	 *
	 * This is the raw formula. If src_start == src_end, the result follows
	 * normal floating-point division semantics.
	 *
	 * @tparam T Built-in floating-point type of the input and output.
	 * @param src_start Start of the source range.
	 * @param src_end End of the source range.
	 * @param dst_start Start of the destination range.
	 * @param dst_end End of the destination range.
	 * @param x Value to remap.
	 * @return The remapped value.
	 */
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	[[nodiscard]] constexpr T remap(T src_start, T src_end, T dst_start, T dst_end, T x) noexcept
	{
		return dst_start + ((x - src_start) * (dst_end - dst_start)) / (src_end - src_start);
	}

	namespace safe
	{
		/**
		 * @brief Safely remap a value from one range to another.
		 *
		 * Maps x from the source range [src_start, src_end] to the destination
		 * range [dst_start, dst_end].
		 *
		 * This is a non-clamping linear remap. Values outside the source range
		 * are mapped outside the destination range.
		 *
		 * If src_start == src_end, the source range has zero length. In that
		 * case this function returns dst_start to avoid division by zero and
		 * NaN propagation.
		 *
		 * @tparam T Built-in floating-point type of the input and output.
		 * @param src_start Start of the source range.
		 * @param src_end End of the source range.
		 * @param dst_start Start of the destination range.
		 * @param dst_end End of the destination range.
		 * @param x Value to remap.
		 * @return The remapped value.
		 */
		template <typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
		[[nodiscard]] constexpr T remap(T src_start, T src_end, T dst_start, T dst_end, T x) noexcept
		{
			const T length = src_end - src_start;

			if (length == T(0)) {
				return dst_start;
			}

			return dst_start + ((x - src_start) * (dst_end - dst_start)) / length;
		}
	} // namespace safe
} // namespace ccm::ext