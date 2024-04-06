/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/types/float128.hpp"

#include <cstdint>
#include <type_traits>

namespace ccm::helpers
{
	template <class FloatingType>
	struct floating_point_traits;

	template <>
	struct floating_point_traits<float>
	{
		static constexpr std::int32_t mantissa_bits			  = 24;	  // FLT_MANT_DIG
		static constexpr std::int32_t exponent_bits			  = 8;	  // sizeof(float) * CHAR_BIT - FLT_MANT_DIG
		static constexpr std::int32_t maximum_binary_exponent = 127;  // FLT_MAX_EXP - 1
		static constexpr std::int32_t minimum_binary_exponent = -126; // FLT_MIN_EXP - 1
		static constexpr std::int32_t exponent_bias			  = 127;
		static constexpr std::int32_t sign_shift			  = 31; // exponent_bits + mantissa_bits - 1
		static constexpr std::int32_t exponent_shift		  = 23; // mantissa_bits - 1

		using uint_type = std::uint32_t;

		static constexpr std::uint32_t exponent_mask			 = 0x000000FFU; // (1u << exponent_bits) - 1
		static constexpr std::uint32_t normal_mantissa_mask		 = 0x00FFFFFFU; // (1u << mantissa_bits) - 1
		static constexpr std::uint32_t denormal_mantissa_mask	 = 0x007FFFFFU; // (1u << (mantissa_bits - 1)) - 1
		static constexpr std::uint32_t special_nan_mantissa_mask = 0x00400000U; // 1u << (mantissa_bits - 2)
		static constexpr std::uint32_t shifted_sign_mask		 = 0x80000000U; // 1u << sign_shift
		static constexpr std::uint32_t shifted_exponent_mask	 = 0x7F800000U; // exponent_mask << exponent_shift
	};

	template <>
	struct floating_point_traits<double>
	{
		static constexpr std::int32_t mantissa_bits			  = 53;	   // DBL_MANT_DIG
		static constexpr std::int32_t exponent_bits			  = 11;	   // sizeof(double) * CHAR_BIT - DBL_MANT_DIG
		static constexpr std::int32_t maximum_binary_exponent = 1023;  // DBL_MAX_EXP - 1
		static constexpr std::int32_t minimum_binary_exponent = -1022; // DBL_MIN_EXP - 1
		static constexpr std::int32_t exponent_bias			  = 1023;
		static constexpr std::int32_t sign_shift			  = 63; // exponent_bits + mantissa_bits - 1
		static constexpr std::int32_t exponent_shift		  = 52; // mantissa_bits - 1

		using uint_type = std::uint64_t;

		static constexpr std::uint64_t exponent_mask			 = 0x00000000000007FFU; // (1ULL << exponent_bits) - 1
		static constexpr std::uint64_t normal_mantissa_mask		 = 0x001FFFFFFFFFFFFFU; // (1ULL << mantissa_bits) - 1
		static constexpr std::uint64_t denormal_mantissa_mask	 = 0x000FFFFFFFFFFFFFU; // (1ULL << (mantissa_bits - 1)) - 1
		static constexpr std::uint64_t special_nan_mantissa_mask = 0x0008000000000000U; // 1ULL << (mantissa_bits - 2)
		static constexpr std::uint64_t shifted_sign_mask		 = 0x8000000000000000U; // 1ULL << sign_shift
		static constexpr std::uint64_t shifted_exponent_mask	 = 0x7FF0000000000000U; // exponent_mask << exponent_shift
	};

#ifdef __SIZEOF_FLOAT128__
	template <>
	struct floating_point_traits<long double> : floating_point_traits<double>
	{
	};
#else
    template <>
    struct floating_point_traits<long double> : floating_point_traits<double>
    {
    };
#endif

	template <typename T>
	using float_bits_t = typename floating_point_traits<T>::uint_type;

	template <typename T>
	inline constexpr typename floating_point_traits<T>::uint_type sign_mask_v = floating_point_traits<T>::shifted_sign_mask;

	// TODO: Add checking for if we support __builtin_bit_cast or not.

	// Taken from MSVC STL for better MSVC compatibility
	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	[[nodiscard]] constexpr T floating_point_abs_bits(const T & x) noexcept
    {
        using traits = floating_point_traits<T>;
		using uint_type = typename traits::uint_type;
		const auto bits = __builtin_bit_cast(uint_type, x);
		return bits & ~traits::shifted_sign_mask;
    }
}

