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

namespace ccm::types
{
	// A type to interact with signed arithmetic types.
	struct Sign
	{
		[[nodiscard]] constexpr bool is_pos() const { return !m_is_negative_val; }
		[[nodiscard]] constexpr bool is_neg() const { return m_is_negative_val; }

		friend constexpr bool operator==(Sign a, Sign b) { return a.m_is_negative_val == b.m_is_negative_val; }

		friend constexpr bool operator!=(Sign a, Sign b) { return !(a == b); }

		static const Sign POS;
		static const Sign NEG;

	private:
		constexpr explicit Sign(bool is_negative) : m_is_negative_val(is_negative) {}

		bool m_is_negative_val;
	};

	inline constexpr Sign Sign::NEG = Sign(true);
	inline constexpr Sign Sign::POS = Sign(false);
} // namespace ccm::types
