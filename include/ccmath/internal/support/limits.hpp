/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

// Light weight wrapper around std::numeric_limits for handling special types not handled by std::numeric_limits
// Still, prefer to use std::numeric_limits. Only use ccm::support::numeric_limits when you can't use std::numeric_limits.

namespace ccm::support
{
	enum class float_round_style : std::int8_t
	{
		round_indeterminate		  = -1,
		round_toward_zero		  = 0,
		round_to_nearest		  = 1,
		round_toward_infinity	  = 2,
		round_toward_neg_infinity = 3
	};

	template <class T, bool = std::is_arithmetic_v<T>>
	class numeric_limits
	{
	public:
		using type = T;

		static constexpr const bool is_specialized = std::numeric_limits<type>::is_specialized;
		[[nodiscard]] static constexpr type min() noexcept { return std::numeric_limits<type>::min(); }
		[[nodiscard]] static constexpr type max() noexcept { return std::numeric_limits<type>::max(); }
		[[nodiscard]] static constexpr type lowest() noexcept { return std::numeric_limits<type>::lowest(); }

		static constexpr const int digits		= std::numeric_limits<type>::digits;
		static constexpr const int digits10		= std::numeric_limits<type>::digits10;
		static constexpr const int max_digits10 = std::numeric_limits<type>::max_digits10;
		static constexpr const bool is_signed	= std::numeric_limits<type>::is_signed;
		static constexpr const bool is_integer	= std::numeric_limits<type>::is_integer;
		static constexpr const bool is_exact	= std::numeric_limits<type>::is_exact;
		static constexpr const int radix		= std::numeric_limits<type>::radix;
		[[nodiscard]] static constexpr type epsilon() noexcept { return std::numeric_limits<type>::epsilon(); }
		[[nodiscard]] static constexpr type round_error() noexcept { return std::numeric_limits<type>::round_error(); }

		static constexpr const int min_exponent	  = std::numeric_limits<type>::min_exponent;
		static constexpr const int min_exponent10 = std::numeric_limits<type>::min_exponent10;
		static constexpr const int max_exponent	  = std::numeric_limits<type>::max_exponent;
		static constexpr const int max_exponent10 = std::numeric_limits<type>::max_exponent10;

		static constexpr const bool has_infinity	  = std::numeric_limits<type>::has_infinity;
		static constexpr const bool has_quiet_NaN	  = std::numeric_limits<type>::has_quiet_NaN;
		static constexpr const bool has_signaling_NaN = std::numeric_limits<type>::has_signaling_NaN;
		[[nodiscard]] static constexpr type infinity() noexcept { return std::numeric_limits<type>::infinity(); }
		[[nodiscard]] static constexpr type quiet_NaN() noexcept { return std::numeric_limits<type>::quiet_NaN(); }
		[[nodiscard]] static constexpr type signaling_NaN() noexcept { return std::numeric_limits<type>::signaling_NaN(); }
		[[nodiscard]] static constexpr type denorm_min() noexcept { return std::numeric_limits<type>::denorm_min(); }

		static constexpr const bool is_iec559  = std::numeric_limits<type>::is_iec559;
		static constexpr const bool is_bounded = std::numeric_limits<type>::is_bounded;
		static constexpr const bool is_modulo  = std::numeric_limits<type>::is_modulo;

		static constexpr const bool traps					 = std::numeric_limits<type>::traps;
		static constexpr const bool tinyness_before			 = std::numeric_limits<type>::tinyness_before;
		static constexpr const float_round_style round_style = static_cast<float_round_style>(std::numeric_limits<type>::round_style);
	};
} // namespace ccm::support
