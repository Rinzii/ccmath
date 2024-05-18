/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/support/bits.hpp"
#include "ccmath/internal/support/type_identity.hpp"


#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>

// NOLINTBEGIN

namespace ccm::support
{
	// A type trait mapping unsigned integers to their half-width unsigned counterparts.
	// clang-format off
	template <typename T> struct half_width;
	template <> struct half_width<std::uint16_t> : ccm::type_identity<std::uint8_t> {};
	template <> struct half_width<std::uint32_t> : ccm::type_identity<std::uint16_t> {};
	template <> struct half_width<std::uint64_t> : ccm::type_identity<std::uint32_t> {};

	template <typename T> using half_width_t = typename half_width<T>::type;
	// clang-format on

	// An array of two elements that can be used in multiword operations.
	template <typename T> struct DoubleWide final : std::array<T, 2> {
		using UP = std::array<T, 2>;
		using UP::UP;
		constexpr DoubleWide(T lo, T hi) : UP({lo, hi}) {}
	};

	// Converts an unsigned value into a DoubleWide<half_width_t<T>>.
	template <typename T> constexpr auto split(T value) {
		static_assert(std::is_unsigned_v<T>);
		using half_type = half_width_t<T>;
		return DoubleWide<half_type>(
			half_type(value),
			half_type(value >> std::numeric_limits<half_type>::digits));
	}

	// The low part of a DoubleWide value.
	template <typename T> constexpr T lo(const DoubleWide<T> &value) {
		return value[0];
	}
	// The high part of a DoubleWide value.
	template <typename T> constexpr T hi(const DoubleWide<T> &value) {
		return value[1];
	}
	// The low part of an unsigned value.
	template <typename T> constexpr half_width_t<T> lo(T value) {
		return lo(split(value));
	}
	// The high part of an unsigned value.
	template <typename T> constexpr half_width_t<T> hi(T value) {
		return hi(split(value));
	}

	// Returns 'a' times 'b' in a DoubleWide<word>. Cannot overflow by construction.
	template <typename word>
	constexpr DoubleWide<word> mul2(word a, word b) {
		if constexpr (std::is_same_v<word, uint8_t>) {
			return split<uint16_t>(uint16_t(a) * uint16_t(b));
		} else if constexpr (std::is_same_v<word, uint16_t>) {
			return split<uint32_t>(uint32_t(a) * uint32_t(b));
		}
		else if constexpr (cpp::is_same_v<word, uint32_t>) {
			return split<uint64_t>(uint64_t(a) * uint64_t(b));
		}
		#ifdef CCM_TYPES_HAS_INT128
		else if constexpr (cpp::is_same_v<word, uint64_t>) {
			return split<__uint128_t>(__uint128_t(a) * __uint128_t(b));
		}
		#endif
		else {
			using half_word = half_width_t<word>;
			const auto shiftl = [](word value) -> word {
				return value << cpp::numeric_limits<half_word>::digits;
			};
			const auto shiftr = [](word value) -> word {
				return value >> cpp::numeric_limits<half_word>::digits;
			};
















	template <size_t Bits, bool Signed, typename WordType = std::uint64_t>
	struct BigInt
	{
	private:
		static_assert(std::is_integral_v<WordType> && std::is_unsigned_v<WordType>, "WordType must be unsigned integer.");

		struct Division
		{
			BigInt quotient;
			BigInt remainder;
		};

	public:
		using word_type		= WordType;
		using unsigned_type = BigInt<Bits, false, word_type>;
		using signed_type	= BigInt<Bits, true, word_type>;

		static constexpr bool is_signed		   = Signed;
		static constexpr std::size_t bits	   = Bits;
		static constexpr std::size_t word_size = sizeof(word_type) * CHAR_BIT;

		static_assert(Bits > 0 && Bits % word_size == 0, "Number of bits in BigInt should be a multiple of word_size.");

		static constexpr std::size_t word_count = Bits / word_size;

		std::array<word_type, word_count> data{};

		constexpr BigInt()					   = default;
		constexpr BigInt(const BigInt & other) = default;

		/**
		 * @brief Extend the internal representation of the big integer by filling remaining words with a specific value.
		 * @param index The starting index from where the extension should begin.
		 * @param is_negative Boolean flag indicating whether the big integer is negative.
		 * @return void
		 */
		constexpr void extend(std::size_t index, bool is_negative)
		{
			const word_type value = is_negative ? std::numeric_limits<word_type>::max() : std::numeric_limits<word_type>::min();
			for (size_t i = index; i < word_count; ++i) { data[i] = value; }
		}

		template <std::size_t OtherBits, bool OtherSigned>
		constexpr BigInt(const BigInt<OtherBits, OtherSigned, word_type> & other)
		{
			if constexpr (OtherBits > Bits) // truncate bits
			{
				for (std::size_t i = 0; i < word_count; ++i) { data[i] = other.data[i]; }
			}
			else // Will be zero or sign extended
			{
				size_t i = 0;
				for (; i < OtherBits / word_size; ++i) { data[i] = other.data[i]; }
				extend(i, Signed && other.is_neg());
			}
		}

		/**
		 * @brief Construct a BigInt from a C-style array of words.
		 * @tparam N The size of the array.
		 * @param nums The C-style array of words.
		 */
		template <size_t N>
		constexpr BigInt(const WordType (&nums)[N]) // NOLINT
		{
			static_assert(N == word_count);
			for (size_t i = 0; i < word_count; ++i) { data[i] = nums[i]; }
		}

		constexpr explicit BigInt(const std::array<WordType, word_count> & words) : data(words) {}


	};
} // namespace ccm

//NOLINTEND