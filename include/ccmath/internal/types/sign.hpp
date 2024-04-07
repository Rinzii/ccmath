/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

// A helper type meant to interact with signed arithmetic types.
struct Sign
{
	[[nodiscard]] inline constexpr bool is_pos() const { return !is_negative; }
	[[nodiscard]] inline constexpr bool is_neg() const { return is_negative; }

	inline friend constexpr bool operator==(Sign a, Sign b) { return a.is_negative == b.is_negative; }

	inline friend constexpr bool operator!=(Sign a, Sign b) { return !(a == b); }

	static const Sign positive;
	static const Sign negative;

private:
	inline constexpr explicit Sign(bool is_negative) : is_negative(is_negative) {}

	bool is_negative;
};

inline constexpr Sign Sign::negative = Sign(true);
inline constexpr Sign Sign::positive = Sign(false);
