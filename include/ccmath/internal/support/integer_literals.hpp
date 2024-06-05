/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

// Code borrowed from LLVM with heavy modifications done for ccmath to allow for both cross-platform and cross-compiler support.
// https://github.com/llvm/llvm-project/

#pragma once

#include "ccmath/internal/support/unreachable.hpp"
#include "ccmath/internal/types/big_int.hpp"
#include "ccmath/internal/types/int128_types.hpp"

#include <climits>
#include <cstddef>
#include <cstdint>

namespace ccm::support
{

	constexpr uint8_t operator""_u8(unsigned long long value)
	{
		return static_cast<uint8_t>(value);
	}

	constexpr uint16_t operator""_u16(unsigned long long value)
	{
		return static_cast<uint16_t>(value);
	}

	constexpr uint32_t operator""_u32(unsigned long long value)
	{
		return static_cast<uint32_t>(value);
	}

	constexpr uint64_t operator""_u64(unsigned long long value)
	{
		return static_cast<uint64_t>(value);
	}

	namespace internal
	{

		// Creates a T by reading digits from an array.
		template <typename T>
		constexpr T accumulate(int base, const uint8_t * digits, size_t size)
		{
			T value{};
			for (; size; ++digits, --size) // NOLINT
			{
				value *= base;
				value += *digits;
			}
			return value;
		}

		// A static buffer to hold the digits for a T.
		template <typename T, int base>
		struct DigitBuffer
		{
			static_assert(base == 2 || base == 10 || base == 16);
			// One character provides log2(base) bits.
			// Base 2 and 16 provide exactly one and four bits per character respectively.
			// For base 10, a character provides log2(10) ≈ 3.32... which we round to 3
			// for the purpose of buffer allocation.
			static constexpr size_t BITS_PER_DIGIT = base == 2 ? 1 : base == 10 ? 3 : base == 16 ? 4 : 0;
			static constexpr size_t MAX_DIGITS	   = sizeof(T) * CHAR_BIT / BITS_PER_DIGIT;
			static constexpr uint8_t INVALID_DIGIT = 255;

			uint8_t digits[MAX_DIGITS] = {};
			size_t size				   = 0;

			constexpr DigitBuffer(const char * str)
			{
				for (; *str != '\0'; ++str) { push(*str); }
			}

			// Returns the digit for a particular character.
			// Returns INVALID_DIGIT if the character is invalid.
			static constexpr uint8_t get_digit_value(const char c)
			{
				const auto to_lower = [](char c) { return c | 32; };
				const auto is_digit = [](char c) { return c >= '0' && c <= '9'; };
				const auto is_alpha = [](char c) { return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); };
				if (is_digit(c)) { return static_cast<uint8_t>(c - '0'); }
				if (base > 10 && is_alpha(c)) { return static_cast<uint8_t>(to_lower(c) - 'a' + 10); }
				return INVALID_DIGIT;
			}

			// Adds a single character to this buffer.
			constexpr void push(char c)
			{
				if (c == '\'')
				{
					return; // ' is valid but not taken into account.
				}
				const uint8_t value = get_digit_value(c);
				if (value == INVALID_DIGIT || size >= MAX_DIGITS)
				{
					// During constant evaluation `__builtin_unreachable` will halt the
					// compiler as it is not executable. This is preferable over `assert` that
					// will only trigger in debug mode. Also we can't use `static_assert`
					// because `value` and `size` are not constant.
					ccm::support::unreachable(); // invalid or too many characters.
				}
				digits[size] = value;
				++size;
			}
		};

		// Generic implementation for native types (including __uint128_t or ExtInt
		// where available).
		template <typename T>
		struct Parser
		{
			template <int base>
			static constexpr T parse(const char * str)
			{
				const DigitBuffer<T, base> buffer(str);
				return accumulate<T>(base, buffer.digits, buffer.size);
			}
		};

		// Specialization for UInt<N>.
		// Because this code runs at compile time, we try to make it efficient. For
		// binary and hexadecimal formats, we read digits by chunks of 64 bits and
		// produce the Big Int internal representation directly. For decimal numbers, we
		// go the slow path and use slower Big Int arithmetic.
		template <size_t N>
		struct Parser<ccm::types::UInt<N>>
		{
			using UIntT = ccm::types::UInt<N>;
			template <int base>
			static constexpr UIntT parse(const char * str)
			{
				const DigitBuffer<UIntT, base> buffer(str);
				if constexpr (base == 10)
				{
					// Slow path, we sum and multiply BigInt for each digit.
					return accumulate<UIntT>(base, buffer.digits, buffer.size);
				}
				else
				{
					// Fast path, we consume blocks of WordType and creates the BigInt's
					// internal representation directly.
					using WordArrayT		  = decltype(UIntT::val);
					using WordType			  = typename WordArrayT::value_type;
					WordArrayT array		  = {};
					size_t size				  = buffer.size;
					const uint8_t * digit_ptr = buffer.digits + size;
					for (size_t i = 0; i < array.size(); ++i)
					{
						constexpr size_t DIGITS = DigitBuffer<WordType, base>::MAX_DIGITS;
						const size_t chunk		= size > DIGITS ? DIGITS : size;
						digit_ptr -= chunk;
						size -= chunk;
						array[i] = accumulate<WordType>(base, digit_ptr, chunk);
					}
					return UIntT(array);
				}
			}
		};

		// Detects the base of the number and dispatches to the right implementation.
		template <typename T>
		constexpr T parse_with_prefix(const char * ptr)
		{
			using P = Parser<T>;
			if (ptr == nullptr) { return T(); }
			if (ptr[0] == '0')
			{
				if (ptr[1] == 'b') return P::template parse<2>(ptr + 2);
				if (ptr[1] == 'x') return P::template parse<16>(ptr + 2);
			}
			return P::template parse<10>(ptr);
		}

	} // namespace internal

	constexpr ccm::types::uint128_t operator""_u128(const char * x)
	{
		return internal::parse_with_prefix<ccm::types::uint128_t>(x);
	}

	constexpr auto operator""_u256(const char * x)
	{
		return internal::parse_with_prefix<ccm::types::UInt<256>>(x);
	}

	template <typename T>
	constexpr T parse_bigint(const char * ptr)
	{
		if (ptr == nullptr) { return T(); }
		if (ptr[0] == '-' || ptr[0] == '+')
		{
			auto positive = internal::parse_with_prefix<T>(ptr + 1);
			return ptr[0] == '-' ? -positive : positive;
		}
		return internal::parse_with_prefix<T>(ptr);
	}

} // namespace ccm::support