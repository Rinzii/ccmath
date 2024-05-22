/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/type_traits.hpp"
#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/types/int128.hpp"
#include "ccmath/internal/types/int128_types.hpp"

#include <cstdint>

namespace ccm::support
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

		static constexpr uint_type exponent_mask			 = 0x000000FFU; // (1u << exponent_bits) - 1
		static constexpr uint_type normal_mantissa_mask		 = 0x00FFFFFFU; // (1u << mantissa_bits) - 1
		static constexpr uint_type denormal_mantissa_mask	 = 0x007FFFFFU; // (1u << (mantissa_bits - 1)) - 1
		static constexpr uint_type special_nan_mantissa_mask = 0x00400000U; // 1u << (mantissa_bits - 2)
		static constexpr uint_type shifted_sign_mask		 = 0x80000000U; // 1u << sign_shift
		static constexpr uint_type shifted_exponent_mask	 = 0x7F800000U; // exponent_mask << exponent_shift

		// 0x7FFFFFFFFFFFFFFF bit representation
		static constexpr float normalize_factor = 4294967296.f; // 2^32
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

		static constexpr uint_type exponent_mask			 = 0x00000000000007FFU; // (1ULL << exponent_bits) - 1
		static constexpr uint_type normal_mantissa_mask		 = 0x001FFFFFFFFFFFFFU; // (1ULL << mantissa_bits) - 1
		static constexpr uint_type denormal_mantissa_mask	 = 0x000FFFFFFFFFFFFFU; // (1ULL << (mantissa_bits - 1)) - 1
		static constexpr uint_type special_nan_mantissa_mask = 0x0008000000000000U; // 1ULL << (mantissa_bits - 2)
		static constexpr uint_type shifted_sign_mask		 = 0x8000000000000000U; // 1ULL << sign_shift
		static constexpr uint_type shifted_exponent_mask	 = 0x7FF0000000000000U; // exponent_mask << exponent_shift

		// 0x43F0000000000000 bit representation
		static constexpr double normalize_factor = 18446744073709551616.0; // 2^64
	};

#if defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT128)
	template <>
	struct floating_point_traits<long double>
	{
		static constexpr std::int32_t mantissa_bits			  = 112;	 // LDBL_MANT_DIG
		static constexpr std::int32_t exponent_bits			  = 15;	 // sizeof(long double) * CHAR_BIT - LDBL_MANT_DIG
		static constexpr std::int32_t maximum_binary_exponent = 16383; // LDBL_MAX_EXP - 1
		static constexpr std::int32_t minimum_binary_exponent = -16382; // LDBL_MIN_EXP - 1
		static constexpr std::int32_t exponent_bias			  = 16383;
		static constexpr std::int32_t sign_shift			  = 79; // exponent_bits + mantissa_bits - 1
		static constexpr std::int32_t exponent_shift		  = 63; // mantissa_bits - 1

		using uint_type = ccm::types::uint128_t;

		static constexpr uint_type exponent_mask			 = 0x0000000000007FFFU; // (1ULL << exponent_bits) - 1
		static constexpr uint_type normal_mantissa_mask		 = 0x0000FFFFFFFFFFFFU; // (1ULL << mantissa_bits) - 1
		static constexpr uint_type denormal_mantissa_mask	 = 0x00007FFFFFFFFFFFU; // (1ULL << (mantissa_bits - 1)) - 1
		static constexpr uint_type special_nan_mantissa_mask = 0x0000400000000000U; // 1ULL << (mantissa_bits - 2)
		static constexpr uint_type shifted_sign_mask		 = 0x8000000000000000U; // 1ULL << sign_shift
		static constexpr uint_type shifted_exponent_mask	 = 0x7FFF000000000000U; // exponent_mask << exponent_shift

		static constexpr long double normalize_factor = 340282366920938463463374607431768211456.0L; // 2^128


	};
#elif defined(CCM_TYPES_LONG_DOUBLE_IS_FLOAT80)
	template <>
struct floating_point_traits<long double>
	{
		static constexpr std::int32_t mantissa_bits			  = 64;	 // LDBL_MANT_DIG
		static constexpr std::int32_t exponent_bits			  = 15;	 // sizeof(long double) * CHAR_BIT - LDBL_MANT_DIG
		static constexpr std::int32_t maximum_binary_exponent = 16383; // LDBL_MAX_EXP - 1
		static constexpr std::int32_t minimum_binary_exponent = -16382; // LDBL_MIN_EXP - 1
		static constexpr std::int32_t exponent_bias			  = 16383;
		static constexpr std::int32_t sign_shift			  = 79; // exponent_bits + mantissa_bits - 1
		static constexpr std::int32_t exponent_shift		  = 63; // mantissa_bits - 1

		using uint_type = ccm::types::uint128_t;

		static constexpr uint_type exponent_mask			 = 0x0000000000007FFFU; // (1ULL << exponent_bits) - 1
		static constexpr uint_type normal_mantissa_mask		 = 0x0000FFFFFFFFFFFFU; // (1ULL << mantissa_bits) - 1
		static constexpr uint_type denormal_mantissa_mask	 = 0x00007FFFFFFFFFFFU; // (1ULL << (mantissa_bits - 1)) - 1
		static constexpr uint_type special_nan_mantissa_mask = 0x0000400000000000U; // 1ULL << (mantissa_bits - 2)
		static constexpr uint_type shifted_sign_mask		 = 0x8000000000000000U; // 1ULL << sign_shift
		static constexpr uint_type shifted_exponent_mask	 = 0x7FFF000000000000U; // exponent_mask << exponent_shift

		static constexpr long double normalize_factor = 340282366920938463463374607431768211456.0L; // 2^128
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


	template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	[[nodiscard]] constexpr T floating_point_abs_bits(const T & x) noexcept
	{
		using traits	= floating_point_traits<T>;
		using uint_type = typename traits::uint_type;
		const auto bits = bit_cast<uint_type>(x);
		return bits & ~traits::shifted_sign_mask;
	}

	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr std::int32_t get_exponent_of_floating_point(T x) noexcept
	{
		const auto bits = bit_cast<ccm::support::float_bits_t<T>>(x);

		const auto shifted_exponent = bits >> ccm::support::floating_point_traits<T>::exponent_shift;
		const auto masked_exponent	= shifted_exponent & ccm::support::floating_point_traits<T>::exponent_mask;
		return masked_exponent - ccm::support::floating_point_traits<T>::exponent_bias;
	}

	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	constexpr T set_exponent_of_floating_point(T x, int exp) noexcept
	{
		const auto bit_casted			  = ccm::support::bit_cast<ccm::support::float_bits_t<T>>(x);
		const auto inverted_exponent_mask = ~ccm::support::floating_point_traits<T>::shifted_exponent_mask;
		const auto masked_exponent = (exp + ccm::support::floating_point_traits<T>::exponent_bias) & ccm::support::floating_point_traits<T>::exponent_mask;
		const auto shifted_masked_exponent = masked_exponent << ccm::support::floating_point_traits<T>::exponent_shift;
		const auto final_bits			   = (bit_casted & inverted_exponent_mask) | shifted_masked_exponent;

		return bit_cast<T>(final_bits);
	}
} // namespace ccm::support
