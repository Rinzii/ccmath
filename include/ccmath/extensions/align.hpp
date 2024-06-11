/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstddef>
#include <type_traits>

namespace ccm::ext
{
	namespace AlignmentRounding
	{
		enum class Direction : char
		{
			eUP,
			eDOWN,
		};
	} // namespace AlignmentRounding

	namespace AR = AlignmentRounding;

	/**
	 * @brief Aligns a value to the nearest multiple of the alignment.
	 * @tparam T The type of the value to align.
	 * @tparam alignment
	 * @tparam mode
	 * @param value
	 * @return
	 */
	template <typename T, std::size_t alignment, AR::Direction mode, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	constexpr T align(T value) noexcept
	{
		static_assert(alignment != 0, "Alignment must be non-zero");

		if constexpr (mode == AR::Direction::eUP)
		{
			/*
			 * What is happening here:
			 * 1. (alignment - 1)
			 *   - First, we subtract 1 from the alignment value. This is done to prepare for rounding up.
			 *   - For example, if alignment is 8, then (alignment - 1) is 7.
			 * 2. (value + (alignment - 1))
			 * 	 - Next, we add (alignment - 1) to value. This ensures that any remainder when value is divided by
			 * 	 	alignment is accounted for, effectively pushing value to the next alignment boundary. This step handles the rounding up.
			 * 3. ~(alignment - 1)
			 *   - The bitwise NOT operator ~ is then applied to (alignment - 1). This flips all the bits.
			 *   	If (alignment - 1) is 7 (0000 0111 in binary), then ~(alignment - 1) becomes 1111 1000 in binary.
			 *   	This creates a mask that clears the lower bits up to the alignment boundary.
			 * 4. (value + (alignment - 1)) & ~(alignment - 1)
			 *   - Finally, we perform a bitwise AND between the adjusted value and the mask created by ~(alignment - 1).
			 *   	This operation zeroes out the lower bits that fall within the alignment range, effectively rounding
			 *   	value up to the nearest multiple of alignment.
			 */
			return (value + (alignment - 1)) & ~(alignment - 1);
		}
		else
		{
			/*
			 * What is happening here:
			 * 1. (alignment - 1)
			 *   - First, we subtract 1 from the alignment value. This is done to prepare for rounding down.
			 *   - For example, if alignment is 8, then (alignment - 1) is 7 (0000 0111 in binary).
			 * 2. ~(alignment - 1)
			 *   - The bitwise NOT operator ~ is then applied to (alignment - 1). This flips all the bits.
			 *   - If (alignment - 1) is 7 (0000 0111 in binary), then ~(alignment - 1) becomes 248 (1111 1000 in binary).
			 *   - This creates a mask that clears the lower bits up to the alignment boundary.
			 * 3. value & ~(alignment - 1)
			 *   - Finally, we perform a bitwise AND between the value and the mask created by ~(alignment - 1).
			 *   - This operation zeroes out the lower bits that fall within the alignment range, effectively rounding
			 *     value down to the nearest multiple of alignment.
			 */
			return value & ~(alignment - 1);
		}
	}

	template <typename T, std::size_t alignment, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	constexpr T align_up(T value) noexcept
	{
		return align<T, alignment, AR::Direction::eUP>(value);
	}

	template <typename T, std::size_t alignment, std::enable_if_t<std::is_integral_v<T>, bool> = true>
	constexpr T align_down(T value) noexcept
	{
		return align<T, alignment, AR::Direction::eDOWN>(value);
	}
} // namespace ccm::ext